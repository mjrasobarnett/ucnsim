// MagFieldDipole
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>

#include "TMath.h"
#include "TGeoMatrix.h"
#include "MagFieldDipole.hpp"
#include "Constants.hpp"

using namespace std;

ClassImp(MagFieldDipole);

//______________________________________________________________________________
MagFieldDipole::MagFieldDipole()
               :MagField(),
                fMoment(),
                fMomentUnitVec(),
                fMomentMag(0.)
{
  // Default constructor.
  Info("MagFieldDipole", "Default Constructor");
} 

//______________________________________________________________________________
MagFieldDipole::MagFieldDipole(const string& name,
                               const TVector3& dipoleMoment,
                               const TGeoShape* shape,
                               const TGeoMatrix* matrix)
               :MagField(name, shape, matrix),
                fMoment(),
                fMomentUnitVec(),
                fMomentMag(0.)
{
  Info("MagFieldDipole", "Constructor");
  // The dipole moment given as an argument should have been defined in the local
  // coordinate frame of the field. Rotate this vector using the supplied matrix
  // so that is it defined in the global frame
  TGeoRotation rotation(*matrix);
  Double_t global[3], local[3] = {dipoleMoment[0],dipoleMoment[1],dipoleMoment[2]};
  rotation.LocalToMaster(local, global);
  // Zero out any rounding errors
  for (int i = 0; i < 3; i++) {global[i] = (TMath::Abs(global[i]) < 1.E-12) ? 0.0 : global[i];}
  // Initialise member variables
  fMoment.SetXYZ(global[0],global[1],global[2]);
  fMomentUnitVec = fMoment.Unit();
  fMomentMag = fMoment.Mag();
  cout << "Defined Dipole Moment - Magnitude: " << fMomentMag << endl;;
  cout << "Direction mx: " << fMomentUnitVec[0] << "\t my: " << fMomentUnitVec[1];
  cout << "\t mz: " << fMomentUnitVec[2] << endl;
}

//______________________________________________________________________________
MagFieldDipole::MagFieldDipole(const MagFieldDipole& other)
               :MagField(other),
                fMoment(other.fMoment),
                fMomentUnitVec(other.fMomentUnitVec),
                fMomentMag(other.fMomentMag)
{
  // Copy constructor
} 

//______________________________________________________________________________
MagFieldDipole &MagFieldDipole::operator=(const MagFieldDipole& other)
{
  // Assignment
  if(this != &other) {
    MagField::operator=(other);
    fMoment = other.fMoment;
    fMomentUnitVec = other.fMomentUnitVec;
    fMomentMag = other.fMomentMag;
  }
  return *this;
}

//______________________________________________________________________________
MagFieldDipole::~MagFieldDipole()
{
  // Destructor
  Info("MagFieldDipole", "Destructor");
}

//______________________________________________________________________________
const TVector3 MagFieldDipole::GetField(const Point& point) const
{
  // Calculate field produce by dipole at provided *global* point in geometry
  // 1. Calculate displacement vector between dipole and measurement point
  TVector3 dipolePositionVector(this->GetMatrix().GetTranslation());
  TVector3 displacement = point.GetPosition() - dipolePositionVector;
  TVector3 displacement_unit = displacement.Unit();
  // 2. Calculate Field produced
  double dotProduct = fMomentUnitVec.Dot(displacement_unit);
  double r_mag = displacement.Mag();
  double r_3 = TMath::Power(r_mag, 3.0);
  double constant = (Constants::mu0_4pi * fMomentMag) / r_3; 
  TVector3 num_lhs = (3.0*dotProduct)*displacement_unit;
  TVector3 vec = num_lhs - fMomentUnitVec;
  TVector3 field = constant*vec;
  return field;
}
