// Clock class
// Author: Matthew Raso-Barnett  24/01/2011

#ifndef CLOCK_H
#define CLOCK_H

class Clock
{
private:
   // -- Members
   static Clock* fgClock;
   
   double fTime;
      
   double fRunEnd;
   double fMaxStepInterval;
   double fSpinMeasFreq;
   double fFieldMeasFreq;
   double fLastSpinMeas;
   double fLastFieldMeas;
   
   // -- Hidden Constructor
   Clock();
   
public:
   // -- Singleton
   static Clock* 	Instance();
	
   // -- Destructor
   virtual ~Clock();
   
   // -- Methods
   void Tick(double interval) {fTime += interval;}
   
   void SetTime(double currTime) {fTime = currTime;}
   void SetEndOfRun(double runTime) {fRunEnd = runTime;}
   void SetMaxStepInterval(double maxStepTime) {fMaxStepInterval = maxStepTime;}
   void SetSpinMeasureFreq(double freq) {fSpinMeasFreq = freq;}
   void SetFieldMeasureFreq(double freq) {fFieldMeasFreq = freq;}
   
   double GetTime() const {return fTime;}
   
   double GetTimeToNextEvent();
   void Reset();
   
};

#endif  /*CLOCK_H*/
