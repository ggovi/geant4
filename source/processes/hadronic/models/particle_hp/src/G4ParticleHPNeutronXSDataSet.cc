//
//
// G. Govi, September 2022
//
#include "G4ParticleHPNeutronXSDataSet.hh"

#include "G4Threading.hh"
#include "G4HadronicParameters.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4Pow.hh"
#include "G4NucleiProperties.hh"
#include "G4PhysicsTable.hh"

G4ParticleHPNeutronXSDataSet::G4ParticleHPNeutronXSDataSet( G4String modelLabel, G4String modelName ):
  G4VCrossSectionDataSet( modelLabel),
  theModelName( modelName ),
  theIOVs() {
  element_cache = nullptr;
  material_cache = nullptr;
  ke_cache = 0.0; 
  xs_cache = 0.0; 
}

G4ParticleHPNeutronXSDataSet::~G4ParticleHPNeutronXSDataSet(){
  if ( theData != nullptr && G4Threading::IsWorkerThread() != true ) {
    for( auto& xsdata: *theData ){
      xsdata.second->clearAndDestroy();
      delete xsdata.second;
    }
    theData->clear();
    // !!!! This causes a crash when enabled... to be investigated 
    //delete theData;
  }
}

G4int G4ParticleHPNeutronXSDataSet::GetVerboseLevel()  const
{
   return G4ParticleHPManager::GetInstance()->GetVerboseLevel();
}
void G4ParticleHPNeutronXSDataSet::SetVerboseLevel( G4int newValue ) 
{
   G4ParticleHPManager::GetInstance()->SetVerboseLevel(newValue);
}

void G4ParticleHPNeutronXSDataSet::setDataStore( std::vector<std::pair<std::string,G4ParticleHPNeutronData > >& dataStore ){
  theDataStoreRef = &dataStore;
}

void G4ParticleHPNeutronXSDataSet::setCallback( G4ParticleHPManagerProxyBase* mgrProxy ){
  theMgrProxy = mgrProxy;
}

void G4ParticleHPNeutronXSDataSet::setParams( int zthreshold, float cutFactor, int maxFails ){
  theZThreshold = zthreshold;
  theCutFactor = cutFactor;
  theMaxFails = maxFails;
}

void G4ParticleHPNeutronXSDataSet::BuildPhysicsTable( const G4ParticleDefinition& ap )
{

  if(&ap!=G4Neutron::Neutron()) 
     throw G4HadronicException(__FILE__, __LINE__, "Attempt to use NeutronHP data for particles other than neutrons!!!");  

  if(theDataStoreRef == nullptr ) throw G4HadronicException(__FILE__, __LINE__, "The Data Store has not been provided");
  if(theMgrProxy == nullptr) throw G4HadronicException(__FILE__, __LINE__, "The Manager Proxy has not been initialized");

  if ( G4Threading::IsWorkerThread() ) {
    theData = theMgrProxy->GetCrossSections();
    theIOVs.clear();
    for( auto& iel : *theData ) theIOVs.add( iel.first );
    theIOVs.sort();
    return;
  }

  if ( theData==nullptr ) 
    theData = new std::map<int,G4PhysicsTable* >;
  else { 
    for(auto& itable: *theData){
      itable.second->clearAndDestroy();
    }
    theIOVs.clear();
  }

  size_t numberOfElements = G4Element::GetNumberOfElements();
  auto& dataStore = *theDataStoreRef;
  static G4ThreadLocal G4ElementTable *theElementTable  = 0 ; if (!theElementTable) theElementTable= G4Element::GetElementTable();
  for(auto& item : dataStore){
    auto& dataSet = item.second;
    int key = theIOVs.add(item.first);
    auto physTable = new G4PhysicsTable( numberOfElements );
    theData->insert(std::make_pair( key, physTable ) );
    for( size_t i=0; i<numberOfElements; ++i ){
      // make a PhysicsVector for each element
      G4PhysicsVector* physVec = dataSet.MakePhysicsVector((*theElementTable)[i]);
      physTable->push_back(physVec);
    }
  }
  theIOVs.sort();

  theMgrProxy->RegisterCrossSections(theData);
}

