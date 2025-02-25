#include "../../inc/asm/RelocTable.hpp"
#include "../../inc/asm/Assembly.hpp"
std::ostream &operator<<(std::ostream &os, const RelocTable &s)
{
    os << "Offset\t Type\tSymbol\tAddend\n";
  for (RelocTableElement* elem : s.elems){
      os << *elem;
  }
  return os;
}

void RelocTable::incLit(unsigned saddr){
  for(RelocTableElement* e : elems){
    if(e->getOffset() >= saddr && Assembly::Instance()->getSymbolTable()->get(e->getSymbolInd())->getBind()!=SymbolTableElement::GLOB)
      e->incLit();
  }
}
void RelocTable::write(std::ostream & os)
{
  for(RelocTableElement* elem : elems){
    elem->write(os);
  }
  Assembly::Instance()->addFileLen(RelocTableElement::getRelocTableElemSize()*elems.size());
}