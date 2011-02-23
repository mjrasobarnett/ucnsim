// Event class
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Event.h"

using namespace std;

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

//______________________________________________________________________________
Event::Event()
{
   // -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Event","Default Constructor");
   #endif
}

//_____________________________________________________________________________
Event::Event(const Event& other)
{
   // -- Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Event","Copy Constructor");
   #endif
}

//______________________________________________________________________________
Event::~Event()
{ 
   // -- Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Event","Destructor");
   #endif
}
