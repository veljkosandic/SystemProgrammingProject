#include "../../inc/asm/Expression.hpp"
#include "../../inc/asm/Assembly.hpp"
#include <stack>
extern unsigned long line_num;
std::string Expression::strops[] = {"&&","||","==","!=","<=","<",">=",">","+","-","|","&","^","!","*","/","%",">>","<<","~","#","(",")"};
 Expression::Operation Expression::StringToType(std::string s)
{
  for(int i=0;i<=(int)END;i++){
    if(s==strops[i])return (Operation)i;
  }
  return NOP;
}
Expression::Expression(SymbolTable* table,std::string expr,std::map<Operation,operationElement>& map,unsigned lc):table(table),opTable(map),res(0,true,false),lc(lc){
    //konverzija infiksnog u postfiksni izraz
    std::stack<ExpressionElement> S;
    int rank = 0;
    while(expr.length()>0){
      std::string x = Assembly::Instance()->extractVar(expr," \t");
      Operation op = StringToType(x);
      if(op == NOP){
        rank++;
        if(x == "."){
            //std::cout << lc << std::endl;
            ExpressionElement e(lc,true,false);
            e.dependencies[Assembly::Instance()->getCurSection()->getId()]=1;
            postfix.push_back(e);
        }
        else
        if(Assembly::Instance()->isALiterral(x)){
            postfix.push_back(ExpressionElement(Assembly::Instance()->getLiterral(x),true,false));
        }else{
            SymbolTableElement* e = table->find(x);
            if(e!=nullptr){
              postfix.push_back(ExpressionElement(e->getNum(),false,false));
              if(!e->isDefined())missing.push_back(e->getNum());
            }else{
              e =new SymbolTableElement(table->getLen(),0,0,SymbolTableElement::Type::NOTYP,SymbolTableElement::Bind::LOC,0,x,false,false);
              table->add(e);
              postfix.push_back(ExpressionElement(e->getNum(),false,false));
              missing.push_back(e->getNum());
            }
        }
      }else{
        while(!S.empty() && opTable[op].ipr<=opTable[(Operation)S.top().value].spr){
          ExpressionElement tmp = S.top();S.pop();
          postfix.push_back(tmp);
          rank = rank + opTable[(Operation)tmp.value].r;
          if(rank<1)throw new AssemblyException("Nepravilan izraz - greska pri konverziji u postfiksnu notaciju",false,lc);
        }
        if(op != END)
          S.push(ExpressionElement((unsigned)op,false,true));
        else S.pop();
      }
    }
    while(!S.empty())
    {
        ExpressionElement tmp = S.top();S.pop();
        postfix.push_back(tmp);
        rank = rank + opTable[(Operation)tmp.value].r;
    }
    if(rank!=1)throw new AssemblyException("Nepravilan izraz - greska pri konverziji u postfiksnu notaciju",false,lc);
    trycalc();
  }
  void Expression::in2post(){

  }
  void Expression::solveSymbol(ExpressionElement& operand){
     SymbolTableElement* e = table->get(operand.value);
     if(!e->isAbs())
     if(e->getBind()!=SymbolTableElement::GLOB || (e->getBind()==SymbolTableElement::GLOB && !e->isSymbolExternUndefined()))
        operand.dependencies[e->getNdx()]=1;
        else operand.dependencies[e->getNum()]=1;
              operand.abs = true;
              operand.value = e->getVal();
  }
  void Expression::trycalc(){
    if(missing.size()!=0)return;
    std::stack<ExpressionElement> S;
    for(ExpressionElement& e : postfix){
      if(!e.op){if(!e.abs)solveSymbol(e);S.push(e);}
      else{
        if(opTable[(Operation)e.value].r==0){
            ExpressionElement operand = S.top();S.pop();
            //if(!operand.abs)solveSymbol(operand);
            if(!operand.abs || !operand.dependencies.empty())throw new AssemblyException("Nije dozvoljeno koriscenje unarnih operatora za simbole!",false,lc);
            switch((Operation)e.value){
              case NEG:{
                operand.value = - operand.value;
              }break;
              case NOTOP:{
                operand.value = ~ (int)operand.value;
              }break;
            }
            S.push(operand);
        }else{
            ExpressionElement operand2 = S.top();S.pop();
            ExpressionElement operand1 = S.top();S.pop();
            //if(!operand1.abs)solveSymbol(operand1);
            //if(!operand2.abs)solveSymbol(operand2);
            ExpressionElement operand(0,true,false);
            operand.dependencies = operand1.dependencies;
            //u sustini, +,- i poredjenja mogu da rade sa labelama
            if((!operand1.dependencies.empty() || !operand2.dependencies.empty()) && ((Operation)e.value <= LOGICAL_OR
             ||(Operation)e.value >=OROP) )
             throw new AssemblyException("Greska pri racunanju izraza : zavisni izrazi se koriste u operaciji " + strops[e.value],false,lc);
            switch((Operation)e.value){
              case LOGICAL_AND:{
                operand.value = operand1.value && operand2.value;
              }break;
              case LOGICAL_OR:{
                operand.value = operand1.value || operand2.value;
              }break;
              case EQ:{
                operand.value = operand1.value - operand2.value;
               for(std::pair<int,unsigned long> x : operand2.dependencies){
                  operand.dependencies[x.first]-=x.second;
                  if(operand.dependencies[x.first]==0)operand.dependencies.erase(x.first);
                }
                if(operand.dependencies.empty() && operand.value==0)operand.value=-1;
                else operand.value=0;
                operand.dependencies.clear();
              }break;
              case NEQ:{
                  operand.value = operand1.value - operand2.value;
               for(std::pair<int,unsigned long> x : operand2.dependencies){
                  operand.dependencies[x.first]-=x.second;
                  if(operand.dependencies[x.first]==0)operand.dependencies.erase(x.first);
                }
                if(operand.dependencies.empty() && operand.value==0)operand.value=0;
                else operand.value=-1;
                operand.dependencies.clear();
              }break;
              case LESSEQ:{
                operand.value = operand1.value - operand2.value;
               for(std::pair<int,unsigned long> x : operand2.dependencies){
                  operand.dependencies[x.first]-=x.second;
                  if(operand.dependencies[x.first]==0)operand.dependencies.erase(x.first);
                }
                if(!operand.dependencies.empty())throw new AssemblyException("Greska pri racunanju izraza : zavisni izrazi se koriste u operaciji " + strops[e.value],false,lc);
                else operand.value= ((int)operand.value <= 0 ? -1 : 0);
              }break;
              case LESS:{
                operand.value = operand1.value - operand2.value;
               for(std::pair<int,unsigned long> x : operand2.dependencies){
                  operand.dependencies[x.first]-=x.second;
                  if(operand.dependencies[x.first]==0)operand.dependencies.erase(x.first);
                }
                if(!operand.dependencies.empty())throw new AssemblyException("Greska pri racunanju izraza : zavisni izrazi se koriste u operaciji " + strops[e.value],false,lc);
                else operand.value= ((int)operand.value < 0 ? -1 : 0);
              }break;
              case GREATEQ:{
                operand.value = operand1.value - operand2.value;
               for(std::pair<int,unsigned long> x : operand2.dependencies){
                  operand.dependencies[x.first]-=x.second;
                  if(operand.dependencies[x.first]==0)operand.dependencies.erase(x.first);
                }
                if(!operand.dependencies.empty())throw new AssemblyException("Greska pri racunanju izraza : zavisni izrazi se koriste u operaciji " + strops[e.value],false,lc);
                else operand.value= ((int)operand.value >= 0 ? -1 : 0);
              }break;
              case GREAT:{
                operand.value = operand1.value - operand2.value;
               for(std::pair<int,unsigned long> x : operand2.dependencies){
                  operand.dependencies[x.first]-=x.second;
                  if(operand.dependencies[x.first]==0)operand.dependencies.erase(x.first);
                }
                if(!operand.dependencies.empty())throw new AssemblyException("Greska pri racunanju izraza : zavisni izrazi se koriste u operaciji " + strops[e.value],false,lc);
                else operand.value= ((int)operand.value > 0 ? -1 : 0);
              }break;
              case ADDOP:{
                operand.value = operand1.value + operand2.value;
                operand.dependencies = operand1.dependencies;
                for(std::pair<int,unsigned long> x : operand2.dependencies){
                  operand.dependencies[x.first]+=x.second;
                }
              }break;
              case SUBOP:{
                operand.value = operand1.value - operand2.value;
               for(std::pair<int,unsigned long> x : operand2.dependencies){
                  operand.dependencies[x.first]-=x.second;
                  if(operand.dependencies[x.first]==0)operand.dependencies.erase(x.first);
                }
              }break;
              case OROP:{
                operand.value = operand1.value | operand2.value;
              }break;
              case ANDOP:{
                operand.value = operand1.value & operand2.value;
              }break;
              case XOROP:{
                operand.value = operand1.value ^ operand2.value;
              }break;
              case NOTOR:{
                operand.value = operand1.value |  ~(operand2.value);
              }break;
              case MULOP:{
                operand.value = operand1.value * operand2.value;
              }break;
              case DIVOP:{
                operand.value = operand1.value / operand2.value;
              }break;
              case MOD:{
                operand.value = operand1.value % operand2.value;
              }break;
              case SHROP:{
                operand.value = operand1.value >> operand2.value;
              }break;
              case SHLOP:{
                operand.value = operand1.value << operand2.value;
              }break;
            }
            S.push(operand);
        }
      }
    }
    ExpressionElement rez = S.top();S.pop();
    if(S.empty()){
      this->res = rez;
      solved= true;
    }else throw new AssemblyException("Nepravilan izraz - greska pri racunanju izraza",false,lc);
  }

void Expression::setSymbol(SymbolTableElement* e){
  int ndx = 0;
  bool dependent = false;
  for(std::pair<int,unsigned long> x : res.dependencies){
      if(x.second==1 && !dependent){ndx = x.first;dependent = true;}
      else if(x.second!=1 || (x.second==1 && dependent) )
        throw new AssemblyException("Izraz nije relokatibilan",false,lc);
  }
  e->setVal(res.value);
  if(dependent){
    e->setNdx(ndx);
  }else e->setAbs(true);
}
