#include "../../inc/linker/Linker.hpp"
#include <iostream>
#include <string.h>
int main(int argc,const char** argv){
  std::cout << "Linker se pokrece..." << std::endl;
  const char* out = nullptr;
  bool inputFiles = false;
  for(int i = 1;i<argc;i++){
    if(strcmp(argv[i],"-o")==0){
      if(out==nullptr && !inputFiles){
        out = argv[i+1];i++;
      }else{
        std::cout << "Nacin koriscenja: [-o <naziv izlaza>] {-place=<ime_sekcije>@<adresa>} <ulazne datoteke>" << std::endl;
        exit(-1);
      }
    }
    else if(strstr(argv[i],"-place")==argv[i]){
      if(inputFiles){
         std::cout << "Nacin koriscenja: [-o <naziv izlaza>] {-place=<ime_sekcije>@<adresa>} <ulazne datoteke>" << std::endl;
        exit(-1);
      }
      Linker::Instance()->insertPlace(argv[i]);
    }else if(argv[i][0]=='-'){
            if(inputFiles){
         std::cout << "Nacin koriscenja: [-o <naziv izlaza>] {-place=<ime_sekcije>@<adresa>} <ulazne datoteke>" << std::endl;
        exit(-1);
      }
      Linker::Instance()->setRegime(argv[i]);
    }
    else{
      if(!Linker::Instance()->isDefined()){
        std::cout << "Nacin koriscenja: [-o <naziv izlaza>] {-place=<ime_sekcije>@<adresa>} <ulazne datoteke>" << std::endl;
        exit(-1);
      }
      inputFiles = true;
      Linker::Instance()->loadFile(argv[i]);
    }
  }
    if(out!=nullptr)  std::cout << "Kreira se fajl " << out << std::endl;
    Linker::Instance()->mapSections();
    Linker::Instance()->mapSyms();
    Linker::Instance()->resolveRelocs();
    //Linker::Instance()->printRes(std::cout);
    if(out!=nullptr){
      Linker::Instance()->finish(out);
      std::cout << "Zavrseno je linkovanje fajla " << out << std::endl;
    }
}