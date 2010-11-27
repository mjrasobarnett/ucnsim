// UCN Observables class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <cassert>

#include "TUCNObservables.h"

using namespace std;

//#define PRINT_CONSTRUCTORS

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNSpinObservables                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
ClassImp(TUCNSpinObservables)

//_____________________________________________________________________________
TUCNSpinObservables::TUCNSpinObservables()
                    :std::map<Double_t, Bool_t>(),
                     TObject()
{
   // Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNSpinObservables","Default Constructor");
   #endif
}

//_____________________________________________________________________________
TUCNSpinObservables::TUCNSpinObservables(const TUCNSpinObservables& other)
                    :std::map<Double_t, Bool_t>(other),
                     TObject(other)
{
   // Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNSpinObservables","Copy Constructor");
   #endif
}

//_____________________________________________________________________________
TUCNSpinObservables& TUCNSpinObservables::operator=(const TUCNSpinObservables& other)
{
   // Assignment
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNSpinObservables","Assignment");
   #endif
   if(this!=&other) {
      std::map<Double_t,Bool_t>::operator=(other);
      TObject::operator=(other);
   }
   return *this;
}

//_____________________________________________________________________________
TUCNSpinObservables::~TUCNSpinObservables()
{
   // Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNSpinObservables","Destructor");
   #endif
}