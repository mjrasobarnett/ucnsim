// Track
// Author: Matthew Raso-Barnett  03/12/2010

#include <iostream>
#include <sstream>
#include <cassert>
#include <stdexcept>

#include "Track.h"

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

using namespace std;

ClassImp(Track)

//_____________________________________________________________________________
Track::Track()
      :TObject()
{
// -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Track", "Default Constructor");
   #endif
} 

//_____________________________________________________________________________
Track::Track(const Track& other)
      :TObject(other)
{
// -- Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Track", "Copy Constructor");
   #endif
}

//_____________________________________________________________________________
Track& Track::operator=(const Track& other)
{
// --assignment operator
   if(this!=&other) {
      TObject::operator=(other);
      this->PurgeContainer();
      fPoints = other.fPoints;
   }
   return *this;
}

//______________________________________________________________________________
Track::~Track()
{
// -- Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Track", "Destructor");
   #endif
   this->PurgeContainer();
}

//______________________________________________________________________________
void Track::PurgeContainer()
{
   // -- Delete all elements in container
   vector<Point*>::iterator it;
   for(it = fPoints.begin(); it != fPoints.end(); ++it) {
      delete *it;
      *it = 0;
   }
}

//______________________________________________________________________________
void Track::AddPoint(const Double_t x, const Double_t y, const Double_t z, const Double_t t)
{
   // -- Add point to track
   fPoints.push_back(new Point(x,y,z,t));
}

//______________________________________________________________________________
const Point& Track::GetPoint(unsigned int i) const
{
   // -- Retrieve point from track
   // Check for requests past bounds of storage
   if (i >= fPoints.size()) {return *(fPoints.back());}
   return *(fPoints[i]);
}

//______________________________________________________________________________
vector<Double_t> Track::OutputPointsArray()
{
   // -- Return an array of size 3*number-of-vertices, which contains just the positions
   // -- of each vertex, to be used for creating a TPolyLine3D for drawing purposes
   vector<Double_t> points;
   // Loop over all vertices
   vector<Point*>::const_iterator vertexIter;
   for (vertexIter = fPoints.begin(); vertexIter != fPoints.end(); vertexIter++) {
      // Fill array of points with X, Y, Z of each vertex
      points.push_back((*vertexIter)->X());
      points.push_back((*vertexIter)->Y());
      points.push_back((*vertexIter)->Z());
   }
   return points;
}

