// TUCNUniformMagField
// Author: Matthew Raso-Barnett  22/05/2009
#include "TMath.h"
#include "TUCNUniformMagField.h"

using namespace std;

ClassImp(TUCNUniformMagField)

//_____________________________________________________________________________
TUCNUniformMagField::TUCNUniformMagField()
                    :TUCNMagField(), fFieldShape(NULL), fFieldMatrix(NULL)
{
// Default constructor.
   Info("TUCNUniformMagField", "Default Constructor");
} 

//_____________________________________________________________________________
TUCNUniformMagField::TUCNUniformMagField(const string& name, const TVector3& field, const TGeoShape* fieldShape, const TGeoMatrix* fieldMatrix)
                    :TUCNMagField(name), fField(field), fFieldShape(fieldShape),
                     fFieldMatrix(fieldMatrix)
{
// Default constructor.
   Info("TUCNUniformMagField", "Constructor");
}

//_____________________________________________________________________________
TUCNUniformMagField::TUCNUniformMagField(const TUCNUniformMagField& other)
                    :TUCNMagField(other), fField(other.fField), fFieldShape(other.fFieldShape),
                     fFieldMatrix(other.fFieldMatrix)
{
// Copy constructor.
} 

//_____________________________________________________________________________
TUCNUniformMagField &TUCNUniformMagField::operator=(const TUCNUniformMagField& other)
{
// Assignment.
   if(this != &other) {
      TUCNMagField::operator=(other);
      fField = other.fField;
      fFieldShape = other.fFieldShape;
      fFieldMatrix = other.fFieldMatrix;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNUniformMagField::~TUCNUniformMagField()
{
// Destructor.
   Info("TUCNUniformMagField", "Destructor");
   if(fFieldShape) delete fFieldShape;
   if(fFieldMatrix) delete fFieldMatrix;
}   

//_____________________________________________________________________________
Bool_t TUCNUniformMagField::Contains(const TVector3& point) const
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
const TVector3& TUCNUniformMagField::GetFieldVector(const TVector3& /*pos*/) const
{
   // No position dependence for a Uniform field so return field vector
   return fField;
}

//______________________________________________________________________________
Bool_t TUCNUniformMagField::Interact(TUCNParticle& particle, const Double_t stepTime) const
{
   // -- Precess spin vector of particle over period of time defined by stepTime
   particle.PrecessSpin(/*this->GetFieldVector()*/ fField, stepTime);
   return kTRUE;
}
