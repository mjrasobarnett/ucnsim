// TUCNFieldManager
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>
#include <string>

#include "TObjArray.h"
#include "TUCNConfigFile.h"
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
Bool_t TUCNFieldManager::Initialise(TUCNConfigFile& configFile)
{
// -- Read in the field environment for the experiment from configFile.
	cout << "-------------------------------------------" << endl;
	cout << "Initialising the Field Environment" << endl;
	cout << "-------------------------------------------" << endl;
	// Start with the Grav Field
	if (configFile.GetBool("Presence","GravField")) {
		Info("Initialise","Gravitational Field set to be ON. Creating...");
		this->AddGravField();
	} else {
		Info("Initialise","Gravitational Field set to be OFF.");
	}
	
	// Set-up the magnetic field
	if (configFile.GetBool("Presence","MagField")) {
		// Field is present
		std::string fieldType = configFile.GetString("Type","MagField");
		Info("Initialise", "Magnetic Field of type: %s present. Creating...", fieldType.c_str());
		Double_t fieldStrength = configFile.GetFloat("FieldStrength","MagField");
		if (fieldType == "Parabolic") {
			// Creating a Rotationally symmetric, parabolically decaying field
			Double_t fieldRadius = configFile.GetFloat("FieldRadius(m)","MagField");
			Double_t gradient = configFile.GetFloat("ParabolicGradient","MagField");
			this->AddParabolicMagField(fieldStrength, gradient, fieldRadius);
		} else if (fieldType == "Uniform") {
			// Creating a Uniform field
			this->AddUniformMagField(fieldStrength);
		} else {
			Error("Initialise", "Magnetic Field set to be present, but Field Type has been set.");
			return kFALSE;
		}
	} else {
		Info("Initialise", "Magnetic Field NOT present.");
	}
	cout << "-------------------------------------------" << endl;
	cout << "Field Environment initialised successfully" << endl;
	cout << "-------------------------------------------" << endl;
	return kTRUE;
}

//______________________________________________________________________________
TUCNGravField* TUCNFieldManager::AddGravField()
{
// Create and store reference to grav field object	
	if (fGravField) {
		Warning("AddGravField","GravField: %s already exists!", fGravField->GetName());
	} else {
		fGravField = new TUCNGravField();
		Info("AddGravField","GravField: %s created.", fGravField->GetName());
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