// TUCNUniformMagField
// Author: Matthew Raso-Barnett  22/05/2009

#include "TUCNUniformMagField.h"

ClassImp(TUCNUniformMagField)

//_____________________________________________________________________________
TUCNUniformMagField::TUCNUniformMagField()
						  :TUCNMagField()
{
// Default constructor.
	Info("TUCNUniformMagField", "Dummy Constructor");
} 

//_____________________________________________________________________________
TUCNUniformMagField::TUCNUniformMagField(Double_t Bx, Double_t By, Double_t Bz)
						  :TUCNMagField("Uniform magnetic field")
{
// Default constructor.
	Info("TUCNUniformMagField", "Constructor");
	fBField[0] = Bx;
	fBField[1] = By;
	fBField[2] = Bz;
} 

//_____________________________________________________________________________
TUCNUniformMagField::TUCNUniformMagField(const TUCNUniformMagField& other)
						  :TUCNMagField(other)
{
// Copy constructor.
} 

//_____________________________________________________________________________
TUCNUniformMagField &TUCNUniformMagField::operator=(const TUCNUniformMagField& mf)
{
// Assignment.
   if(this != &mf) {
		TUCNMagField::operator=(mf);
		for (Int_t i=0; i < 3; i++)
			fBField[i] = mf.fBField[i];
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
void TUCNUniformMagField::Field(const Double_t *pos, Double_t *field)
{
	field[0] = fBField[0];
	field[1] = fBField[1];
	field[2] = fBField[2];
}   

