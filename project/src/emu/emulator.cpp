#include "../../inc/emu/CPU.hpp"
int main(int argc,const char** argv){
  if(argc!=2){
    std::cout << "Nacin koriscenja: emulator <naziv_hex_datoteke>"<< std::endl;
    exit(-1);
  }
  CPU::Instance()->loadMemory(argv[1]);
  CPU::Instance()->start();
  CPU::Instance()->onFinish();
  return 0;
}