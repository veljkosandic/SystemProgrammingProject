#include "../../inc/common/SymbolTableElement.hpp"
#include <ios>
std::ostream& operator<<(std::ostream& os,const SymbolTableElement& s){
  {
      os << s.num << "\t" 
      << std::setfill('0')<< std::setw(8) << std::hex << s.value << " "
       <<std::hex << s.size << "\t" << ((s.type==SymbolTableElement::NOTYP) ? "NOTYP" : "SCTN") << 
      "\t" << ((s.bind == SymbolTableElement::GLOB) ? "GLOB" : "LOC") << "\t" << (!s.abs ? (s.Ndx==0 ? "*UND*" : std::to_string(s.Ndx)) : "*ABS*") << "\t" << s.name << "\t";
      for(SymbolTableElement::FlinkElement flink : s.forwardRefs){
        os << "(" << flink.section <<  "," << std::setfill('0')<< std::setw(8) << std::hex << flink.size <<  ")";
      }
      os << std::endl;
      return os;
    }
}
void SymbolTableElement::write(std::ostream& os,unsigned long strloc){
      unsigned tmp = type | bind << 8 | (abs && Ndx==0) << 16 | abs << 24;
      unsigned long x = type==SCTN ? sectionHeaderLoc : Ndx;
      os.write(reinterpret_cast<char*>(&value),sizeof(unsigned));
      os.write(reinterpret_cast<char*>(&size),sizeof(unsigned));
      os.write(reinterpret_cast<char*>(&tmp),sizeof(unsigned));
      os.write(reinterpret_cast<char*>(&x),sizeof(unsigned long));
      os.write(reinterpret_cast<char*>(&strloc),sizeof(unsigned long));
    }

SymbolTableElement* SymbolTableElement::getFromFile(std::istream& is,unsigned long sthloc,unsigned long stringloc,unsigned long num){
  SymbolTableElement* e = new SymbolTableElement();
  is.seekg(sthloc,is.beg);
  is.read(reinterpret_cast<char*>(&(e->value)),sizeof(unsigned));
  is.read(reinterpret_cast<char*>(&(e->size)),sizeof(unsigned));
  unsigned tmp = 0;
  is.read(reinterpret_cast<char*>(&(tmp)),sizeof(unsigned));
  e->type = (Type)(tmp & 0xFF);
  e->bind = (Bind)((tmp>>8) & 0xFF);
  e->abs = (tmp >> 24) & 0xFF;
  e->defined = true;
  e->num=num;
  unsigned long x = 0;
  is.read(reinterpret_cast<char*>(&(x)),sizeof(unsigned long));
  if(e->type == SCTN){
    e->Ndx = num;
    e->sectionHeaderLoc = x;
  }else{
    e->Ndx = x;
  }
  is.read(reinterpret_cast<char*>(&(x)),sizeof(unsigned long));
  is.seekg(stringloc + x,is.beg);
  while(true){
    char c = is.get();
    if(c=='\0')break;
    e->name+=c;
  }
  return e;
}