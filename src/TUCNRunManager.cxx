// TUCNRunManager
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>
#include <vector>

#include "TUCNRunManager.h"

#include "TUCNGeoManager.h"
#include "TUCNGeoNavigator.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"
#include "TUCNUniformMagField.h"

#include "TObjArray.h"
#include "TRandom.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TVirtualGeoTrack.h"

#include "../geom/geometries.h"
#include "Units.h"
#include "Constants.h"

using std::cout;
using std::endl;
using std::cerr;

//#define VERBOSE_MODE

ClassImp(TUCNRunManager)

//_____________________________________________________________________________
TUCNRunManager::TUCNRunManager()
					:TNamed("RunManager", "Default Run Manager")
{
// -- Default constructor
   Info("TUCNRunManager", "Constructor");
	fManager = new TUCNGeoManager("GeoManager", "Geometry Manager");
	fRuns = new TObjArray();
} 

//_____________________________________________________________________________
TUCNRunManager::TUCNRunManager(const TUCNRunManager& runm)
					:TNamed(runm),
					 fManager(runm.fManager),
					 fRuns(runm.fRuns)
{
// -- Copy Constructor
	Info("TUCNRunManager", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNRunManager& TUCNRunManager::operator=(const TUCNRunManager& runm)
{
// --assignment operator
	if(this!=&runm) {
      TNamed::operator=(runm);
		fManager = runm.fManager;
		fRuns = runm.fRuns;
	}
   return *this;
}

//______________________________________________________________________________
TUCNRunManager::~TUCNRunManager()
{ 
// -- Destructor
	Info("TUCNRunManager", "Destructor");
	fManager->Delete();
	fRuns->Delete();
}

// -- METHODS --

//______________________________________________________________________________
void TUCNRunManager::InitialiseGeometry()
{ 
// -- 
	Info("TUCNRunManager", "InitialiseGeometry");
	
	// Input parameters to calculate f and W
	Double_t observedLifetime = 150.*Units::s;
	Double_t fermiPotential = (0.91*Units::m)*Constants::height_equivalent_conversion;
	Double_t totalEnergy = (0.52*Units::m)*Constants::height_equivalent_conversion;
	Double_t initialVelocity = TMath::Sqrt(2.*totalEnergy/Constants::neutron_mass);
	Double_t meanFreePath = 0.16*Units::m;
	
	// Calculate f = W/V and hence W
	Double_t X = totalEnergy/fermiPotential;
	Double_t L = 2.0*((1./X)*TMath::ASin(TMath::Sqrt(X)) - TMath::Sqrt((1./X) - 1.));
	cout << "E: " << totalEnergy/Units::neV << "\t" << "V: " << fermiPotential/Units::neV << "\t" << "E/V: " << X << "\t"<< "L: " << L << endl;
	Double_t f = meanFreePath/(initialVelocity*observedLifetime*L);
	Double_t W = f*fermiPotential;
	cout << "f: " << f << "\t" << "W: " << W/Units::neV << endl;
	
	// -- Create Geometry using function specified in geometries.C macro
	CreateGeometry_v5(fermiPotential, W);
		
	// -- Define Mag Field
	TUCNUniformMagField* magfield = new TUCNUniformMagField("Uniform magnetic field", 0.0,1.0,1.0);
	fManager->AddMagField(magfield);
}

//______________________________________________________________________________
TUCNRun* TUCNRunManager::GetRun(Int_t index) const
{
	return (index<fRuns->GetEntries()) ? static_cast<TUCNRun*>(fRuns->At(index)) : 0;
}

//______________________________________________________________________________
void TUCNRunManager::AddRuns(Int_t numberOfRuns)
{
	for(Int_t i = 0; i < numberOfRuns; i++) {
		Int_t index = this->GetNumberOfRuns();
		TUCNRun* newRun = new TUCNRun();
	   fRuns->AddAtAndExpand(newRun , index);
	}
}

//_____________________________________________________________________________
void	TUCNRunManager::WriteRunsToFile()
{
	TFile f("runs.root","recreate");
	fRuns->Write();
	f.Close();
}
