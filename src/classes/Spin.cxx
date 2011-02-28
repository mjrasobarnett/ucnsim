// UCN Spin class
// Author: Matthew Raso-Barnett  19/08/2010
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Algorithms.h"
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
//#define VERBOSE

using namespace std;
using namespace Algorithms;

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
   Double_t probSpinUp = this->CalculateProbSpinUp(axis);
   // Check for errorneous probability
   if (probSpinUp < 0.0 || probSpinUp > 1.0) {
      if (TMath::Abs(probSpinUp - 1.0) < 1.E-10) {
         probSpinUp = 1.0;
      } else if (TMath::Abs(probSpinUp) < 1.E-10) {
         probSpinUp = 0.0;
      } else {
         bool up = probSpinUp > 1.0 ? false : true;
         cout << probSpinUp - 1.0 << endl;
         cout << "Up? " << up << endl;
         bool equal = probSpinUp == 1.0 ? true : false;
         cout << "Equal to 1? " << equal << endl;
         axis.Print();
         throw runtime_error("Spin::IsSpinUp - Found probability outside of 0 -> 1 range.");
      }
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
           :TObject(), fUpRe(0.), fUpIm(0.), fDownRe(0.), fDownIm(0.)
{
   // Constructor
//   Info("Spinor","Default Constructor");
}

//_____________________________________________________________________________
Spinor::Spinor(const Spinor& other)
           :TObject(other), fUpRe(other.fUpRe), fUpIm(other.fUpIm), fDownRe(other.fDownRe),
            fDownIm(other.fDownIm)
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
      fUpRe = other.fUpRe;
      fUpIm = other.fUpIm;
      fDownRe = other.fDownRe;
      fDownIm = other.fDownIm;
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
   const Double_t mag = 1.0/TMath::Sqrt(2.0 + 2.0*TMath::Abs(unit.Z()));
   fUpRe = (unit.Z() + 1.0)*mag;
   fUpIm = 0.;
   fDownRe = unit.X()*mag;
   fDownIm = unit.Y()*mag;
}

//_____________________________________________________________________________
void Spinor::PolariseDown(const TVector3& axis)
{
   // -- Set spinor components to be eigenvector of the operator for spin in the direction
   // -- defined by 'axis'. Expressions derived in notebook.
   TVector3 unit = axis.Unit();
   const Double_t mag = 1.0/TMath::Sqrt(2.0 + 2.0*TMath::Abs(unit.Z()));
   fUpRe = (unit.Z() - 1.0)*mag;
   fUpIm = 0.;
   fDownRe = unit.X()*mag;
   fDownIm = unit.Y()*mag;
}

//_____________________________________________________________________________
void Spinor::Print(Option_t* /*option*/) const
{
   cout << "Spin Up: " << fUpRe << " + i" << fUpIm << endl;
   cout << "Spin Down: " << fDownRe << " + i" << fDownIm << endl;
}

