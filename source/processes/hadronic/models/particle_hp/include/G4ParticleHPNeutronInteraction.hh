//
//
// Class Description ( G4ParticleHPNeutronInteraction )
// Base Class for the Neutron HP Cross Sections
// Supports the loading of temperature-dependent data sets
// A dedicated function serves the selection of the appropriate data set
// Class Description - End

// G. Govi, August 2022
//
#ifndef G4ParticleHPNeutronInteraction_h
#define G4ParticleHPNeutronInteraction_h 1

#include "globals.hh"
#include "G4ParticleHPChannel.hh"
#include "G4HadronicInteraction.hh"

class G4ParticleHPNeutronInteractionData {
  public:
  explicit G4ParticleHPNeutronInteractionData( const G4String& dataSetFolder );
  
  ~G4ParticleHPNeutronInteractionData();
  
  std::vector<G4ParticleHPChannel*>& selectDataSet( G4double temperature );
  std::vector<G4ParticleHPChannel*>& getDefaultDataSet();
  void BuildPhysicsTable( G4ParticleHPFinalState* templateFinalState );
  
  private:
  G4String theDataSetFolder;
  std::map<int,std::vector<G4ParticleHPChannel*> >* theInteraction = nullptr;
  G4int numEle;
  std::vector<int> theIOVs;
};

template <typename FinalState> class G4ParticleHPNeutronInteraction : public G4HadronicInteraction
{
  public: 
  
  G4ParticleHPNeutronInteraction( const G4String& modelName, const G4String& dataSetFolder ): G4HadronicInteraction( modelName) ,
											      theData(dataSetFolder){
  }

  ~G4ParticleHPNeutronInteraction(){
  }

  virtual G4HadFinalState * ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& aTargetNucleus) = 0;

  public:
  void BuildPhysicsTable(const G4ParticleDefinition&){
    std::unique_ptr<G4ParticleHPFinalState> theFS( new FinalState );
    theData.BuildPhysicsTable( theFS.get() );
  }

  std::vector<G4ParticleHPChannel*>& selectDataSet( G4double temperature ){
    return theData.selectDataSet( temperature );
  }

  std::vector<G4ParticleHPChannel*>& getDefaultDataSet(){
    return theData.getDefaultDataSet();
  }
  
  private:
  G4ParticleHPNeutronInteractionData theData;
};

#endif
