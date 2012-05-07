// ElecFieldArray.cxx

#include <iostream>
#include <cassert>
#include <stdexcept>

#include "ElecFieldArray.hpp"
#include "Constants.hpp"

//#define VERBOSE_MODE

using namespace std;

ClassImp(ElecFieldArray);

//_____________________________________________________________________________
ElecFieldArray::ElecFieldArray()
               :FieldArray("ElecFieldArray")
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
   // -- the electric field(s) ( B = E x v / c^2 )
   TVector3 efield = GetField(point,volume);
//   printf("EField - Ex:%f \t Ey:%f \t Ez: %f \n", efield.X(), efield.Y(), efield.Z());
   // Calculate relativistic velocity vector
//   printf("Velocity - vx:%f \t vy:%f \t vz: %f \n", velocity.X(), velocity.Y(), velocity.Z());
   // Calculate motional BField
   Double_t bx = (velocity.Z()*efield.Y() - velocity.Y()*efield.Z())/(TMath::Power(Constants::c_light,2.0));
   Double_t by = (velocity.X()*efield.Z() - velocity.Z()*efield.X())/(TMath::Power(Constants::c_light,2.0));
   Double_t bz = (velocity.Y()*efield.X() - velocity.X()*efield.Y())/(TMath::Power(Constants::c_light,2.0));
   TVector3 bfield(bx, by, bz);
//   printf("BField - Bx:%0.14f \t By:%0.14f \t Bz: %0.14f \n", bfield.X(), bfield.Y(), bfield.Z());
   return bfield;
}
