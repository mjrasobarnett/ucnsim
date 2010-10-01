// TUCNFieldManager
// Author: Matthew Raso-Barnett  28/10/2009

#ifndef ROOT_TUCNFieldManager
#define ROOT_TUCNFieldManager

#include "TNamed.h"
#include "TUCNGravField.h"
#include "TUCNMagFieldManager.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNFieldManager                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNConfigFile;
class TUCNRun;

class TUCNFieldManager : public TNamed 
{
protected:
   TUCNGravField*    fGravField;
   TUCNMagFieldManager* fMagFieldManager;
   
public:
   // -- constructors
   TUCNFieldManager();
   TUCNFieldManager(const TUCNFieldManager&); 
   TUCNFieldManager& operator=(const TUCNFieldManager&);
   
   // -- destructor
   virtual ~TUCNFieldManager();
   
   // -- methods
   Bool_t Initialise(TUCNConfigFile& configFile, const TUCNRun& run);
   
   // Grav Fields
   TUCNGravField* AddGravField();
   TUCNGravField* GravField() const {return fGravField;}
   
   // Mag Fields
   
   ClassDef(TUCNFieldManager,1)
};

#endif
