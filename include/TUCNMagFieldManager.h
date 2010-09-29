// TUCNMagFieldManager
// Author: Matthew Raso-Barnett  29/09/2010

#ifndef ROOT_TUCNMagFieldManager
#define ROOT_TUCNMagFieldManager

#include "TNamed.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNMagFieldManager                                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNMagFieldManager : public TNamed 
{
private:
   
public:
   // -- constructors
   TUCNMagFieldManager();
   TUCNMagFieldManager(const TUCNMagFieldManager&); 
   TUCNMagFieldManager& operator=(const TUCNMagFieldManager&);
   
   // -- destructor
   virtual ~TUCNMagFieldManager();

   ClassDef(TUCNMagFieldManager, 1)
};

#endif
