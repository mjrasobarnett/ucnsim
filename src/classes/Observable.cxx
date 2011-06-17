#include <iostream>

#include "Observable.h"

//#define PRINT_CONSTRUCTORS 
//#define VERBOSE

using namespace std;


//______________________________________________________________________________
Observable::Observable()
        :fObservers(),
         fSharedObservers()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Observable Default Constructor" << endl;
   #endif
}

//______________________________________________________________________________
Observable::Observable(const Observable& other)
        :fObservers(other.fObservers),
         fSharedObservers(other.fSharedObservers)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Observable Copy Construct" << endl;
   #endif
} 

//______________________________________________________________________________
Observable::~Observable()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Observable Destruct" << endl;
   #endif
   this->DetachAll();
}

//_____________________________________________________________________________
void Observable::Attach(Observer* observer)
{
   // -- Add an observer to the particle's list of observers
   fObservers.push_back(observer);
}

//______________________________________________________________________________
void Observable::AttachSharedObserver(Observer* observer)
{
   // -- Add an observer to the particle's list of observers
   fSharedObservers.push_back(observer);
}

//_____________________________________________________________________________
void Observable::DetachAll()
{
   // -- Remove all observers from the particle's list of observers
   fObservers.clear();
   fSharedObservers.clear();
}

//_____________________________________________________________________________
void Observable::WriteObservers(Data& data)
{
   // -- Write *ONLY* the observers that watching this observable to the file
   // -- Don't write the shared observers.
   vector<Observer*>::iterator it;
   for(it = fObservers.begin(); it != fObservers.end(); it++) {
      // Write out to Tree
      (*it)->WriteToFile(data);
   }
}

//_____________________________________________________________________________
void Observable::NotifyObservers(const Point& point, const TVector3& velocity, const std::string& context)
{
   // -- Notify *ALL* Observers of change
   vector<Observer*>::iterator it;
   // Notify my observers
   for(it = fObservers.begin(); it != fObservers.end(); it++) {
      (*it)->RecordEvent(point, velocity, context);
   }
   // Notify my shared observers
   for(it = fSharedObservers.begin(); it != fSharedObservers.end(); it++) {
      (*it)->RecordEvent(point, velocity, context);
   }
}

