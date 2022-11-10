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
//
//
 // Hadronic Process: Very Low Energy Neutron X-Sections
 // original by H.P. Wellisch, TRIUMF, 14-Feb-97
 // Has the Cross-section data for all materials.
// P. Arce, June-2014 Conversion neutron_hp to particle_hp
//
#ifndef G4ParticleHPNeutronData_h
#define G4ParticleHPNeutronData_h 1
#include "globals.hh"
#include "G4Element.hh"
//#include "G4ParticleHPElasticData.hh"
//#include "G4ParticleHPInelasticData.hh"
//#include "G4ParticleHPFissionData.hh"
//#include "G4ParticleHPCaptureData.hh"
#include "G4ParticleHPNeutronElementData.hh"

#include <vector>

class G4ParticleHPNeutronData
{
public:

  G4ParticleHPNeutronData();

  void Init( G4String dataFolderPath, G4int ZThreshold);

  ~G4ParticleHPNeutronData();

  inline G4PhysicsVector * MakePhysicsVector(G4Element * thE)
  {
     if ( numEle <= (G4int)thE->GetIndex() ) addPhysicsVector(); 
     return DoPhysicsVector((theData[thE->GetIndex()]).GetData());
  }

  G4PhysicsVector * DoPhysicsVector(G4ParticleHPVector * theVector);
  
  static std::vector<std::pair<std::string,G4ParticleHPNeutronData > >* MakeXSDataStore( G4String modelName, G4int ZThreshold=0 );
  
private:

  std::vector< G4ParticleHPNeutronElementData > theData;
  G4int numEle;
  void addPhysicsVector();

  G4String theDataFolderPath;
  G4int theZThreshold;
};

#endif
