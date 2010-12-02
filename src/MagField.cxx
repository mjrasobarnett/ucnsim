// MagField
// Author: Matthew Raso-Barnett  30/09/2009

#include "MagField.h"

//______________________________________________________________________________
// MagField - ABC for magnetic field.
//
//______________________________________________________________________________

ClassImp(MagField)

//_____________________________________________________________________________
MagField::MagField()
             :TNamed()
{
// Default constructor.
   Info("MagField", "Default Constructor");
}

//_____________________________________________________________________________
MagField::MagField(const std::string& name)
             :TNamed(name, "")
{
// Default constructor.
   Info("MagField", "Constructor");
}

//_____________________________________________________________________________
MagField::MagField(const MagField& other)
             :TNamed(other)
{
// Copy constructor.
} 

//_____________________________________________________________________________
MagField &MagField::operator=(const MagField& other)
{
// Assignment.
   if(this != &other) {
      TNamed::operator=(other);
   }
   return *this;
}

//______________________________________________________________________________
MagField::~MagField()
{
// Destructor.
   Info("MagField", "Destructor");
}

