#ifndef _SYMBOL_TABLE_ELEMENT_HPP
#define _SYMBOL_TABLE_ELEMENT_HPP
#include <iostream>
#include <list>
#include <ostream>
#include <iomanip>
class SymbolTableElement{
  public:
  enum Type{
    NOTYP=0,SCTN
  };
  enum Bind{
    LOC=0,GLOB
  };
  struct FlinkElement{
    int section;
    unsigned size;
    int opCode;
    std::string str;
    unsigned long pos;
    FlinkElement(int section,unsigned size,int opcode,std::string str,unsigned long pos) :
     section(section),size(size),opCode(opcode),str(str),pos(pos){}
  };
  private:
   unsigned long num;
   unsigned value;
   unsigned size;
   unsigned sectionHeaderLoc=0;//samo ako je simbol sekcija
   Type type;
   Bind bind;
   static const unsigned long SECTIONELEMSIZE = 3 * sizeof(unsigned)  + 2*sizeof(unsigned long); 
   unsigned long Ndx;//0-nedefinisan
   std::string name;
   bool abs;
   bool defined;//da li je definisan u kodu
   
   std::list<FlinkElement> forwardRefs;//int-indeks sekcije,unsigned long pomjeraj 
   public:
    SymbolTableElement(unsigned long num=0,unsigned value=0,unsigned size=0,Type type=NOTYP,Bind bind=LOC,unsigned long Ndx=0,std::string name="",bool isAbs=false,bool defined=false){
      this->num=num;
      this->value=value;
      this->size=size;
      this->type=type;
      this->bind=bind;
      this->Ndx=Ndx;
      this->name=name;
      this->abs=isAbs;
      this->defined=defined;
    }
    void setSectionHeaderLoc(unsigned x){
      this->sectionHeaderLoc=x;
      }
    static unsigned long getSectionElemSize(){return SECTIONELEMSIZE;}
    SymbolTableElement (const SymbolTableElement&) = delete;
    SymbolTableElement (SymbolTableElement&&) = delete;
    SymbolTableElement& operator=(const SymbolTableElement&) = delete;
    SymbolTableElement& operator=(SymbolTableElement&&) = delete;
    unsigned long getNum() const{return num;}
    unsigned  getVal() const{return value;}
    unsigned getSize() const{return size;}
    Type getType() const{return type;}
    Bind getBind() const{return bind;}
    void incLit() {value+=8;}
    unsigned long getNdx() const{return Ndx;}
    std::string getName() const{return name;}
    bool isAbs() const{return abs;}
    bool isDefined() const{return defined;}
    bool allRefsCleared() const{return forwardRefs.size()==0;}
    FlinkElement getRef(){FlinkElement x = forwardRefs.front(); forwardRefs.pop_front();return x;}
    //void putRef(int i,unsigned long x){forwardRefs.push_back(std::pair<int,unsigned long>(i,x));}
    void setNum(unsigned long num){this->num=num;}
    void setVal(unsigned long val){this->value=val;}
    void setSize(unsigned long size){this->size=size;}
    void setType(Type type){this->type=type;}
    void setBind(Bind bind){this->bind=bind;}
    void setNdx(unsigned long ndx){this->Ndx = ndx;}
    void setAbs(bool abs){this->abs=abs;}
    void setDefined(bool defined){this->defined = defined;}
    void setName(std::string name){this->name=name;}
    void insertForwardRef(int i,unsigned j,int opcode,std::string str,unsigned long pos){
      forwardRefs.push_back(FlinkElement(i,j,opcode,str,pos));
    }
    friend std::ostream& operator<< (std::ostream& os,const SymbolTableElement& s);
    void write(std::ostream& os,unsigned long strloc);
    static SymbolTableElement* getFromFile(std::istream& is,unsigned long sthloc,unsigned long strloc,unsigned long num);
    unsigned long getSectionHeaderLoc() const{return sectionHeaderLoc;}
    bool isSymbolExternUndefined(){return (!abs && Ndx==0);}
};
#endif