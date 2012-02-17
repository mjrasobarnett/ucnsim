// Observable
// Author: Matthew Raso-Barnett  13/12/2010

#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <vector>
#include <string>
#include "Observer.hpp"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// Observable -                                                             //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Point;

class Observable {
private:
   // Observers
   std::vector<Observer* > fObservers; // Observers that are watching me and only me
   
public:
   Observable();
   Observable(const Observable&);
   virtual ~Observable();
   
   // -- Observer management
   Int_t     CountObservers() {return fObservers.size();}
   void      Attach(Observer* observer);
   void      DetachAll();
   Observer* GetObserver(int index) {return fObservers[index];}
   
   void NotifyObservers(const Point& point, const TVector3& velocity, const std::string& context);

};

#endif
