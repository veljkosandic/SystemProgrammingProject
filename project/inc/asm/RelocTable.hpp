#ifndef _RELOC_TABLE_HPP_
#define _RELOC_TABLE_HPP_
#include <vector>
#include "../common/RelocTableElement.hpp"
#include <ostream>
class RelocTable{
    private:
    std::vector<RelocTableElement*> elems;
  public:
    RelocTable(){}
    RelocTable (const RelocTable&) = delete;
    RelocTable (RelocTable&&) = delete;
    RelocTable& operator=(const RelocTable&) = delete;
    RelocTable& operator=(RelocTable&&) = delete;
    void add(RelocTableElement* elem){elems.push_back(elem);}
    RelocTableElement* get(unsigned long i) const{return elems.at(i);}
    unsigned long getLen() const{return elems.size();}
    friend std::ostream& operator<< (std::ostream& os,const RelocTable& s);
    ~RelocTable(){
      while(elems.size()>0){
        RelocTableElement* tmp = elems.back();
        delete tmp;
        elems.pop_back();
      }
    }
    void write(std::ostream& os);
    void incLit(unsigned saddr);
};


#endif