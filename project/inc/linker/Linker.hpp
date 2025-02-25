#ifndef _LINKER_HPP_
#define _LINKER_HPP_
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include "FileInfo.hpp"
class LinkerException : public std::exception{
  private:
   std::string file;
   std::string info;
  public:
   LinkerException(std::string file,std::string info){
    this->file=file;
    this->info= info;
  }
  friend std::ostream& operator<< (std::ostream& o,const LinkerException& e);
};
class Linker{
public:
    enum Regime{
      UNDEFINED=0,RELOCATABLE,HEX
    };
    static Linker* Instance(){
      if(inst==nullptr){
        inst = new Linker();
      }
      return inst;   
    }
    void insertPlace(std::string str){
      unsigned long eq = str.find_first_of("=",0);
      unsigned long at = str.find_first_of("@",0);
      if(eq==std::string::npos || eq==std::string::npos || eq > at){
        std::cout << "Nacin koriscenja: [-o <naziv izlaza>] {-place=<ime_sekcije>@<adresa>} <ulazne datoteke>" << std::endl;
        exit(-1);
      }
      std::string section = str.substr(eq+1,at-eq-1);
      unsigned long val = strtol(str.substr(at+1,sizeof(str)-at).c_str(),NULL,16);
      //std::cout << section << " " << std::hex << val ;
      if(section!="#implicit" && section.find_first_not_of("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890_")!= std::string::npos){
        std::cout << "Nacin koriscenja: [-<tip fajla>] [-o <naziv izlaza>] {-place=<ime_sekcije>@<adresa>} <ulazne datoteke>" << std::endl;
        exit(-1);
      }
      places[val] = section;
    }
    void loadFile(std::string file){
      try{
        FileInfo* f = new FileInfo(file);
        //std::cout << *f;
        files.push_back(f);
      }catch(LinkerException* e){
        std::cout << *e;
        exit(-1);
      }
    }
    void setRegime(std::string s){
      if(regime != UNDEFINED){
        std::cout << "Nacin koriscenja: [-<tip fajla>] [-o <naziv izlaza>] {-place=<ime_sekcije>@<adresa>} <ulazne datoteke>" << std::endl;
        exit(-1);
      }
      for(int i=0;i<strs->size();i++){
        if(s==strs[i]){regime = (Regime)(i+1);return;}
      }
    }
    bool isDefined(){
      return regime !=UNDEFINED;
    }
    int mapSections();
    int mapSyms();
    int resolveRelocs();
    void printRes(std::ostream& os);
    int finish(const char* file){
      try{
      switch(regime){
        case HEX:{
          createHexFile(file);
        }break;
        case RELOCATABLE:{
          createObjFile(file);
        }break;
        default:{
          throw new LinkerException("","vtf");
        }
      }
    }catch(LinkerException* e){
      std::cout << *e;exit(-1);
    }
    return 0;
    }
private:
    const std::string magic = "SSPROJF";
    unsigned long filelen = 0;
    static Linker* inst;
    static const std::string strs[];
    std::vector<FileInfo*> files;
    Regime regime = UNDEFINED;
    std::map<unsigned long,std::string> places;//tenk ju c++ standard
    std::vector<SymbolTableElement*> newst;
    std::map<std::string,FileInfo::SectionInfo> newsectionInfos;
    Linker(){newst.push_back(new SymbolTableElement());};
    SymbolTableElement* find(std::string name);
    void createHexFile(const char *file);
    void createHexBin(const char* file);
    void createHexTxt(const char* file);
    void createObjFile(const char *file);
    ~Linker();
};
#endif