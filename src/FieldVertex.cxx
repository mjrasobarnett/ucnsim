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
             fPoint(), fBField()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Default Construct" << endl;
   #endif
}

//______________________________________________________________________________
FieldVertex::FieldVertex(double x, double y, double z, double t, double bx, double by, double bz)
            :TObject(),
             fPoint(x,y,z,t), fBField(bx, by, bz)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Construct" << endl;
   #endif
}


//______________________________________________________________________________
FieldVertex::FieldVertex(const Point& point, const TVector3& bfield)
            :TObject(),
             fPoint(point), fBField(bfield)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "FieldVertex Construct" << endl;
   #endif
}

//______________________________________________________________________________
FieldVertex::FieldVertex(const FieldVertex& other)
            :TObject(other),
             fPoint(other.fPoint),
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
      fPoint = other.fPoint;
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
   if (other.GetPoint() == fPoint) {
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
   oss << "(" << fPoint.X() << ", " << fPoint.Y() << ", " << fPoint.X() << ", " << fPoint.T();
   oss << "), ";
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