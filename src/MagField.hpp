// MagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef MAGFIELD_H
#define MAGFIELD_H

#include "Field.hpp"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// MagField - ABC for magnetic field.                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Point;

class MagField : public Field {
   
public:
   MagField();
   MagField(const std::string& name, const TGeoShape* shape, const TGeoMatrix* matrix);
   MagField(const MagField&);
   virtual ~MagField();
   
   virtual const TVector3 GetField(const Point& point) const = 0;
   
   ClassDef(MagField, 1)              // Abstract base Mag field class
};

#endif /* MAGFIELD_H */
