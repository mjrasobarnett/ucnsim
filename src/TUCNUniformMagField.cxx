// TUCNUniformMagField
// Author: Matthew Raso-Barnett  22/05/2009
#include "TMath.h"
#include "TUCNUniformMagField.h"

using namespace std;

ClassImp(TUCNUniformMagField)

//_____________________________________________________________________________
TUCNUniformMagField::TUCNUniformMagField()
                    :TUCNMagField()
{
// Default constructor.
   Info("TUCNUniformMagField", "Default Constructor");
} 

//_____________________________________________________________________________
TUCNUniformMagField::TUCNUniformMagField(const string& name, const Double_t& Bx, const Double_t& By, const Double_t& Bz)
                    :TUCNMagField(name)
{
// Default constructor.
   Info("TUCNUniformMagField", "Constructor");
   fBx = Bx;
   fBy = By;
   fBz = Bz;
   fBMag = TMath::Sqrt(fBx*fBx + fBy*fBy + fBz*fBz);
} 

//_____________________________________________________________________________
TUCNUniformMagField::TUCNUniformMagField(const TUCNUniformMagField& other)
                    :TUCNMagField(other), fBx(other.fBx), fBy(other.fBy), fBz(other.fBz),
                     fBMag(other.fBMag)
{
// Copy constructor.
} 

//_____________________________________________________________________________
TUCNUniformMagField &TUCNUniformMagField::operator=(const TUCNUniformMagField& other)
{
// Assignment.
   if(this != &other) {
      TUCNMagField::operator=(other);
      fBx = other.fBx;
      fBy = other.fBy;
      fBz = other.fBz;
      fBMag = other.fBMag;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNUniformMagField::~TUCNUniformMagField()
{
// Destructor.
   Info("TUCNUniformMagField", "Destructor");
}   

//_____________________________________________________________________________
void TUCNUniformMagField::GetFieldVector(const TVector3& /*pos*/, TVector3& field) const
{
   // Copy field vector to provided vector
   field.SetX(fBx);
   field.SetY(fBy);
   field.SetZ(fBz);
}
