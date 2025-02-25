#ifndef _INSTRUCTION_HPP_
#define _INSTRUCTION_HPP_
typedef unsigned char uint8;
typedef unsigned short uint16;
class SectionInfo;
class Instruction{
  private:
    uint8 instruction[8] ;
  public:
    Instruction(uint8 oc,uint8 mod,uint8 regA,uint8 regB,uint8 regC,uint16 disp){
        instruction[0]=oc;
        instruction[1]=mod;
        instruction[2]=regA;
        instruction[3]=regB;
        instruction[4]=regC;
        instruction[5]=disp & 0xF;
        instruction[6]=(disp>>8) & 0xF;
        instruction[7]=(disp>>4) & 0xF;
    }
    uint8 getOc() const{return instruction[0];}
    uint8 getMod() const{return instruction[1];}
    uint8 getregA() const{return instruction[2];}
    uint8 getregB() const{return instruction[3];}
    uint8 getregC() const{return instruction[4];}
    uint16 getDisp() const{return instruction[5] + (instruction[7]<<4) + (instruction[6])<<8;}
    void setOc(uint8 oc){instruction[0]=oc;}
    void setMod(uint8 mod){instruction[1]=mod;}
    void setregA(uint8 regA){instruction[2]=regA;}
    void setregB(uint8 regB){instruction[3]=regB;}
    void setregC(uint8 regC){instruction[4]=regC;}
    void setDisp(uint16 disp){instruction[5]=disp & 0xF;//disp < 2 ^ 11
        instruction[6]=(disp>>8) & 0xF;
        instruction[7]=(disp>>4) & 0xF;}
    void insertIntoSection(SectionInfo* s,unsigned long lc,bool insert=false);
};

#endif