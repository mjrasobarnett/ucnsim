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
         :Field()
{
// Default constructor.
   Info("MagField", "Default Constructor");
}

//_____________________________________________________________________________
MagField::MagField(const std::string& name, const TGeoShape* shape, const TGeoMatrix* matrix)
         :Field(name, shape, matrix)
{
// Default constructor.
   Info("MagField", "Constructor");
}

//_____________________________________________________________________________
MagField::MagField(const MagField& other)
         :Field(other)
{
// Copy constructor.
} 

//______________________________________________________________________________
MagField::~MagField()
{
// Destructor.
   Info("MagField", "Destructor");
}

