#include "../../inc/asm/LiterralTable.hpp"

LiterralTable::~LiterralTable()
{
  while (elems.size() > 0)
  {
    LiterralTableElement *tmp = elems.back();
    delete tmp;
    elems.pop_back();
  }
}
std::ostream &operator<<(std::ostream &os, const LiterralTable &s)
{
  os << "Value\tAddress\t TrueLit NeedAddr\n";
  for (LiterralTableElement *e : s.elems)
    os << *e << std::endl;
  return os;
}
LiterralTableElement *LiterralTable::find(unsigned long ind, bool trueLiterral,unsigned long& from)
{
  //std::cout << "elem";
  for (unsigned long i = from;i<elems.size();i++)
  {
    if (elems[i]->getValue() == ind && elems[i]->isTrueLiterral() == trueLiterral && elems[i]->isDefined())
      return elems[i];
  }
  return nullptr;
}

void LiterralTable::litInc(unsigned addr){
  for(LiterralTableElement* e : elems){
    if(e->getNeeds()[0] >= addr)
    e->litInc();
  }
}