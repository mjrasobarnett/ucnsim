// UniformElecField
// Author: Matthew Raso-Barnett  20/03/2011

#ifndef UNIFORMELECFIELD_H
#define UNIFORMELECFIELD_H

#include "ElecField.hpp"
#include "TVector3.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// UniformElecField                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class UniformElecField : public ElecField
{
private:
   // Electric field vector
   TVector3 fField;
   
public:
   UniformElecField();
   UniformElecField(const std::string& name, const TVector3& field, const TGeoShape* fieldShape, const TGeoMatrix* fieldPosition);
   UniformElecField(const UniformElecField&);
   virtual ~UniformElecField();
   
   virtual const TVector3 GetField(const Point& point) const;
   
   ClassDef(UniformElecField, 1)              // uniform electric field class
};

#endif
