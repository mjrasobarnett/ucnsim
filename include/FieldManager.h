// FieldManager
// Author: Matthew Raso-Barnett  28/10/2009

#ifndef ROOT_FieldManager
#define ROOT_FieldManager

#include "TNamed.h"
#include "GravField.h"
#include "MagFieldManager.h"
#include "TVector3.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    FieldManager                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class RunConfig;

class FieldManager : public TNamed 
{
private:
   GravField*    fGravField;
   MagFieldManager* fMagFieldManager;
   
   GravField* AddGravField();
   
public:
   // -- constructors
   FieldManager();
   FieldManager(const FieldManager&); 
   FieldManager& operator=(const FieldManager&);
   
   // -- destructor
   virtual ~FieldManager();
   
   // -- methods
   Bool_t Initialise(const RunConfig& runConfig);
   
   // Grav Fields
   const GravField* const GetGravField() const {return fGravField;}
   
   // Mag Fields
   const TVector3 GetMagField(const TVector3& position, const string volume = "") const;
   
   ClassDef(FieldManager,1)
};

#endif
