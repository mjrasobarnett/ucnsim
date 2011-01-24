// Clock class
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Clock.h"
#include "RunConfig.h"

using namespace std;

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

//______________________________________________________________________________
Clock::Clock(const RunConfig& config)
{
   // -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Clock","Default Constructor");
   #endif
}

//_____________________________________________________________________________
Clock::Clock(const Clock& other)
{
   // -- Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Clock","Copy Constructor");
   #endif
}

//______________________________________________________________________________
Clock::~Clock()
{ 
   // -- Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Clock","Destructor");
   #endif
}

//______________________________________________________________________________
double Clock::GetTimeOfNextEvent(double currentTime) const
{
   return 0.0;
}
