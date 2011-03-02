// Clock class
// Author: Matthew Raso-Barnett  24/01/2011

#ifndef CLOCK_H
#define CLOCK_H

class RunConfig;

class Clock
{
private:
   // -- Members
   static Clock* fgClock;
   
   // -- Times
   double fTime;
   double fRunEnd;
   double fLastSpinMeas;
   double fLastFieldMeas;
   double fLastTrackMeas;
   
   // -- Event intervals
   double fMaxStepInterval;
   double fSpinMeasInterval;
   double fFieldMeasInterval;
   double fTrackMeasInterval;
   
   // -- Hidden Constructor
   Clock();
      
public:
   // -- Singleton
   static Clock* 	Instance();
   
   // -- Destructor
   virtual ~Clock();
   
   // -- Methods
   bool     Initialise(const RunConfig& runConfig);
   void     Tick(double interval) {fTime += interval;}
   double   GetTimeToNextEvent();
   double   GetTime() const {return fTime;}
   void     Reset();
   
};

#endif  /*CLOCK_H*/
