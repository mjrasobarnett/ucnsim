// ElecField
// Author: Matthew Raso-Barnett  20/03/2011

#ifndef ELECFIELD_H
#define ELECFIELD_H

#include "Field.hpp"
#include "TVector3.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    ElecField - ABC for Electric field.                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Point;

class ElecField : public Field {

public:
   ElecField();
   ElecField(const std::string& name, const TGeoShape* shape, const TGeoMatrix* matrix);
   ElecField(const ElecField&);
   virtual ~ElecField();
   
   virtual const TVector3 GetField(const Point& point) const = 0;
   virtual const TVector3 GetMagField(const Point& point, const TVector3& velocity) const;
   
   ClassDef(ElecField, 1)              // Abstract base Elec field class
};

#endif /* ELECFIELD_H */
