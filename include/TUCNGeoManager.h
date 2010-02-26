// TUCNGeoManager
// Author: Matthew Raso-Barnett  22/05/2009

#ifndef ROOT_TUCNGeoManager
#define ROOT_TUCNGeoManager

#ifndef 	ROOT_TGeoManager
#include "TGeoManager.h"
#endif

#include "TUCNGeoNavigator.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNGeoManager													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNGeoManager : public TGeoManager 
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
	
	Int_t 						GetNumberDetected() const {return static_cast<TUCNGeoNavigator*>(this->GetCurrentNavigator())->GetNumberDetected();}
	Int_t 						GetNumberLost() const {return static_cast<TUCNGeoNavigator*>(this->GetCurrentNavigator())->GetNumberLost();}
	Int_t 						GetNumberDecayed() const {return static_cast<TUCNGeoNavigator*>(this->GetCurrentNavigator())->GetNumberDecayed();}
					
   ClassDef(TUCNGeoManager, 1)          // UCN geometry manager
};

#endif
