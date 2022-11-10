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
// particle_hp -- source file
// J.P. Wellisch, Nov-1996
// A prototype of the low energy neutron transport model.
//
// 070523 add neglecting doppler broadening on the fly. T. Koi
// 070613 fix memory leaking by T. Koi
// 071002 enable cross section dump by T. Koi
// 080428 change checking point of "neglecting doppler broadening" flag 
//        from GetCrossSection to BuildPhysicsTable by T. Koi
// 081024 G4NucleiPropertiesTable:: to G4NucleiProperties::
//
// P. Arce, June-2014 Conversion neutron_hp to particle_hp
//
#include "G4ParticleHPInelasticData.hh"
#include "G4ParticleHPManager.hh"
//#include "G4Neutron.hh"
//#include "G4ElementTable.hh"
//#include "G4ParticleHPData.hh"
//#include "G4HadronicParameters.hh"
//#include "G4Pow.hh"
#include "G4SystemOfUnits.hh"

class G4ParticleHPInelasticDataProxy: public G4ParticleHPManagerProxyBase {
public:
  G4ParticleHPInelasticDataProxy(){}
  void RegisterCrossSections( std::map<int,G4PhysicsTable *>* theCrossSections ) override {
    G4ParticleHPManager* hpmanager = G4ParticleHPManager::GetInstance();
    hpmanager->RegisterInelasticCrossSections( theCrossSections );
  }
  std::map<int,G4PhysicsTable *>* GetCrossSections() override {
    G4ParticleHPManager* hpmanager = G4ParticleHPManager::GetInstance();
    return hpmanager->GetInelasticCrossSections();
  }
};

G4ParticleHPInelasticData::G4ParticleHPInelasticData(G4ParticleDefinition*) 
   :G4ParticleHPNeutronXSDataSet("NeutronHPInelasticXS",MODEL_NAME),
   theMgrProxy()
{
   theMgrProxy.reset( new G4ParticleHPInelasticDataProxy );
   setDataStore( dataStore() );
   setCallback( theMgrProxy.get() );
   setParams( 0, CUT_FACTOR, MAX_FAILS );
  SetMinKinEnergy( 0*CLHEP::MeV );                                   
  SetMaxKinEnergy( 20*CLHEP::MeV );                                   
}

G4ParticleHPInelasticData::~G4ParticleHPInelasticData()
{
}

std::vector<std::pair<std::string,G4ParticleHPNeutronData > >& G4ParticleHPInelasticData::dataStore(){
  static G4ThreadLocal std::unique_ptr<std::vector<std::pair<std::string,G4ParticleHPNeutronData > > > theXSData;
  if ( !theXSData.get() ) theXSData.reset(G4ParticleHPNeutronData::MakeXSDataStore(MODEL_NAME));  
  return *theXSData;
}

/*
G4bool G4ParticleHPInelasticData::IsApplicable(const G4DynamicParticle*aP, const G4Element*)
{
  G4bool result = true;
  G4double eKin = aP->GetKineticEnergy();
  if(eKin>20*MeV||aP->GetDefinition()!=G4Neutron::Neutron()) result = false;
  return result;
}
*/

void G4ParticleHPInelasticData::CrossSectionDescription(std::ostream& outFile) const
{
   outFile << "Extension of High Precision cross section for inelastic reaction of proton, deuteron, triton, He3 and alpha below 20MeV\n";
}
