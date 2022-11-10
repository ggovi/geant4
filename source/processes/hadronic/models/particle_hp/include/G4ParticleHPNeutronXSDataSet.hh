//
//
// Class Description ( G4ParticleHPNeutronXSDataSet )
// Base Class for the Neutron HP Cross Sections
// Supports the loading of temperature-dependent data sets
// A dedicated function serves the selection of the appropriate data set
// Class Description - End

// G. Govi, September 2022
//
#ifndef G4ParticleHPNeutronXSDataSet_h
#define G4ParticleHPNeutronXSDataSet_h 1

#include "G4VCrossSectionDataSet.hh"
#include "G4CrossSectionDataFiles.hh"
#include "G4ParticleHPNeutronData.hh"

#include "G4ParticleHPManager.hh"

class G4PhysicsVector;
class G4PhysicsTable;
class G4Element;

// Callback functions for interaction-specific actions on the generic cached dataset
//class G4ParticleHPDataProxy: public G4ParticleHPManagerProxyBase {
//public:
//  virtual ~G4ParticleHPDataProxy(){}
//  virtual std::vector<std::pair<std::string,std::unique_ptr<G4ParticleHPData> > >& CrossSectionDataStore() = 0;
//  virtual G4PhysicsVector* MakePhysicsVector(G4ParticleHPData& fromData, G4Element * element) = 0;
//};
class G4ParticleHPManagerProxyBase;

// Base class for Neutron Cross Sections
class G4ParticleHPNeutronXSDataSet : public G4VCrossSectionDataSet {
public:
  G4ParticleHPNeutronXSDataSet(G4String modelLabel, G4String modelName);
  ~G4ParticleHPNeutronXSDataSet();

  G4int GetVerboseLevel() const;
  void SetVerboseLevel( G4int newValue );
  void BuildPhysicsTable(const G4ParticleDefinition& ap);
  void DumpPhysicsTable(const G4ParticleDefinition& aP);
  G4bool IsIsoApplicable( const G4DynamicParticle* , G4int /*Z*/ , G4int /*A*/ ,
			  const G4Element* /*elm*/ , const G4Material* /*mat*/ );
  G4double GetCrossSection(const G4DynamicParticle* aP, const G4Element*anE, G4double aT);
  G4double GetIsoCrossSection( const G4DynamicParticle* dp , G4int /*Z*/ , G4int /*A*/ ,
                               const G4Isotope* /*iso*/  , const G4Element* element , const G4Material* material );
protected:
  void setDataStore( std::vector<std::pair<std::string,G4ParticleHPNeutronData > >& dataStore );
  void setCallback( G4ParticleHPManagerProxyBase* mgrProxy );
  void setParams( int zthreshold, float cutFactor, int maxFails=0 );
private:
  // required to identify the process-specific folder in the data directory
  G4String theModelName;
  // callback (owned by the derived class)
  G4ParticleHPManagerProxyBase* theMgrProxy = nullptr;
  // process-specific params
  int theZThreshold = 0;
  float theCutFactor = 0.03;
  int theMaxFails = 0;
  // reference to the static data cache
  std::vector<std::pair<std::string,G4ParticleHPNeutronData > >* theDataStoreRef = nullptr;
  // process-specific references poiting to the static data cache
  std::map<int,G4PhysicsTable* >* theData = nullptr;
  // intervals of validity for temperature-specific data 
  CrossSectionDataIOVSet theIOVs;
  // instance local cache items
  G4double ke_cache;
  G4double xs_cache;
  const G4Element* element_cache;
  const G4Material* material_cache;

};

#endif
