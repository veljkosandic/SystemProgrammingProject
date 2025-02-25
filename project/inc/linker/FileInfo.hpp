#ifndef _FILE_INFO_H_
#define _FILE_INFO_H_
#include "../common/SymbolTableElement.hpp"
#include "../common/RelocTableElement.hpp"
#include <vector>
#include <utility>
#include <map>
class FileInfo{
  public:
  struct SectionInfo{
    std::vector<RelocTableElement*> relocTable;
    std::vector<unsigned char> data;
    unsigned long offset=0;
    unsigned long dataoff = 0;
    unsigned long relocoff = 0;
  };
  private:
  std::vector<SymbolTableElement*> st;
  std::map<unsigned long,SectionInfo> sectionInfo;
  std::string filename;
  public:
  FileInfo(std::string filename);
  friend std::ostream& operator<<(std::ostream& os,const FileInfo& f);
  std::string getFilename() const{return filename;}
  std::vector<SymbolTableElement*>& getSymbolTable(){return st;}
  std::vector<RelocTableElement*>& getRelocTable(unsigned long id){return sectionInfo[id].relocTable;}
  std::vector<unsigned char>& getData(unsigned long id){return sectionInfo[id].data;}
  unsigned long getOffset(unsigned long id) {return sectionInfo[id].offset;}
  unsigned long setOffset(unsigned long id,unsigned long val) {return sectionInfo[id].offset=val;}
  std::vector<unsigned long> getSectionNdxs();
  std::string getName(unsigned long id) {return st[id]->getName();}
  FileInfo(const FileInfo&) = delete;
  FileInfo(FileInfo&&) = delete;
  FileInfo& operator=(const FileInfo&) = delete;
  FileInfo& operator=(FileInfo&&) = delete;
  ~FileInfo();
};
#endif