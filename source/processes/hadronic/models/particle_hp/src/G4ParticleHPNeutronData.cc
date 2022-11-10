//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// neutron_hp -- source file
// J.P. Wellisch, Nov-1996
// A prototype of the low energy neutron transport model.
//
// P. Arce, June-2014 Conversion neutron_hp to particle_hp
//
#include "G4ParticleHPNeutronData.hh"
#include "G4PhysicsFreeVector.hh"
#include "G4CrossSectionDataFiles.hh"

G4ParticleHPNeutronData::G4ParticleHPNeutronData()
  : theDataFolderPath( "" )
{
  numEle = G4Element::GetNumberOfElements();
}

void G4ParticleHPNeutronData::Init( G4String dataFolderPath, G4int ZThreshold){
  theDataFolderPath = dataFolderPath;
  theZThreshold = ZThreshold;
  theData.resize( numEle );
  for (G4int i=0; i<numEle; i++){
    theData[i].Init((*(G4Element::GetElementTable()))[i], theDataFolderPath, ZThreshold);
  }
}
  
G4ParticleHPNeutronData::~G4ParticleHPNeutronData(){}

#include <filesystem>
std::vector<std::pair<std::string,G4ParticleHPNeutronData> >* G4ParticleHPNeutronData::MakeXSDataStore(G4String modelName, G4int ZThreshold){
  auto aDataStore = new std::vector<std::pair<std::string,G4ParticleHPNeutronData> >;
  size_t numberOfDataFolders = 1;
  std::filesystem::path dataDir;
  std::vector<std::string> dataSets;            
  if(std::getenv("G4NEUTRONHPTEMPDATA")){
    // this is expected for the usage of temperature-specific data sets
    dataDir = std::getenv("G4NEUTRONHPTEMPDATA");
    numberOfDataFolders = crossSectionDataSet::getTempDataSets(dataDir,dataSets);
  } else {
    if ( !std::getenv("G4NEUTRONHPDATA")){
      throw G4HadronicException(__FILE__, __LINE__, "Please setenv G4NEUTRONHPDATA to point to the neutron cross-section files.");
    }
    dataDir = std::getenv("G4NEUTRONHPDATA");
    dataSets.push_back("");
  }
  aDataStore->resize(numberOfDataFolders);
  for( size_t j=0; j<numberOfDataFolders; j++ ){
    std::string subFolder = dataSets[j];
    // append the leaf of the temperature subfolder (if any )
    std::filesystem::path dataSetDir = dataDir;
    if(!subFolder.empty()) dataSetDir = dataDir/subFolder;
    dataSetDir /= modelName.data();
    (*aDataStore)[j].first = subFolder;
    (*aDataStore)[j].second.Init( dataSetDir.string(), ZThreshold );
  }
  return aDataStore;
}

  G4PhysicsVector * G4ParticleHPNeutronData::DoPhysicsVector(G4ParticleHPVector * theVector)
  {
    G4int len = theVector->GetVectorLength();
    if(len==0) {
      return new G4PhysicsFreeVector(0, 0., 0.);
    }
    G4double emin = theVector->GetX(0);
    G4double emax = theVector->GetX(len-1);
    
   //  G4int dummy; G4cin >> dummy;     
    G4PhysicsFreeVector * theResult = new G4PhysicsFreeVector(len, emin, emax);
    for (G4int i=0; i<len; i++)
    {
      theResult->PutValues(i, theVector->GetX(i), theVector->GetY(i));
    }
    return theResult;
  }

void G4ParticleHPNeutronData::addPhysicsVector()
{
   for ( G4int i = numEle; i < (G4int)G4Element::GetNumberOfElements() ; i++ )
   {
      theData.emplace_back();
      theData[i].Init((*(G4Element::GetElementTable()))[i], theDataFolderPath,theZThreshold);
   }
   numEle = G4Element::GetNumberOfElements();
}
