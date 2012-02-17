// MagFieldLoop

#ifndef MAGFIELDLOOP_H
#define MAGFIELDLOOP_H

#include "MagField.hpp"
#include "TVector3.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// MagFieldLoop                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Point;
class Tube;
class TGeoMatrix;

class MagFieldLoop : public MagField
{
private:
   // Magnetic field vector
   double fCurrent;
   int fTurns;
   double fRadius;

   const TVector3 OffAxisFieldCalc(const Point& localPoint, const double r) const;
   const TVector3 OnAxisFieldCalc(const Point& localPoint) const;

public:
   MagFieldLoop();
   MagFieldLoop(const std::string& name, const double current, const int turns, const Tube* tube, const TGeoMatrix* matrix);
   MagFieldLoop(const MagFieldLoop&);
   MagFieldLoop& operator=(const MagFieldLoop&);
   virtual ~MagFieldLoop();
   
   virtual const TVector3 GetField(const Point& point) const;
   
   ClassDef(MagFieldLoop, 1)
};

#endif /* MAGFIELDLOOP_H */
