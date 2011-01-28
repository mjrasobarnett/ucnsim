// Clock class
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Clock.h"

using namespace std;

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

Clock *Clock::fgClock = NULL;

//______________________________________________________________________________
Clock::Clock()
      :fTime(0.),
       fRunEnd(0.),
       fMaxStepInterval(0.),
       fSpinMeasFreq(0.),
       fLastSpinMeas(0.),
       fLastFieldMeas(0.)
       
{
   // -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Clock","Default Constructor");
   #endif
   fgClock = this;
}

//______________________________________________________________________________
Clock::~Clock()
{ 
   // -- Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Clock","Destructor");
   #endif
   fgClock = NULL;
}

//_____________________________________________________________________________
Clock *Clock::Instance()
{
// Return pointer to singleton.
   if (!fgClock) fgClock = new Clock();
   return fgClock;
}   


//______________________________________________________________________________
double Clock::GetTimeToNextEvent()
{
   // If end of run has been reached
   if (fTime >= fRunEnd) return 0.0;
   // Set next event time to maximum step size permitted
   double nextInterval = fMaxStepInterval;
   // Check if we will reach end of run in this time and adjust
   if (fTime + nextInterval >= fRunEnd) {nextInterval = fRunEnd - fTime;}
   // Update LastSpin Measurement time if we reached it
   if (fTime >= fLastSpinMeas + fSpinMeasFreq) {fLastSpinMeas += fSpinMeasFreq;}
   // Check if we will make a spin measurement in this time
   const double nextSpinMeasureTime = fLastSpinMeas + fSpinMeasFreq;
   if (fTime + nextInterval >= nextSpinMeasureTime) {nextInterval = nextSpinMeasureTime - fTime;}
   // Update LastField Measurement time if we reached it
   if (fTime >= fLastFieldMeas + fFieldMeasFreq) {fLastFieldMeas += fFieldMeasFreq;}
   // Check if we will make a field measurement in this time
   const double nextFieldMeasureTime = fLastFieldMeas + fFieldMeasFreq;
   if (fTime + nextInterval >= nextFieldMeasureTime) {nextInterval = nextFieldMeasureTime - fTime;}
   return nextInterval;
}

//______________________________________________________________________________
void Clock::Reset()
{
   fTime = 0.0;
   fLastSpinMeas = 0.0;
   fLastFieldMeas = 0.0;
}
