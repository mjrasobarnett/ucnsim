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

#include "TObjArray.h"
#include "TRandom.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TVirtualGeoTrack.h"

#include "../geom/geometries.h"
#include "Units.h"

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
TGeoNode* TUCNRunManager::CreateGeometry()
{ 
// -- 
	Info("TUCNRunManager", "CreateGeometry");
	// -- Create Geometry using function specified in geometries.C macro
	return CreateGeometry_v1(fManager);
}

//______________________________________________________________________________
TUCNRun* TUCNRunManager::GetRun(Int_t index) const
{
	return (index<fRuns->GetEntries()) ? static_cast<TUCNRun*>(fRuns->At(index)) : 0;
}

//______________________________________________________________________________
Int_t TUCNRunManager::AddRun()
{
	Int_t index = this->GetNumberOfRuns();
	TUCNRun* newRun = new TUCNRun();
   fRuns->AddAtAndExpand(newRun , index);
   return index;
}

