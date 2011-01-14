// Track
// Author: Matthew Raso-Barnett  03/12/2010

#include <iostream>
#include "Point.h"

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

using namespace std;

//_____________________________________________________________________________
Point::Point()
      :fX(0.), fY(0.), fZ(0.), fT(0.)
{
// -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point - Default Constructor" << endl;
   #endif
} 

//_____________________________________________________________________________
Point::Point(double x, double y, double z, double t)
      :fX(x), fY(y), fZ(z), fT(t)
{
// -- constructor
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point", "Constructor" << endl;
   #endif
}

//_____________________________________________________________________________
Point::Point(const Point& other)
      :fX(other.fX), fY(other.fY), fZ(other.fZ), fT(other.fT)
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
      fX = other.fX;
      fY = other.fY;
      fZ = other.fZ;
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
   if (other.X() == fX && other.Y() == fY && other.Z() == fZ && other.T() == fT) {
      return true;
   } else {
      return false;
   }
}
