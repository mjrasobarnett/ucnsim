// MagField
// Author: Matthew Raso-Barnett  30/09/2009

#include "MagField.hpp"

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
  #ifdef PRINT_CONSTRUCTORS
    Info("MagField", "Default Constructor");
  #endif
}

//_____________________________________________________________________________
MagField::MagField(const std::string& name, const TGeoShape* shape, const TGeoMatrix* matrix)
         :Field(name, shape, matrix)
{
// Default constructor.
  #ifdef PRINT_CONSTRUCTORS
    Info("MagField", "Constructor");
  #endif
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
  #ifdef PRINT_CONSTRUCTORS
    Info("MagField", "Destructor");
  #endif
}

