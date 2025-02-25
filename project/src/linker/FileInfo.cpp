#include "../../inc/linker/FileInfo.hpp"
#include "../../inc/linker/Linker.hpp"
#include <fstream>
#include <iostream>
FileInfo::FileInfo(std::string filename){
  this->filename = filename;
  std::fstream in(filename.c_str(),std::ios::binary| std::ios::in | std::ios::out);
  char magic[8];
  in.read(magic,8*sizeof(char));
  if(std::string(magic)!="SSPROJF")
    throw new LinkerException(this->filename,"zadati fajl nije dobrog binarnog formata!");
  int tmp;
  in.read(reinterpret_cast<char*>(&tmp),sizeof(int));
  if(tmp==1)
    throw new LinkerException(this->filename,"unesen je izvsni fajl");
  in.read(reinterpret_cast<char*>(&tmp),sizeof(int));
  unsigned long stlen = 0,stloc=0,stringtloc=0;
  in.read(reinterpret_cast<char*>(&stlen),sizeof(unsigned long)); 
  in.read(reinterpret_cast<char*>(&stloc),sizeof(unsigned long));
  in.read(reinterpret_cast<char*>(&stringtloc),sizeof(unsigned long));
  //in.seekg(stloc);
  for(unsigned long i = 0;i<stlen;i++){
    SymbolTableElement *e =SymbolTableElement::getFromFile(in,stloc,stringtloc,i);
    stloc+= SymbolTableElement::getSectionElemSize();
    if(e->getType()==SymbolTableElement::SCTN){
        unsigned datasize,relcnt;
        unsigned long dataoff,relocoff;
        in.seekg(e->getSectionHeaderLoc(),in.beg);
        in.read(reinterpret_cast<char*>(&(datasize)),sizeof(unsigned));
        in.read(reinterpret_cast<char*>(&(relcnt)),sizeof(unsigned));
        in.read(reinterpret_cast<char*>(&(dataoff)),sizeof(unsigned long));
        in.read(reinterpret_cast<char*>(&(relocoff)),sizeof(unsigned long));
        in.seekg(dataoff,in.beg);
        for(unsigned i=0;i<datasize;i++)
          sectionInfo[e->getNum()].data.push_back(in.get());
        in.seekg(dataoff,in.beg);
        for(unsigned i=0;i<relcnt;i++){
          sectionInfo[e->getNum()].relocTable.push_back(RelocTableElement::getFromFile(in,relocoff));
          relocoff+=RelocTableElement::getRelocTableElemSize();
        }
        sectionInfo[e->getNum()].offset=0;
    }
    st.push_back(e);
  }
  in.close();
}

std::ostream &operator<<(std::ostream &os, const FileInfo &f)
{
  os << "Fajl: " << f.filename << std::endl;
  os << "Tabela simbola:\n";
  os << "Num\tValue\t Size\tType\tBind\tNdx\tName\n";
  for (SymbolTableElement* elem : f.st){
      os << *elem;
  }
  os << "Informacije o sekcijama:\n";
  for(std::pair<unsigned long,FileInfo::SectionInfo> info : f.sectionInfo){
      os << "Naziv sekcije:" << f.st[info.first]->getName() << "\n";
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
  return os;
}
std::vector<unsigned long> FileInfo::getSectionNdxs(){
  std::vector<unsigned long> vec;
  for(std::pair<unsigned long,SectionInfo> x : sectionInfo){
    vec.push_back(x.first);
  }
  return vec;
}
FileInfo::~FileInfo(){
  //brisanje relokacionih zapisa
    for(std::map<unsigned long,FileInfo::SectionInfo>::iterator iter = sectionInfo.begin();iter!=sectionInfo.end();std::advance(iter,1)){//nije prijenos po referenci pa mora ovako
      std::vector<RelocTableElement*> rtable = iter->second.relocTable;
      while(rtable.size()>0){
        RelocTableElement* e = rtable[rtable.size()-1];
        rtable.pop_back();
        delete e;
      }
  }  
  //brisanje tabele simbola
  while(st.size()>0){
    SymbolTableElement* e = st[st.size()-1];
    st.pop_back();
    delete e;
  }
}