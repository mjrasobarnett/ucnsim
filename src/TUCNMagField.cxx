// TUCNMagField
// Author: Matthew Raso-Barnett  30/09/2009

#include "TUCNMagField.h"

//______________________________________________________________________________
// TUCNMagField - ABC for magnetic field.
//
//______________________________________________________________________________

ClassImp(TUCNMagField)

//_____________________________________________________________________________
TUCNMagField::TUCNMagField()
             :TNamed()
{
// Default constructor.
   Info("TUCNMagField", "Default Constructor");
}

//_____________________________________________________________________________
TUCNMagField::TUCNMagField(const std::string& name)
             :TNamed(name, "")
{
// Default constructor.
   Info("TUCNMagField", "Constructor");
}

//_____________________________________________________________________________
TUCNMagField::TUCNMagField(const TUCNMagField& other)
             :TNamed(other)
{
// Copy constructor.
} 

//_____________________________________________________________________________
TUCNMagField &TUCNMagField::operator=(const TUCNMagField& other)
{
// Assignment.
   if(this != &other) {
      TNamed::operator=(other);
   }
   return *this;
}

//______________________________________________________________________________
TUCNMagField::~TUCNMagField()
{
// Destructor.
   Info("TUCNMagField", "Destructor");
}

