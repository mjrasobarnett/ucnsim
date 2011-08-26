// MagFieldDipole
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef MAGFIELDDIPOLE_H
#define MAGFIELDDIPOLE_H

#include "MagField.h"
#include "TVector3.h"

#include <string>
#include "Particle.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// MagFieldDipole                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class MagFieldDipole : public MagField
{
private:
   // Magnetic field vector
   TVector3 fMoment;
   TVector3 fMomentUnitVec;
   double fMomentMag;

public:
   MagFieldDipole();
   MagFieldDipole(const std::string& name, const TVector3& dipoleMoment, const TGeoShape* fieldShape, const TGeoMatrix* fieldPosition);
   MagFieldDipole(const MagFieldDipole&);
   MagFieldDipole& operator=(const MagFieldDipole&);
   virtual ~MagFieldDipole();
   
   virtual const TVector3 GetField(const Point& point) const;
   
   ClassDef(MagFieldDipole, 1)              // uniform mag field class
};

#endif /* MAGFIELDDIPOLE_H */
