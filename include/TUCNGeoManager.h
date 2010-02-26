// TUCNGeoManager
// Author: Matthew Raso-Barnett  22/05/2009

#ifndef ROOT_TUCNGeoManager
#define ROOT_TUCNGeoManager

#ifndef 	ROOT_TGeoManager
#include "TGeoManager.h"
#endif

#include "TUCNGeoNavigator.h"
#include "TUCNMagField.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNGeoManager													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNGeoManager : public TGeoManager 
{
	Bool_t						fGravity;						// The grav field direction vector
	
protected:
	TObjArray*					fMagFields;
	
public:
	
	// -- constructors
   TUCNGeoManager();
   TUCNGeoManager(const char *name, const char *title);
	TUCNGeoManager(const TUCNGeoManager&); 
	TUCNGeoManager& operator=(const TUCNGeoManager&);
	
	// -- destructor
   virtual ~TUCNGeoManager();
	
	// -- methods
	Int_t							AddMagField(TUCNMagField *field);
	TObjArray*					GetListOfMagFields() const       {return fMagFields;}
	TUCNMagField*				GetMagField(const char* name) const;
	
	// Make UCNVolumes using TUCNGeoBuilder
	TGeoVolume*					MakeUCNBox(const char *name, TGeoMedium *medium, Double_t dx, Double_t dy, Double_t dz);
 	TGeoVolume*					MakeUCNTube(const char *name, TGeoMedium *medium, Double_t rmin, Double_t rmax, Double_t dz);
		
	// Make Gravitational Field
	void							SetGravity(Bool_t gravityPresent) {fGravity = gravityPresent;}
	Bool_t						CheckForGravity() {return fGravity;}
	
	Int_t 						GetNumberDetected() const {return static_cast<TUCNGeoNavigator*>(this->GetCurrentNavigator())->GetNumberDetected();}
	Int_t 						GetNumberLost() const {return static_cast<TUCNGeoNavigator*>(this->GetCurrentNavigator())->GetNumberLost();}
	Int_t 						GetNumberDecayed() const {return static_cast<TUCNGeoNavigator*>(this->GetCurrentNavigator())->GetNumberDecayed();}
					
   ClassDef(TUCNGeoManager, 1)          // UCN geometry manager
};

#endif
