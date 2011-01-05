#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "Point.hpp"

//#define PRINT_CONSTRUCTORS 
//#define VERBOSE

using namespace std;

//______________________________________________________________________________
Point::Point()
      :fX(0.), fY(0.), fZ(0.)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point Default Construct" << endl;
   #endif
}

//______________________________________________________________________________
Point::Point(double x, double y, double z)
      :fX(x), fY(y), fZ(z)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point Construct" << endl;
   #endif
}

//______________________________________________________________________________
Point::Point(const Point& other)
      :fX(other.fX),
       fY(other.fY),
       fZ(other.fZ)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point Copy Construct" << endl;
   #endif
}

//______________________________________________________________________________
Point::~Point()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point Destruct" << endl;
   #endif
}

//______________________________________________________________________________
bool Point::operator==(const Point& other) const
{
   if (other.X() == fX && other.Y() == fY && other.Z() == fZ){
      return true;
   } else {
      return false;
   }
}

//______________________________________________________________________________
string Point::ToString() const 
{
   ostringstream oss;
   oss.precision(2);
   oss << "(" << fX << ", " << fY << ", " << fZ << ")";
   return oss.str();
}

//______________________________________________________________________________
double Point::SquaredDistanceTo(const Point& point) const
{
   double xdist = pow((point.X() - this->X()), 2.0);
   double ydist = pow((point.Y() - this->Y()), 2.0);
   double zdist = pow((point.Z() - this->Z()), 2.0);
   return xdist + ydist + zdist;
}

//______________________________________________________________________________
double Point::DistanceTo(const Point& point) const
{
   return sqrt(SquaredDistanceTo(point));
}
