#ifndef _EXPRESSION_HPP_
#define _EXPRESSION_HPP_
#include <vector>
#include <string>
#include <map>
class SymbolTableElement;
class SymbolTable;
class Expression{//tenk ju mr. Milo Tomasevic, veri kul
  public:
    struct operationElement{
    public:
    int ipr;
    int spr;
    int r;
    operationElement(int ipr,int spr,int r):ipr(ipr),spr(spr),r(r){};
    operationElement():operationElement(0,0,0){};
  };
  enum Operation{
  LOGICAL_AND=0,LOGICAL_OR,EQ,NEQ,LESSEQ,LESS,GREATEQ,GREAT,ADDOP,SUBOP,OROP,ANDOP,XOROP,NOTOR,MULOP,DIVOP,MOD,SHROP,SHLOP,NOTOP,NEG,START,END,NOP
  }; 
  private:
  unsigned long lc;
  static std::string strops[];
  SymbolTable* table;
  struct ExpressionElement{
  public:
    unsigned value;//abs=true->vrednost literala,abs=false-indeks u tabeli literala,op - operacija (konvertovati u operation)
    bool abs;
    bool op;
    std::map<int,unsigned long> dependencies; //od kojih sekcija zavisi izraz
    ExpressionElement(unsigned value,bool abs,bool op):value(value),abs(abs),op(op){};
    ExpressionElement():ExpressionElement(0,0,0){};
  };
  bool solved = false;
  std::vector<ExpressionElement> postfix;
  std::vector<unsigned > missing;
  std::map<Operation,operationElement> opTable;
  void in2post();
  Operation StringToType(std::string s);
  ExpressionElement res;
  void solveSymbol(ExpressionElement& e);
  public:
    void trycalc();
  Expression(SymbolTable* table,std::string expr,std::map<Operation,operationElement>& map,unsigned lc);
  bool isSolved() const{return solved;}
  std::vector<unsigned>& missingSyms(){return missing;}
    void setSymbol(SymbolTableElement* e);
};

#endif