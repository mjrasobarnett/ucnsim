// TUCNFieldManager
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>

#include "TObjArray.h"
#include "TUCNGravField.h"
#include "TUCNMagField.h"
#include "TUCNUniformMagField.h"
#include "TUCNParabolicMagField.h"
#include "TUCNFieldManager.h"

#include "Units.h"
#include "Constants.h"

using std::cout;
using std::endl;
using std::cerr;

//#define VERBOSE_MODE

ClassImp(TUCNFieldManager)

//_____________________________________________________________________________
TUCNFieldManager::TUCNFieldManager()
					  :TNamed("FieldManager", "Default Field Manager")
{
// -- Default constructor
   Info("TUCNFieldManager", "Constructor");
	fMagField = 0;
	fGravField = 0;
	
} 

//_____________________________________________________________________________
TUCNFieldManager::TUCNFieldManager(const TUCNFieldManager& other)
					  :TNamed(other),
						fMagField(other.fMagField),
						fGravField(other.fGravField)
{
// -- Copy Constructor
	Info("TUCNFieldManager", "Copy Constructor");
	
}

//_____________________________________________________________________________
TUCNFieldManager& TUCNFieldManager::operator=(const TUCNFieldManager& other)
{
// --assignment operator
	if(this!=&other) {
      TNamed::operator=(other);
		fMagField = other.fMagField;
		fGravField = other.fGravField;
	}
   return *this;
}

//______________________________________________________________________________
TUCNFieldManager::~TUCNFieldManager()
{ 
// -- Destructor
	Info("TUCNFieldManager", "Destructor");
	if (fMagField)  delete fMagField;
	if (fGravField) delete fGravField;
}

// -- METHODS --

//______________________________________________________________________________
TUCNGravField* TUCNFieldManager::AddGravField()
{
// Create and store reference to grav field object	
	if (fGravField) {
		cerr << "Field: " << fGravField->GetName() << " already exists." << endl;
	} else {
		fGravField = new TUCNGravField();
		cerr << "Field: " << fGravField->GetName() << " created." << endl;
	}
	return fGravField;
}

//______________________________________________________________________________
TUCNMagField* TUCNFieldManager::AddUniformMagField(const Double_t Bx, const Double_t By, const Double_t Bz)
{
	if (fMagField) {
		cerr << "Field: " << fMagField->GetName() << " already exists. Deleting this field before adding new field..." << endl;
		delete fMagField;
		fMagField = 0;
	}
	
	const char* name = "UniformMagField";
	fMagField = new TUCNUniformMagField(name, Bx, By, Bz);
	return fMagField;
}

//______________________________________________________________________________
TUCNMagField* TUCNFieldManager::AddParabolicMagField(const Double_t maxB, const Double_t alpha, const Double_t maxR)
{
	if (fMagField) {
		cerr << "Field: " << fMagField->GetName() << " already exists. Deleting this field before adding new field..." << endl;
		delete fMagField;
		fMagField = 0;
	}
	
	const char* name = "ParabolicMagField";
	fMagField = new TUCNParabolicMagField(name, maxB, alpha, maxR);
	return fMagField;
}