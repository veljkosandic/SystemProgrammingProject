#include "../../inc/linker/Linker.hpp"
#include <fstream>
#include <utility>
Linker* Linker::inst = nullptr;
const std::string Linker::strs[] = {"-relocatable","-hex"};
std::ostream &operator<<(std::ostream &o, const LinkerException &e)
{
  o << "Greska pri linkovanju :";
  if(e.file!="")o << " fajl " << e.file << " :" ;
  o << e.info << "\n" << std::endl;
  return o;
}
int Linker::mapSections(){
  try{
    for(FileInfo* f : files){
       for(unsigned long i : f->getSectionNdxs()){
        std::string secname = f->getSymbolTable()[i]->getName();
        if(newsectionInfos.find(secname)==newsectionInfos.end()){//sekcija nije definisana
          newsectionInfos[secname].data=f->getData(i);
          newsectionInfos[secname].offset=0;
          f->setOffset(i,0);
          newst.push_back(new SymbolTableElement(newst.size(),0,0,SymbolTableElement::SCTN,SymbolTableElement::LOC,newst.size(),secname,false,true));
        }else{
          f->setOffset(i,newsectionInfos[secname].data.size());
          newsectionInfos[secname].data.insert(newsectionInfos[secname].data.end(),f->getData(i).begin(),f->getData(i).end());
        }
       }
    }
  }catch(LinkerException* e){
    std::cout << *e;
    exit(-1);
  }
  return 0;
}
int Linker::mapSyms(){
  try{
    for(FileInfo* f : files){
       for(SymbolTableElement* e: f->getSymbolTable()){
        if(e->getBind()==SymbolTableElement::LOC)continue;
        //std::cout << e->getName() << std::endl;
        //if(e->getType()==SymbolTableElement::SCTN)continue;
        if(e->getNdx()!=0 && f->getSymbolTable()[e->getNdx()]->getType()!=SymbolTableElement::SCTN)continue;//naporan equ
        SymbolTableElement* newelem = find(e->getName());
        if(newelem==nullptr){//simbol na koji smo prvi put naisli
          SymbolTableElement* tmp = new SymbolTableElement(newst.size(),0,0,SymbolTableElement::NOTYP,SymbolTableElement::GLOB,0,e->getName(),false,true);
          if(e->getNdx()!=0 || e->isAbs()){//simbol je definisan
            tmp->setAbs(e->isAbs());
            tmp->setType(e->getType());
            tmp->setBind(e->getBind());
            if(!e->isAbs()){
            tmp->setVal(e->getVal() + f->getOffset(e->getNdx()));
            tmp->setNdx(find(f->getSymbolTable()[e->getNdx()]->getName())->getNdx());
            }
            else tmp->setVal(e->getVal());
          }
          newst.push_back(tmp);
        }
        else{
        if((newelem->getNdx()!=0 || newelem->isAbs()) && (e->getNdx()!=0 || e->isAbs()) && newelem->getBind()==SymbolTableElement::GLOB && e->getBind()==SymbolTableElement::GLOB)throw new LinkerException(f->getFilename(),"Visestruko definisanje simbola " + e->getName()); 
           if(e->getBind()==SymbolTableElement::LOC){
             SymbolTableElement* tmp = new SymbolTableElement(newst.size(),0,0,SymbolTableElement::NOTYP,SymbolTableElement::GLOB,0,e->getName(),false,true);
            tmp->setAbs(e->isAbs());
            tmp->setType(e->getType());
            tmp->setBind(e->getBind());
            if(!e->isAbs()){
            tmp->setVal(e->getVal() + f->getOffset(e->getNdx()));
            tmp->setNdx(find(f->getSymbolTable()[e->getNdx()]->getName())->getNdx());
            }
            else tmp->setVal(e->getVal());
           }
           else if(e->getNdx()!=0 || e->isAbs()){//simbol je definisan
            newelem->setAbs(e->isAbs());
            newelem->setType(e->getType());
            if(!e->isAbs()){
            newelem->setVal(e->getVal() + f->getOffset(e->getNdx()));
            newelem->setNdx(find(f->getSymbolTable()[e->getNdx()]->getName())->getNdx());
            }
            else newelem->setVal(e->getVal());
          }
        }
       }
    }
    /*for(SymbolTableElement *e : newst){
      if(e->getNdx()==0 && !e->isAbs())throw new LinkerException(""," nedefinisan simbol " + e->getName());
    }*/
    //da se razrijese i externovi
    for(FileInfo* f : files){
      for(SymbolTableElement* e: f->getSymbolTable()){
                if(e->getBind()==SymbolTableElement::LOC)continue;
         if(!(e->getNdx()!=0 && f->getSymbolTable()[e->getNdx()]->getType()!=SymbolTableElement::SCTN))continue;//naporan equ
          SymbolTableElement* parent = find(f->getSymbolTable()[e->getNdx()]->getName());
          if(parent->getNdx()!=0 || parent->isAbs() ){
            e->setVal(e->getVal() + parent->getVal());
            e->setAbs(parent->isAbs());
            e->setNdx(parent->getNdx());
          }else e->setNdx(parent->getNum());
          SymbolTableElement* newelem = find(e->getName());
          if(newelem==nullptr){//simbol na koji smo prvi put naisli
          newelem = new SymbolTableElement(newst.size(),0,0,SymbolTableElement::NOTYP,SymbolTableElement::GLOB,0,e->getName(),false,true);
            newelem->setAbs(e->isAbs());
            newelem->setType(e->getType());
            newelem->setBind(e->getBind());
            newelem->setVal(e->getVal());
            newelem->setNdx(e->getNdx());
            newst.push_back(newelem);
        }else{
          if((newelem->getNdx()!=0 || newelem->isAbs()) && (e->getNdx()!=0 || e->isAbs()) && newelem->getBind()==SymbolTableElement::GLOB && e->getBind()==SymbolTableElement::GLOB)throw new LinkerException(f->getFilename(),"Visestruko definisanje simbola " + e->getName()); 
          if(e->getBind()==SymbolTableElement::LOC){
             SymbolTableElement* tmp = new SymbolTableElement(newst.size(),0,0,SymbolTableElement::NOTYP,SymbolTableElement::GLOB,0,e->getName(),false,true);
            tmp->setAbs(e->isAbs());
            tmp->setType(e->getType());
            tmp->setBind(e->getBind());
              newelem->setVal(e->getVal());
              newelem->setNdx(e->getNdx());
           }
           else if(e->getNdx()!=0 || e->isAbs()){//simbol je definisan
            newelem->setAbs(e->isAbs());
            newelem->setType(e->getType());
            newelem->setVal(e->getVal());
            newelem->setNdx(e->getNdx());
        }
        }
        //std::cout << *e;
      }
    }
    bool changed = false;
    do{
      changed = false;
      for(SymbolTableElement* e: newst){
         if(!(e->getNdx()!=0 && newst[e->getNdx()]->getType()!=SymbolTableElement::SCTN))continue;//naporan equ
          SymbolTableElement* parent = find(newst[e->getNdx()]->getName());
          if(parent->getNdx()!=0 || parent->isAbs() ){
            e->setVal(e->getVal() + parent->getVal());
            e->setAbs(parent->isAbs());
            e->setNdx(parent->getNdx());
            changed = true;
          }else e->setNdx(parent->getNum());
        //std::cout << *e;
      }
    }while(changed);
  }catch(LinkerException* e){
    std::cout << *e;
    exit(-1);
  }
  return 0;
}

