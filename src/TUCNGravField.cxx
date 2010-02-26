// TUCNGravField
// Author: Matthew Raso-Barnett  22/05/2009

#include "TVector3.h"

#include "TUCNGravField.h"

ClassImp(TUCNGravField)

// -- Static Data Members Initialised
TUCNGravField* 	TUCNGravField::fgInstance = NULL;
const Double_t 	TUCNGravField::fGravAcceleration; // Initialised in header, definition in implimentation must not specify an initial value (static const)

//_____________________________________________________________________________
TUCNGravField::TUCNGravField()
				  :fGlobalFieldVector()
{
// Default constructor.
	Info("TUCNGravField", "Constructor");
   fgInstance = this;
} 

//_____________________________________________________________________________
TUCNGravField::TUCNGravField(const TUCNGravField& other)
              :TObject(other)
{
// Copy constructor.
   Error("copy constructor","copying not allowed for TUCNGravField");
} 

//_____________________________________________________________________________
TUCNGravField::~TUCNGravField()
{
// Destructor.
	Info("TUCNGravField", "Destructor");
	fgInstance = NULL;
}   

//_____________________________________________________________________________
TUCNGravField &TUCNGravField::operator=(const TUCNGravField&)
{
// Assignment.
   Error("Assignment","assignment not allowed for TUCNGravField");
   return *this;
}

//_____________________________________________________________________________
TUCNGravField* TUCNGravField::Instance(TUCNGeoManager *geom) 
{
// Return pointer to singleton.   
   if (!fgInstance) fgInstance = new TUCNGravField();
   fgInstance->SetUCNGeometry(geom);
	return fgInstance;
}   

//_____________________________________________________________________________
void TUCNGravField::Direction(Double_t nx, Double_t ny, Double_t nz)
{
// Define the Field Direction Vector, ensure its normalised, and return a pointer to it.	
	fGlobalFieldVector.SetXYZ(nx, ny, nz);
	fGlobalFieldVector = fGlobalFieldVector.Unit();
}
