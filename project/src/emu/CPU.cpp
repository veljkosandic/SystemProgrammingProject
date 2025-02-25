#include "../../inc/emu/CPU.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
CPU* CPU::inst = nullptr;
using namespace std::chrono;
const double CPU::thresholds[] = {0.5,1,1.5,2,5,10,30,60};
void CPU::CPUcycle(){
  steady_clock::time_point begin = steady_clock::now();
  steady_clock::time_point end = steady_clock::now();
  double extra = duration_cast<duration<double>>(end-begin).count();
  while(!finished){
  write_term_in=false;
  //fetch faza - dohvatanje instrukcije
  unsigned instr = memory.read(r[PC]);
  r[PC]+=4;
  //dekodovanje i izvrsavanje
  unsigned char oc = (instr >> 28) & 0xF;
  unsigned char mod = (instr >> 24) & 0xF;
  unsigned char regA = (instr >> 20) & 0xF;
  unsigned char regB = (instr >> 16) & 0xF;
  unsigned char regC = (instr >> 12) & 0xF;
  int Disp = (instr << 4) & 0xFF0 | (instr >> 8) & 0xF;
  if(Disp & 0x800)
  Disp |=0xFFFFF000;
  switch(oc){
    case OP_HALT:{
      if(instr == 0){
        finished = true;return;
      }else{
        onIllegalInstruction();
      }
    }break;
    case OP_INT:{
      if(instr == 0x10000000){
        push(STATUS,true);
        push(PC,false);
        c[CAUSE]=SOFTWARE;
        c[STATUS] = c[STATUS] |=(0x04);
        r[PC]=c[HANDLER];
      }else{
        onIllegalInstruction();
      }
    }break;
    case OP_CALL:{
      if(regC != 0){onIllegalInstruction();}
      switch(mod){
        case 0x0:{
          push(PC,false);
          r[PC] = r[regA] + r[regB] + Disp;
        }break;
        case 0x1:{
          push(PC,false);
          r[PC] = memory.read(r[regA]+r[regB] + Disp);
        }break;
        default:{
          onIllegalInstruction();
        }break;
      }
    }break;
    case OP_JMP:{
      switch(mod){
        case 0x0:{
          r[PC] = r[regA] + Disp;
        }break;
        case 0x1:{
          if(r[regB]==r[regC])r[PC] = r[regA] + Disp;
        }break;
        case 0x2:{
          if(r[regB]!=r[regC])r[PC] = r[regA] + Disp;
        }break;
        case 0x3:{
          if((int)r[regB]>(int)r[regC])r[PC]=r[regA]+Disp;
        }break;
        case 0x8:{
          r[PC] = memory.read(r[regA] + Disp);
        }break;
        case 0x9:{
          if(r[regB]==r[regC])r[PC]=memory.read(r[regA]+Disp);
        }break;
        case 0xA:{
          if(r[regB]!=r[regC])r[PC]=memory.read(r[regA] + Disp);
        }break;
        case 0xB:{
          if((int)r[regB]>(int)r[regC])r[PC]=memory.read(r[regA] + Disp);
        }break;
        default:{
          onIllegalInstruction();
        }
      }
    }break;
    case OP_XCHG:{
      if(mod!=0 || regA!=0 || Disp!=0){onIllegalInstruction();break;}
      unsigned temp = r[regB];r[regB]=r[regC];r[regC]=temp;
    }break;
    case OP_ARITHMETIC:{
      if(Disp!=0){onIllegalInstruction();break;}
      switch(mod){
        case 0x0:{
          r[regA] = r[regB] + r[regC];
        }break;
        case 0x1:{
          r[regA] = r[regB] - r[regC];
        }break;
        case 0x2:{
          r[regA] = r[regB] * r[regC];
        }break;
        case 0x3:{
          r[regA] = r[regB] / r[regC];
        }break;
        default:{
          onIllegalInstruction();
        }
      }
    }break;
    case OP_LOGICAL:{
      if(Disp!=0){onIllegalInstruction();break;}
      switch(mod){
        case 0x0:{
          r[regA] = ~r[regB];
        }break;
        case 0x1:{
          r[regA] = r[regB] & r[regC];
        }break;
        case 0x2:{
          r[regA] = r[regB] | r[regC];
        }break;
        case 0x3:{
          r[regA] = r[regB] ^ r[regC];
        }break;
        default:{
          onIllegalInstruction();
        }break;
      }
    }break;
    case OP_SHIFT:{
      if(Disp!=0){onIllegalInstruction();break;}
      switch(mod){
        case 0x0:{
          r[regA] = r[regB] << r[regC];
        }break;
        case 0x1:{
          r[regA] = r[regB] >> r[regC];
        }break;
        default:{
          onIllegalInstruction();
        }
      }
    }break;
    case OP_STORE:{
      unsigned val;
      switch(mod){
        case 0x0:{
          val = r[regA]+r[regB]+Disp;
          memory.write(val,r[regC]);
        }break;
        case 0x2:{
          val = memory.read(r[regA]+r[regB]+Disp);
          memory.write(val,r[regC]);
        }break;
        case 0x1:{
          val = r[regA];
          r[regA]=r[regA]+Disp;memory.write(r[regA],r[regC]);
        }break;
        default:{
          onIllegalInstruction();
        }
        }
        if(val == TIM_CFG)
          interruptThreshold=thresholds[r[regC]];
        if(val == TERM_OUT){
          write(STDOUT_FILENO,&r[regC],1);
        }
    }break;
    case OP_LOAD:{
      switch(mod){
        case 0x0:{
          if(regB>=3){onIllegalInstruction();break;}
          r[regA]=c[regB];
        }break;
        case 0x1:{
          r[regA]=r[regB]+Disp;
        }break;
        case 0x2:{
          r[regA]=memory.read(r[regB]+r[regC]+Disp);
        }break;
        case 0x3:{
          unsigned temp = 0;
          if(instr==POPPC)temp = r[regA];
          r[regA]=memory.read(r[regB]);
          r[regB]=r[regB]+Disp;
          //ja kad razumna pretpostavka
          if(instr==POPPC){
            unsigned nextinstr = memory.read(temp);
            if(nextinstr==POPSTATUS){
              //dekodovanje i izvrsavanje
              oc = (nextinstr >> 28) & 0xF;
              mod = (nextinstr >> 24) & 0xF;
              regA = (nextinstr >> 20) & 0xF;
              regB = (nextinstr >> 16) & 0xF;
              regC = (nextinstr >> 12) & 0xF;
              int Disp = (nextinstr << 4) & 0xFF0 | (nextinstr >> 8) & 0xF;//uvijek 4
              c[regA]=memory.read(r[regB]);
              r[regB]=r[regB]+Disp;                           
            }
          }
        }break;
        case 0x4:{
          if(regA>=3){onIllegalInstruction();break;}
          c[regA]=r[regB];
        }break;
        case 0x5:{
          if(regA>=3){onIllegalInstruction();break;}
          c[regA]=c[regB] | Disp;
        }break;
        case 0x6:{
          if(regA>=3){onIllegalInstruction();break;}
          c[regA]=memory.read(r[regB]+r[regC]+Disp);
        }break;
        case 0x7:{
          if(regA>=3){onIllegalInstruction();break;}
          c[regA]=memory.read(r[regB]);
          r[regB]=r[regB]+Disp;
        }break;
        default:{
          onIllegalInstruction();
        }break;
      }
    }break;
    default:{
      onIllegalInstruction();
    }break;
  }
  if(finished)return;
  //provjera prekida
  //terminal
  char tmp = 0;
  r[0]=0;
  if(!(c[STATUS] & (MASK_TERMINAL | MASK_INTERRUPT)) && read(fd_termin,&tmp,1)>0){
    memory.write(TERM_IN,tmp);
    push(STATUS,true);
    push(PC,false);
    c[CAUSE] = TERMINAL;
    c[STATUS] |=MASK_INTERRUPT;
    r[PC] = c[HANDLER];
  }
  //timer
  end = steady_clock::now();
  elapsed +=(duration_cast<duration<double>>(end-begin).count() + extra);
  begin = steady_clock::now();
  if(!(c[STATUS] & (MASK_TIMER | MASK_INTERRUPT)) && elapsed >=interruptThreshold){
    push(STATUS,true);
    push(PC,false);
    c[CAUSE] = TIMER;
    c[STATUS] |=MASK_INTERRUPT;
    r[PC] = c[HANDLER];
    elapsed=0;
  }
  //cur = steady_clock::now();
}
}
void CPU::loadMemory(const char* filename){
  std::fstream in(filename,std::ios::binary| std::ios::in | std::ios::out);
  try{
   char magic[8];
  in.read(magic,8*sizeof(char));
  if(std::string(magic)!="SSPROJF")
    throw new EmulatorException("zadati fajl nije dobrog binarnog formata!");
  int tmp;
  in.read(reinterpret_cast<char*>(&tmp),sizeof(int));
  if(tmp==0)
    throw new EmulatorException("unesen je relokatibilan fajl!");
  in.read(reinterpret_cast<char*>(&tmp),sizeof(int));
  unsigned long sectioncnt,sectionpos;
   in.read(reinterpret_cast<char*>(&sectioncnt),sizeof(unsigned long));
  in.read(reinterpret_cast<char*>(&sectionpos),sizeof(unsigned long));
  for(unsigned long i=0;i<sectioncnt;i++){
    in.seekg(sectionpos,in.beg);
    unsigned size,saddr;
    unsigned long saddrfile;
    in.read(reinterpret_cast<char*>(&size),sizeof(unsigned));
    in.read(reinterpret_cast<char*>(&saddr),sizeof(unsigned));
    in.read(reinterpret_cast<char*>(&saddrfile),sizeof(unsigned long));
    unsigned char* data = new unsigned char[size];
    in.seekg(saddrfile,in.beg);
  in.read(reinterpret_cast<char*>(&data[0]),size*sizeof(unsigned char));
      memory.loadSection(saddr,size,data);
    delete data;
    sectionpos += 2*sizeof(unsigned) + sizeof(unsigned long);
  }
  }catch(EmulatorException* e){
    std::cout << *e << std::endl;
    exit(-1);
  }
}