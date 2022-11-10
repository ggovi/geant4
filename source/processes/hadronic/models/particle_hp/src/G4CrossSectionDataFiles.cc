//
//
// G. Govi, October 2022
//
#include "G4CrossSectionDataFiles.hh"
#include <filesystem>
#include <algorithm>

namespace crossSectionDataSet {
  size_t getTempDataSets( const std::string& topFolder, std::vector<std::string>& values ){
    size_t sz = 0;
    for (const auto &f : std::filesystem::directory_iterator(topFolder)){
      std::string folderName = f.path().filename();
      values.push_back(folderName);
      sz++;
    }
    return sz;
  }
}

CrossSectionDataIOVSet::CrossSectionDataIOVSet():
  theIOVs(){
}
  
CrossSectionDataIOVSet::~CrossSectionDataIOVSet(){}

int CrossSectionDataIOVSet::add( const std::string& leaf ){
  int since = 0;
  if(!leaf.empty()){
    since = std::stoi(leaf);
    if(since) since -= iovHalfSize;
  }
  theIOVs.push_back( since );
  return since;
}
  
void CrossSectionDataIOVSet::add( int k ){
  theIOVs.push_back( k );
}  

void CrossSectionDataIOVSet::sort(){
  std::sort(theIOVs.begin(),theIOVs.end());
}

#include <iostream>
int CrossSectionDataIOVSet::find( G4double temperature ){
  if(!theIOVs.size()) return 0;
  int target = std::floor(temperature);
  auto p = std::upper_bound( theIOVs.begin(), theIOVs.end(), target );
  if( p != theIOVs.begin()){
    p -= 1;
  } else {
    p = theIOVs.end();
  }
  if( p == theIOVs.end() ) return -1;
  int ret = *p;
  if( isTempDependentSet() && (target-ret) > 2*iovHalfSize ) return -1;
  return ret;
}

bool CrossSectionDataIOVSet::isTempDependentSet(){
  if( theIOVs.size() == 0 ) return false;
  if( theIOVs.size() == 1  && theIOVs[0]==0 ) return false;
  return true;
}

void CrossSectionDataIOVSet::clear(){
  theIOVs.clear();
}







  
