#include "../../inc/asm/LiterralTableElement.hpp"
#include <iomanip>

std::ostream& operator<< (std::ostream& os,const LiterralTableElement& s){
  if(s.defined){
    os << s.value << "\t" << std::setfill('0')<< std::setw(8) << std::hex << s.literralAdress << " " << s.trueLiterral << "\t";
  }else{
    os << s.value << "\t" << "********" << " " << s.trueLiterral << "\t ";
  }
  for(int i=0;i<s.needLocations.size();i++){
    os << std::setfill('0')<< std::setw(8) << std::hex << s.needLocations[i];
    if(i != s.needLocations.size()-1)os << ",";
  }
  return os;
}