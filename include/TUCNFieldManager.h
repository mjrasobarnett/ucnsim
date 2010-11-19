// TUCNFieldManager
// Author: Matthew Raso-Barnett  28/10/2009

#ifndef ROOT_TUCNFieldManager
#define ROOT_TUCNFieldManager

#include "TNamed.h"
#include "TUCNGravField.h"
#include "TUCNMagFieldManager.h"
#include "TVector3.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNFieldManager                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNRunConfig;

class TUCNFieldManager : public TNamed 
{
private:
   TUCNGravField*    fGravField;
   TUCNMagFieldManager* fMagFieldManager;
   
   TUCNGravField* AddGravField();
   
public:
   // -- constructors
   TUCNFieldManager();
   TUCNFieldManager(const TUCNFieldManager&); 
   TUCNFieldManager& operator=(const TUCNFieldManager&);
   
   // -- destructor
   virtual ~TUCNFieldManager();
   
   // -- methods
   Bool_t Initialise(const TUCNRunConfig& runConfig);
   
   // Grav Fields
   const TUCNGravField* const GetGravField() const {return fGravField;}
   
   // Mag Fields
   const TUCNMagField* const GetMagField(const TVector3&, const string&) const;
   
   ClassDef(TUCNFieldManager,1)
};

#endif