//_____________________________________________________________________________
Bool_t Spinor::Precess(const TVector3& avgMagField, const Double_t precessTime)
{
   // -- Take mag field (in the global coordinate system) and precess about it
   #ifdef VERBOSE
      cout << "Precess Spinor:" << endl;
      this->Print();
   #endif
   const Double_t omegaX = Neutron::gyromag_ratio*avgMagField.X();
   const Double_t omegaY = Neutron::gyromag_ratio*avgMagField.Y();
   const Double_t omegaZ = Neutron::gyromag_ratio*avgMagField.Z();
   // Precession frequency
   const Double_t omega = TMath::Sqrt(omegaX*omegaX + omegaY*omegaY + omegaZ*omegaZ);
   assert(Precision::IsEqual(avgMagField.Mag(), 5.0E-6));
   // If no field exists, no precession is made
   if (Precision::IsEqual(omega, 0.0)) return kFALSE;
   
   const Double_t precessAngle = (omega*precessTime)/2.0;
   const Double_t costheta = TMath::Cos(precessAngle);
   const Double_t sintheta = TMath::Sin(precessAngle);
   const Double_t omX = omegaX/omega;
   const Double_t omY = omegaY/omega;
   const Double_t omZ = omegaZ/omega;
   
   // Spin Up Real Part
   const Double_t newUpRe = fUpRe*costheta + ((fUpIm*omZ + fDownIm*omX) - fDownRe*omY)*sintheta;
   // Spin Up Imaginary Part
   const Double_t newUpIm = fUpIm*costheta - (fDownRe*omX + fUpRe*omZ + fDownIm*omY)*sintheta;
   
   // Spin Down Real Part
   const Double_t newDownRe = fDownRe*costheta + ((fUpIm*omX + fUpRe*omY) - fDownIm*omZ)*sintheta;
   // Spin Down Imaginary Part
   const Double_t newDownIm = fDownIm*costheta + ((fDownRe*omZ + fUpIm*omY) - fUpRe*omX)*sintheta;
   
   // Update spinor
   fUpRe = newUpRe;
   fUpIm = newUpIm;
   fDownRe = newDownRe;
   fDownIm = newDownIm;
   
   #ifdef VERBOSE
      cout << "Precess Time: " << precessTime << endl;
      cout << "Mag Field - Bx: " << avgMagField.X() << "\t";
      cout << "By: " << avgMagField.Y() << "\t Bz: " << avgMagField.Z() << endl;
      cout << "Omega - X: " << omegaX << "\t";
      cout << "Y: " << omegaY << "\t Z: " << omegaZ << "\t Mag: " << omega << endl;
      cout << "Precess Angle: " << precessAngle << endl;
      this->Print();
      cout << "-----------------------------------------------" << endl;
   #endif
   
   #ifdef VERBOSE
      cout << "costheta: " << costheta << endl;
      cout << "sintheta: " << sintheta << endl;
      cout << "omX: " << omX << endl;
      cout << "omY: " << omY << endl;
      cout << "omZ: " << omZ << endl;
      cout << "-----------------------------------------------" << endl;
   #endif
   
   this->CalculateProbSpinUp(TVector3(1.,0.,0.));
   this->CalculateProbSpinUp(TVector3(0.,1.,0.));
   this->CalculateProbSpinUp(TVector3(0.,0.,1.));
   return kTRUE;
}

//_____________________________________________________________________________
Double_t Spinor::CalculateProbSpinUp(const TVector3& axis) const
{
   // -- Calculate probability of being spin 'up' along provided axis
   // -- See notebook for equations
   TVector3 measurementAxis = axis.Unit();
   const Double_t ux = measurementAxis.X();
   const Double_t uy = measurementAxis.Y();
   // Take absolute value of Z component to ensure we don't take uz = -1
   const Double_t uz = TMath::Abs(measurementAxis.Z());
   const Double_t denominator = 2.0 + 2.0*uz;
   
   // Calculate the numerator components -- all equations are in notebook
   Double_t numer1 = (uz + 1.0)*(uz + 1.0)*(fUpRe*fUpRe + fUpIm*fUpIm);
   Double_t numer2 = (ux*ux + uy*uy)*(fDownRe*fDownRe + fDownIm*fDownIm);
   Double_t numer3 = 2.0*(uz + 1.0)*ux*(fUpRe*fDownRe + fUpIm*fDownIm);
   Double_t numer4 = 2.0*(uz + 1.0)*uy*(fDownIm*fUpRe - fDownRe*fUpIm);
   
   // Calculate probability
   Double_t prob = (numer1 + numer2 + numer3 + numer4)/denominator;
   #ifdef VERBOSE
      cout << "Calculate Prob Spin Up: " << endl;
      this->Print();
      cout << "denominator: " << denominator << endl;
      cout << "Numer1: " << numer1 << "\t";
      cout << "Numer2: " << numer2 << "\t";
      cout << "Numer3: " << numer3 << "\t";
      cout << "Numer4: " << numer4 << "\t" << endl;
      cout << "Probability: " << prob << endl;
      cout << "-----------------------------------------------" << endl;
   #endif
   assert(Precision::IsGreaterOrEqual(prob,0.0) && Precision::IsLessOrEqual(prob,1.0));
   return prob;
}
