#ifndef ASSEMBLY_HPP_
#define ASSEMBLY_HPP_
#include <set>
#include <string>
#include "SymbolTable.hpp"
#include <fstream>
#include <list>
#include <map>
#include "SectionInfo.hpp"
#include "Instruction.hpp"
#include "Expression.hpp"
class Assembly{
  private:
    static Assembly* inst;
    static const unsigned long INSTRUCTION_LEN = 4;
    const std::string magic = "SSPROJF";
    unsigned long filelen = 0;
    static std::string ops[];
    std::string curline="";
    unsigned long lc = 0;
    bool isDone = false;
    SymbolTable st;
    std::vector<SectionInfo*> sections;
    SectionInfo* curSection;
    unsigned long ln=1;
    std::vector<std::pair<int,Expression*>> TNS;//prvi - indeks u tabeli simbola, drugi - izraz koji se treba izracunati
    enum OpType{
      DIR_GLOBAL=0,DIR_EXTERN,DIR_SECTION,DIR_WORD,DIR_SKIP,DIR_ASCII,DIR_EQU,DIR_END,HALT,INT,IRET,CALL,
      RET,JMP,BEQ,BNE,BGT,PUSH,POP,XCHG,ADD,SUB,MUL,DIV,NOT,AND,OR,XOR,SHL,SHR,
      LD,ST,CSRRD,CSRWR,UNKNOWN
    };
    std::map<OpType,std::vector<Instruction>> instructionBase;
    OpType StringToType(std::string s);
    Assembly(){st.add(new SymbolTableElement(0,0,0,SymbolTableElement::Type::NOTYP,SymbolTableElement::Bind::LOC,0,"",false,true));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(HALT,{Instruction(0,0,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(INT,{Instruction(1,0,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(IRET,{Instruction(0x9,0x3,0xF,0xE,0,0x4),Instruction(0x9,0x7,0,0xE,0,0x4)}));
      //instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(IRET,{Instruction(0x9,0x2,0xF,0xE,0x0,0x0),Instruction(0x9,0x6,0x0,0xE,0x0,0xFFC),Instruction(0x9,0x1,0xE,0xE,0x0,0xFF8)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(CALL,{Instruction(0x2,0,0xF,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(RET,{Instruction(0x9,0x3,0xF,0xE,0,0x4)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(JMP,{Instruction(0x3,0,0xF,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(BEQ,{Instruction(0x3,0,0xF,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(BNE,{Instruction(0x3,0,0xF,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(BGT,{Instruction(0x3,0,0xF,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(PUSH,{Instruction(0x8,0x1,0xE,0,0,0XFFC)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(POP,{Instruction(0x9,0x3,0,0xE,0,0x4)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(XCHG,{Instruction(0x4,0,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(ADD,{Instruction(0x5,0,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(SUB,{Instruction(0x5,0x1,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(MUL,{Instruction(0x5,0x2,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(DIV,{Instruction(0x5,0x3,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(NOT,{Instruction(0x6,0,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(AND,{Instruction(0x6,0x1,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(OR,{Instruction(0x6,0x2,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(XOR,{Instruction(0x6,0x3,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(SHL,{Instruction(0x7,0,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(SHR,{Instruction(0x7,0x1,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(LD,{Instruction(0x9,0,0,0,0,0)}));//ima vise ovdje
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(ST,{Instruction(0x8,0,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(CSRRD,{Instruction(0x9,0,0,0,0,0)}));
      instructionBase.insert(std::pair<OpType,std::vector<Instruction>>(CSRWR,{Instruction(0x9,0x4,0,0,0,0)}));
      map[Expression::LOGICAL_AND]=Expression::operationElement(3,3,-1);
      map[Expression::LOGICAL_OR]=Expression::operationElement(2,2,-1);
      map[Expression::EQ]=Expression::operationElement(4,4,-1);
      map[Expression::NEQ]=Expression::operationElement(4,4,-1);
      map[Expression::LESSEQ]=Expression::operationElement(4,4,-1);
      map[Expression::LESS]=Expression::operationElement(4,4,-1);
      map[Expression::GREAT]=Expression::operationElement(4,4,-1);
      map[Expression::GREATEQ]=Expression::operationElement(4,4,-1);
      map[Expression::ADDOP]=Expression::operationElement(5,5,-1);
      map[Expression::SUBOP]=Expression::operationElement(5,5,-1);
      map[Expression::OROP]=Expression::operationElement(6,6,-1);
      map[Expression::ANDOP]=Expression::operationElement(6,6,-1);
      map[Expression::XOROP]=Expression::operationElement(6,6,-1);
      map[Expression::NOTOP]=Expression::operationElement(6,6,-1);
      map[Expression::MULOP]=Expression::operationElement(7,7,-1);
      map[Expression::DIVOP]=Expression::operationElement(7,7,-1);
      map[Expression::MOD]=Expression::operationElement(7,7,-1);
      map[Expression::SHROP]=Expression::operationElement(7,7,-1);
      map[Expression::SHLOP]=Expression::operationElement(7,7,-1);
      map[Expression::NOTOP]=Expression::operationElement(9,8,0);
      map[Expression::NEG]=Expression::operationElement(9,8,0);
      map[Expression::START]=Expression::operationElement(10,0,0);
      map[Expression::END]=Expression::operationElement(1,0,0);
    }
    bool definedwith12Bits(int val,int offset){
      int max = (1 << 11) - 1;
      int min =0xFFFFF801;
      if(val-offset <= max && val - offset >= min)return true;
      else return false; 
    }
  public:
    Assembly (const Assembly&) = delete;
    Assembly (Assembly&&) = delete;
    Assembly& operator=(const Assembly&) = delete;
    Assembly& operator=(Assembly&&) = delete;
    static Assembly* Instance(){
      if(inst==nullptr){
        inst = new Assembly();
      }
      return inst;   
    }
    unsigned long getFileLen() const{return filelen;}
    void addFileLen(unsigned long val){filelen+=val;}
    int firstPassAnalysis(const char*);
    int backpatching();
    int solveEqus();
    int assembleFile(const char* filename);
    int parse(std::string data);
    int labelAnalysis(const char* label);
    int getln() const{return ln;}
    int analyseOp(int op,const char* param);
    int completeLiterralTable();
    SymbolTable* getSymbolTable() {return &st;}
    std::string& getCurLine () {return curline;}
    std::string extractVar(std::string& s,std::string ignore);
    bool isALiterral(std::string param);
    unsigned getLiterral(std::string var);
    SectionInfo* getCurSection() {if(curSection==nullptr)createImplicitSection();return curSection;}
    private:
    void createImplicitSection(){
        SymbolTableElement* e= new SymbolTableElement();
        e->setNum(st.getLen());
        e->setName("#implicit");//radice jer sekcija ne moze imati @ u sebi
        e->setType(SymbolTableElement::Type::SCTN);
        e->setNdx(e->getNum());
        e->setDefined(true);
        lc=0;
        SectionInfo* sectioni = new SectionInfo(e->getNum(),e->getName());
        st.add(e);
        curSection=sectioni;
        sections.push_back(sectioni);
    }
    bool secondPass = false;
    std::map<Expression::Operation,Expression::operationElement> map;
    void InsertIntoLiterralTable(unsigned val,int lsection,bool trueLiterral){
        /*LiterralTableElement* e = curSection->getLiterralTable()->find(val,lsection,trueLiterral);
        std::cout << val << " " << lsection << " " << trueLiterral << "\n";
        if(e==nullptr){
          e = new LiterralTableElement(lsection,val,trueLiterral);
          curSection->getLiterralTable()->add(e);
        }*/
        LiterralTableElement* e =new LiterralTableElement(lsection,val,trueLiterral);
        curSection->getLiterralTable()->add(e);
        e->getNeeds().push_back(lc);
    }
    void solveBranches(Instruction& instr,unsigned char valnolitt,unsigned char vallitt,std::string operand,int op1,const char* p);
};
class AssemblyException : public std::exception{
  private:
   std::string info;
   int ln;
   std::string code;
   bool noline;
  public:
   AssemblyException(std::string s,bool noline = false,unsigned long line = 0);
  friend std::ostream& operator>> (std::ostream& o,const AssemblyException& e);
};
#endif