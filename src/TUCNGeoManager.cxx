// TUCNGeoManager
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <fstream>

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TVirtualGeoTrack.h"
#include "TParticle.h"

#include "TUCNGeoBuilder.h"
#include "TUCNGeoNavigator.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"
#include "TUCNParabola.h"
#include "Units.h"

#include "TUCNGeoManager.h"

using std::cout;
using std::endl;

//#define VERBOSE_MODE


ClassImp(TUCNGeoManager)

//_____________________________________________________________________________
	TUCNGeoManager::TUCNGeoManager() 
						:TGeoManager()
{
// Default Constructor
	Info("TUCNGeoManager", "Default Constructor");
	fGravity = kFALSE;
}

//_____________________________________________________________________________
TUCNGeoManager::TUCNGeoManager(const char *name, const char *title) 
					:TGeoManager(name, title)
{
// Constructor
	Info("TUCNGeoManager","UCNGeometry %s, %s created", this->GetName(), this->GetTitle());
	fGravity = kFALSE;
}

//_____________________________________________________________________________
TUCNGeoManager& TUCNGeoManager::operator=(const TUCNGeoManager& gm)
{
// --assignment operator
	if(this!=&gm) {
      TGeoManager::operator=(gm);
		fGravity = gm.fGravity;
	}
   return *this;
}

//_____________________________________________________________________________
TUCNGeoManager::~TUCNGeoManager()
{
// Destructor
	Info("TUCNGeoManager", "Destructor");
	for (Int_t i = 0; i < this->GetNtracks(); i++) {
		this->GetTrack(i)->GetParticle()->Delete();
	}	
	if (fGravity == kTRUE) delete TUCNGravField::Instance();
	delete TUCNParabola::Instance();
}


//_____________________________________________________________________________
TGeoVolume* TUCNGeoManager::MakeUCNBox(const char *name, TGeoMedium *medium, Double_t dx, Double_t dy, Double_t dz)
{
// Make in one step a volume pointing to a box shape with given medium.
	return TUCNGeoBuilder::UCNInstance(this)->MakeUCNBox(name, medium, dx, dy, dz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeUCNTube(const char *name, TGeoMedium *medium,
	Double_t rmin, Double_t rmax, Double_t dz)
{
// Make in one step a volume pointing to a tube shape with given medium.
	return TUCNGeoBuilder::UCNInstance(this)->MakeUCNTube(name, medium, rmin, rmax, dz);
}
