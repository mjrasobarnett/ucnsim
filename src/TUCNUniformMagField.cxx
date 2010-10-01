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
void TUCNUniformMagField::GetFieldVector(const TVector3& /*pos*/, TVector3& /*field*/) const
{
   // Copy field vector to provided vector
}

//______________________________________________________________________________
Bool_t TUCNUniformMagField::Interact(TUCNParticle& /*particle*/, const Double_t /*stepTime*/) const
{
   // -- Precess spin vector of particle over period of time defined by stepTime
   
   return kTRUE;
}
