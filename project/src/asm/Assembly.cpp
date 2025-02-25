#include "../../inc/asm/Assembly.hpp"
#include <fstream>
#include  <bits/stdc++.h>
extern int lexicalAnalysis(const char*);
extern std::string extractLine(unsigned long line);
extern unsigned long line_num;
Assembly *Assembly::inst = nullptr;
std::string Assembly::ops[] = {".global",".extern",".section",".word",".skip",".ascii",".equ",".end",
"halt","int","iret","call","ret","jmp","beq","bne","bgt","push","pop","xchg","add","sub","mul","div","not"
,"and","or","xor","shl","shr","ld","st","csrrd","csrwr"};
AssemblyException::AssemblyException(std::string s,bool noline,unsigned long line){
  {
    this->info=s;
    if(!noline){
    if(!line){
      this->ln = line_num;
      this->code = extractLine(this->ln);
    }
    else this->ln = line;
    }
    this->noline = noline;
  }
}
int Assembly::firstPassAnalysis(const char* c)
{
  std::string line;
  try{
    lexicalAnalysis(c);
  /*    std::cout <<"Tabela simbola:\n" <<  st;
  std::cout <<"Sekcije:\n";
  for(SectionInfo* sec : sections){
    std::cout << *sec << std::endl;
  }  */
      for(int i=1;i<st.getLen();i++){
        if(!st.get(i)->isDefined() && st.get(i)->getBind()==SymbolTableElement::GLOB)//nedefinisani globalni simbol
            st.get(i)->setDefined(true);
  }
  }
  catch(AssemblyException* e){
    std::cout >> *e;
    if(!isDone)
    //fclose(f);
    exit(-1);
  };
  return 0;
}
 int Assembly::backpatching()
{
  secondPass=true;
  try{
    for(int i=1;i<st.getLen();i++){
        if(!st.get(i)->isDefined()){
          throw new AssemblyException("Neki simboli su ostali nedefinisani!" + st.getUndefinedNames(),true);
  }
  }
  isDone = false;
  for(int i=0;i<st.getLen();i++){
    SymbolTableElement* e = st.get(i);
    while(!e->allRefsCleared()){
      SymbolTableElement::FlinkElement elem = e->getRef();
      lc = elem.size;
      line_num=elem.pos;
      for(SectionInfo* s : sections){
        if(s->getId()==elem.section){curSection=s;break;}
      }
      analyseOp(elem.opCode,elem.str.c_str());
    }
  }
  }catch(AssemblyException* e){
    std::cout >> *e;
    exit(-1);
  }
  return 0;
}
int Assembly::solveEqus()
{
  try{
  bool change = false;
  while(TNS.size()>0){
    change = false;
    for(unsigned long i=0;i<TNS.size();){
      std::pair<int,Expression*>& expr = TNS[i];
      for(unsigned long j=0;j<expr.second->missingSyms().size();){
        if(st.get(expr.second->missingSyms()[j])->isDefined()){
          std::vector<unsigned>::iterator iter = expr.second->missingSyms().begin();
          std::advance(iter,j);
          expr.second->missingSyms().erase(iter);
        }else j++;
      }
        if(expr.second->missingSyms().size()==0){
          SymbolTableElement* elem = st.get(expr.first);
          change = true;
          expr.second->trycalc();
          if(!expr.second->isSolved())throw new AssemblyException("doslo je do neuspjesne provjere simbola (ne bi smjelo da se desi)",true);
          expr.second->setSymbol(elem);
          elem->setDefined(true);
          std::vector<std::pair<int,Expression*>>::iterator iter = TNS.begin();
          std::advance(iter,i);
          delete expr.second;
          TNS.erase(iter);
        }else i++;
      }
      if(change==false)break;
  }
  if(TNS.size()!=0)
  throw new AssemblyException("doslo je do neuspjesnog razrjesavanja .equ simbola ",true);
  }catch(AssemblyException* e){
    std::cout >> *e;
    exit(-1);
  };
  return 0;
}
int Assembly::completeLiterralTable(){
  try{
  for(SectionInfo* s : sections){
    s->resolveLiterralTable();
  }
  }catch(AssemblyException* e){
    std::cout >> *e;
    exit(-1);
  }
  return 0;
  }
