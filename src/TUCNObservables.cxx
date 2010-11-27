// UCN Observables class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <cassert>

#include "TUCNObservables.h"

using namespace std;


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
   Info("TUCNSpinObservables","Default Constructor");
}

//_____________________________________________________________________________
TUCNSpinObservables::TUCNSpinObservables(const TUCNSpinObservables& other)
                    :std::map<Double_t, Bool_t>(other),
                     TObject(other)
{
   // Copy Constructor
   Info("TUCNSpinObservables","Copy Constructor");
}

//_____________________________________________________________________________
TUCNSpinObservables& TUCNSpinObservables::operator=(const TUCNSpinObservables& other)
{
   // Assignment
   Info("TUCNSpinObservables","Assignment");
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
   Info("TUCNSpinObservables","Destructor");
}