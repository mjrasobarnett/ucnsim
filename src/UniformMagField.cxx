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

//_____________________________________________________________________________
const TVector3& UniformMagField::GetFieldVector(const TVector3& /*pos*/) const
{
   // No position dependence for a Uniform field so return field vector
   return fField;
}

//______________________________________________________________________________
Bool_t UniformMagField::Interact(Particle& particle, const Double_t stepTime) const
{
   // -- Precess spin vector of particle over period of time defined by stepTime
   particle.PrecessSpin(/*this->GetFieldVector()*/ fField, stepTime);
   return kTRUE;
}
