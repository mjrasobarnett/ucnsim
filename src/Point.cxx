// Track
// Author: Matthew Raso-Barnett  03/12/2010

#include <iostream>
#include "Point.h"

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

using namespace std;

ClassImp(Point)

//_____________________________________________________________________________
Point::Point()
       :TObject(),
        fX(0.), fY(0.), fZ(0.), fT(0.)
{
// -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Point", "Default Constructor");
   #endif
} 

//_____________________________________________________________________________
Point::Point(const Double_t x, const Double_t y, const Double_t z, const Double_t t)
       :TObject(),
        fX(x), fY(y), fZ(z), fT(t)
{
// -- constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Point", "Constructor");
   #endif
}

//_____________________________________________________________________________
Point::Point(const Point& other)
      :TObject(other),
       fX(other.fX), fY(other.fY), fZ(other.fZ), fT(other.fT)
{
// -- Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Point", "Copy Constructor");
   #endif
}

//_____________________________________________________________________________
Point& Point::operator=(const Point& other)
{
// --assignment operator
   if(this!=&other) {
      TObject::operator=(other);
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
      Info("Point", "Destructor");
   #endif
}