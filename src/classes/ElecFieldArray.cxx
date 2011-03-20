// ElecFieldArray.cxx

#include <iostream>
#include <cassert>
#include <stdexcept>

#include "ElecFieldArray.h"
#include "Constants.h"

//#define VERBOSE_MODE

using namespace std;

ClassImp(ElecFieldArray);

//_____________________________________________________________________________
ElecFieldArray::ElecFieldArray()
               :FieldArray()
{
   // Default constructor
   Info("ElecFieldArray", "Default Constructor");
}

//_____________________________________________________________________________
ElecFieldArray::ElecFieldArray(const ElecFieldArray& other)
               :FieldArray(other)
{
   //copy constructor
   Info("ElecFieldArray", "Copy Constructor");
}

//_____________________________________________________________________________
ElecFieldArray::~ElecFieldArray()
{
   // Destructor
   Info("ElecFieldArray", "Destructor");
}

//_____________________________________________________________________________
const TVector3 ElecFieldArray::GetMagField(const Point& point, const TVector3& velocity, const string volume) const
{
   // -- Calculate the magnetic field seen by the particle in a frame moving w.r.t
   // -- the electric field(s)
   TVector3 efield = GetField(point,volume);
   // Calculate relativistic velocity vector
   TVector3 beta = velocity*(1.0/Constants::c_light);
   // Calculate relativistic gamma factor, gamma/c
   Double_t gamma_over_c = (Constants::c_light/TMath::Sqrt(1 - TMath::Power(beta.Mag(),2.0)));
   // Calculate motional BField
   Double_t bx = gamma_over_c*(beta.Z()*efield.Y() - beta.Y()*efield.Z());
   Double_t by = gamma_over_c*(beta.X()*efield.Z() - beta.Z()*efield.X());
   Double_t bz = gamma_over_c*(beta.Y()*efield.X() - beta.X()*efield.Y());
   TVector3 bfield(bx, by, bz);
   return bfield;
}
