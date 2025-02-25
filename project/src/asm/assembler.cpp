#include <iostream>
#include <string.h>
#include <fstream>
#include "../../inc/asm/Assembly.hpp"
int main(int argc,const char** argv){
  std::cout << "Asembler se pokrece..." << std::endl;
  if((argc !=2 && argc !=4) || (argc ==2 && strcmp(argv[1],"-o")==0) || (argc == 4 && strcmp(argv[1],"-o")!=0)){
    std::cout << "Nacin koriscenja: ./asembler [ -o izlazna_datoteka ] ulazna_datoteka" << std::endl;
    return -2;
  }
  const char* src = (argc>2) ? argv[3] : argv[1];
  std::cout << "Vrsi se asembliranje fajla " << src << std::endl;
  //std::cout << "Pocinje prvi prolazak asemblera..." << std::endl;
  Assembly::Instance()->firstPassAnalysis(src);
  Assembly::Instance()->solveEqus();
  Assembly::Instance()->backpatching();
  Assembly::Instance()->completeLiterralTable();
  if(argc==4)
    Assembly::Instance()->assembleFile(argv[2]);
  std::cout << "Zavrseno je asembliranje fajla " << src << std::endl;  
  return 0;
}