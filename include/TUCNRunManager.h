// TUCNRunManager
// Author: Matthew Raso-Barnett  01/08/2009

#ifndef ROOT_TUCNRunManager
#define ROOT_TUCNRunManager

#include "TNamed.h"

#include "TUCNGeoManager.h"

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
	
public:
	// -- constructors
   TUCNRunManager();
   
	// -- destructor
	virtual ~TUCNRunManager();

	// -- methods
	TUCNGeoManager* 		GetGeoManager() { return fManager; } 
	
	TGeoNode*				CreateGeometry();
	
	

	
	
	
   ClassDef(TUCNRunManager, 1)      
};

#endif
