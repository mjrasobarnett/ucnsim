// TUCNMagField
// Author: Matthew Raso-Barnett  30/09/2009

#include "TUCNMagField.h"

//______________________________________________________________________________
// TUCNMagField - ABC for magnetic field. Derived classes are encouraged to
// use the TVirtualMagField named constructor and must implement the method:
//    Field(const Double_t *x, Double_t *B)
//
//______________________________________________________________________________

ClassImp(TUCNMagField)

//_____________________________________________________________________________
TUCNMagField::TUCNMagField()
				 :TNamed()
{
// Default constructor.
	Info("TUCNMagField", "Dummy Constructor");
} 

//_____________________________________________________________________________
TUCNMagField::TUCNMagField(const char *name)
				 :TNamed(name, "")
{
// Default constructor.
	Info("TUCNMagField", "Constructor");
}

//______________________________________________________________________________
TUCNMagField::~TUCNMagField()
{
// Destructor.
	Info("TUCNMagField", "Destructor");
}
