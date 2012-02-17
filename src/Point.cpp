// Track
// Author: Matthew Raso-Barnett  03/12/2010

#include <iostream>
#include "Point.hpp"
#include "TMath.h"

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

using namespace std;

//_____________________________________________________________________________
Point::Point()
      :fPos(0.,0.,0.), fT(0.)
{
// -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point - Default Constructor" << endl;
   #endif
} 

//_____________________________________________________________________________
Point::Point(double x, double y, double z, double t)
      :fT(t)
{
// -- constructor
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point", "Constructor" << endl;
   #endif
   fPos.SetXYZ(x,y,z);
}

//_____________________________________________________________________________
Point::Point(TVector3 pos, double t)
      :fPos(pos), fT(t)
{
// -- constructor
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point", "Constructor" << endl;
   #endif
}

//_____________________________________________________________________________
Point::Point(double r, double theta, double z)
      :fT(0.)
{
  // -- constructor   
  fPos.SetXYZ(r*TMath::Cos(theta), r*TMath::Sin(theta), z);
}

//_____________________________________________________________________________
Point::Point(const Point& other)
      :fPos(other.fPos), fT(other.fT)
{
// -- Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point", "Copy Constructor" << endl;
   #endif
}

//_____________________________________________________________________________
Point& Point::operator=(const Point& other)
{
// --assignment operator
   if(this!=&other) {
      fPos = other.fPos;
      fT = other.fT;
   }
   return *this;
}

//______________________________________________________________________________
Point::~Point()
{
// -- Destructor
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point", "Destructor" << endl;
   #endif
}

//______________________________________________________________________________
bool Point::operator==(const Point& other) const
{
   if (fPos == other.GetPosition() && fT == other.T()) {
      return true;
   } else {
      return false;
   }
}
