// Clock class
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Clock.h"
#include "RunConfig.h"

using namespace std;

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

Clock *Clock::fgClock = NULL;

//______________________________________________________________________________
Clock::Clock()
      :fTime(0.),
       fRunEnd(0.),
       fLastSpinMeas(0.),
       fLastFieldMeas(0.),
       fLastTrackMeas(0.),
       fMaxStepInterval(0.),
       fSpinMeasInterval(0.),
       fFieldMeasInterval(0.),
       fTrackMeasInterval(0.)
       
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

//_____________________________________________________________________________
bool Clock::Initialise(const RunConfig& runConfig)
{
   fRunEnd = runConfig.RunTime();
   fMaxStepInterval = runConfig.MaxStepTime();
   fSpinMeasInterval = runConfig.SpinMeasureInterval();
   fFieldMeasInterval = runConfig.FieldMeasureInterval();
   fTrackMeasInterval = runConfig.TrackMeasureInterval();
   return true;
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
   // Check if we are measuring spin at a specific frequency
   if (fSpinMeasInterval > 0.0) {
      // Update LastSpin Measurement time if we reached it
      if (fTime >= fLastSpinMeas + fSpinMeasInterval) {fLastSpinMeas += fSpinMeasInterval;}
      // Check if we will make a spin measurement in this time
      const double nextSpinMeasureTime = fLastSpinMeas + fSpinMeasInterval;
      if (fTime + nextInterval >= nextSpinMeasureTime) {
         nextInterval = nextSpinMeasureTime - fTime;
      }
   }
   // Check if we are measuring field at a specific frequency
   if (fFieldMeasInterval > 0.0) {
      // Update LastField Measurement time if we reached it
      if (fTime >= fLastFieldMeas + fFieldMeasInterval) {fLastFieldMeas += fFieldMeasInterval;}
      // Check if we will make a field measurement in this time
      const double nextFieldMeasureTime = fLastFieldMeas + fFieldMeasInterval;
      if (fTime + nextInterval >= nextFieldMeasureTime) {
         nextInterval = nextFieldMeasureTime - fTime;
      }
   }
   // Check if we are measuring track at a specific frequency
   if (fTrackMeasInterval > 0.0) {
      // Update LastField Measurement time if we reached it
      if (fTime >= fLastTrackMeas + fTrackMeasInterval) {fLastTrackMeas += fTrackMeasInterval;}
      // Check if we will make a field measurement in this time
      const double nextTrackMeasureTime = fLastTrackMeas + fTrackMeasInterval;
      if (fTime + nextInterval >= nextTrackMeasureTime) {
         nextInterval = nextTrackMeasureTime - fTime;
      }
   }
   return nextInterval;
}

//______________________________________________________________________________
void Clock::Reset()
{
   fTime = 0.0;
   fLastSpinMeas = 0.0;
   fLastFieldMeas = 0.0;
   fLastTrackMeas = 0.0;
}
