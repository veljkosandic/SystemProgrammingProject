#ifndef _LITERRAL_TABLE_ELEMENT_HPP_
#define _LITERRAL_TABLE_ELEMENT_HPP_
#include <vector>
#include <iostream>
class LiterralTableElement{
  private:
  unsigned long literralAdress=0;//adresa na koju cemo smjestiti literal
  int literralSection=0;//sekcija koja trazi zadati literal(za vise sekcija a isti literal imacemo vise ulaza)
  unsigned long value;//ili redni broj simbola ili vrijednost
  bool trueLiterral;
  bool defined = false;
  std::vector<unsigned long> needLocations;//moraju biti u istoj sekciji
  public:
  LiterralTableElement(int sec,unsigned long val,bool isTrueLiterral):literralSection(sec),value(val),trueLiterral(isTrueLiterral){};
  LiterralTableElement (const LiterralTableElement&) = delete;
  LiterralTableElement (LiterralTableElement&&) = delete;
  LiterralTableElement& operator=(const LiterralTableElement&) = delete;
  LiterralTableElement& operator=(LiterralTableElement&&) = delete;
  unsigned long getLiterralAdress() const{return literralAdress;}
  int getLiterralSection() const{return literralSection;}
  bool isTrueLiterral() const{return trueLiterral;}
  bool isDefined() const{return defined;}
  unsigned long getValue() const{return value;}
  void litInc(){needLocations[0]+=8;}
  void define(unsigned value){defined = true;literralAdress = value;}
  std::vector<unsigned long>& getNeeds(){return needLocations;}
  friend std::ostream& operator<< (std::ostream& os,const LiterralTableElement& s);
};
#endif