int Assembly::labelAnalysis(const char* l) {
  if(isDone)return 0;
  std::string label = l;
  label.erase(label.length()-1,1);
  if(curSection==nullptr){createImplicitSection();/*throw new AssemblyException("Nije definisana nijedna tabela simbola");*/} //kad se budu obradile sekcije, a to je nekad sad
  //provjera da li je labela dvaput definisana:
  for(int i = 0;i<this->st.getLen();i++){
    if(st.get(i)->getName()== label ){  
      SymbolTableElement* e =st.get(i);   
      if(e->isDefined())throw new AssemblyException("Dvostruko definisanje labele " + label);
      e->setDefined(true);
      e->setNdx(curSection->getId());
      e->setVal(lc);
      return 0;
    }
  }
  //dodavanje u tabelu simbola
  //SymbolTableElement(unsigned long num,unsigned long value,unsigned long size,Type type,Bind bind,unsigned long Ndx,std::string name,bool isAbs=false,bool defined=false)
  this->st.add(new SymbolTableElement(st.getLen(),lc,0,SymbolTableElement::Type::NOTYP,SymbolTableElement::Bind::LOC,curSection->getId(),label,false,true));
  //std::cout << (this->st);
  return 0;
}

Assembly::OpType Assembly::StringToType(std::string s)
{
  for(int i=0;i<(int)UNKNOWN;i++){
    if(s==ops[i])return (OpType)i;
  }
  return UNKNOWN;
}
int Assembly::analyseOp(int op1,const char* p){//pia momenat
  if(isDone)return 0;
  OpType op = (OpType)op1;
  std::string param = p==nullptr ? " " : p;
  //std::cout << ops[op1] << " " << param << std::endl;
  std::vector<Instruction> instructions;
  if(op >= HALT)instructions = instructionBase[op];
  if(curSection==nullptr && op!=DIR_GLOBAL && op!=DIR_SECTION && op!= DIR_EXTERN && op!=DIR_EQU && op!=DIR_END)
    createImplicitSection();
    //throw new AssemblyException("Nije definisana nijedna sekcija!");
  switch(op){
    case DIR_GLOBAL:{
      while(param.length()>0){
          std::string symbol = extractVar(param,", \t");
          SymbolTableElement* elem = st.find(symbol);
          if(elem ){
            elem->setBind(SymbolTableElement::Bind::GLOB);
          }else{
            SymbolTableElement*  e= new SymbolTableElement();
            e->setNum(st.getLen());
            e->setBind(SymbolTableElement::Bind::GLOB);
            e->setName(symbol);
            st.add(e);
          }
      }
      //std::cout << st;
    }break;
    case DIR_EXTERN:{
      while(param.length()>0){
          std::string symbol = extractVar(param," \t,");
          SymbolTableElement* elem = st.find(symbol);
          if(elem && elem->isDefined()){
            throw new AssemblyException("Dvostruko definisanje simbola " + symbol);
          }else{
            
            SymbolTableElement*  e = elem;
            if(e==nullptr){
              e= new SymbolTableElement();
              e->setNum(st.getLen());
              e->setName(symbol);
              st.add(e);
            }
            e->setBind(SymbolTableElement::Bind::GLOB);
            e->setDefined(true);
            //e->setAbs(true);
          }
        }
      //std::cout << st;
    }break;
    case DIR_SECTION:{
        //std::cout << param;
        std::string symbol = extractVar(param," \t");
        SymbolTableElement* elem = st.find(symbol);
        if(elem && elem->isDefined()){
          throw new AssemblyException("Dvostruko definisanje sekcije " + symbol);
        }
        SymbolTableElement* e = elem;
        if(e==nullptr){
        e= new SymbolTableElement();
        e->setNum(st.getLen());
        e->setName(symbol);
        }
        e->setType(SymbolTableElement::Type::SCTN);
        e->setNdx(e->getNum());
        e->setDefined(true);
        //e->setAbs(false);
        lc=0;
        SectionInfo* sectioni = new SectionInfo(e->getNum(),e->getName());
        st.add(e);
        curSection=sectioni;
        sections.push_back(sectioni);
    }break;
    case DIR_WORD:{
      while(param.length()>0){
          std::string var = extractVar(param," \t,");
          if(isALiterral(var)){
            unsigned long cnt;
            if(var.length()>2 && (var[1]=='X' || var[1]=='x'))
            cnt = strtol(var.c_str(),NULL,16);
            else cnt = strtol(var.c_str(),NULL,10);
            if(cnt>=((1UL<<32)))throw new AssemblyException("Prevelika velicina literala: " + var);
            for(int i=0;i<4;i++){
              curSection->getSectionData()->push_back((char)(cnt&0xFF));
              cnt>>=8;
            }
          }else if(var=="."){
            curSection->getRelocTable()->add(new RelocTableElement(lc,RelocTableElement::ABS,curSection->getId(),lc));
             for(int i=0;i<4;i++)
              curSection->getSectionData()->push_back((char)0);
            }else
            {
            SymbolTableElement* elem = st.find(var);
            if(!elem){
              this->st.add(new SymbolTableElement(st.getLen(),0,0,SymbolTableElement::Type::NOTYP,SymbolTableElement::Bind::LOC,0,var,false,false));
              elem = st.find(var);
            }
            if(elem->isDefined()){
              if(elem->isAbs()){
                unsigned long cnt = elem->getVal(); 
              for(int i=0;i<4;i++){
                if(!secondPass)
                curSection->getSectionData()->push_back((char)(cnt&0xFF));
                else
                curSection->getSectionData()->operator[](lc + i) = ((char)(cnt&0xFF));
                cnt>>=8;
              }
              }else{
              if(elem->getBind()==SymbolTableElement::GLOB){
                curSection->getRelocTable()->add(new RelocTableElement(lc,RelocTableElement::ABS,elem->getNum(),0));
              }else{
                curSection->getRelocTable()->add(new RelocTableElement(lc,RelocTableElement::ABS,elem->getNdx(),elem->getVal()));  
              }
              if(!secondPass)
                for(int i=0;i<4;i++)
                  curSection->getSectionData()->push_back((char)0);
              }
            }else{
              elem->insertForwardRef(curSection->getId(),lc,op1,std::string(p),line_num);
              for(int i=0;i<4;i++)
              curSection->getSectionData()->push_back((char)0);
            }
          }
          lc+=INSTRUCTION_LEN;;
      }
    }break;
    case DIR_SKIP:{
        if(!curSection)createImplicitSection();//throw new AssemblyException("Nije definisana nijedna sekcija");
        long cnt = 0;
        param = extractVar(param," \t");
        if(param.length()>2 && (param[1]=='X' || param[1]=='x'))
            cnt = strtol(param.c_str(),NULL,16);
            else cnt = strtol(param.c_str(),NULL,10);
        for(long i=0;i<cnt;i++)curSection->getSectionData()->push_back((char)0);
        lc+=cnt;
    }break;
    case DIR_ASCII:{
        if(!curSection)createImplicitSection();//throw new AssemblyException("Nije definisana nijedna sekcija");
        long i = 0;
        /*std::cout << param << std::endl;*/
        while(param[i]!='\"')i++;i++;
        while(param[i]!='\"'){curSection->getSectionData()->push_back((unsigned char)param[i]);i++;lc++;}
    }break;
    case DIR_EQU:{
        std::string var = extractVar(param," \t,");
        SymbolTableElement* elem = st.find(var);
        if(!elem){
              this->st.add(new SymbolTableElement(st.getLen(),0,0,SymbolTableElement::Type::NOTYP,SymbolTableElement::Bind::LOC,0,var,false,false));
              elem = st.find(var);
            }
        if(elem->isDefined()){
          throw new AssemblyException("Dvostruko definisanje simbola " + elem->getName());
        }
        Expression* e = new Expression(&st,param,map,lc);
        if(e->isSolved()){
          e->setSymbol(elem);
          elem->setDefined(true);
          delete e;
        }else{
          this->TNS.push_back(std::pair<int,Expression*>(elem->getNum(),e));
        }
    }break;
    case DIR_END:{
      isDone=true;
    }break;
    case HALT:case INT:case RET:case IRET:{//baza se ne mijenja
      for (Instruction i : instructions){
        i.insertIntoSection(curSection,lc);
        lc+=INSTRUCTION_LEN;
      }
    }break;
    case CALL:{//rad sa instrukcijama skoka
      std::string operand = extractVar(param," \t");
      solveBranches(instructions[0],0,0x1,operand,op1,p);
      instructions[0].insertIntoSection(curSection,lc);lc+=INSTRUCTION_LEN;
    }break;
    case JMP:{
      std::string operand = extractVar(param," \t");
      solveBranches(instructions[0],0,0x8,operand,op1,p);
      instructions[0].insertIntoSection(curSection,lc);lc+=INSTRUCTION_LEN;
    }break;
    case BEQ:{
      unsigned long gpr1 = strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      unsigned long gpr2 = strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      std::string operand = extractVar(param," \t");      
      solveBranches(instructions[0],0x1,0x9,operand,op1,p);
      instructions[0].setregB(gpr1);instructions[0].setregC(gpr2);
      instructions[0].insertIntoSection(curSection,lc);        lc+=INSTRUCTION_LEN;
    }break;
    case BNE:{
      unsigned long gpr1 = strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      unsigned long gpr2 = strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      std::string operand = extractVar(param," \t"); 
      solveBranches(instructions[0],0x2,0xA,operand,op1,p);
      instructions[0].setregB(gpr1);instructions[0].setregC(gpr2);
      instructions[0].insertIntoSection(curSection,lc);        lc+=INSTRUCTION_LEN;
    }break;
    case BGT:{
      unsigned long gpr1 = strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      unsigned long gpr2 = strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      std::string operand = extractVar(param," \t");
      solveBranches(instructions[0],0x3,0xB,operand,op1,p);
      instructions[0].setregB(gpr1);instructions[0].setregC(gpr2);
      instructions[0].insertIntoSection(curSection,lc);lc+=INSTRUCTION_LEN;
    }break;
    case PUSH:{//rad iskljucivo sa registrima
      unsigned long gpr = strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      instructions[0].setregC(gpr);
      instructions[0].insertIntoSection(curSection,lc);lc+=INSTRUCTION_LEN;
    }break;
    case POP:{
      unsigned long gpr = strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      instructions[0].setregA(gpr);
      instructions[0].insertIntoSection(curSection,lc);lc+=INSTRUCTION_LEN;
    }break;
    case XCHG:{
      unsigned long gprs=strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      unsigned long gprd=strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      instructions[0].setregB(gprs);instructions[0].setregC(gprd);
      instructions[0].insertIntoSection(curSection,lc);lc+=INSTRUCTION_LEN;
    }break;
    case NOT:{
      unsigned long gpr=strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      instructions[0].setregA(gpr);instructions[0].setregB(gpr);
      instructions[0].insertIntoSection(curSection,lc);lc+=INSTRUCTION_LEN;
    }break;
    case ADD:case SUB:case MUL:case DIV:case AND:case OR:case XOR:case SHL:case SHR:{
      unsigned long gprs=strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      unsigned long gprd=strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      //std::cout << gprs << " " << gprd;
      instructions[0].setregA(gprd);instructions[0].setregB(gprd);instructions[0].setregC(gprs);
      instructions[0].insertIntoSection(curSection,lc);lc+=INSTRUCTION_LEN;
    }break;
    case LD:{//e ovo ce biti malo gadno
      std::string operand = extractVar(param," \t");
      unsigned long gpr=strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      instructions[0].setregA(gpr);
      if(operand[0]=='%'){//registarsko direktno
        unsigned long gpr1 = strtol(extractVar(operand,"r %\t").c_str(),NULL,10);
        instructions[0].setMod(0x1);instructions[0].setregB(gpr1);
      }else
        if(operand[0]=='['){//registarsko indirektno sa pomjerajem
          unsigned long gpr1 = strtol(extractVar(operand,"r +%\t[]").c_str(),NULL,10);
          std::string op = extractVar(operand,"+ \t]");
          unsigned long val = 0;
          if(op.length()>0){
              if(isALiterral(op)){
                val = getLiterral(op);
                if((int)val >= 1<<11 && (int)val <  1 << 11 | 1)
                  throw new AssemblyException("Literal mora biti velicine 12 bita!");
              }
             else if(op=="."){
              throw new AssemblyException("Tekuca vrijednost location counter-a se ne moze definisati na velicini od 12 bita!");
              }else{
                SymbolTableElement* e = st.find(op);
                if(e && e->isDefined()){
                  if(!e->isAbs())
                    throw new AssemblyException("Simbol se ne moze definisati na velicini od 12 bita!");
                  val = e->getVal();
                if((int)val >= 1<<11 && (int)val <  1 << 11 | 1)
                  throw new AssemblyException("Simbol mora biti velicine 12 bita!");
              }else{
                if(!e){
                this->st.add(new SymbolTableElement(st.getLen(),0,0,SymbolTableElement::Type::NOTYP,SymbolTableElement::Bind::LOC,0,op,false,false));
                e = st.find(op);
                }
                e->insertForwardRef(curSection->getId(),lc,op1,std::string(p),line_num);
                }
              }
          }
              instructions[0].setregB(gpr1);instructions[0].setDisp(val);instructions[0].setMod(0x2);
        }
      else if(operand[0]=='$'){//skoro pa neposredno
        std::string op = extractVar(operand,"$ \t");
        if(isALiterral(op)){
          unsigned val = getLiterral(op);
          if(definedwith12Bits(val,0)){
            instructions[0].setMod(0x01);instructions[0].setDisp(val);}
          else if(definedwith12Bits(val,lc + 4)){
            instructions[0].setMod(0x01);instructions[0].setDisp(val - lc - 4);instructions[0].setregB(0xF);
          }
          else{
            InsertIntoLiterralTable(val,0,true);
            instructions[0].setMod(0x2);instructions[0].setregB(0xF);
          }
        } else if(op=="."){
          instructions[0].setMod(0x01);instructions[0].setregB(0xF);instructions[0].setDisp(-4);
       }else{
        SymbolTableElement* e = st.find(op);
        if(e && e->isDefined()){
            if(!e->isAbs()){
              int val = e->getVal();
              if(e->getNdx()==curSection->getId() &&  definedwith12Bits(val,lc+4)){
                instructions[0].setMod(0x01);instructions[0].setDisp(val -lc - 4);instructions[0].setregB(0xF);
              }else{
              instructions[0].setMod(0x2);instructions[0].setregB(0xF);
              InsertIntoLiterralTable(e->getNum(),e->getNdx(),false);
              }
            }
            else{
              unsigned val = e->getVal();
              if(definedwith12Bits(val,lc+4)){
                instructions[0].setMod(0x01);instructions[0].setDisp(val);
              }else if(definedwith12Bits(val,lc+4)){
                instructions[0].setMod(0x01);instructions[0].setDisp(val -lc - 4);instructions[0].setregB(0xF);
              }else{
              InsertIntoLiterralTable(val,0,true);
              instructions[0].setMod(0x2);instructions[0].setregB(0xF);
              }
          }
      }else{
        if(!e){
          this->st.add(new SymbolTableElement(st.getLen(),0,0,SymbolTableElement::Type::NOTYP,SymbolTableElement::Bind::LOC,0,op,false,false));
          e = st.find(op);
          }
          e->insertForwardRef(curSection->getId(),lc,op1,std::string(p),line_num);
          }
        }
      }else{//memorijsko direktno?
        std::string op = operand;
        if(isALiterral(op)){
          unsigned val = getLiterral(op);
          if(!secondPass && definedwith12Bits(val,0)){
             instructions[0].setMod(0x2);instructions[0].setregB(0x0);instructions[0].setDisp(val);
          }else if(!secondPass && definedwith12Bits(val,lc+4)){
            instructions[0].setMod(0x2);instructions[0].setregB(0xF);instructions[0].setDisp(val -lc - 4);
          }
          else{
            InsertIntoLiterralTable(val,0,true);
            instructions[0].setMod(0x2);instructions[0].setregB(0xF);
            instructions.push_back(instructions[0]);//implementirati insert za instrukciju
            instructions[1].setregB(gpr);
          }//moze eventualno i ako se moze dohvatiti sa pcem
        }else
        if(op=="."){
          instructions[0].setMod(0x02);instructions[0].setregB(0xF);instructions[0].setregC(0x0);instructions[0].setDisp(-4);
       }else
        {
        SymbolTableElement* e = st.find(op);
        if(e && e->isDefined()){
            if(!e->isAbs()){
              unsigned val = e->getVal();
              if(!secondPass && e->getNdx()==curSection->getId() && definedwith12Bits(val,lc + 4)){
                instructions[0].setMod(0x2);instructions[0].setregB(0xF);instructions[0].setDisp(val -lc - 4);
              }else{
              instructions[0].setMod(0x2);instructions[0].setregB(0xF);
              InsertIntoLiterralTable(e->getNum(),e->getNdx(),false);
              instructions.push_back(instructions[0]);
              instructions[1].setregB(gpr);
              }
            }
            else{
              unsigned val = e->getVal(); 
              if(!secondPass && definedwith12Bits(val,0)){
               instructions[0].setMod(0x02);instructions[0].setDisp(val);instructions[0].setregB(0x0);                
              }else if(!secondPass && definedwith12Bits(val,lc+4)){
                instructions[0].setMod(0x2);instructions[0].setregB(0xF);instructions[0].setDisp(val -lc - 4);                
              }
              else{
              InsertIntoLiterralTable(val,0,true);
              instructions[0].setMod(0x2);instructions[0].setregB(0xF);
              instructions.push_back(instructions[0]);
              instructions[1].setregB(gpr);
              }
            }
      }else{
        if(!e){
          this->st.add(new SymbolTableElement(st.getLen(),0,0,SymbolTableElement::Type::NOTYP,SymbolTableElement::Bind::LOC,0,operand,false,false));
          e = st.find(op);
          }
          e->insertForwardRef(curSection->getId(),lc,op1,std::string(p),line_num);instructions.push_back(instructions[0]); //mora ovako, jer insertovanje u drugom prolazu pravi problem
          }
        }
      }
          for (Instruction i : instructions){
        i.insertIntoSection(curSection,lc);
        lc+=INSTRUCTION_LEN;
      }
    }break;
    case ST:{
      unsigned long gpr=strtol(extractVar(param,"r %\t").c_str(),NULL,10);
      std::string operand = extractVar(param," \t");
      instructions[0].setregC(gpr);
      if(operand[0]=='%'){//registarsko direktno
        unsigned long gpr1 = strtol(extractVar(operand,"r +%\t").c_str(),NULL,10);
        instructions[0]=Instruction(0x9,0x2,gpr1,gpr,0,0);
      }else
        if(operand[0]=='['){//registarsko indirektno sa pomjerajem
          unsigned long gpr1 = strtol(extractVar(operand,"r +%\t[]").c_str(),NULL,10);
          std::string op = extractVar(operand," \t]");
          unsigned long val = 0;
          if(op.length()>0){
              if(isALiterral(op)){
                val = getLiterral(op);
                if((int)val >= 1<<11 && (int)val <  1 << 11 | 1)
                  throw new AssemblyException("Literal mora biti velicine 12 bita!");
              }
              else{
                SymbolTableElement* e = st.find(op);
                if(e && e->isDefined()){
                  if(!e->isAbs())
                    throw new AssemblyException("Simbol se ne moze definisati na velicini od 12 bita!");
                  val = e->getVal();
                if((int)val >= 1<<11 && (int)val <  1 << 11 | 1)
                  throw new AssemblyException("Simbol mora biti velicine 12 bita!");
              }else{
                if(!e){
                this->st.add(new SymbolTableElement(st.getLen(),0,0,SymbolTableElement::Type::NOTYP,SymbolTableElement::Bind::LOC,0,op,false,false));
                e = st.find(op);
                }
                e->insertForwardRef(curSection->getId(),lc,op1,std::string(p),line_num);
                }
              }
          }
              instructions[0].setregA(gpr1);instructions[0].setDisp(val);          
        }
      else if(operand[0]=='$'){
        throw new AssemblyException("Nemoguce je izvrsiti ST nad neposrednom vrijednoscu!");
      }else{
        std::string op = extractVar(operand," \t");
        if(isALiterral(op)){
          unsigned val = getLiterral(op);
          if(definedwith12Bits(val,0)){
            instructions[0].setMod(0x0);instructions[0].setDisp(val);instructions[0].setregA(0x0);
          }
          else if(definedwith12Bits(val,lc+4)){
            instructions[0].setMod(0x0);instructions[0].setDisp(val -lc - 4);instructions[0].setregA(0xF);
          }
          else{
            InsertIntoLiterralTable(val,0,true);
            instructions[0].setMod(0x2);instructions[0].setregA(0xF);
          }
        }
        else if(op=="."){
          instructions[0].setMod(0x00);instructions[0].setregA(0xF);instructions[0].setregB(0x0);instructions[0].setDisp(-4);
       }else{
        SymbolTableElement* e = st.find(op);
        if(e && e->isDefined()){
            if(!e->isAbs()){
              int val = e->getVal();
              if(e->getNdx()==curSection->getId() &&  definedwith12Bits(val,lc+4)){
                instructions[0].setMod(0x0);instructions[0].setDisp(val -lc - 4);instructions[0].setregA(0xF);               
              }else{
              instructions[0].setMod(0x2);instructions[0].setregA(0xF);
              InsertIntoLiterralTable(e->getNum(),e->getNdx(),false);
              }
            }
            else{
              unsigned val = e->getVal(); 
              if(definedwith12Bits(val,0)){
                instructions[0].setMod(0x0);instructions[0].setDisp(val);instructions[0].setregA(0);instructions[0].setDisp(val);
              }else if(definedwith12Bits(val,lc + 4)){
                instructions[0].setMod(0x0);instructions[0].setDisp(val - lc - 4);instructions[0].setregA(0xF);instructions[0].setDisp(val);                
              }
              else{
              InsertIntoLiterralTable(val,0,true);
              instructions[0].setMod(0x2);instructions[0].setregA(0xF);
          }
          }
      }else{
        if(!e){
          this->st.add(new SymbolTableElement(st.getLen(),0,0,SymbolTableElement::Type::NOTYP,SymbolTableElement::Bind::LOC,0,op,false,false));
          e = st.find(op);
          }
          e->insertForwardRef(curSection->getId(),lc,op1,std::string(p),line_num);
          }
        }
      }
          for (Instruction i : instructions){
        i.insertIntoSection(curSection,lc);
        lc+=INSTRUCTION_LEN;
      }
    }break;
    case CSRRD:{
        std::string csrname = extractVar(param,"% \t");
        unsigned long csr = (csrname=="status" ? 0 : (csrname=="handler" ? 1 : 2) );
        unsigned long gpr=strtol(extractVar(param,"r %\t").c_str(),NULL,10);
        instructions[0].setregA(gpr);instructions[0].setregB(csr);
        instructions[0].insertIntoSection(curSection,lc);lc+=INSTRUCTION_LEN;
    }break;
    case CSRWR:{
        unsigned long gpr=strtol(extractVar(param,"r %\t").c_str(),NULL,10);
        std::string csrname = extractVar(param,"% \t");
        unsigned long csr = (csrname=="status" ? 0 : (csrname=="handler" ? 1 : 2) );
        instructions[0].setregB(gpr);instructions[0].setregA(csr);
        instructions[0].insertIntoSection(curSection,lc);lc+=INSTRUCTION_LEN;
    }break;
    case UNKNOWN:{
      throw new AssemblyException("Pronadjena je nepostojeca operacija!(ne bi trebalo da se desi)");
    }
  }
  return 0;
}

