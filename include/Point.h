// Point
// Author: Matthew Raso-Barnett  03/12/2010

#ifndef POINT_H
#define POINT_H

#include "TObject.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//       Point                                                           //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class Point : public TObject
{
private:
   Double_t fX, fY, fZ, fT;
   
public:
   // -- constructors
   Point();
   Point(Double_t x, Double_t y, Double_t z, Double_t t);
   Point(const Point&); 
   Point& operator=(const Point&);
   // -- destructor
   virtual ~Point();
   
   // -- methods
   Double_t X() const {return fX;}
   Double_t Y() const {return fY;}
   Double_t Z() const {return fZ;}
   Double_t T() const {return fT;}
   
   void SetPoint(Double_t x, Double_t y, Double_t z, Double_t t) {fX=x; fY=y; fZ=z; fT=t;}
   
   ClassDef(Point, 1)
};

#endif
