// MagFieldArray
// Author: Matthew Raso-Barnett  29/09/2010

#ifndef MAGFIELDARRAY_H
#define MAGFIELDARRAY_H

#include <string>
#include "FieldArray.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    MagFieldArray                                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Point;

class MagFieldArray : public FieldArray
{
private:
   // Don't allow copy construction - otherwise we run into problems with ownership of pointers
   MagFieldArray(const MagFieldArray&);

public:
   // -- constructors
   MagFieldArray();
   
   // -- destructor
   virtual ~MagFieldArray();

   // -- methods
   const TVector3 GetMagField(const Point& point, const std::string = "") const;
   
   ClassDef(MagFieldArray, 1)
};

#endif /* MAGFIELDARRAY_H */
