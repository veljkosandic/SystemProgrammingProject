#include "../../inc/asm/SectionInfo.hpp"
#include "../../inc/asm/Instruction.hpp"   
//tenk ju c++ funkcija stvari, veri kul 
  void Instruction::insertIntoSection(SectionInfo* s,unsigned long lc,bool insert){
    int x = 0;
    for(int i=6;i>=0;i-=2,x++){
      if(s->getSectionData()->size() > lc + x)
        s->getSectionData()->operator[](lc + x)=((instruction[i+1] | (instruction[i]<<4)) & 0xFF);
      else
        s->getSectionData()->push_back((instruction[i+1] | (instruction[i]<<4)) & 0xFF);
    }
  }