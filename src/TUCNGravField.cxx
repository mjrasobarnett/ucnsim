// TUCNGravField
// Author: Matthew Raso-Barnett  22/05/2009

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 	Field is initialised with a field direction pointing downwards 	  //
// 	along the z-axis with respect to the global cooradinate system. 	  //
//                                      											  //
////////////////////////////////////////////////////////////////////////////

#include "TVector3.h"

#include "TUCNGravField.h"

ClassImp(TUCNGravField)

// -- Static Data Members Initialised
const Double_t 	TUCNGravField::fGravAcceleration; 

//_____________________________________________________________________________
TUCNGravField::TUCNGravField()
				  :TNamed("GravField","Universal Grav Field Vector"),
					fGlobalFieldVector(0.,0.,-1.)
{
// -- Default constructor.
	Info("TUCNGravField", "Constructor");
} 

//_____________________________________________________________________________
TUCNGravField::TUCNGravField(const TUCNGravField& other)
              :TNamed(other),
					fGlobalFieldVector(other.fGlobalFieldVector)
{
// -- Copy constructor.
   Info("TUCNGravField","Copy Constructor");
} 

//_____________________________________________________________________________
TUCNGravField& TUCNGravField::operator=(const TUCNGravField& other)
{
// -- Assignment operator
	if(this!=&other) {
		TNamed::operator=(other);
      fGlobalFieldVector = other.fGlobalFieldVector;
	}
   return *this;
}

//_____________________________________________________________________________
TUCNGravField::~TUCNGravField()
{
// -- Destructor.
	Info("TUCNGravField", "Destructor");
}   

//_____________________________________________________________________________
void TUCNGravField::Direction(Double_t nx, Double_t ny, Double_t nz)
{
// Define the Field Direction Vector, ensure its normalised, and return a pointer to it.	
	fGlobalFieldVector.SetXYZ(nx, ny, nz);
	fGlobalFieldVector = fGlobalFieldVector.Unit();
}