int Linker::resolveRelocs(){
  try{
    for(FileInfo* f : files){
      for(unsigned long i : f->getSectionNdxs()){
        for(RelocTableElement* e : f->getRelocTable(i)){//za pocetak ubacujemo sve, kasnije cemo vidjeti koje mozemo da razrijesimo
          SymbolTableElement* s = find(f->getSymbolTable()[e->getSymbolInd()]->getName());
          if(s->isAbs() && e->getType()==RelocTableElement::ABS){//eventualno da srijedimo relokacije za ABS simbole
           std::map<std::string,FileInfo::SectionInfo>::iterator iter = newsectionInfos.find(f->getName(i));
          unsigned val = s->getVal() + e->getAddend(),addr = e->getOffset() + f->getOffset(i);
            for(int j=0;j<4;j++){
              iter->second.data[j + addr] = val & 0xFF;
              val>>=8;
            }
          }else{
            newsectionInfos[f->getSymbolTable()[i]->getName()].relocTable.push_back(new RelocTableElement(
              e->getOffset() + f->getOffset(i),e->getType(),s->getNum(),e->getAddend()
              + (s->getType()==SymbolTableElement::SCTN ?  f->getOffset(e->getSymbolInd()) : 0)));
        }
        }
      }
    }
  }catch(LinkerException* e){
    std::cout << *e;
    exit(-1);
  }
  return 0;
}
SymbolTableElement* Linker::find(std::string name){
  for(SymbolTableElement*e : newst){
    if(e->getName()==name)return e;
  }
  return nullptr;
}
void Linker::printRes(std::ostream& os)
{
  os << "Rezultat linkovanja:\n";
  os << "Tabela simbola:\n";
  os << "Num\tValue\t Size\tType\tBind\tNdx\tName\n";
  for (SymbolTableElement* elem : newst){
      os << *elem;
  }
  os << "Informacije o sekcijama:\n";
  for(std::pair<std::string,FileInfo::SectionInfo> info : newsectionInfos){
      os << "Naziv sekcije:" << info.first << "\n";
      os << "Tabela relokacionih zapisa:\n";
      os << "Offset\t Type\tSymbol\tAddend\n";
      for(RelocTableElement* e: info.second.relocTable){
        os << *e;
      }
      os << "Sadrzaj sekcije:";
      for(unsigned i=0;i<info.second.data.size();i++){
        if(i%16==0)os << std::endl;
        else if(i !=0 && i % 4 == 0)os << "  ";
        os << std::setfill('0') << std::setw(2) <<std::hex << (int)(info.second.data[i]) << " ";
  }
  os << std::endl;
  }
  os << std::endl;
  return;
}
void Linker::createHexFile(const char *file){
  //ispitivanje da li su svi simboli definisani
  //printRes(std::cout);std::cout << std::endl;
  for(SymbolTableElement* e : newst){
    if(e->getNum()==0)continue;
    if(e->getNdx()==0 && !e->isAbs())
      throw new LinkerException(""," simbol " + e->getName() + " je nedefinisan!");
  }
  //ispitivanje zadatih duzina i sredjivanje konacnih vrijednosti simbola
  unsigned long cur = 0;
  for(std::pair<unsigned long,std::string> place : places){//trebalo bi da std::map sortira po unsigned long
    unsigned long size = newsectionInfos[place.second].data.size();
    if(cur > place.first || cur + size >=0xFFFFFF00)throw new LinkerException(""," nije moguce smjestanje sekcije " + place.second);
    cur = place.first;
    unsigned long num = find(place.second)->getNum();
    for(SymbolTableElement* e : newst){
      if(e->getNdx()==num)
      e->setVal(e->getVal() + cur);
    }
    cur+=newsectionInfos[place.second].data.size();
  }
  for(SymbolTableElement* e : newst){
    if(e->getNum()==0)continue;
    if(e->getType()!=SymbolTableElement::SCTN)break;
    unsigned long size = newsectionInfos[e->getName()].data.size();
    bool found = false;
    for(std::pair<unsigned long,std::string> place : places){
      if(place.second==e->getName()){found = true;break;}
    }
    if(found)continue;
    if(cur + size >=0xFFFFFF00)throw new LinkerException(""," nije moguce smjestanje sekcije " + e->getName());
    unsigned long num = e->getNum();
    for(SymbolTableElement* e : newst){
      if(e->getNdx()==num)
      e->setVal(e->getVal() + cur);
    }
    cur+=newsectionInfos[e->getName()].data.size();
  }
  //razrjesavanje relokacionih zapisa
  for(unsigned long i = 1; i < newst.size() && newst[i]->getType()==SymbolTableElement::SCTN;i++){
    std::map<std::string,FileInfo::SectionInfo>::iterator iter = newsectionInfos.find(newst[i]->getName());
    for(unsigned long j = 0 ;j <iter->second.relocTable.size();j++){
        RelocTableElement* e = iter->second.relocTable[j];
        unsigned val = newst[e->getSymbolInd()]->getVal() + iter->second.relocTable[j]->getAddend(),addr=e->getOffset();
        switch(e->getType()){
          case RelocTableElement::ABS:{
            for(int i=0;i<4;i++){
              iter->second.data[i + addr] = val & 0xFF;
              val>>=8;
            }
          }break;
          case RelocTableElement::PCREL:{//potencijalno ne mora biti ovako
            val -= (addr + newst[i]->getVal());
            for(int i=0;i<4;i++){
              iter->second.data[i + addr] = val & 0xFF;
              val>>=8;
            }
          }break;
        }
    }
  }
  //std::cout << "Stanje prije kreiranja hex fajla:\n";
  //printRes(std::cout);
  //std::cout << "duzina: " << filelen << std::endl;
  //kreiranje .hex fajla
  createHexBin(file);
  //aj kao da ima i ona tekstualna varijanta
  createHexTxt(file);
  std::ofstream out;
  out.open(std::string(file) + ".otxt");
  printRes(out);
  out.close();
}
void Linker::createObjFile(const char *file){
  //printRes(std::cout);
  std::ofstream out;
  out.open(file, std::ios::binary);
  //zaglavlje
  unsigned long nullval = 0 ;
  unsigned long stlen = newst.size();
  unsigned long tmp = (5*sizeof(long));
  out.write(magic.c_str(),sizeof(magic.c_str()));
  out.write(reinterpret_cast<char*>(&nullval),sizeof(int));//0 - relokatibilan fajl, 1 - izvrsni fajl
  out.write(reinterpret_cast<char*>(&nullval),sizeof(int));//padding
  out.write(reinterpret_cast<char*>(&stlen),sizeof(unsigned long));//broj elemenata u tabeli simbola
  out.write(reinterpret_cast<char*>(&tmp),sizeof(long));//lokacija tabele simbola
  out.write(reinterpret_cast<char*>(&nullval),sizeof(unsigned long));//lokacija tabele stringova
  filelen+=40;
  unsigned long stloc,stringtloc,shloc;
  //upis podataka sekcija
  for(std::map<std::string,FileInfo::SectionInfo>::iterator iter = newsectionInfos.begin();iter!=newsectionInfos.end();std::advance(iter,1)){//nije prijenos po referenci pa mora ovako
    std::vector<unsigned char>& data = iter->second.data;
    if(data.size()==0)continue;
    iter->second.dataoff = filelen;
    out.write(reinterpret_cast<char*>(&data[0]),sizeof(unsigned char)*data.size());
    filelen+=data.size();
  }
  //upis relokativnih zapisa
  for(std::map<std::string,FileInfo::SectionInfo>::iterator iter = newsectionInfos.begin();iter!=newsectionInfos.end();std::advance(iter,1)){//nije prijenos po referenci pa mora ovako
    if(iter->second.relocTable.size()!=0)iter->second.relocoff=filelen;
    for(RelocTableElement *e : iter->second.relocTable){
      e->write(out);
      filelen += e->getRelocTableElemSize();
    }
  }
  shloc = filelen;
  //upis zaglavlja sekcija
  for(std::pair<std::string,FileInfo::SectionInfo> section : newsectionInfos ){
    FileInfo::SectionInfo& info = section.second;
    unsigned long tmp = 0;tmp = info.data.size();
    out.write(reinterpret_cast<char*>(&(tmp)),sizeof(unsigned));tmp = info.relocTable.size();
    out.write(reinterpret_cast<char*>(&(tmp)),sizeof(unsigned));
    out.write(reinterpret_cast<char*>(&(section.second.dataoff)),sizeof(unsigned long));
    out.write(reinterpret_cast<char*>(&(section.second.relocoff)),sizeof(unsigned long));
    find(section.first)->setSectionHeaderLoc(filelen);
    filelen+=2*(sizeof(unsigned)+sizeof(unsigned long));
  }
  stloc = filelen;stringtloc = filelen + newst.size() * SymbolTableElement::getSectionElemSize();
  //upis tabele simbola i tabele stringova
  unsigned long cur = 0;
  for(SymbolTableElement* e: newst){
    e->write(out,cur);
    cur+=e->getName().size() + 1;
  }
  for(SymbolTableElement* e: newst){
    std::string str = e->getName();
    out.write(str.c_str(),e->getName().size()*sizeof(char) + 1);
  }
  filelen+=cur;
  filelen+=newst.size()*SymbolTableElement::getSectionElemSize();
  out.close();
  //dodaj pokazivace
  out.open(file, std::ios::in | std::ios::out);
  out.seekp(24);
  out.write(reinterpret_cast<char*>(&stloc),sizeof(long));//lokacija tabele simbola
  out.write(reinterpret_cast<char*>(&stringtloc),sizeof(unsigned long));//lokacija tabele stringova
  out.close();
    //ispis tekstualnog fajla
  out.open(std::string(file) + ".otxt");
  printRes(out);
  out.close();
  std::cout << "duzina: " << filelen << std::endl;
}


