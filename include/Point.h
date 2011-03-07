// Point
// Author: Matthew Raso-Barnett  03/12/2010

#ifndef POINT_H
#define POINT_H

#include "TVector3.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//       Point                                                           //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class Point {
private:
   TVector3 fPos;
   double fT;
   
public:
   // -- constructors
   Point();
   Point(double x, double y, double z, double t);
   Point(TVector3 pos, double t);
   Point(const Point&); 
   Point& operator=(const Point&);
   // -- destructor
   virtual ~Point();
   
   bool operator==(const Point& other) const;
   bool operator!=(const Point& other) const {return !(*this==other);}
   
   // -- methods
   double X() const {return fPos.X();}
   double Y() const {return fPos.Y();}
   double Z() const {return fPos.Z();}
   double T() const {return fT;}
   const TVector3& GetPosition() const {return fPos;}
   
   void SetT(double t) {fT = t;}
   void SetPoint(double x, double y, double z, double t) {fPos.SetXYZ(x,y,z); fT=t;}
   
};

#endif
