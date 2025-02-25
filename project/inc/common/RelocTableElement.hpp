#include <ostream>
#include <iomanip>
#ifndef _RELOC_TABLE_ELEMENT_HPP_
#define _RELOC_TABLE_ELEMENT_HPP_
class RelocTableElement{
  public:
    enum Type{
      ABS,PCREL
    };
    static unsigned long getRelocTableElemSize() {return RELOCTABLEELEMSIZE;}
  private:
    static const unsigned long RELOCTABLEELEMSIZE = sizeof(Type) + 2 *sizeof(int) + sizeof(unsigned);
    unsigned offset;
    Type type;
    int symbolInd;
    int addend;
  public:
    RelocTableElement(unsigned offset,Type type,int symbolInd,int addend){
      this->offset=offset;
      this->type=type;
      this->symbolInd=symbolInd;
      this->addend=addend;
    }
    RelocTableElement (const RelocTableElement&) = delete;
    RelocTableElement (RelocTableElement&&) = delete;
    RelocTableElement& operator=(const RelocTableElement&) = delete;
    RelocTableElement& operator=(RelocTableElement&&) = delete;
    void incLit(){offset+=8;/*if(type==PCREL)addend+=8;*/}//da li je globalna relokacija
    unsigned  getOffset() const{return offset;}
    Type getType() const{return type;}
    int getSymbolInd() const{return symbolInd;}
    int getAddend()const {return addend;}
    friend std::ostream& operator<< (std::ostream& os,const RelocTableElement& s){
      os << std::setfill('0')<< std::setw(8) << std::hex << s.offset << " " << ((s.type==ABS) ? "ABS" : "PCREL")
      << "\t" << s.symbolInd << "\t" << s.addend << "\n";
      return os;
    }
  static RelocTableElement* getFromFile(std::istream& is,unsigned long rteloc){
  RelocTableElement* e = new RelocTableElement(0,(Type)0,0,0);
  is.seekg(rteloc,is.beg);
  is.read(reinterpret_cast<char*>(&(e->offset)),sizeof(unsigned));
  is.read(reinterpret_cast<char*>(&(e->type)),sizeof(RelocTableElement::Type));
  is.read(reinterpret_cast<char*>(&(e->symbolInd)),sizeof(unsigned));
  is.read(reinterpret_cast<char*>(&(e->addend)),sizeof(int));
  return e;
}
void write(std::ostream &os){
  unsigned int tmp;tmp = offset;
    os.write(reinterpret_cast<char*>(&tmp),sizeof(unsigned));
    tmp = type;
    os.write(reinterpret_cast<char*>(&tmp),sizeof(RelocTableElement::Type));
    tmp = symbolInd;
    os.write(reinterpret_cast<char*>(&tmp),sizeof(unsigned));
    tmp = addend;
    os.write(reinterpret_cast<char*>(&tmp),sizeof(int));
}
};


#endif