std::ostream &operator>>(std::ostream &o, const AssemblyException &e)
{
  if(!e.noline)
  o << "Greska pri asembliranju,linija " << e.ln << " :" << e.info << "\n" <<e.code << std::endl;
  else o << "Greska pri asembliranju : " << e.info << std::endl;
  return o;
}
std::string Assembly::extractVar(std::string& s,std::string ignore){
  int poc = s.find_first_not_of(ignore,0);
  if(poc==std::string::npos)poc = s.length();
  int kraj = s.find_first_of(ignore,poc);
  if(kraj==std::string::npos)kraj = s.length();
  std::string var = s.substr(poc,kraj-poc);
  s.erase(0,kraj+1);
  if(ignore.find_first_of("r")!=std::string::npos && var=="pc")return "15";
  if(ignore.find_first_of("r")!=std::string::npos && var=="sp")return "14";
  return var;
}
bool Assembly::isALiterral(std::string param){
  if(param.find_first_not_of("1234567890")==std::string::npos)return true;
  if(param[0]=='0' && (param[1]=='x' || param[0]=='X') && param.length()>2){
    return param.find_first_not_of("1234567890ABCDEFabcdef",2)==std::string::npos;
  }
  return 0;
}
unsigned Assembly::getLiterral(std::string var){
        unsigned long cnt;
        if(var.length()>2 && (var[1]=='X' || var[1]=='x'))
        cnt = strtol(var.c_str(),NULL,16);
        else cnt = strtol(var.c_str(),NULL,10);
        if(cnt>=((1UL<<32)))throw new AssemblyException("Prevelika velicina literala: " + var);
        return cnt;
};
void Assembly::solveBranches(Instruction &instr, unsigned char valnolitt, unsigned char vallitt, std::string operand,int op1,const char* p)
{
  int val = 0;
      if(isALiterral(operand)){
        unsigned int val = getLiterral(operand);
        if(definedwith12Bits(val,0)){
          instr.setDisp((int)val);
          instr.setregA(0);
          instr.setMod(valnolitt);
        }else{
        InsertIntoLiterralTable(getLiterral(operand),0,true);
        instr.setMod(vallitt);
        }
      }else if(operand=="."){
        instr.setMod(valnolitt);
        instr.setDisp(-4);
      }
      else{
        SymbolTableElement* e = st.find(operand);
        if(e && e->isDefined()){
          if(e->getNdx()==curSection->getId()){
            //recimo negdje ovdje
            val =e->getVal() - (lc + (int)INSTRUCTION_LEN  );
            if(val < 1<<11 && val >= -(1 << 11)){instr.setDisp(val);instr.setMod(valnolitt);}
            else {InsertIntoLiterralTable(val,0,true);instr.setMod(vallitt);}
          }else{
          if(e->isAbs()){
            if(definedwith12Bits(e->getVal(),0)){
              instr.setDisp((int)e->getVal());
              instr.setregA(0);
              instr.setMod(valnolitt);
            }else{
            InsertIntoLiterralTable(e->getVal(),0,true);instr.setMod(vallitt);}}
          else {InsertIntoLiterralTable(e->getNum(),e->getNdx(),false);
          instr.setMod(vallitt);}
          }
        }else{
          if(!e){
            this->st.add(new SymbolTableElement(st.getLen(),0,0,SymbolTableElement::Type::NOTYP,SymbolTableElement::Bind::LOC,0,operand,false,false));
            e = st.find(operand);
          }
          e->insertForwardRef(curSection->getId(),lc,op1,std::string(p),line_num);
        }
      }
}
int Assembly::assembleFile(const char *filename)
{
  std::ofstream out;
  out.open(filename, std::ios::binary);
  /*std::cout << "Nakon svih obrada:\n";
  std::cout <<"Tabela simbola:\n" <<  st;
  std::cout <<"Sekcije:\n";
  for(SectionInfo* sec : sections){
  std::cout << *sec << std::endl;
  }*/
  //zaglavlje
  unsigned long nullval = 0 ;
  unsigned long stlen = st.getLen(); 
  unsigned long tmp = (5*sizeof(long));
  out.write(magic.c_str(),sizeof(magic.c_str()));
  out.write(reinterpret_cast<char*>(&nullval),sizeof(int));//0 - relokatibilan fajl, 1 - izvrsni fajl
  out.write(reinterpret_cast<char*>(&nullval),sizeof(int));//padding
  out.write(reinterpret_cast<char*>(&stlen),sizeof(unsigned long));//broj elemenata u tabeli simbola
  out.write(reinterpret_cast<char*>(&tmp),sizeof(long));//lokacija tabele simbola
  out.write(reinterpret_cast<char*>(&nullval),sizeof(unsigned long));//lokacija tabele stringova
  filelen+=40;
  unsigned long stloc,stringtloc,shloc;
  //upis podataka sekcija
  for(SectionInfo* si : sections){
    si->writeData(out);
  }
  //upis relokativnih zapisa
    for(SectionInfo* si : sections){
    si->writeRelocTable(out);
  }
  shloc = filelen;
  //upis zaglavlja sekcija
  for(int i = 0;i < sections.size();i++){
    sections[i]->writeSectionHeaderInfo(out);
  }
  stloc = filelen;stringtloc = filelen + st.getLen() * SymbolTableElement::getSectionElemSize();
  //upis tabele simbola i tabele stringova
  st.writeSymbolTable(out);
  out.close();
  //dodaj pokazivace
  out.open(filename, std::ios::in | std::ios::out);
  out.seekp(24);
  out.write(reinterpret_cast<char*>(&stloc),sizeof(long));//lokacija tabele simbola
  out.write(reinterpret_cast<char*>(&stringtloc),sizeof(unsigned long));//lokacija tabele stringova
  out.close();
  std::cout << "duzina fajla: " << filelen << std::endl;
  //aj kao da imam i neki tekstualni fajl
  out.open(std::string(filename) + ".otxt");
  out <<"Tabela simbola:\n" <<  st;
  out <<"Sekcije:\n";
  for(SectionInfo* sec : sections){
  out << *sec << std::endl;
  }
  out.close();
  return 0;
}