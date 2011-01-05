#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "FieldVertex.h"

//#define PRINT_CONSTRUCTORS 
//#define VERBOSE

using namespace std;

//______________________________________________________________________________
FieldVertex::FieldVertex()
      :fX(0.), fY(0.), fZ(0.)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Default Construct" << endl;
   #endif
}

//______________________________________________________________________________
FieldVertex::FieldVertex(double x, double y, double z)
      :fX(x), fY(y), fZ(z)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Construct" << endl;
   #endif
}

//______________________________________________________________________________
FieldVertex::FieldVertex(const FieldVertex& other)
      :fX(other.fX),
       fY(other.fY),
       fZ(other.fZ)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Copy Construct" << endl;
   #endif
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
   if (other.X() == fX && other.Y() == fY && other.Z() == fZ){
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
   oss << "(" << fX << ", " << fY << ", " << fZ << ")";
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
