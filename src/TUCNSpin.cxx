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
Bool_t TUCNSpin::Precess(const TVector3& avgMagField, const Double_t precessTime)
{
   // -- Choose method for spin precession
   // Quantum mechanical method
   fSpinor.Precess(avgMagField, precessTime);
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
   // -- Set Spin's polarisation along defined axis
   if (up == kTRUE) {
      fSpinor.PolariseUp(axis);
   } else {
      fSpinor.PolariseDown(axis);
   }
   fSpinor.Print();
   return kTRUE;
}

//_____________________________________________________________________________
Double_t TUCNSpin::CalculateProbSpinUp(const TVector3& axis) const
{
   // -- Calculate probability of being spin 'up' along provided axis
   return fSpinor.CalculateProbSpinUp(axis);
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

//_____________________________________________________________________________
Bool_t TUCNSpinor::Precess(const TVector3& avgMagField, const Double_t precessTime)
{
   // -- Take mag field (in the global coordinate system) and precess about it
   Double_t omegaX = Constants::neutron_gyromag_ratio*avgMagField.X();
   Double_t omegaY = Constants::neutron_gyromag_ratio*avgMagField.Y();
   Double_t omegaZ = Constants::neutron_gyromag_ratio*avgMagField.Z();
   // Precession frequency
   Double_t omega = TMath::Sqrt(omegaX*omegaX + omegaY*omegaY + omegaZ*omegaZ);
   Double_t precessAngle = (omega*precessTime)/2.0;
   if (omega == 0.0) return kFALSE;
   Double_t upRe = fUp.Re();
   Double_t upIm = fUp.Im();
   Double_t downRe = fDown.Re();
   Double_t downIm = fDown.Im();
   
   // Spin Up Real Part
   Double_t newUpRe = upRe*TMath::Cos(precessAngle) + ((upIm*omegaZ - downIm*omegaX - downRe*omegaY)/omega)*TMath::Sin(precessAngle);
   // Spin Up Imaginary Part
   Double_t newUpIm = upIm*TMath::Cos(precessAngle) + ((downRe*omegaX - upRe*omegaZ - downIm*omegaY)/omega)*TMath::Sin(precessAngle);
   
   // Spin Down Real Part
   Double_t newDownRe = downRe*TMath::Cos(precessAngle) + ((upIm*omegaX - downIm*omegaZ + upRe*omegaY)/omega)*TMath::Sin(precessAngle);
   // Spin Down Imaginary Part
   Double_t newDownIm = downIm*TMath::Cos(precessAngle) + ((downRe*omegaZ - upRe*omegaX + upIm*omegaY)/omega)*TMath::Sin(precessAngle);
   
   // Update spinor
   TComplex newUp(newUpRe, newUpIm);
   TComplex newDown(newDownRe, newDownIm);
   fUp = newUp;
   fDown = newDown;
   
//   TVector3 axis(0,1,0);
//   CalculateProbSpinUp(axis);
   
   return kTRUE;
}

//_____________________________________________________________________________
Double_t TUCNSpinor::CalculateProbSpinUp(const TVector3& axis) const
{
   // -- Calculate probability of being spin 'up' along provided axis
   // -- See notebook for equations
   TVector3 measurementAxis = axis.Unit();
   Double_t ux = measurementAxis.X();
   Double_t uy = measurementAxis.Y();
   // Take absolute value of Z component to ensure we don't take uz = -1
   Double_t uz = TMath::Abs(measurementAxis.Z());
   Double_t denominator = 2.0 + 2.0*uz;
   TComplex conjugateUp = TComplex::Conjugate(fUp);
   TComplex conjugateDown = TComplex::Conjugate(fDown);
   
   TComplex numer1 = (uz + 1.0)*(uz + 1.0)*fUp*conjugateUp;
   TComplex numer2 = (ux*ux + uy*uy)*fDown*conjugateDown;
   TComplex numer3 = (uz + 1.0)*(ux + TComplex::I()*uy)*fUp*conjugateDown;
   TComplex numer4 = (uz + 1.0)*(ux - TComplex::I()*uy)*fDown*conjugateUp;
   
   TComplex prob = (numer1 + numer2 + numer3 + numer4)/denominator;
//   cout << "Probability: " << prob.Re() << "\t" << prob.Im() << endl;
   return prob.Re();
}
