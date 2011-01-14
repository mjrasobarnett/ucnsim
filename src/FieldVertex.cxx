#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "FieldVertex.h"

//#define PRINT_CONSTRUCTORS 
//#define VERBOSE

using namespace std;

ClassImp(FieldVertex);

//______________________________________________________________________________
FieldVertex::FieldVertex()
      :TObject(),
       fX(0.), fY(0.), fZ(0.), fBField()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Default Construct" << endl;
   #endif
}

//______________________________________________________________________________
FieldVertex::FieldVertex(double x, double y, double z, double bx, double by, double bz)
      :TObject(),
       fX(x), fY(y), fZ(z), fBField(bx, by, bz)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Construct" << endl;
   #endif
}

//______________________________________________________________________________
FieldVertex::FieldVertex(const FieldVertex& other)
      :TObject(other),
       fX(other.fX),
       fY(other.fY),
       fZ(other.fZ),
       fBField(other.fBField)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Copy Construct" << endl;
   #endif
}

//______________________________________________________________________________
FieldVertex& FieldVertex::operator=(const FieldVertex& other)
{
   // Assignment
   if(this!=&other) {
      TObject::operator=(other);
      fX = other.fX;
      fY = other.fY;
      fZ = other.fZ;
      fBField = other.fBField;
   }
   return *this;
   
}

//______________________________________________________________________________
FieldVertex::~FieldVertex()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Destruct" << endl;
   #endif
}

//______________________________________________________________________________
bool FieldVertex::operator==(const FieldVertex& other) const
{
   if (other.X() == fX && other.Y() == fY && other.Z() == fZ) {
      return true;
   } else {
      return false;
   }
}

//______________________________________________________________________________
string FieldVertex::ToString() const 
{
   ostringstream oss;
   oss.precision(2);
   oss << "(" << fX << ", " << fY << ", " << fZ << "), ";
   oss << "(" << fBField.X() << ", " << fBField.Y() << ", " << fBField.Z() << ") ";
   return oss.str();
}

//______________________________________________________________________________
double FieldVertex::SquaredDistanceTo(const FieldVertex& vertex) const
{
   double xdist = pow((vertex.X() - this->X()), 2.0);
   double ydist = pow((vertex.Y() - this->Y()), 2.0);
   double zdist = pow((vertex.Z() - this->Z()), 2.0);
   return xdist + ydist + zdist;
}

//______________________________________________________________________________
double FieldVertex::DistanceTo(const FieldVertex& vertex) const
{
   return sqrt(SquaredDistanceTo(vertex));
}
