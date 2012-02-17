// ElecField
// Author: Matthew Raso-Barnett  30/09/2009

#include "ElecField.hpp"

//______________________________________________________________________________
// ElecField - ABC for magnetic field.
//
//______________________________________________________________________________

ClassImp(ElecField)

//_____________________________________________________________________________
ElecField::ElecField()
          :Field()
{
// Default constructor.
   Info("ElecField", "Default Constructor");
}

//_____________________________________________________________________________
ElecField::ElecField(const std::string& name, const TGeoShape* shape, const TGeoMatrix* matrix)
          :Field(name, shape, matrix)
{
// Default constructor.
   Info("ElecField", "Constructor");
}

//_____________________________________________________________________________
ElecField::ElecField(const ElecField& other)
          :Field(other)
{
// Copy constructor.
} 

//______________________________________________________________________________
ElecField::~ElecField()
{
// Destructor.
   Info("ElecField", "Destructor");
}

//______________________________________________________________________________
const TVector3 ElecField::GetMagField(const Point& /*point*/, const TVector3& velocity) const
{
   return velocity;
}
