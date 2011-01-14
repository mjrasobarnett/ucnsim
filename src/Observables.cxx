// UCN Observables class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <cassert>

#include "Observables.h"

using namespace std;

//#define PRINT_CONSTRUCTORS

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    SpinObservables                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
ClassImp(SpinObservables)

//_____________________________________________________________________________
SpinObservables::SpinObservables()
                    :std::map<Double_t, Bool_t>(),
                     TObject()
{
   // Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinObservables","Default Constructor");
   #endif
}

//_____________________________________________________________________________
SpinObservables::SpinObservables(const SpinObservables& other)
                    :std::map<Double_t, Bool_t>(other),
                     TObject(other)
{
   // Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinObservables","Copy Constructor");
   #endif
}

//_____________________________________________________________________________
SpinObservables& SpinObservables::operator=(const SpinObservables& other)
{
   // Assignment
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinObservables","Assignment");
   #endif
   if(this!=&other) {
      std::map<Double_t,Bool_t>::operator=(other);
      TObject::operator=(other);
   }
   return *this;
}

//_____________________________________________________________________________
SpinObservables::~SpinObservables()
{
   // Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinObservables","Destructor");
   #endif
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    BounceObservables                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
ClassImp(BounceObservables)

//_____________________________________________________________________________
BounceObservables::BounceObservables()
                      :TObject(), fSpecular(0), fDiffuse(0)
{
   // Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("BounceObservables","Default Constructor");
   #endif
}

//_____________________________________________________________________________
BounceObservables::BounceObservables(const BounceObservables& other)
                      :TObject(other), fSpecular(other.fSpecular), fDiffuse(other.fDiffuse)
{
   // Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("BounceObservables","Copy Constructor");
   #endif
}

//_____________________________________________________________________________
BounceObservables& BounceObservables::operator=(const BounceObservables& other)
{
   // Assignment
   #ifdef PRINT_CONSTRUCTORS
      Info("BounceObservables","Assignment");
   #endif
   if(this!=&other) {
      TObject::operator=(other);
      fSpecular=other.fSpecular;
      fDiffuse=other.fDiffuse;
   }
   return *this;
}

//_____________________________________________________________________________
BounceObservables::~BounceObservables()
{
   // Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("BounceObservables","Destructor");
   #endif
}


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    FieldObservables                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
ClassImp(FieldObservables)


//_____________________________________________________________________________
FieldObservables::FieldObservables()
                 :std::vector<const FieldVertex*>(),
                  TObject()
{
   // Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("FieldObservables","Default Constructor");
   #endif
}

//_____________________________________________________________________________
FieldObservables::FieldObservables(const FieldObservables& other)
                    :std::vector<const FieldVertex*>(other),
                     TObject(other)
{
   // Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("FieldObservables","Copy Constructor");
   #endif
}

//_____________________________________________________________________________
FieldObservables::~FieldObservables()
{
   // Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("FieldObservables","Destructor");
   #endif
   PurgeContainer();
}

//______________________________________________________________________________
void FieldObservables::PurgeContainer()
{
   // -- Delete all elements in container
   if (this->empty() == kFALSE) {
      vector<const FieldVertex*>::iterator it;
      for(it = this->begin(); it != this->end(); ++it) {
         delete *it;
         *it = 0;
      }
   }
}
