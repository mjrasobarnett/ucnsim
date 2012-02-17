// ElecFieldArrayay
// Author: Matthew Raso-Barnett  29/09/2010

#ifndef ELECFIELD_ARRAY_H
#define ELECFIELD_ARRAY_H

#include "TObject.h"
#include <string>
#include "FieldArray.hpp"
#include "TVector3.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    ElecFieldArrayay                                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class ElecFieldArray : public FieldArray
{
private:
   
public:
   // -- constructors
   ElecFieldArray();
   ElecFieldArray(const ElecFieldArray& other);
   
   // -- destructor
   virtual ~ElecFieldArray();

   // -- methods
   const TVector3 GetMagField(const Point& point, const TVector3& velocity, const std::string = "") const;
   
   ClassDef(ElecFieldArray, 1)
};

#endif /* ELECFIELDARRAY_H */
