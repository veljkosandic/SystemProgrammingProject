#ifndef _SYMBOL_TABLE_HPP_
#define _SYMBOL_TABLE_HPP_
#include "../common/SymbolTableElement.hpp"
#include <vector>
#include <iostream>
class SymbolTable{
  private:
    std::vector<SymbolTableElement*> elems;
  public:
    SymbolTable(){}
    SymbolTable (const SymbolTable&) = delete;
    SymbolTable (SymbolTable&&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;
    SymbolTable& operator=(SymbolTable&&) = delete;
    void add(SymbolTableElement* elem){elems.push_back(elem);}
    SymbolTableElement* get(unsigned long i) const{return elems.at(i);}
    unsigned long getLen() const{return elems.size();}
    friend std::ostream& operator<< (std::ostream& os,const SymbolTable& s);
    SymbolTableElement* find(std::string name);
    void incLit(unsigned saddr,int ndx);
    void writeSymbolTable(std::ostream& os);
    std::string getUndefinedNames();
    ~SymbolTable();
};

#endif