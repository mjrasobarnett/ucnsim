// MagFieldLoop
#include <iostream>

#include "MagFieldLoop.hpp"

#include "Tube.hpp"
#include "Point.hpp"
#include "TMath.h"
#include "TGeoMatrix.h"
#include "Constants.hpp"
#include "Algorithms.hpp"
#include "DataAnalysis.hpp"

#include "gsl/gsl_sf_ellint.h"

using namespace std;

ClassImp(MagFieldLoop);

//______________________________________________________________________________
MagFieldLoop::MagFieldLoop()
             :MagField(),
              fCurrent(0.),
              fTurns(0.),
              fRadius(0.)
{
  // Default constructor.
  Info("MagFieldLoop", "Default Constructor");
} 

//______________________________________________________________________________
MagFieldLoop::MagFieldLoop(const string& name,
                           const double current,
                           const int turns,
                           const Tube* tube,
                           const TGeoMatrix* matrix)
             :MagField(name, tube, matrix),
              fCurrent(current),
              fTurns(turns),
              fRadius(tube->GetRmin())
{
  // constructor
  Info("MagFieldLoop", "Constructor");  
}

//______________________________________________________________________________
MagFieldLoop::MagFieldLoop(const MagFieldLoop& other)
             :MagField(other),
              fCurrent(other.fCurrent),
              fTurns(other.fTurns),
              fRadius(other.fRadius)
{
  // Copy constructor
} 

//______________________________________________________________________________
MagFieldLoop &MagFieldLoop::operator=(const MagFieldLoop& other)
{
  // Assignment
  if(this != &other) {
    MagField::operator=(other);
    fCurrent = other.fCurrent;
    fTurns = other.fTurns;
    fRadius = other.fRadius;  
  }
  return *this;
}

//______________________________________________________________________________
MagFieldLoop::~MagFieldLoop()
{
  // Destructor
  Info("MagFieldLoop", "Destructor");
}

//______________________________________________________________________________
const TVector3 MagFieldLoop::GetField(const Point& point) const
{
  // -- Calculate magnetic field from Loop
  // Convert point to local coordinate frame of Loop
  TVector3 localField;
  Point localPoint;
  Analysis::Geometry::GlobalToLocalPoint(point, localPoint, this->GetMatrix());
  double r = TMath::Sqrt(localPoint.X()*localPoint.X() + localPoint.Y()*localPoint.Y());
  // Work out whether to do an on-axis or off-axis calculation
  if (Algorithms::Precision::IsEqual(r, 0.0)) {
    localField = OnAxisFieldCalc(localPoint);
  } else {
    localField = OffAxisFieldCalc(localPoint, r);
  }
  // Rotate field back into the Global Frame
  TGeoRotation rotation(this->GetMatrix());
  Double_t global[3], local[3] = {localField.X(), localField.Y(), localField.Z()};
  rotation.LocalToMaster(local, global);
  // Return field
  TVector3 globalField(global[0], global[1], global[2]);
  return globalField;
}

//______________________________________________________________________________
const TVector3 MagFieldLoop::OffAxisFieldCalc(const Point& localPoint, const double r) const
{
  // -- Calculate off-axis field from current loop
  double theta = TMath::ATan2(localPoint.Y(),localPoint.X());
  double alpha = r / fRadius;
  double alpha_2 = alpha*alpha;
  double beta = localPoint.Z() / fRadius;
  double beta_2 = beta*beta;
  double gamma = localPoint.Z() / r;
  double q = TMath::Power((1.0 + alpha), 2.0) + beta_2;
  double k = TMath::Sqrt((4.0*alpha)/q);
  // Calculate Elliptical Integrals using GSL
  double K_k = gsl_sf_ellint_Kcomp(k, GSL_PREC_APPROX);
  double E_k = gsl_sf_ellint_Ecomp(k, GSL_PREC_APPROX);

  double B_0 = (fCurrent*fTurns*Constants::mu0)/(2.0*fRadius);
  double const_factor = B_0/(TMath::Pi()*TMath::Sqrt(q));
  double numer_1 = 1.0 - alpha_2 - beta_2;
  double numer_2 = 1.0 + alpha_2 + beta_2;
  double denom = q - (4.0*alpha);   
  // Bz
  double bz = const_factor*(E_k*(numer_1/denom) + K_k);
  // Br
  double br = const_factor*gamma*(E_k*(numer_2/denom) - K_k);
  double bx = br*TMath::Cos(theta);
  double by = br*TMath::Sin(theta);
  TVector3 field(bx, by, bz);
  return field;
}

//______________________________________________________________________________
const TVector3 MagFieldLoop::OnAxisFieldCalc(const Point& localPoint) const
{
  // -- Calculate on-axis field from current loop 
  double rad_2 = fRadius*fRadius;
  double z_2 = localPoint.Z()*localPoint.Z();
  double numer = Constants::mu0*fCurrent*fTurns*rad_2;
  double denom = 2.0*TMath::Power(rad_2 + z_2, (3.0/2.0));
  double bz = numer / denom;
  TVector3 field(0.,0.,bz);
  return field;
}

