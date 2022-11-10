//
//
// Class Description ( CrossSectionDataIOVSet )
// Utility Class for the handling of Neutron HP Cross Sections Data Files
// Supports the selection of the appropriate data set per temperature
// Class Description - End

// G. Govi, October 2022
//
#ifndef G4CrossSectionDataFiles_h
#define G4CrossSectionDataFiles_h 1

#include "G4Types.hh"
#include <vector>

// scan the top level data folder to identify the temperature-dependent data sets
namespace crossSectionDataSet {
  size_t getTempDataSets( const std::string& topFolder, std::vector<std::string>& values );
}

// interval of validity collections for the temperature-specific data sets
class CrossSectionDataIOVSet {
public:
  static const int iovHalfSize=10;
public:
  CrossSectionDataIOVSet();
  
  ~CrossSectionDataIOVSet();

  int add( const std::string& leaf );
  void add( int k );

  void sort();

  int find( G4double temperature );

  bool isTempDependentSet();

  void clear();

private:
  std::vector<int> theIOVs;
};

#endif
