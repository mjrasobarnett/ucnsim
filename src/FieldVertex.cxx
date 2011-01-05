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
      :fPos(0.,0.,0.)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Default Construct" << endl;
   #endif
}

//______________________________________________________________________________
FieldVertex::FieldVertex(double x, double y, double z)
      :fPos(x,y,z)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Construct" << endl;
   #endif
}

//______________________________________________________________________________
FieldVertex::FieldVertex(const FieldVertex& other)
      :fPos(other.fPos)
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
   if (other.X() == this->X() && other.Y() == this->Y() && other.Z() == this->Z()) {
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
   oss << "(" << this->X() << ", " << this->Y() << ", " << this->Z() << ")";
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
