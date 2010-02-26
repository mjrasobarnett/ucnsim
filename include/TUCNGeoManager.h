// TUCNGeoManager
// Author: Matthew Raso-Barnett  22/05/2009

#ifndef ROOT_TUCNGeoManager
#define ROOT_TUCNGeoManager

#include "TGeoManager.h"
#include "TUCNGeoNavigator.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNGeoManager													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TGeoVolume;
class TGeoMatrix;

class TUCNGeoManager : public TNamed 
{
	
protected:
	Int_t 						fSourceVolumeIndex;
	Int_t							fSourceMatrixIndex;
	
public:
	
	// -- constructors
   TUCNGeoManager();
   TUCNGeoManager(const char *name, const char *title);
	TUCNGeoManager(const TUCNGeoManager&); 
	TUCNGeoManager& operator=(const TUCNGeoManager&);
	
	// -- destructor
   virtual ~TUCNGeoManager();
	
	// -- methods
	
	// Source Volume
	void							SetSourceVolume(TGeoVolume* sourceVolume);
	TGeoVolume*					GetSourceVolume() const;
	void							SetSourceMatrix(TGeoMatrix* sourceMatrix);
	TGeoMatrix*					GetSourceMatrix() const;
	
	// Make UCNVolumes using TUCNGeoBuilder
	TGeoVolume*					MakeUCNBox(const char *name, TGeoMedium *medium, Double_t dx, Double_t dy, Double_t dz);
 	TGeoVolume*					MakeUCNTube(const char *name, TGeoMedium *medium, Double_t rmin, Double_t rmax, Double_t dz);
	
	ClassDef(TUCNGeoManager, 1)          // UCN geometry manager
};

#endif