void G4ParticleHPNeutronXSDataSet::DumpPhysicsTable(const G4ParticleDefinition& ap)
{
  if(&ap!=G4Neutron::Neutron()) 
     throw G4HadronicException(__FILE__, __LINE__, "Attempt to use NeutronHP data for particles other than neutrons!!!");
  
  if ( theData==nullptr )
    throw G4HadronicException(__FILE__, __LINE__, "No data has been loaded.");

#ifdef G4VERBOSE
  if ( G4HadronicParameters::Instance()->GetVerboseLevel() == 0 ) return;

  //
  //  Dump element based cross section
  //  range 10e-5 eV to 20 MeV
  // 1 0 point per decade
  // in barn
  //

   G4cout << G4endl;
   G4cout << G4endl;
   G4cout << theModelName <<" Cross Section of Neutron HP"<< G4endl;
   size_t numberOfElements = G4Element::GetNumberOfElements();
   static G4ThreadLocal G4ElementTable *theElementTable  = 0 ; if (!theElementTable) theElementTable= G4Element::GetElementTable();
   
   // loop on the temperature...
   for( auto& dataSet: *theData ){
     G4cout << "(Pointwise cross-section at "<<dataSet.first<<" Kelvin.)" << G4endl;
     G4cout << G4endl;
     G4cout << "Name of Element" << G4endl;
     G4cout << "Energy[eV]  XS[barn]" << G4endl;
     G4cout << G4endl;

     for ( size_t i = 0 ; i < numberOfElements ; ++i )
       {

	 G4cout << (*theElementTable)[i]->GetName() << G4endl;

	 if ( (*((*dataSet.second)(i))).GetVectorLength() == 0 ) 
	   {
	     G4cout << "The cross-section data for the "<<theModelName<<" model of this element is not available." << G4endl; 
	     G4cout << G4endl; 
	     continue;
	   }

	 G4int ie = 0;

	 for ( ie = 0 ; ie < 130 ; ie++ )
	   {
	     G4double eKinetic = 1.0e-5 * G4Pow::GetInstance()->powA ( 10.0 , ie/10.0 ) *eV;
	     G4bool outOfRange = false;

	     if ( eKinetic < 20*MeV )
	       {
		 G4cout << eKinetic/eV << " " << (*((*dataSet.second)(i))).GetValue(eKinetic, outOfRange)/barn << G4endl;
	       }
	     
	   }

	 G4cout << G4endl;
       }
   }

  #endif
}

G4bool G4ParticleHPNeutronXSDataSet::IsIsoApplicable( const G4DynamicParticle* dp , 
						      G4int /*Z*/ , G4int /*A*/ ,
						      const G4Element* /*elm*/ ,
						      const G4Material* /*mat*/ )
{
   G4double eKin = dp->GetKineticEnergy();
   if ( eKin > GetMaxKinEnergy() 
     || eKin < GetMinKinEnergy() 
     || dp->GetDefinition() != G4Neutron::Neutron() ) return false;                                   
   return true;
}

