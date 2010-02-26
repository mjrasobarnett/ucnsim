// TUCNParabolicMagField
// Author: Matthew Raso-Barnett  22/05/2009

#include "TMath.h"

#include "TUCNParabolicMagField.h"

ClassImp(TUCNParabolicMagField)

//_____________________________________________________________________________
TUCNParabolicMagField::TUCNParabolicMagField()
							 :TUCNMagField()
{
// Default constructor.
	Info("TUCNParabolicMagField", "Dummy Constructor");
} 

//_____________________________________________________________________________
TUCNParabolicMagField::TUCNParabolicMagField(const char *name, Double_t maxB, Double_t parabolicGradient, Double_t fieldRadius)
							 :TUCNMagField(name)
{
// Default constructor.
	Info("TUCNParabolicMagField", "Constructor");
	fBMax = maxB;
	fParabolicGradient = parabolicGradient;
	fFieldRadius = fieldRadius;
} 

//_____________________________________________________________________________
TUCNParabolicMagField::TUCNParabolicMagField(const TUCNParabolicMagField& mf)
							 :TUCNMagField(mf),
							  fBMax(mf.fBMax),
							  fParabolicGradient(mf.fParabolicGradient),
							  fFieldRadius(mf.fFieldRadius)
{
// Copy constructor.
} 

//_____________________________________________________________________________
TUCNParabolicMagField &TUCNParabolicMagField::operator=(const TUCNParabolicMagField& mf)
{
// Assignment.
   if(this != &mf) {
		TUCNMagField::operator=(mf);
		fBMax = mf.fBMax;
		fParabolicGradient = mf.fParabolicGradient;
		fFieldRadius = mf.fFieldRadius;
	}
	return *this;
}

//_____________________________________________________________________________
TUCNParabolicMagField::~TUCNParabolicMagField()
{
// Destructor.
	Info("TUCNParabolicMagField", "Destructor");
}   

//_____________________________________________________________________________
void TUCNParabolicMagField::Field(const Double_t* pos, Double_t* field)
{
	// B(r,z) = Bmax - A(r^2)/(R^2) with A=(0.1)Bmax
	
	// For now we assume we are in the local coordinates of the cylinder
	Double_t r = TMath::Sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
	Double_t theta = TMath::ACos(pos[0]/r);
	field[0] = this->FieldStrength(pos)*r*TMath::Cos(theta);
	field[1] = this->FieldStrength(pos)*r*TMath::Sin(theta); 
}   

//_____________________________________________________________________________
Double_t TUCNParabolicMagField::FieldStrength(const Double_t* pos) const
{
	// B(r,z) = Bmax - A(r^2)/(R^2) with A=(0.1)Bmax
	
	// For now we assume we are in the local coordinates of the cylinder
	Double_t r = TMath::Sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
	Double_t B = fBMax - (fParabolicGradient*r*r)/(fFieldRadius*fFieldRadius);
	return B;
}
