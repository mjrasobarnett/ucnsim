// UCN Spin class
// Author: Matthew Raso-Barnett  19/08/2010
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Constants.h"
#include "Units.h"

#include "Spin.h"
#include "TRandom.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Spin                                                             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

//#define PRINT_CONSTRUCTORS 
#define VERBOSE

using namespace std;

ClassImp(Spin)

//_____________________________________________________________________________
Spin::Spin()
         :TObject()
{
   // Constructor
//   Info("Spin","Default Constructor");
}

//_____________________________________________________________________________
Spin::Spin(const Spin& other)
         :TObject(other), fSpinor(other.fSpinor)
{
   // Copy Constructor
//   Info("Spin","Copy Constructor");
}

//_____________________________________________________________________________
Spin& Spin::operator=(const Spin& other)
{
   // Assignment
//   Info("Spin","Assignment");
   if(this!=&other) {
      TObject::operator=(other);
      fSpinor = other.fSpinor;   
   }
   return *this;
}

//_____________________________________________________________________________
Spin::~Spin()
{
   // Destructor
//   Info("Spin","Destructor");
}

//_____________________________________________________________________________
Bool_t Spin::Precess(const TVector3& avgMagField, const Double_t precessTime)
{
   // -- Choose method for spin precession
   // Quantum mechanical method
   fSpinor.Precess(avgMagField, precessTime);
   return kTRUE;
}

//_____________________________________________________________________________
void Spin::Print(Option_t* /*option*/) const
{
   fSpinor.Print();
}

//_____________________________________________________________________________
Bool_t Spin::Polarise(const TVector3& axis, const Bool_t up)
{
   // -- Set Spin's polarisation along defined axis
   if (up == kTRUE) {
      fSpinor.PolariseUp(axis);
   } else {
      fSpinor.PolariseDown(axis);
   }
   return kTRUE;
}

//_____________________________________________________________________________
Double_t Spin::CalculateProbSpinUp(const TVector3& axis) const
{
   // -- Calculate probability of being spin 'up' along provided axis
   return fSpinor.CalculateProbSpinUp(axis);
}

//_____________________________________________________________________________
Bool_t Spin::IsSpinUp(const TVector3& axis) const
{
   // -- Calculate whether particle is in the Spin-up state defined by measurement Axis
   // -- This is a non-destructive calculation, that does not leave the the particle in this
   // -- state after the calculation, as opposed to MeasureSpinUp()
   const double probSpinUp = this->CalculateProbSpinUp(axis);
   bool up = probSpinUp > 1.0 ? false : true;
   cout.precision(10);
   cout << (double)probSpinUp << endl;
   cout << "Up? " << up << endl;
   bool equal = probSpinUp == 1.0 ? true : false;
   cout << "Equal to 1? " << equal << endl;   
   // Check for errorneous probability
   if (probSpinUp < 0.0 || probSpinUp > 1.0) {
      axis.Print();
      throw runtime_error("Spin::IsSpinUp - Found probability outside of 0 -> 1 range.");
   }
   // Roll dice to determine whether Spin Up or Spin Down
   if (gRandom->Uniform(0.0,1.0) <= probSpinUp) {return kTRUE;}
   // Spin down
   return kFALSE;
}


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Spinor                                                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(Spinor)

//_____________________________________________________________________________
Spinor::Spinor()
           :TObject(), fUp(0,0), fDown(0,0)
{
   // Constructor
//   Info("Spinor","Default Constructor");
}

//_____________________________________________________________________________
Spinor::Spinor(const Spinor& other)
           :TObject(other), fUp(other.fUp), fDown(other.fDown)
{
   // Copy Constructor
//   Info("Spinor","Copy Constructor");
}

//_____________________________________________________________________________
Spinor& Spinor::operator=(const Spinor& other)
{
   // Assignment
//   Info("Spinor","Assignment");
   if(this!=&other) {
      TObject::operator=(other);
      fUp = other.fUp;
      fDown = other.fDown;
   }
   return *this;
}

//_____________________________________________________________________________
Spinor::~Spinor()
{
   // Destructor
//   Info("Spinor","Destructor");
}

//_____________________________________________________________________________
void Spinor::PolariseUp(const TVector3& axis)
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
void Spinor::PolariseDown(const TVector3& axis)
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
void Spinor::Print(Option_t* /*option*/) const
{
   cout << "Spin Up: " << fUp.Re() << " + i" << fUp.Im() << endl;
   cout << "Spin Down: " << fDown.Re() << "+ i" << fDown.Im() << endl;
}

//_____________________________________________________________________________
Bool_t Spinor::Precess(const TVector3& avgMagField, const Double_t precessTime)
{
   // -- Take mag field (in the global coordinate system) and precess about it
   #ifdef VERBOSE
      cout << "Precess Spinor:" << endl;
      this->Print();
   #endif
   Double_t omegaX = Neutron::gyromag_ratio*avgMagField.X();
   Double_t omegaY = Neutron::gyromag_ratio*avgMagField.Y();
   Double_t omegaZ = Neutron::gyromag_ratio*avgMagField.Z();
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
   #ifdef VERBOSE
      cout << "Precess Time: " << precessTime << endl;
      cout << "Mag Field - Bx: " << avgMagField.X() << "\t";
      cout << "By: " << avgMagField.Y() << "\t Bz: " << avgMagField.Z() << endl;
      cout << "Omega - X: " << omegaX << "\t";
      cout << "Y: " << omegaY << "\t Z: " << omegaZ << "\t Mag: " << omega << endl;
      this->Print();
      cout << "-----------------------------------------------" << endl;
   #endif
   return kTRUE;
}

//_____________________________________________________________________________
Double_t Spinor::CalculateProbSpinUp(const TVector3& axis) const
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
   #ifdef VERBOSE
      cout.precision(10);
      cout << "Calculate Prob Spin Up: " << endl;
      cout << "denominator: " << denominator << endl;
      cout << conjugateUp.Re() << "\t" << conjugateUp.Im() << endl;
      cout << conjugateDown.Re() << "\t" << conjugateDown.Im() << endl;
      cout << numer1.Re() << "\t" << numer1.Im() << endl;
      cout << numer2.Re() << "\t" << numer2.Im() << endl;
      cout << numer3.Re() << "\t" << numer3.Im() << endl;
      cout << numer4.Re() << "\t" << numer4.Im() << endl;
      cout.precision(10);
      cout << "Probability: " << prob.Re() << "\t" << prob.Im() << endl;
      cout << "-----------------------------------------------" << endl;
   #endif
   return prob.Re();
}
