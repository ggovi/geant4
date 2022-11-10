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
// 070618 fix memory leaking by T. Koi
// 071002 enable cross section dump by T. Koi
// 081024 G4NucleiPropertiesTable:: to G4NucleiProperties::
// 081124 Protect invalid read which caused run time errors by T. Koi
// 100729 Add safty for 0 lenght cross sections by T. Ko
// P. Arce, June-2014 Conversion neutron_hp to particle_hp
//
#include "G4ParticleHPFissionData.hh"
#include "G4ParticleHPManager.hh"
//#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
//#include "G4Neutron.hh"
//#include "G4ElementTable.hh"
//#include "G4ParticleHPData.hh"
//#include "G4ParticleHPManager.hh"
//#include "G4HadronicParameters.hh"
//#include "G4Pow.hh"

class G4ParticleHPFissionDataProxy: public G4ParticleHPManagerProxyBase {
public:
  G4ParticleHPFissionDataProxy(){}
  void RegisterCrossSections( std::map<int,G4PhysicsTable *>* theCrossSections ) override {
    G4ParticleHPManager* hpmanager = G4ParticleHPManager::GetInstance();
    hpmanager->RegisterFissionCrossSections( theCrossSections );
  }
  std::map<int,G4PhysicsTable *>* GetCrossSections() override {
    G4ParticleHPManager* hpmanager = G4ParticleHPManager::GetInstance();
    return hpmanager->GetFissionCrossSections();
  }
};

G4ParticleHPFissionData::G4ParticleHPFissionData() 
   :G4ParticleHPNeutronXSDataSet("NeutronHPFissionXS",MODEL_NAME),
   theMgrProxy()
{
   theMgrProxy.reset( new G4ParticleHPFissionDataProxy );
   setDataStore( dataStore() );
   setCallback( theMgrProxy.get() );
   setParams( Z_THRESHOLD, CUT_FACTOR );
   SetMinKinEnergy( 0*MeV );                                   
   SetMaxKinEnergy( 20*MeV );                                   

   //BuildPhysicsTable(*G4Neutron::Neutron());
}
   
G4ParticleHPFissionData::~G4ParticleHPFissionData()
{
}

std::vector<std::pair<std::string,G4ParticleHPNeutronData > >& G4ParticleHPFissionData::dataStore(){
  static G4ThreadLocal std::unique_ptr<std::vector<std::pair<std::string,G4ParticleHPNeutronData > > > theXSData;
  if ( !theXSData.get() ) theXSData.reset(G4ParticleHPNeutronData::MakeXSDataStore(MODEL_NAME,Z_THRESHOLD-1));  
  return *theXSData;
}

/*
G4bool G4ParticleHPFissionData::IsApplicable(const G4DynamicParticle*aP, const G4Element*)
{
  G4bool result = true;
  G4double eKin = aP->GetKineticEnergy();
  if(eKin>20*MeV||aP->GetDefinition()!=G4Neutron::Neutron()) result = false;
  return result;
}
*/

void G4ParticleHPFissionData::CrossSectionDescription(std::ostream& outFile) const
{
   outFile << "High Precision cross data based on Evaluated Nuclear Data Files (ENDF) for induced fission reaction of neutrons below 20MeV\n" ;
}
