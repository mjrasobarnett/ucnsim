// UCN Spin class
// Author: Matthew Raso-Barnett  19/08/2010
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Constants.h"
#include "Units.h"

#include "TUCNSpin.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNSpin                                                             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNSpin)

//_____________________________________________________________________________
TUCNSpin::TUCNSpin()
          :TObject()
{
   // Constructor
//   Info("TUCNSpin","Default Constructor");
}

//_____________________________________________________________________________
TUCNSpin::TUCNSpin(const TUCNSpin& other)
         :TObject(other), fSpin(other.fSpin)
{
   // Copy Constructor
//   Info("TUCNSpin","Copy Constructor");
}

//_____________________________________________________________________________
TUCNSpin& TUCNSpin::operator=(const TUCNSpin& other)
{
   // Assignment
//   Info("TUCNSpin","Assignment");
   if(this!=&other) {
      TObject::operator=(other);
      fSpin = other.fSpin;   
   }
   return *this;
}

//_____________________________________________________________________________
TUCNSpin::~TUCNSpin()
{
   // Destructor
//   Info("TUCNSpin","Destructor");
}

//_____________________________________________________________________________
Bool_t Precess(const TVector3& /*localMagField*/, const Double_t /*precessTime*/)
{
   // Take mag field in the same coordinate system as spin vector and precess about it
   
   return kTRUE;
}


