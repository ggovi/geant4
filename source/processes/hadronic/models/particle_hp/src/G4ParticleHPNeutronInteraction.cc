//
//
// G. Govi, August 2022
//
#include "G4ParticleHPNeutronInteraction.hh"
#include "G4ParticleHPManager.hh"
#include "G4Threading.hh"

G4ParticleHPNeutronInteractionData::G4ParticleHPNeutronInteractionData( const G4String& dataSetFolder ): theDataSetFolder(dataSetFolder), theIOVs() {
}

G4ParticleHPNeutronInteractionData::~G4ParticleHPNeutronInteractionData(){
  if ( ! G4Threading::IsWorkerThread() ) {
    if ( theInteraction != nullptr ) {
      for (auto im: *theInteraction){
	for ( std::vector<G4ParticleHPChannel*>::iterator ite = im.second.begin(); ite != im.second.end() ; ite++ ) {
	  delete *ite;
	}
      }
      theInteraction->clear();
    }
  }
}

std::vector<G4ParticleHPChannel*>& G4ParticleHPNeutronInteractionData::selectDataSet( G4double temperature ) {
  // the case of non-initalized class
  if(theInteraction == nullptr) throw G4HadronicException(__FILE__, __LINE__, "The Data Set has not been loaded.");
  // identify the data set (the default case will deliver the key=0 )
  int target = std::floor(temperature);
  auto p = std::upper_bound( theIOVs.begin(), theIOVs.end(), target );
  if( p != theIOVs.begin()){
    p -= 1;
  } else {
    p = theIOVs.end();
  }
  if( p == theIOVs.end() ) throw G4HadronicException(__FILE__, __LINE__, "Data for temperature "+std::to_string(temperature)+" is not available");
  auto data = theInteraction->find( *p );
  if( data == theInteraction->end() ) throw G4HadronicException(__FILE__, __LINE__, "Data for temperature "+std::to_string(temperature)+" has not been loaded");
  return data->second;
}

std::vector<G4ParticleHPChannel*>& G4ParticleHPNeutronInteractionData::getDefaultDataSet() {
  if(theInteraction == nullptr) throw G4HadronicException(__FILE__, __LINE__, "The Data Set has not been loaded.");
  return theInteraction->begin()->second;
}

#include <filesystem>
size_t getTempDataSets( const std::string& topFolder, std::vector<std::string>& values ){
  size_t sz = 0;
  for (const auto &f : std::filesystem::directory_iterator(topFolder)){
    std::string folderName = f.path().filename();
    // it may be required to make a numeric-wise sort  
    values.push_back(folderName);
    sz++;
  }
  return sz; 
}
void G4ParticleHPNeutronInteractionData::BuildPhysicsTable( G4ParticleHPFinalState* templateFinalState ){
  
  G4ParticleHPManager* hpmanager = G4ParticleHPManager::GetInstance();
  
  theInteraction = hpmanager->GetCaptureFinalStates();
  
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
    if ( !std::getenv("G4NEUTRONHPDATA") ){
      if( !std::getenv("G4NEUTRONHPTEMPDATA") ){
	throw G4HadronicException(__FILE__, __LINE__, "Please setenv G4NEUTRONHPDATA or G4NEUTRONHPTEMPDATA to point to the neutron cross-section files.");
      } else {
	// this is expected for the usage of temperature-specific data sets
	dataDir = std::getenv("G4NEUTRONHPTEMPDATA");
	dataDir.concat( theDataSetFolder.data() );
	numberOfDataFolders = getTempDataSets(dataDir,dataSets);
      }
    } else {
      dataDir = std::getenv("G4NEUTRONHPDATA");
      dataDir.concat( theDataSetFolder.data() );
      dataSets.push_back("");
    }

    theIOVs.resize( numberOfDataFolders );
    for( size_t j=0; j<numberOfDataFolders; j++ ){
      std::string subFolder = dataSets[j];
      // append the leaf of the temperature subfolder (if any )
      if(!subFolder.empty()) dataDir /= subFolder;
      // ... and set the corresponding key for the map
      int key = subFolder.empty()? 0: std::stoi(subFolder);
      theIOVs[j]=key;
      auto iInserted = theInteraction->insert(std::make_pair(key,std::vector<G4ParticleHPChannel*>()));
      for ( G4int i = numEle ; i < (G4int)G4Element::GetNumberOfElements() ; i++ ) 
	{
	  iInserted.first->second.push_back( new G4ParticleHPChannel );
	  ((iInserted.first->second)[i])->Init((*(G4Element::GetElementTable()))[i], dataDir.string());
	  ((iInserted.first->second)[i])->Register(templateFinalState);
	}
    }
    hpmanager->RegisterCaptureFinalStates( theInteraction );
  } else {
    // retrieve the iovs...
    theIOVs.clear();
    for( auto& iel : *theInteraction ) theIOVs.push_back( iel.first );
  }
  numEle = G4Element::GetNumberOfElements();

}
  
