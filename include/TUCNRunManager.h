// TUCNRunManager
// Author: Matthew Raso-Barnett  01/08/2009

#ifndef ROOT_TUCNRunManager
#define ROOT_TUCNRunManager

#include "TNamed.h"

#include "TUCNGeoManager.h"
#include "TUCNRun.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNRunManager													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNRunManager : public TNamed 
{

protected:
	TUCNRunManager(const TUCNRunManager&); 
 	TUCNRunManager& operator=(const TUCNRunManager&);
	
	TUCNGeoManager*				fManager;
	TObjArray*						fRuns;
	
public:
	// -- constructors
   TUCNRunManager();
   
	// -- destructor
	virtual ~TUCNRunManager();

	// -- methods
	TUCNGeoManager* 		GetGeoManager() const {return fManager;} 
	TGeoNode*				CreateGeometry();
	
	Int_t						GetNumberOfRuns() const {return fRuns->GetEntries();}
	TUCNRun*					GetRun(Int_t index) const;
	Int_t						AddRun();
	
	
   ClassDef(TUCNRunManager, 1)      
};

#endif
