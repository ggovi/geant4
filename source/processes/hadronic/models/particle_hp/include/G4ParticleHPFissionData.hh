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
// 080417 Add IsZAApplicable method (return false) by T. Koi
//
// P. Arce, June-2014 Conversion neutron_hp to particle_hp
//
#ifndef G4ParticleHPFissionData_h
#define G4ParticleHPFissionData_h 1

// Class Description
// Cross-section data set for a high precision (based on evaluated data
// libraries) description of neutron induced fission below 20 MeV; 
// To be used in your physics list in case you need this physics.
// In this case you want to register an object of this class with 
// the corresponding process.
// Class Description - End

#include "G4ParticleHPNeutronXSDataSet.hh"
//#include "G4VCrossSectionDataSet.hh"
//#include "G4DynamicParticle.hh"
//#include "G4Element.hh"
//#include "G4ParticleDefinition.hh"
//#include "G4PhysicsTable.hh"

class G4ParticleHPFissionDataProxy;

class G4ParticleHPFissionData : public G4ParticleHPNeutronXSDataSet
{
   public:
      static constexpr const char* const MODEL_NAME = "Fission";
      static const int Z_THRESHOLD=88;
      static constexpr const float CUT_FACTOR=0.01;
      static std::vector<std::pair<std::string,G4ParticleHPNeutronData > >& dataStore();

   public:
   
      G4ParticleHPFissionData();
   
      ~G4ParticleHPFissionData();
   
      //G4bool IsApplicable(const G4DynamicParticle*, const G4Element*);
      //G4bool IsZAApplicable( const G4DynamicParticle* , G4double /*ZZ*/, G4double /*AA*/)
      //{ return false;}

     virtual void CrossSectionDescription(std::ostream&) const;

   private:

      std::unique_ptr<G4ParticleHPFissionDataProxy> theMgrProxy;

};

#endif
