// TUCNFieldManager
// Author: Matthew Raso-Barnett  28/10/2009

#ifndef ROOT_TUCNFieldManager
#define ROOT_TUCNFieldManager

#include "TNamed.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNFieldManager													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNGravField;

class TUCNFieldManager : public TNamed 
{

protected:
	TObjArray*			fMagFields;
	TUCNGravField*		fGravField;
	
public:
	// -- constructors
   TUCNFieldManager();
   TUCNFieldManager(const TUCNFieldManager&); 
	TUCNFieldManager& operator=(const TUCNFieldManager&);
	
	// -- destructor
	virtual ~TUCNFieldManager();

	// -- methods

	
   ClassDef(TUCNFieldManager, 1)      
};

#endif
