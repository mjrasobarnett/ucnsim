// TUCNFieldManager
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>

#include "TObjArray.h"
#include "TUCNGravField.h"
#include "TUCNMagField.h"

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
	fMagFields = new TObjArray();
	fGravField = 0;
	
} 

//_____________________________________________________________________________
TUCNFieldManager::TUCNFieldManager(const TUCNFieldManager& other)
					  :TNamed(other),
						fMagFields(other.fMagFields),
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
		fMagFields = other.fMagFields;
		fGravField = other.fGravField;
	}
   return *this;
}

//______________________________________________________________________________
TUCNFieldManager::~TUCNFieldManager()
{ 
// -- Destructor
	Info("TUCNFieldManager", "Destructor");
	if (fMagFields) {fMagFields->Delete(); delete fMagFields;}
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
