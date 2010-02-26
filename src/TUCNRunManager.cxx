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
	
} 

//_____________________________________________________________________________
TUCNRunManager::TUCNRunManager(const TUCNRunManager& runm)
					:TNamed(runm),
					 fManager(runm.fManager)
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
	}
   return *this;
}

//______________________________________________________________________________
TUCNRunManager::~TUCNRunManager()
{ 
// -- Destructor
	Info("TUCNRunManager", "Destructor");
	fManager->Delete();
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

