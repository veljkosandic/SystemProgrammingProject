#include "../../inc/asm/SymbolTable.hpp"
#include "../../inc/asm/Assembly.hpp"
SymbolTableElement *SymbolTable::find(std::string name)
{
  for(int i = 0;i<this->elems.size();i++){
    if(elems.at(i)->getName()==name)return elems.at(i);
  }
  return nullptr;
}
std::ostream& operator<< (std::ostream& os,const SymbolTable& s){
  os << "Num\tValue\t Size\tType\tBind\tNdx\tName\tFLINK(section,address)\n";
  for (SymbolTableElement* elem : s.elems){
      os << *elem;
  }
  return os;
}

void SymbolTable::incLit(unsigned saddr,int ndx){
  for(SymbolTableElement* e : elems){
    if(e->getVal() >= saddr && e->getNdx()==ndx)
      e->incLit();

  }
}
SymbolTable::~SymbolTable(){
      while(elems.size()>0){
        SymbolTableElement* tmp = elems.back();
        delete tmp;
        elems.pop_back();
      }
    }
void SymbolTable::writeSymbolTable(std::ostream &os)
{
  unsigned long cur = 0,cnt=0;
  /*for(std::vector<SymbolTableElement*>::iterator iter = elems.begin();iter != elems.end();std::advance(iter,1)){
    if((*iter)->getNdx()!=0 && this->elems[(*iter)->getNdx()]->isSymbolExternUndefined()){
      std::cout << "*UPOZORENJE* simbol "+(*iter)->getName() + " zavisi od extern simbola pa kao takav nece moci da se dalje linkuje!" << std::endl;
      std::vector<SymbolTableElement*>::iterator old = iter;
      std::advance(iter,1);
      elems.erase(old);
    }
  }*/
  for(SymbolTableElement* e : elems){
    e->write(os,cur);
    cur += e->getName().size() + 1;
  }

 for(SymbolTableElement* e : elems){
    std::string str = e->getName();
    os.write(str.c_str(),e->getName().size()*sizeof(char) + 1);
  }
  Assembly::Instance()->addFileLen(cur);
  Assembly::Instance()->addFileLen(SymbolTableElement::getSectionElemSize()*elems.size());
}    

std::string SymbolTable::getUndefinedNames(){
  std::string str = "\n";
  for(SymbolTableElement* e: elems){
    if(!e->isDefined())str = str + e->getName() + "\n";
  }
  return str;
}