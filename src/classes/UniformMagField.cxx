// UniformMagField
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>

#include "TMath.h"
#include "TGeoMatrix.h"
#include "UniformMagField.h"

using namespace std;

ClassImp(UniformMagField)

//_____________________________________________________________________________
UniformMagField::UniformMagField()
                :MagField(),
                 fField()
{
// Default constructor.
   Info("UniformMagField", "Default Constructor");
} 

//_____________________________________________________________________________
UniformMagField::UniformMagField(const string& name, const TVector3& field, const TGeoShape* shape, const TGeoMatrix* matrix)
                :MagField(name, shape, matrix),
                 fField()
{
// Default constructor.
   Info("UniformMagField", "Constructor");
   // Convert field vector supplied to global coordinates
   Double_t global[3], local[3] = {field[0],field[1],field[2]};
   matrix->LocalToMaster(local, global);
   // Zero out any rounding errors
   for (int i = 0; i < 3; i++) {global[i] = (TMath::Abs(global[i]) < 1.E-12) ? 0.0 : global[i];}
   fField.SetXYZ(global[0],global[1],global[2]);
   cout << "Built Uniform Magnetic Field - ";
   cout << "Bx: " << fField[0] << "\t By: " << fField[1];
   cout << "\t Bz: " << fField[2] << endl;
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
   Info("UniformMagField", "Destructor");
}

//______________________________________________________________________________
const TVector3 UniformMagField::GetField(const TVector3& /*position*/) const
{
   // No position dependence for a Uniform field so return field vector
   return fField;
}
