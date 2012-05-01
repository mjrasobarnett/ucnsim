// GravField
// Author: Matthew Raso-Barnett  22/05/2009

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 	Field is initialised with a field direction pointing downwards 	  //
// 	along the z-axis with respect to the global cooradinate system. 	  //
//                                      											  //
////////////////////////////////////////////////////////////////////////////

#include "TVector3.h"

#include "GravField.hpp"

ClassImp(GravField)

// -- Static Data Members Initialised
const double GravField::fGravAcceleration = 9.80665; 

//_____________________________________________________________________________
GravField::GravField()
				  :TNamed("GravField","Universal Grav Field Vector"),
					 fGlobalFieldVector(0.,0.,-1.)
{
// -- Default constructor.
	#ifdef PRINT_CONSTRUCTORS
    Info("GravField", "Constructor");
  #endif
} 

//_____________________________________________________________________________
GravField::GravField(double nx, double ny, double nz)
          :TNamed("GravField","Universal Grav Field Vector"),
           fGlobalFieldVector(0.,0.,0.)
{
  // constructor
  #ifdef PRINT_CONSTRUCTORS
    Info("GravField", "Constructor");
  #endif
  this->Direction(nx, ny, nz);
}

//_____________________________________________________________________________
GravField::GravField(const GravField& other)
              :TNamed(other),
					fGlobalFieldVector(other.fGlobalFieldVector)
{
// -- Copy constructor.
  #ifdef PRINT_CONSTRUCTORS
    Info("GravField","Copy Constructor");
  #endif
} 

//_____________________________________________________________________________
GravField& GravField::operator=(const GravField& other)
{
// -- Assignment operator
	if(this!=&other) {
		TNamed::operator=(other);
      fGlobalFieldVector = other.fGlobalFieldVector;
	}
   return *this;
}

//_____________________________________________________________________________
GravField::~GravField()
{
// -- Destructor.
	#ifdef PRINT_CONSTRUCTORS
    Info("GravField", "Destructor");
  #endif
}   

//_____________________________________________________________________________
void GravField::Direction(Double_t nx, Double_t ny, Double_t nz)
{
// Define the Field Direction Vector, ensure its normalised, and return a pointer to it.	
	fGlobalFieldVector.SetXYZ(nx, ny, nz);
	fGlobalFieldVector = fGlobalFieldVector.Unit();
}
