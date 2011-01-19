// Point
// Author: Matthew Raso-Barnett  03/12/2010

#ifndef POINT_H
#define POINT_H

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//       Point                                                           //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class Point {
private:
   double fX, fY, fZ, fT;
   
public:
   // -- constructors
   Point();
   Point(double x, double y, double z, double t);
   Point(const Point&); 
   Point& operator=(const Point&);
   // -- destructor
   virtual ~Point();
   
   bool operator==(const Point& other) const;
   bool operator!=(const Point& other) const {return !(*this==other);}
   
   // -- methods
   double X() const {return fX;}
   double Y() const {return fY;}
   double Z() const {return fZ;}
   double T() const {return fT;}
   
   void SetT(double t) {fT = t;}
   void SetPoint(double x, double y, double z, double t) {fX=x; fY=y; fZ=z; fT=t;}
   
};

#endif
