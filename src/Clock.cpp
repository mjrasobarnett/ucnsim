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
       fMaxStepInterval(0.),
       fExternalEvents()
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
   return true;
}

//______________________________________________________________________________
void Clock::ScheduleEvent(const std::string name, const double interval, const double prevMeasureTime)
{
   // -- Add this event to the Clock to be scheduled in Run
   cout << "Scheduling Event: " << name << "\t" << "Measurement Interval: " << interval << "s" << endl;
   EventTimer newEvent(name, interval, prevMeasureTime);
   fExternalEvents.push_back(newEvent);
}

//______________________________________________________________________________
bool Clock::CancelEvent(const std::string name)
{
   // Remove named event from list
   vector<EventTimer>::iterator eventIt;
   for (eventIt = fExternalEvents.begin(); eventIt != fExternalEvents.end(); eventIt++) {
      if (eventIt->fEventName == name) {
         fExternalEvents.erase(eventIt);
         return true;
      }
   }
   return false;
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
   // Finally check our list of external events to see if any will occur 
   // within next proposed interval. If so, set nextInterval to the time to this event
   vector<EventTimer>::iterator eventIt;
   for (eventIt = fExternalEvents.begin(); eventIt != fExternalEvents.end(); eventIt++) {
      EventTimer& event = (*eventIt);
      // Check if the event is only measured at a specific frequency
      if (event.fMeasInterval > 0.0) {
         // Update this event's last measurement time if we reached it
         if (fTime >= event.fPreviousMeas + event.fMeasInterval) {event.fPreviousMeas += event.fMeasInterval;}
         // Check if we will make a spin measurement in this time
         const double nextEventMeas = event.fPreviousMeas + event.fMeasInterval;
         if (fTime + nextInterval >= nextEventMeas) {
            nextInterval = nextEventMeas - fTime;
         }
      }
   }
   return nextInterval;
}

//______________________________________________________________________________
void Clock::Reset()
{
   fTime = 0.0;
   // Set the time of previous measurement for every external event back to 0
   vector<EventTimer>::iterator eventIt;
   for (eventIt = fExternalEvents.begin(); eventIt != fExternalEvents.end(); eventIt++) {
      eventIt->fPreviousMeas = 0.0;
   }
}