G4double G4ParticleHPNeutronXSDataSet::GetCrossSection(const G4DynamicParticle* aP, const G4Element*anE, G4double aT)
{
  G4double result = 0;
  if(theZThreshold && anE->GetZ()<theZThreshold) return result;
  G4bool outOfRange;
  G4int index = anE->GetIndex();

  if( theData == nullptr )
    throw G4HadronicException(__FILE__, __LINE__, "No data has been loaded.");

  // find the dataset for the specified temperature
  int ind = theIOVs.find( aT );
  if(ind == -1){
    // can be this recoverd? (#fix me in this case...)
    throw G4HadronicException(__FILE__, __LINE__, "Data for temperature "+std::to_string(aT)+" is not available");
  }

  auto iset = theData->find( ind );
  if( iset == theData->end() ){
    throw G4HadronicException(__FILE__, __LINE__, "Data set identified for temperature "+std::to_string(aT)+" has not been found in memory");
  }

  const auto& theCrossSections = iset->second;
  
  // 100729 TK add safety
  if ( ( ( *theCrossSections )( index ) )->GetVectorLength() == 0 ) return result;

  // prepare neutron
  G4double eKinetic = aP->GetKineticEnergy();
  G4ReactionProduct theNeutronRP( aP->GetDefinition() );
  theNeutronRP.SetMomentum( aP->GetMomentum() );
  theNeutronRP.SetKineticEnergy( eKinetic );

  if( theIOVs.isTempDependentSet() || G4ParticleHPManager::GetInstance()->GetNeglectDoppler() )
  {
     //NEGLECT_DOPPLER
     G4double factor = 1.0;
     if ( eKinetic < aT * k_Boltzmann ) {
        // below 0.1 eV neutrons 
        // Have to do some, but now just igonre.   
        // Will take care after performance check.  
        // factor = factor * targetV;
     }
     return ( (*((*theCrossSections)(index))).GetValue(eKinetic, outOfRange) )* factor; 
  }

  // prepare thermal nucleus
  G4Nucleus aNuc;
  G4double eps = 0.0001;
  G4double theA = anE->GetN();
  G4double theZ = anE->GetZ();
  G4double eleMass; 
  eleMass = ( G4NucleiProperties::GetNuclearMass( static_cast<G4int>(theA+eps) , static_cast<G4int>(theZ+eps) )
	     ) / G4Neutron::Neutron()->GetPDGMass();
  
  G4ReactionProduct boosted;
  G4double aXsection;
  
  // MC integration loop
  G4int counter = 0;
  G4double buffer = 0;
  G4int failCount = 0;
  G4int size = G4int(std::max(10., aT/60*kelvin));
  G4ThreeVector neutronVelocity = 1./G4Neutron::Neutron()->GetPDGMass()*theNeutronRP.GetMomentum();
  G4double neutronVMag = neutronVelocity.mag();

#ifndef G4PHP_DOPPLER_LOOP_ONCE
  while(counter == 0 || std::abs(buffer-result/std::max(1,counter)) > theCutFactor*buffer) // Loop checking, 11.05.2015, T. Koi
  {
    if(counter) buffer = result/counter;
    while (counter<size) // Loop checking, 11.05.2015, T. Koi
    {
      counter ++;
#endif
      G4ReactionProduct aThermalNuc = aNuc.GetThermalNucleus(eleMass, aT);
      boosted.Lorentz(theNeutronRP, aThermalNuc);
      G4double theEkin = boosted.GetKineticEnergy();
      aXsection = (*((*theCrossSections)(index))).GetValue(theEkin, outOfRange);
      if(theMaxFails && aXsection<0){
        if(failCount<theMaxFails){
	  failCount++;
#ifndef G4PHP_DOPPLER_LOOP_ONCE
	  counter--;
	  continue;
#endif
	} else {
	  aXsection = 0;
	}
      }
      // velocity correction.
      G4ThreeVector targetVelocity = 1./aThermalNuc.GetMass()*aThermalNuc.GetMomentum();
      aXsection *= (targetVelocity-neutronVelocity).mag()/neutronVMag;
      result += aXsection;
    }
#ifndef G4PHP_DOPPLER_LOOP_ONCE
    size += size;
  }
  result /= counter;
  return result;
#endif
}

G4double G4ParticleHPNeutronXSDataSet::GetIsoCrossSection( const G4DynamicParticle* dp ,
							   G4int /*Z*/ , G4int /*A*/ ,
							   const G4Isotope* /*iso*/  ,
							   const G4Element* element ,
							   const G4Material* material )
{
   if ( dp->GetKineticEnergy() == ke_cache && element == element_cache &&  material == material_cache ) return xs_cache;

   ke_cache = dp->GetKineticEnergy();
   element_cache = element;
   material_cache = material;
   G4double xs = GetCrossSection( dp , element , material->GetTemperature() );
   xs_cache = xs;
   return xs;
}







  
