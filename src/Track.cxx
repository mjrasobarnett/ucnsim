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
      fVertices = other.fVertices;
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
   if (fVertices.empty() == kFALSE) {
      vector<Vertex*>::iterator it;
      for(it = fVertices.begin(); it != fVertices.end(); ++it) {
         delete *it;
         *it = 0;
      }
   }
}

//______________________________________________________________________________
void Track::AddVertex(const Double_t x, const Double_t y, const Double_t z, const Double_t t)
{
   // -- Add point to track
   fVertices.push_back(new Vertex(x,y,z,t));
}

//______________________________________________________________________________
const Vertex& Track::GetVertex(unsigned int i) const
{
   // -- Retrieve point from track
   // Check for requests past bounds of storage
   if (i >= fVertices.size()) {return *(fVertices.back());}
   return *(fVertices[i]);
}

//______________________________________________________________________________
vector<Double_t> Track::OutputPointsArray()
{
   // -- Return an array of size 3*number-of-vertices, which contains just the positions
   // -- of each vertex, to be used for creating a TPolyLine3D for drawing purposes
   vector<Double_t> points;
   // Loop over all vertices
   vector<Vertex*>::const_iterator vertexIter;
   for (vertexIter = fVertices.begin(); vertexIter != fVertices.end(); vertexIter++) {
      // Fill array of points with X, Y, Z of each vertex
      points.push_back((*vertexIter)->X());
      points.push_back((*vertexIter)->Y());
      points.push_back((*vertexIter)->Z());
   }
   return points;
}

//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________


ClassImp(Vertex)

//_____________________________________________________________________________
Vertex::Vertex()
       :TObject(),
        fX(0.), fY(0.), fZ(0.), fT(0.)
{
// -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Vertex", "Default Constructor");
   #endif
} 

//_____________________________________________________________________________
Vertex::Vertex(const Double_t x, const Double_t y, const Double_t z, const Double_t t)
       :TObject(),
        fX(x), fY(y), fZ(z), fT(t)
{
// -- constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Vertex", "Constructor");
   #endif
}

//_____________________________________________________________________________
Vertex::Vertex(const Vertex& other)
      :TObject(other),
       fX(other.fX), fY(other.fY), fZ(other.fZ), fT(other.fT)
{
// -- Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Vertex", "Copy Constructor");
   #endif
}

//_____________________________________________________________________________
Vertex& Vertex::operator=(const Vertex& other)
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
Vertex::~Vertex()
{
// -- Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Vertex", "Destructor");
   #endif
}
