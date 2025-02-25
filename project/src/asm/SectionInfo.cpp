#include "../../inc/asm/SectionInfo.hpp"
#include "../../inc/asm/Assembly.hpp"
#include <list>
#include <algorithm>

std::ostream &operator<<(std::ostream &os, const SectionInfo &s)
{
  os << s.sectionName << ":" << std::endl;
  os << "Tabela relokacionih zapisa:\n";
  os << *(s.reloctable);
  os << "\nTabela literala:\n";
  os << *(s.littable);
  os <<"\nSadrzaj sekcije:";
  for(int i=0;i<s.data->size();i++){
    if(i%16==0)os << std::endl;
    else if(i !=0 && i % 4 == 0)os << "  ";
    os << std::setfill('0') << std::setw(2) <<std::hex << (int)((s.data)->operator[](i)) << " ";
  }
  os << "\n";
  return os;
}
void SectionInfo::resolveLiterralTable(){
  unsigned sectionSize = this->data->size();
  littable->sort();
  LiterralTable* newtable = new LiterralTable();
  for(unsigned long i = 0;i<littable->getLen();i++){
    LiterralTableElement* elem = littable->get(i);
    unsigned next = sectionSize,needAddr = elem->getNeeds()[0];
    //ako je literal vec definisan, da ga ne mucim
    LiterralTableElement* tmp = nullptr;
    unsigned long ind = 0;
    while(true){
      tmp = newtable->find(elem->getValue(),elem->isTrueLiterral(),ind);
      if(tmp == nullptr)break;
      unsigned litAddr = tmp->getLiterralAdress();
      if((int)(litAddr - needAddr - 4 ) <= MAX_OFFSET || (int)(litAddr - needAddr - 4) >= -(MAX_OFFSET + 1)){
        unsigned offs =litAddr - (needAddr + 4);
        insertIntoData(offs,needAddr);tmp->getNeeds().push_back(needAddr);
        break;
      }
      ind++;
    }
    if(tmp!=nullptr)continue;
    //ako nije lagano i moram da ga mecem usred sekcije - umetnem jmp pa onda literal
    if(next - (needAddr+4)>=MAX_OFFSET){
    /*next = needAddr + 4;//najjednostavnije
    Instruction instr = Instruction(0x3,0x0,0xF,0x0,0x0,0x4);
    for(int i=0;i<8;i++)data->insert(data->begin()+next,0);
    instr.insertIntoSection(this,next);
    //de lako matori - treba sve da se dosta stvari pomjeri za 8
    //tabela simbola
    Assembly::Instance()->getSymbolTable()->incLit(next,sectionId);
    //tabela literala
    littable->litInc(next);
    //relokacioni zapisi
    this->reloctable->incLit(next);
    //skokovi

    //au brt pa i word - to je ok relokacioni zapis ce to da sredi - paziti na globalnu realokaciju
    next+=4;
    sectionSize+=4;*/
    //exit(-1);
    throw new AssemblyException("Nemoguce je kreirati tabelu literala!");
    }
    //ako je lagano i mogu da ga stavim na kraj sekcije - svakako mecem i kad nije lagano
      insertIntoData(next-(needAddr+4),needAddr);
      LiterralTableElement*e = new LiterralTableElement(elem->getLiterralSection(),elem->getValue(),elem->isTrueLiterral());
      e->define(next);e->getNeeds().push_back(needAddr);
      unsigned val = 0;
      if(!elem->isTrueLiterral()){
        SymbolTableElement* ste = Assembly::Instance()->getSymbolTable()->get(elem->getValue());
        if(ste->getBind()==SymbolTableElement::GLOB /*&& !(ste->getBind()!=0 && Assembly::Instance()->getSymbolTable()->get(ste->getNdx())->isSymbolExternUndefined())*/)
          reloctable->add(new RelocTableElement(next,RelocTableElement::ABS,elem->getValue(),0));
          else reloctable->add(new RelocTableElement(next,RelocTableElement::ABS,ste->getNdx(),ste->getVal()));
      }else {e->define(next);val = elem->getValue();}
      for(int i=0;i<4;i++){
        if(next + i == data->size()){
        data->push_back(val & 0xFF);
        }
        else{
          data->operator[](next + i) = val & 0xFF;
        }
        val>>=8;
      }
      sectionSize+=4;
      newtable->add(e);
  }
  delete littable;
  littable = newtable;
}

void SectionInfo::writeRelocTable(std::ofstream &os)
{
  if(reloctable->getLen()==0)return;
  relocoff=Assembly::Instance()->getFileLen();reloctable->write(os);
}

void SectionInfo::writeSectionHeaderInfo(std::ostream &os)
{
  unsigned len = data->size();
  os.write(reinterpret_cast<char*>(&(len)),sizeof(unsigned));len = reloctable->getLen();
  os.write(reinterpret_cast<char*>(&(len)),sizeof(unsigned));
  os.write(reinterpret_cast<char*>(&(this->dataoff)),sizeof(unsigned long));
  os.write(reinterpret_cast<char*>(&(this->relocoff)),sizeof(unsigned long));
  Assembly::Instance()->getSymbolTable()->get(sectionId)->setSectionHeaderLoc(Assembly::Instance()->getFileLen());
  Assembly::Instance()->addFileLen(SECTIONHEADERSIZE);
}
void SectionInfo::insertIntoData(unsigned val, unsigned saddr)
{
  data->operator[](saddr) = ((val>>4)&0xFF);
  data->operator[](saddr+1) |= (val&0xF);
}

void SectionInfo::writeData(std::ofstream &os)
{
  if(data->size()==0)return;
  dataoff = Assembly::Instance()->getFileLen();
  os.write(reinterpret_cast<char*>(&data->operator[](0)),sizeof(unsigned char)*data->size());
  Assembly::Instance()->addFileLen(data->size());
}