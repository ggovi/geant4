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

class G4ParticleHPManagerProxyBase {
public:
  virtual ~G4ParticleHPManagerProxyBase(){}
  virtual void RegisterInteractionFinalStates( std::map<int,std::vector<G4ParticleHPChannel*> >* val ) = 0;
  virtual std::map<int,std::vector<G4ParticleHPChannel*> >* GetInteractionFinalStates() = 0;
};

class G4ParticleHPNeutronInteractionData {
  public:
  G4ParticleHPNeutronInteractionData( const G4String& modelName, const G4String& dataSetFolder );
  
  ~G4ParticleHPNeutronInteractionData();

  void setupManagerProxy( G4ParticleHPManagerProxyBase* proxy );  
  
  std::vector<G4ParticleHPChannel*>& selectDataSet( G4double temperature );
  std::vector<G4ParticleHPChannel*>& getDefaultDataSet();
  void BuildPhysicsTable( G4ParticleHPFinalState* templateFinalState, int ZThreshold );
  
  private:
  G4String theModelName;
  G4String theDataSetFolder;
  G4ParticleHPManagerProxyBase* theManagerProxy;
  std::map<int,std::vector<G4ParticleHPChannel*> >* theInteraction = nullptr;
  G4String dirName;
  G4int numEle = 0;
  std::vector<int> theIOVs;
};

template <typename FinalState> class G4ParticleHPNeutronInteraction : public G4HadronicInteraction
{
  public:
  static const int Z_THRESHOLD;
  
  public: 
  
  G4ParticleHPNeutronInteraction( const G4String& modelName, const G4String& dataSetFolder ): G4HadronicInteraction( modelName),
											      theData(modelName, dataSetFolder){
  }

  ~G4ParticleHPNeutronInteraction(){
  }

  void setupManagerProxy( G4ParticleHPManagerProxyBase* proxy ){
    theData.setupManagerProxy( proxy );
  }

  virtual G4HadFinalState * ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& aTargetNucleus) = 0;

  public:
  void BuildPhysicsTable(const G4ParticleDefinition&){
    std::unique_ptr<G4ParticleHPFinalState> theFS( new FinalState );
    theData.BuildPhysicsTable( theFS.get(), Z_THRESHOLD );
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

template <typename FinalState> const int G4ParticleHPNeutronInteraction<FinalState>::Z_THRESHOLD=0;

#endif
