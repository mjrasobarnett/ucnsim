// UniformMagField
// Author: Matthew Raso-Barnett  22/05/2009
#include "TMath.h"
#include "UniformMagField.h"

using namespace std;

ClassImp(UniformMagField)

//_____________________________________________________________________________
UniformMagField::UniformMagField()
                :MagField()
{
// Default constructor.
   Info("UniformMagField", "Default Constructor");
} 

//_____________________________________________________________________________
UniformMagField::UniformMagField(const string& name, const TVector3& field, const TGeoShape* shape, const TGeoMatrix* matrix)
                :MagField(name, shape, matrix), 
                 fField(field)
{
// Default constructor.
   Info("UniformMagField", "Constructor");
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
