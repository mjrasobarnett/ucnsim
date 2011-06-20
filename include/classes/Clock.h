// Clock class
// Author: Matthew Raso-Barnett  24/01/2011

#ifndef CLOCK_H
#define CLOCK_H

#include <string>
#include <vector>

class RunConfig;

class Clock
{
private:
   // -- Members
   static Clock* fgClock;
   
   
   // -- Times
   double fTime;
   double fRunEnd;
   double fMaxStepInterval;
   
   // -- Event intervals
   typedef struct EventTimer {
      // EventTimer struct is just a convenient way to encapsulate the information
      // about any external events (think an Observer's periodic measurements)
      // needed by the clock to schedule them
      std::string fEventName;
      double fMeasInterval;
      double fPreviousMeas;
      // Constuctor
      EventTimer(std::string name, double interval, double prevMeas) : fEventName(name), fMeasInterval(interval), fPreviousMeas(prevMeas) { }
   };
   // List of all External Events
   std::vector<EventTimer> fExternalEvents;
   
   // -- Hidden Constructor
   Clock();
   Clock(const Clock&);
   Clock& operator=(const Clock&);
   
public:
   // -- Singleton
   static Clock* 	Instance();
   
   // -- Destructor
   virtual ~Clock();
   
   // -- Methods
   bool     Initialise(const RunConfig& runConfig);
   void     ScheduleEvent(const std::string name, const double measFrequency, const double prevMeasureTime = 0.0);
   bool     CancelEvent(const std::string name);
   void     Tick(double interval) {fTime += interval;}
   double   GetTimeToNextEvent();
   double   GetTime() const {return fTime;}
   void     Reset();
   
};

#endif  /*CLOCK_H*/
