#ifndef _LITERRAL_TABLE_HPP_
#define _LITERRAL_TABLE_HPP_
#include "LiterralTableElement.hpp"
#include <algorithm>
class LiterralTable{
  private:
    std::vector<LiterralTableElement*> elems;
  public:
    LiterralTable(){}
    LiterralTable (const LiterralTable&) = delete;
    LiterralTable (LiterralTable&&) = delete;
    LiterralTable& operator=(const LiterralTable&) = delete;
    LiterralTable& operator=(LiterralTable&&) = delete;
    void add(LiterralTableElement* elem){elems.push_back(elem);}
    LiterralTableElement* get(unsigned long i) const{return elems.at(i);}
    unsigned long getLen() const{return elems.size();}
    friend std::ostream& operator<< (std::ostream& os,const LiterralTable& s);
    LiterralTableElement* find(unsigned long ind,bool trueLiterral,unsigned long& from);
    void sort(){std::sort(elems.begin(),elems.end(),[](LiterralTableElement* x1,LiterralTableElement* x2){
      return x1->getNeeds()[0] < x2->getNeeds()[0];
    });}
    void litInc(unsigned saddr);
    ~LiterralTable();  
};
#endif