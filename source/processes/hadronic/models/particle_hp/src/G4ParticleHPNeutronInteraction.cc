//
//
// G. Govi, August 2022
//
#include "G4ParticleHPNeutronInteraction.hh"
#include "G4ParticleHPManager.hh"
#include "G4Threading.hh"

G4ParticleHPNeutronInteractionData::G4ParticleHPNeutronInteractionData( const G4String& modelName,
									const G4String& dataSetFolder ):
  theModelName( modelName ),
  theDataSetFolder(dataSetFolder),
  theManagerProxy( nullptr ),
  theIOVs() {
}

G4ParticleHPNeutronInteractionData::~G4ParticleHPNeutronInteractionData(){
  //if ( ! G4Threading::IsMasterThread() ) {
  if ( ! G4Threading::IsWorkerThread() ) {
    if ( theInteraction != nullptr ) {
      for (auto im: *theInteraction){
  	for ( std::vector<G4ParticleHPChannel*>::iterator ite = im.second.begin(); ite != im.second.end() ; ite++ ) {
  	  delete *ite;
  	}
      }
      theInteraction->clear();
    }
    // !!!! This causes a crash when enabled (double delete?)... to be investigated 
    //delete theInteraction;
  }
}

void G4ParticleHPNeutronInteractionData::setupManagerProxy( G4ParticleHPManagerProxyBase* proxy ){
  theManagerProxy = proxy;
}

std::vector<G4ParticleHPChannel*>& G4ParticleHPNeutronInteractionData::selectDataSet( G4double temperature ) {
  // the case of non-initalized class
  if(theInteraction == nullptr) throw G4HadronicException(__FILE__, __LINE__, "The Data Set has not been loaded.");
  // identify the data set (the default case will deliver the key=0 )
  int setId = theIOVs.find( temperature );
  if( setId == -1 ) throw G4HadronicException(__FILE__, __LINE__, "Data for temperature "+std::to_string(temperature)+" is not available");
  auto data = theInteraction->find( setId );
  if( data == theInteraction->end() ) throw G4HadronicException(__FILE__, __LINE__, "Data for temperature "+std::to_string(temperature)+" has not been loaded");
  return data->second;
}

std::vector<G4ParticleHPChannel*>& G4ParticleHPNeutronInteractionData::getDefaultDataSet() {
  if(theInteraction == nullptr) throw G4HadronicException(__FILE__, __LINE__, "The Data Set has not been loaded.");
  return theInteraction->begin()->second;
}

#include <filesystem>
void G4ParticleHPNeutronInteractionData::BuildPhysicsTable( G4ParticleHPFinalState* templateFinalState, int ZThreshold )
{
  if(theManagerProxy == nullptr) throw G4HadronicException(__FILE__, __LINE__, "Manager Proxy has not been initialized");

  theInteraction = theManagerProxy->GetInteractionFinalStates();

   if ( G4Threading::IsMasterThread() ) {

     if ( theInteraction == nullptr ) theInteraction = new std::map<int,std::vector<G4ParticleHPChannel*> >;

      if ( numEle == (G4int)G4Element::GetNumberOfElements() ) return;

      if ( theInteraction->size() && (theInteraction->begin()->second.size() == G4Element::GetNumberOfElements()) ) {
         numEle = G4Element::GetNumberOfElements();
         return;
      }

      size_t numberOfDataFolders = 1;
      std::filesystem::path dataDir;
      std::vector<std::string> dataSets;            
      if ( !std::getenv("G4NEUTRONHPDATA")){
	  throw G4HadronicException(__FILE__, __LINE__, "Please setenv G4NEUTRONHPDATA to point to the neutron cross-section files.");
      }
      if(std::getenv("G4NEUTRONHPTEMPDATA")){
	// this is expected for the usage of temperature-specific data sets
	dataDir = std::getenv("G4NEUTRONHPTEMPDATA");
	numberOfDataFolders = crossSectionDataSet::getTempDataSets(dataDir,dataSets);
      } else {
	dataDir = std::getenv("G4NEUTRONHPDATA");
	dataSets.push_back("");
      }

      theIOVs.clear();
      for( size_t j=0; j<numberOfDataFolders; j++ ){
	std::filesystem::path dataSetDir = dataDir;
	std::string subFolder = dataSets[j];
	// append the leaf of the temperature subfolder (if any )
	if(!subFolder.empty()) dataSetDir = dataDir/subFolder;
	dataSetDir = dataSetDir/theDataSetFolder.data();
	int key = theIOVs.add(subFolder);
	auto iInserted = theInteraction->insert(std::make_pair(key,std::vector<G4ParticleHPChannel*>()));
	for ( G4int i = numEle ; i < (G4int)G4Element::GetNumberOfElements() ; i++ ) {
	  iInserted.first->second.push_back( new G4ParticleHPChannel );
	  if(ZThreshold==0 || (*(G4Element::GetElementTable()))[i]->GetZ()> ZThreshold ){
	    ((iInserted.first->second)[i])->Init((*(G4Element::GetElementTable()))[i], dataSetDir.string());
	    ((iInserted.first->second)[i])->Register(templateFinalState);
	  }
	}
      }
      theIOVs.sort();
      theManagerProxy->RegisterInteractionFinalStates( theInteraction );
   } else {
     // retrieve the iovs...
     theIOVs.clear();
     for( auto& iel : *theInteraction ) theIOVs.add( iel.first );
     theIOVs.sort();
   }
   numEle = G4Element::GetNumberOfElements();
}






  
