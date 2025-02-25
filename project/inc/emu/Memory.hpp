#ifndef _MEMORY_HPP_
#define _MEMORY_HPP_
class Memory{//tri nivoa po 8 bita adrese
public:
static const unsigned TABLE_CNT = 256;
static const unsigned BLOCK_LEN = 256;
static const unsigned TERM_OUT = 0xFFFFFF00;
static const unsigned TERM_INT = 0xFFFFFF04;
static const unsigned TIM_CFG = 0xFFFFFF10;
struct PageDescriptorTable;
struct MemoryBlock;
struct PageDescriptor{
  PageDescriptorTable* nextDesc = nullptr;//mora se konvertovati ili u pokazivac na memorijski blok ili na tabelu deskriptora
  MemoryBlock* nextBlock = nullptr;
    ~PageDescriptor(){
      if(nextDesc)delete nextDesc;
      if(nextBlock)delete nextBlock;
      nextDesc =nullptr; nextBlock = nullptr;
    }
};
struct PageDescriptorTable{
  PageDescriptor** descriptors;
  PageDescriptorTable(){
    descriptors = new PageDescriptor*[TABLE_CNT];
    for(unsigned long i=0;i<TABLE_CNT;i++)descriptors[i] = new PageDescriptor();
  }
  ~PageDescriptorTable(){for(unsigned long i=0;i<TABLE_CNT;i++)delete descriptors[i];delete[] descriptors;}
};
struct MemoryBlock{
  unsigned char* data;
  MemoryBlock(){
    data = new unsigned char[BLOCK_LEN];
    for(unsigned long i=0;i<BLOCK_LEN;i++)data[i]=0;
  }
  ~MemoryBlock(){delete[] data;data = nullptr;}
};
private:
  PageDescriptorTable* start;
  MemoryBlock* Traverse(unsigned addr){
    PageDescriptorTable* cur = start;
    for(int i=0;i<2;i++){
      unsigned ind = (addr >> ((3-i)*8)) & 0xFF;
      PageDescriptor* pd = (cur->descriptors[ind]);
      if(pd->nextDesc==nullptr){
        PageDescriptorTable* pt = new PageDescriptorTable();
        pd->nextDesc = pt;
      }
      cur = (PageDescriptorTable*)pd->nextDesc;
    }
    unsigned ind =(addr>>8) & 0xFF;
    PageDescriptor* pd = (cur->descriptors[ind]);
    if(pd->nextBlock==nullptr){
      MemoryBlock* mb = new MemoryBlock();
      pd->nextBlock = mb;
    }
      return pd->nextBlock;
  }
  unsigned char readByte(unsigned addr){
    MemoryBlock* mb = Traverse(addr);
    return mb->data[addr & 0xFF];
  }
  void writeByte(unsigned addr,unsigned char res){
    MemoryBlock* mb = Traverse(addr);
    mb->data[addr & 0xFF] = res;
  }
public:
  Memory(){
    start = new PageDescriptorTable();
  }
  unsigned read(unsigned addr){
    unsigned val= 0;
    for(unsigned i=0;i<4;i++){
      val |= readByte(addr + i) << (i*8);
    }
    return val;
  }

  void write(unsigned addr,unsigned val){
    for(unsigned i=0;i<4;i++){
      writeByte(addr + i,(val >> (8*i))&0xFF); 
    }
  }
  ~Memory(){
    delete start;
  }
  void loadSection(unsigned sadd,unsigned len,unsigned char* data);
};
#endif