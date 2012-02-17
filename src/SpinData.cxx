// SpinData class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <cassert>

#include "SpinData.h"

using namespace std;

//#define PRINT_CONSTRUCTORS

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    SpinData                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
ClassImp(SpinData)

//_____________________________________________________________________________
SpinData::SpinData()
         :map<Double_t, const Spin*>(),
          TObject()
{
   // Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinData","Default Constructor");
   #endif
}

//_____________________________________________________________________________
SpinData::SpinData(const SpinData& other)
         :map<Double_t, const Spin*>(other),
          TObject(other)
{
   // Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinData","Copy Constructor");
   #endif
}

//_____________________________________________________________________________
SpinData& SpinData::operator=(const SpinData& other)
{
   // Assignment
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinData","Assignment");
   #endif
   if(this!=&other) {
      map<Double_t,const Spin*>::operator=(other);
      TObject::operator=(other);
   }
   return *this;
}

//_____________________________________________________________________________
SpinData::~SpinData()
{
   // Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinData","Destructor");
   #endif
   PurgeContainer();
}

//______________________________________________________________________________
void SpinData::PurgeContainer()
{
   // -- Delete all elements in container
   map<Double_t, const Spin*>::iterator it;
   for(it = this->begin(); it != this->end(); ++it) {
      delete it->second;
      it->second = NULL;
   }
}
