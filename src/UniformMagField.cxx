// UniformMagField
// Author: Matthew Raso-Barnett  22/05/2009
#include "TMath.h"
#include "UniformMagField.h"

using namespace std;

ClassImp(UniformMagField)

//_____________________________________________________________________________
UniformMagField::UniformMagField()
                    :MagField(), fFieldShape(NULL), fFieldMatrix(NULL)
{
// Default constructor.
   Info("UniformMagField", "Default Constructor");
} 

//_____________________________________________________________________________
UniformMagField::UniformMagField(const string& name, const TVector3& field, const TGeoShape* fieldShape, const TGeoMatrix* fieldMatrix)
                    :MagField(name), fField(field), fFieldShape(fieldShape),
                     fFieldMatrix(fieldMatrix)
{
// Default constructor.
   Info("UniformMagField", "Constructor");
}

//_____________________________________________________________________________
UniformMagField::UniformMagField(const UniformMagField& other)
                    :MagField(other), fField(other.fField), fFieldShape(other.fFieldShape),
                     fFieldMatrix(other.fFieldMatrix)
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
      fFieldShape = other.fFieldShape;
      fFieldMatrix = other.fFieldMatrix;
   }
   return *this;
}

//_____________________________________________________________________________
UniformMagField::~UniformMagField()
{
// Destructor.
   Info("UniformMagField", "Destructor");
   if(fFieldShape) delete fFieldShape;
   if(fFieldMatrix) delete fFieldMatrix;
}   

//_____________________________________________________________________________
Bool_t UniformMagField::Contains(const TVector3& point) const
{
   // -- Check whether supplied point is contained by Field-shape
   // First convert point to local coordinates
   Double_t masterPoint[3] = {point[0], point[1], point[2]};
   Double_t localPoint[3] = {0.,0.,0.};
   fFieldMatrix->MasterToLocal(masterPoint, localPoint);
   // Check and return whether its contained
   return fFieldShape->Contains(localPoint);
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
