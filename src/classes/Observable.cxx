#include <iostream>

#include "Observable.h"

//#define PRINT_CONSTRUCTORS 
//#define VERBOSE

using namespace std;


//______________________________________________________________________________
Observable::Observable()
        :fObservers()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Observable Default Constructor" << endl;
   #endif
}

//______________________________________________________________________________
Observable::Observable(const Observable& other)
        :fObservers(other.fObservers)
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

//_____________________________________________________________________________
void Observable::DetachAll()
{
   // -- Remove all observers from the particle's list of observers
   fObservers.erase(fObservers.begin(), fObservers.end());
}

//_____________________________________________________________________________
void Observable::WriteObservers(Data& data)
{
   // -- Get Observers to write their data to the tree
   vector<Observer*>::iterator it;
   for(it = fObservers.begin(); it != fObservers.end(); it++) {
      // Write out to Tree
      (*it)->WriteToFile(data);
   }
}
