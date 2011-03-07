// Observable
// Author: Matthew Raso-Barnett  13/12/2010

#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <vector>
#include <string>
#include "Observer.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// Observable -                                                             //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Point;

class Observable {
private:
   // Observers
   std::vector<Observer* > fObservers;
   
public:
   Observable();
   Observable(const Observable&);
   virtual ~Observable();
   
   // -- Observer management
   Int_t     CountObservers() {return fObservers.size();}
   void      Attach(Observer* observer);
   void      DetachAll();
   Observer* GetObserver(int index) {return fObservers[index];}
   void      WriteObserversToFile(TDirectory* particleDir);
   
   virtual void      NotifyObservers(const Point& point, const std::string& context) = 0;

};

#endif
