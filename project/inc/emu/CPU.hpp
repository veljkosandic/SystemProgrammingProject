#ifndef _CPU_HPP_
#define _CPU_HPP_
#include <termios.h>
#include "Memory.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <iomanip>
#define SP 14 
#define PC 15
#define STATUS 0
#define HANDLER 1
#define CAUSE 2
#define TERM_OUT 0xFFFFFF00
#define TERM_IN 0xFFFFFF04
#define TIM_CFG 0xFFFFFF10
#define ILLEGAL_INSTR 0x1
#define TIMER 0x2
#define TERMINAL 0x3
#define SOFTWARE 0x4
#define MASK_TIMER 0x1
#define MASK_TERMINAL 0x2
#define MASK_INTERRUPT 0x4
#define OP_HALT 0x0
#define OP_INT 0x1
#define OP_CALL 0x2
#define OP_JMP 0x3
#define OP_XCHG 0x4
#define OP_ARITHMETIC 0x5
#define OP_LOGICAL 0x6
#define OP_SHIFT 0x7
#define OP_STORE 0x8
#define OP_LOAD 0x9 
#define POPSTATUS 0x970E0400 //veoma jadno
#define POPPC 0x93FE0400
//prekoracenja?
class CPU{
  static const double thresholds[];
  unsigned r[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x40000000};//r14 - sp,r15 - pc, r0 - ozicen na 0
  unsigned c[3];//handler,status,cause
  bool finished = false;
  Memory memory;
  void push(unsigned char num,bool status){
    r[SP]-=4;
    if(status)memory.write(r[SP],c[num]);
    else memory.write(r[SP],r[num]);
  }
  void pop(unsigned char num,bool status){
    if(status)c[num] = memory.read(r[SP]);
    else r[num]= memory.read(r[SP]);
    r[SP]+=4;
  }
  void onIllegalInstruction(){
    push(STATUS,true);
    push(PC,false);
    c[CAUSE] = ILLEGAL_INSTR;
    c[STATUS] |=MASK_INTERRUPT;
    r[PC] = c[HANDLER];
  }
  //periferija
  bool write_term_in = false;
  struct termios config;
  struct termios config_old;
  int fd_termin;
  //tajmer
  //steady_clock::time_point cur;
  double interruptThreshold = 0.5;
  double elapsed = 0;
  unsigned long cycleTime = 0;
  int checkTimer(){return 0;}
  int checkConsole(){return 0;}
  CPU(){
   }
  static CPU* inst;
  public:
   void CPUcycle();
   void start(){
        fd_termin = open(ttyname(STDIN_FILENO), O_RDWR | O_NONBLOCK);
    tcgetattr(fd_termin, &(this->config_old));
        config.c_iflag = 0;
        config.c_oflag = 0;
        config.c_cflag = CS8|CREAD|CLOCAL;
        config.c_lflag = 0;
        config.c_cc[VMIN]=1;
        config.c_cc[VTIME]=0;
        tcsetattr(fd_termin,TCSANOW,&config);
       // cur = steady_clock::now();
        CPUcycle();
   }
   bool isFinshed() const{return finished;}
    static CPU* Instance(){
      if(inst==nullptr){
        inst = new CPU();
      }
      return inst;   
    }
  void loadMemory(const char* filename);
    void onFinish(){
    tcsetattr(fd_termin,TCSANOW,&config_old);
    tcflush(fd_termin,TCIFLUSH);
    std::cout << "\n-----------------------------------------------------------------" << std::endl;
    std::cout << "Emulated processor executed halt instruction" << std::endl;
    std::cout << "Emulated processor state:" << std::endl;
    for(int i=0;i<16;i++){
      if(i%4==0)std::cout << std::endl;
      if(i<10)std::cout << " ";
      std::cout << "r" << std::dec <<  i;
      std::cout <<  "=0x" << std::setfill('0')<< std::setw(8) << std::hex << r[i] << "\t";
    }
    std::cout << std::endl;
  }
};
class EmulatorException : public std::exception{
    private:
   std::string info;
  public:
   EmulatorException(std::string s):info(s){};
  friend std::ostream& operator<< (std::ostream& o,const EmulatorException& e){
    o << "Greska pri emuliranju: " + e.info;
    return o;
  }
};
#endif