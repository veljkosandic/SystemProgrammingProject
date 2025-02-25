#include "../../inc/emu/Memory.hpp"
void Memory::loadSection(unsigned saddr,unsigned len,unsigned char* data){
  MemoryBlock* block = Traverse(saddr);
  unsigned long mask = 0xFF;
  for(unsigned long addr=saddr,i=0;addr<saddr+len;addr++,i++){
    unsigned index = addr & mask;
    block->data[index]=data[i];
    if((index)==0xFF)
      block = Traverse(addr+1);
  }
}