void Linker::createHexBin(const char* file){
 std::ofstream out;
  out.open(file, std::ios::binary);
  //zaglavlje
  unsigned long nullval = 1 ;
  unsigned long stlen = newsectionInfos.size();
  unsigned long tmp = (4*sizeof(long));
  out.write(magic.c_str(),sizeof(magic.c_str()));
  out.write(reinterpret_cast<char*>(&nullval),sizeof(int));//0 - relokatibilan fajl, 1 - izvrsni fajl
  nullval = 0;
  out.write(reinterpret_cast<char*>(&nullval),sizeof(int));//padding
  out.write(reinterpret_cast<char*>(&stlen),sizeof(unsigned long));//broj elemenata u tabeli sekcija
  out.write(reinterpret_cast<char*>(&tmp),sizeof(long));//lokacija tabele sekcija
  //u tabeli sekcija se cuvaju pocetna adresa, duzina sekcije i lokacija fajla na kojoj je pocetak sekcije smjesten
  filelen+=32;
    //upis podataka sekcija
  for(std::map<std::string,FileInfo::SectionInfo>::iterator iter = newsectionInfos.begin();iter!=newsectionInfos.end();std::advance(iter,1)){//nije prijenos po referenci pa mora ovako
    if(iter->second.data.size()==0)continue;
    iter->second.dataoff = filelen;
    out.write(reinterpret_cast<char*>(&(iter->second.data[0])),sizeof(unsigned char)*iter->second.data.size());
    filelen+=iter->second.data.size();
  }
  //ubacivanje sekcija u tabelu sekcija
  tmp = filelen;
  for(std::pair<std::string,FileInfo::SectionInfo>section : newsectionInfos){
    unsigned size = section.second.data.size();
    unsigned long saddr = find(section.first)->getVal();
    out.write(reinterpret_cast<char*>(&size),sizeof(unsigned));
    out.write(reinterpret_cast<char*>(&saddr),sizeof(unsigned));
    out.write(reinterpret_cast<char*>(&section.second.dataoff),sizeof(unsigned long));
    filelen += 2*sizeof(unsigned) + sizeof(unsigned long);
  }
  out.close();
  //dodaj pokazivace
  out.open(file, std::ios::in | std::ios::out);
  out.seekp(24);
  out.write(reinterpret_cast<char*>(&tmp),sizeof(long));//lokacija tabele simbola
  out.close();
  std::cout << "duzina: " << filelen << std::endl;

}
void Linker::createHexTxt(const char* file){
  std::ofstream out(std::string(file) + ".hextxt");
  unsigned long cur = 0;
  unsigned i = 0;
  for(std::pair<unsigned long,std::string> place : places){//trebalo bi da std::map sortira po unsigned long
    unsigned long tmp = find(place.second)->getVal();
    if(tmp - cur - 1< 7 - i){
      for(;cur < tmp;cur++){
            out << " " << std::setfill('0')<< std::setw(2) << std::hex << 0;
            i++;if(i==8){out << std::endl;i=0;}           
      }
    }
    cur = tmp;
    for(unsigned char c : newsectionInfos[place.second].data){
      if(i==0)out << std::setfill('0')<< std::setw(8) << std::hex << cur << ":";
      out << " " << std::setfill('0')<< std::setw(2) << std::hex << (int)c;
      i++;if(i==8){out << std::endl;i=0;}
      cur++;
    }
  }
  for(SymbolTableElement* e : newst){
    if(e->getNum()==0)continue;
    if(e->getType()!=SymbolTableElement::SCTN)break;
    bool found = false;
    for(std::pair<unsigned long,std::string> place : places){
      if(place.second==e->getName()){found = true;break;}
    }
    if(found)continue;
    unsigned long tmp = e->getVal();
    if(tmp - cur - 1 < 7-i){
      for(;cur < tmp;cur++){
            out << " " << std::setfill('0')<< std::setw(2) << std::hex << 0;
            i++;if(i==8){out << std::endl;i=0;}
      }
    }
    cur = tmp;
    for(unsigned char c : newsectionInfos[e->getName()].data){
      if(i==0)out << std::setfill('0')<< std::setw(8) << std::hex << cur << ":";
      out << " " << std::setfill('0')<< std::setw(2) << std::hex << (int)c;
      i++;if(i==8){out << std::endl;i=0;}
      cur++;
    }
  }
  out.close();
}

Linker::~Linker(){
  //brisanje fajlova
  while(files.size()>0){
    FileInfo *f = files[files.size()-1];
    files.pop_back();
    delete f;
  }
  //brisanje tabele simbola
  while(newst.size()>0){
    SymbolTableElement *e = newst[newst.size()-1];
    newst.pop_back();
    delete e;
  }
  //brisanje relokacionih zapisa
    for(std::map<std::string,FileInfo::SectionInfo>::iterator iter = newsectionInfos.begin();iter!=newsectionInfos.end();std::advance(iter,1)){//nije prijenos po referenci pa mora ovako
      std::vector<RelocTableElement*> rtable = iter->second.relocTable;
      while(rtable.size()>0){
        RelocTableElement* e = rtable[rtable.size()-1];
        rtable.pop_back();
        delete e;
      }
  }
}