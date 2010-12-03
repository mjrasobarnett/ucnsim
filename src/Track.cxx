// Track
// Author: Matthew Raso-Barnett  03/12/2010

#include <iostream>
#include <sstream>
#include <cassert>
#include <stdexcept>

#include "Track.h"

//#define VERBOSE_MODE

using namespace std;

ClassImp(Track)

//_____________________________________________________________________________
Track::Track()
      :vector<Vertex>(),
       TObject()
{
// -- Default constructor
   Info("Track", "Default Constructor");
} 

//_____________________________________________________________________________
Track::Track(const Track& other)
      :vector<Vertex>(other),
       TObject(other)
{
// -- Copy Constructor
   Info("Track", "Copy Constructor");
}

//_____________________________________________________________________________
Track& Track::operator=(const Track& other)
{
// --assignment operator
   if(this!=&other) {
      vector<Vertex>::operator=(other);
      TObject::operator=(other);
   }
   return *this;
}

//______________________________________________________________________________
Track::~Track()
{
// -- Destructor
   Info("Track", "Destructor");
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
   Info("Vertex", "Default Constructor");
} 

//_____________________________________________________________________________
Vertex::Vertex(const Double_t x, const Double_t y, const Double_t z, const Double_t t)
       :TObject(),
        fX(x), fY(y), fZ(z), fT(t)
{
// -- constructor
   Info("Vertex", "Constructor");
}

//_____________________________________________________________________________
Vertex::Vertex(const Vertex& other)
      :TObject(other),
       fX(other.fX), fY(other.fY), fZ(other.fZ), fT(other.fT)
{
// -- Copy Constructor
   Info("Vertex", "Copy Constructor");
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
   Info("Vertex", "Destructor");
}
