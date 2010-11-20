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

using namespace std;

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
         :TObject(other), fSpinor(other.fSpinor)
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
      fSpinor = other.fSpinor;   
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
Bool_t TUCNSpin::Precess(const TVector3& /*localMagField*/, const Double_t /*precessTime*/)
{
   // Take mag field in the same coordinate system as spin vector and precess about it
   
   
   return kTRUE;
}

//_____________________________________________________________________________
void TUCNSpin::Print(Option_t* /*option*/) const
{
   fSpinor.Print();
}

//_____________________________________________________________________________
Bool_t TUCNSpin::Polarise(const TVector3& axis, const Bool_t up)
{
   if (up == kTRUE) {
      fSpinor.PolariseUp(axis);
   } else {
      fSpinor.PolariseDown(axis);
   }
   fSpinor.Print();
   return kTRUE;
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNSpinor                                                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNSpinor)

//_____________________________________________________________________________
TUCNSpinor::TUCNSpinor()
           :TObject(), fUp(0,0), fDown(0,0)
{
   // Constructor
//   Info("TUCNSpinor","Default Constructor");
}

//_____________________________________________________________________________
TUCNSpinor::TUCNSpinor(const TUCNSpinor& other)
           :TObject(other), fUp(other.fUp), fDown(other.fDown)
{
   // Copy Constructor
//   Info("TUCNSpinor","Copy Constructor");
}

//_____________________________________________________________________________
TUCNSpinor& TUCNSpinor::operator=(const TUCNSpinor& other)
{
   // Assignment
//   Info("TUCNSpinor","Assignment");
   if(this!=&other) {
      TObject::operator=(other);
      fUp = other.fUp;
      fDown = other.fDown;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNSpinor::~TUCNSpinor()
{
   // Destructor
//   Info("TUCNSpinor","Destructor");
}

//_____________________________________________________________________________
void TUCNSpinor::PolariseUp(const TVector3& axis)
{
   // -- Set spinor components to be eigenvector of the operator for spin in the direction
   // -- defined by 'axis'. Expressions derived in notebook.
   TVector3 unit = axis.Unit();
   Double_t mag = 1.0/TMath::Sqrt(2.0 + 2.0*TMath::Abs(unit.Z()));
   TComplex up((unit.Z() + 1.0)*mag, 0);
   TComplex down(unit.X()*mag, unit.Y()*mag);
   fUp = up;
   fDown = down;
}

//_____________________________________________________________________________
void TUCNSpinor::PolariseDown(const TVector3& axis)
{
   // -- Set spinor components to be eigenvector of the operator for spin in the direction
   // -- defined by 'axis'. Expressions derived in notebook.
   TVector3 unit = axis.Unit();
   Double_t mag = 1.0/TMath::Sqrt(2.0 + 2.0*TMath::Abs(unit.Z()));
   TComplex up((unit.Z() - 1.0)*mag, 0);
   TComplex down(unit.X()*mag, unit.Y()*mag);
   fUp = up;
   fDown = down;
}

//_____________________________________________________________________________
void TUCNSpinor::Print(Option_t* /*option*/) const
{
   cout << "Spin Up: " << fUp.Re() << " + i" << fUp.Im() << endl;
   cout << "Spin Down: " << fDown.Re() << "+ i" << fDown.Im() << endl;
}
