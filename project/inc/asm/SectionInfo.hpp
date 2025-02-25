#ifndef _SECTION_INFO_HPP_
#define _SECTION_INFO_HPP_
#include <vector>
#include "RelocTable.hpp"
#include "LiterralTable.hpp"
class SectionInfo{
  private:
    unsigned long sectionId;//id koji se nalazi u tabeli simbola
    std::string sectionName;
    std::vector<unsigned char>* data;
    RelocTable* reloctable;
    LiterralTable* littable;
    unsigned long dataoff = 0;
    unsigned long relocoff = 0;
    unsigned const MAX_OFFSET = 2 << 11 - 1;
    static const unsigned long SECTIONHEADERSIZE = 2*sizeof(unsigned) +2 * sizeof(unsigned long);
  public:
    SectionInfo(unsigned long sectionId,std::string sectionName){
      this->sectionId=sectionId;
      this->sectionName=sectionName;
      reloctable = new RelocTable();
      data = new std::vector<unsigned char>();
      littable = new LiterralTable();
    }
    unsigned long getId() const{return sectionId;}
    const std::string getName() const{return sectionName;}
    std::vector<unsigned char>* getSectionData() const{return data;}
    RelocTable* getRelocTable() const {return reloctable;}
    friend std::ostream& operator<< (std::ostream& os,const SectionInfo& s);
    LiterralTable* getLiterralTable() const{return littable;}
    void insertIntoData(unsigned num,unsigned saddr);
    void resolveLiterralTable();
    void writeData(std::ofstream& os);
    void writeRelocTable(std::ofstream& os);
    void writeSectionHeaderInfo(std::ostream& os);
    ~SectionInfo(){
      delete reloctable;
      delete data;
      delete littable;
    }

};
#endif