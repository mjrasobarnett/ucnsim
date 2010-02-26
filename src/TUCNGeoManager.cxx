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
#include "TUCNMagField.h"
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
	Info("TUCNGeoManager", "Dummy Constructor");
	fGravity = kFALSE;
	fMagFields = NULL;
}

//_____________________________________________________________________________
TUCNGeoManager::TUCNGeoManager(const char *name, const char *title) 
					:TGeoManager(name, title)
{
// Constructor
	Info("TUCNGeoManager","UCNGeometry %s, %s created", this->GetName(), this->GetTitle());
	fGravity = kFALSE;
	fMagFields = new TObjArray(5);
}

//_____________________________________________________________________________
TUCNGeoManager& TUCNGeoManager::operator=(const TUCNGeoManager& gm)
{
// --assignment operator
	if(this!=&gm) {
      TGeoManager::operator=(gm);
		fGravity = gm.fGravity;
		fMagFields = gm.fMagFields;
	}
   return *this;
}

//_____________________________________________________________________________
TUCNGeoManager::TUCNGeoManager(const TUCNGeoManager& gm) 
					:TGeoManager(gm),
					 fGravity(gm.fGravity),
					 fMagFields(gm.fMagFields)
{
// Copy Constructor
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
	if (fMagFields) {fMagFields->Delete(); delete fMagFields;}
	delete TUCNParabola::Instance();
}

//_____________________________________________________________________________
Int_t TUCNGeoManager::AddMagField(TUCNMagField *field)
{
// Add a mag field to the list. Returns index of the field in list.
   Info("AddMagField", "AddMagField");
	Int_t index = -1;
   if (!field) return -1;
   TObjArray* fields = this->GetListOfMagFields();
	index = fields->GetEntriesFast();
   fields->AddAtAndExpand(field,index);
	return index;
}

//_____________________________________________________________________________
TUCNMagField* TUCNGeoManager::GetMagField(const char* name) const
{
// Search for a named mag field. All trailing blanks stripped.
	TString sname = name;
   sname = sname.Strip();
   TUCNMagField* field = static_cast<TUCNMagField*>(this->GetListOfMagFields()->FindObject(sname.Data()));
   return field;
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
