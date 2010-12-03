// Track
// Author: Matthew Raso-Barnett  03/12/2010

#include <iostream>
#include <sstream>
#include <cassert>
#include <stdexcept>

#include "Track.h"

//#define VERBOSE_MODE

ClassImp(Track)

//_____________________________________________________________________________
Track::Track()
      :TObject()
{
// -- Default constructor
   Info("Track", "Default Constructor");
} 

//_____________________________________________________________________________
Track::Track(const Track& other)
      :TObject(other)
{
// -- Copy Constructor
   Info("Track", "Copy Constructor");
}

//_____________________________________________________________________________
Track& Track::operator=(const Track& other)
{
// --assignment operator
   if(this!=&other) {
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
