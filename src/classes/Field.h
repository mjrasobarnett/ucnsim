// Field
// Author: Matthew Raso-Barnett  20/03/2011

#ifndef FIELD_H
#define FIELD_H

#include "TNamed.h"
#include "TVector3.h"
#include "TGeoShape.h"
#include "TGeoMatrix.h"

#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    Field - ABC for electromagnetic fields                              //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class FieldVertex;
class Point;

class Field : public TNamed {
private:
   // Field Extent -- a virtual volume that defines the extent of this field in the geometry
   const TGeoShape*  fFieldShape;
   const TGeoMatrix* fFieldMatrix;
   
protected:
   FieldVertex ConvertToGlobalFrame(const FieldVertex& point) const;
   FieldVertex ConvertToLocalFrame(const FieldVertex& point) const;
   
public:
   Field();
   Field(const std::string& name, const TGeoShape* shape, const TGeoMatrix* matrix);
   Field(const Field&);
   virtual ~Field();
   
   virtual Bool_t Contains(const TVector3& point) const;
   virtual const TVector3 GetField(const Point& point) const = 0;
   
   ClassDef(Field, 1)              // Abstract base field class
};

#endif /* FIELD_H */
