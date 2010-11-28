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

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNBounceObservables                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
ClassImp(TUCNBounceObservables)

//_____________________________________________________________________________
TUCNBounceObservables::TUCNBounceObservables()
                      :TObject(), fSpecular(0), fDiffuse(0)
{
   // Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNBounceObservables","Default Constructor");
   #endif
}

//_____________________________________________________________________________
TUCNBounceObservables::TUCNBounceObservables(const TUCNBounceObservables& other)
                      :TObject(other), fSpecular(other.fSpecular), fDiffuse(other.fDiffuse)
{
   // Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNBounceObservables","Copy Constructor");
   #endif
}

//_____________________________________________________________________________
TUCNBounceObservables& TUCNBounceObservables::operator=(const TUCNBounceObservables& other)
{
   // Assignment
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNBounceObservables","Assignment");
   #endif
   if(this!=&other) {
      TObject::operator=(other);
      fSpecular=other.fSpecular;
      fDiffuse=other.fDiffuse;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNBounceObservables::~TUCNBounceObservables()
{
   // Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNBounceObservables","Destructor");
   #endif
}