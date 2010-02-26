// TUCNGeoBBox
// Author: Matthew Raso-Barnett  22/05/2009

#include "TObject.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoBuilder.h"

#include "TUCNGeoBBox.h"
#include "TUCNGeoTube.h"

#include "TUCNGeoBuilder.h"


ClassImp(TUCNGeoBuilder)

TUCNGeoBuilder *TUCNGeoBuilder::fgUCNInstance = NULL;

//_____________________________________________________________________________
TUCNGeoBuilder::TUCNGeoBuilder()
					:TGeoBuilder()
{
// Default constructor.
   Info("TUCNGeoBuilder", "Constructor");
	fgUCNInstance = this;
} 

//_____________________________________________________________________________
TUCNGeoBuilder::TUCNGeoBuilder(const TUCNGeoBuilder& other)
					:TGeoBuilder(other)
{
// Copy constructor.
   Error("TUCNGeoBuilder","copying not allowed for TUCNGeoBuilder");
} 

//_____________________________________________________________________________
TUCNGeoBuilder::~TUCNGeoBuilder()
{
// Destructor.
   Info("TUCNGeoBuilder", "Destructor");
	fgUCNInstance = NULL;
}   

//_____________________________________________________________________________
TUCNGeoBuilder &TUCNGeoBuilder::operator=(const TUCNGeoBuilder&)
{
// Assignment.
   Error("TUCNGeoBuilder","assignment not allowed for TGeoBuilder");
   return *this;
}



//_____________________________________________________________________________
TUCNGeoBuilder *TUCNGeoBuilder::UCNInstance(TGeoManager *geom) 
{
// Return pointer to singleton.
   if (!geom) {
      printf("ERROR: Cannot create geometry builder with NULL geometry\n");
      return NULL;
   }   
   if (!fgUCNInstance) fgUCNInstance = new TUCNGeoBuilder();
	return fgUCNInstance;
}   

//_____________________________________________________________________________
TGeoVolume* TUCNGeoBuilder::MakeUCNBox(const char *name, TGeoMedium *medium, Double_t dx, Double_t dy, Double_t dz)
{
// Make in one step a volume pointing to a box shape with given medium.
   TUCNGeoBBox *box = new TUCNGeoBBox(name, dx, dy, dz);
   TGeoVolume *vol = 0;
   if (box->IsRunTimeShape()) {
      vol = gGeoManager->MakeVolumeMulti(name, medium);
      vol->SetShape(box);
   } else {
      vol = new TGeoVolume(name, box, medium);
   }
   return vol;
}

//_____________________________________________________________________________
TGeoVolume* TUCNGeoBuilder::MakeUCNTube(const char *name, TGeoMedium *medium, Double_t rmin, Double_t rmax, Double_t dz)
{
// Make in one step a volume pointing to a tube shape with given medium.
   if (rmin>rmax) {
      Error("MakeUCNTube", "tube %s, Rmin=%g greater than Rmax=%g", name,rmin,rmax);
   }
   TUCNGeoTube *tube = new TUCNGeoTube(name, rmin, rmax, dz);
   TGeoVolume *vol = 0;
   if (tube->IsRunTimeShape()) {
      vol = gGeoManager->MakeVolumeMulti(name, medium);
      vol->SetShape(tube);
   } else {
      vol = new TGeoVolume(name, tube, medium);
   }
   return vol;
}
