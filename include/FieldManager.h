// FieldManager
// Author: Matthew Raso-Barnett  28/10/2009

#ifndef ROOT_FieldManager
#define ROOT_FieldManager

#include <string>
#include "TNamed.h"
#include "TVector3.h"
#include "GravField.h"
#include "MagFieldArray.h"
#include "ElecFieldArray.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    FieldManager                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class RunConfig;
class Point;

class FieldManager : public TNamed 
{
private:
   GravField*    fGravField;
   MagFieldArray* fMagFieldArray;
   ElecFieldArray* fElecFieldArray;
   
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
   MagFieldArray& GetMagFieldArray() const {return *fMagFieldArray;}
   const TVector3 GetMagField(const Point& point, const TVector3& vel, const string volume = "") const;
   
   
   
   ClassDef(FieldManager,1)
};

#endif
