// Clock class
// Author: Matthew Raso-Barnett  24/01/2011

#ifndef CLOCK_H
#define CLOCK_H

class RunConfig;

class Clock
{
private:
   // -- Members
   
   
public:
   // -- Constructors
   Clock(const RunConfig& config);
   Clock(const Clock& other);
   
   // -- Destructor
   virtual ~Clock();
   
   // -- Methods
   double GetTimeOfNextEvent(double currentTime) const;
   
};

#endif  /*CLOCK_H*/
