// UniformMagField
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>

#include "TMath.h"
#include "TGeoMatrix.h"
#include "UniformMagField.hpp"

using namespace std;

ClassImp(UniformMagField)

//_____________________________________________________________________________
UniformMagField::UniformMagField()
                :MagField(),
                 fField()
{
// Default constructor.
  #ifdef PRINT_CONSTRUCTORS
    Info("UniformMagField", "Default Constructor");
  #endif
} 

//_____________________________________________________________________________
UniformMagField::UniformMagField(const string& name, const TVector3& field, const TGeoShape* shape, const TGeoMatrix* matrix)
                :MagField(name, shape, matrix),
                 fField()
{
// Default constructor.
  #ifdef PRINT_CONSTRUCTORS
    Info("UniformMagField", "Constructor");
  #endif
  // Convert field vector supplied to global coordinates
  Double_t global[3], local[3] = {field[0],field[1],field[2]};
  matrix->LocalToMaster(local, global);
  // Zero out any rounding errors
  for (int i = 0; i < 3; i++) {global[i] = (TMath::Abs(global[i]) < 1.E-12) ? 0.0 : global[i];}
  fField.SetXYZ(global[0],global[1],global[2]);
  printf("Uniform Magnetic Field - Bx: %.4f \t By: %.4f \t Bz: %.4f\n", fField[0], fField[1], fField[2]);
}

//_____________________________________________________________________________
UniformMagField::UniformMagField(const UniformMagField& other)
                :MagField(other),
                 fField(other.fField)
{
// Copy constructor.
} 

//_____________________________________________________________________________
UniformMagField &UniformMagField::operator=(const UniformMagField& other)
{
// Assignment.
   if(this != &other) {
      MagField::operator=(other);
      fField = other.fField;
   }
   return *this;
}

//_____________________________________________________________________________
UniformMagField::~UniformMagField()
{
// Destructor.
  #ifdef PRINT_CONSTRUCTORS
    Info("UniformMagField", "Destructor");
  #endif
}

//______________________________________________________________________________
const TVector3 UniformMagField::GetField(const Point& /*point*/) const
{
   // No position dependence for a Uniform field so return field vector
   return fField;
}
