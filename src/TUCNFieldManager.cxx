// TUCNFieldManager
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>
#include <string>

#include "TObjArray.h"
#include "TUCNConfigFile.h"
#include "TUCNRun.h"
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
using std::string;

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
Bool_t TUCNFieldManager::Initialise(TUCNConfigFile& configFile, const TUCNRun& run)
{
// -- Read in the field environment for the experiment from configFile.
   cout << "-------------------------------------------" << endl;
   cout << "Initialising the Field Environment" << endl;
   cout << "-------------------------------------------" << endl;
   // Set-up the Grav Field
   if (configFile.GetBool("GravField",run.GetName())) {
      Info("Initialise","Gravitational Field set to be ON. Creating...");
      this->AddGravField();
   } else {
      Info("Initialise","Gravitational Field set to be OFF.");
   }
   // Set-up the magnetic field
   if (configFile.GetBool("MagField",run.GetName())) {
      // Field is present. Read in Field Map
      std::string fieldMapName = configFile.GetString("MagFieldMap",run.GetName());
      if (fieldMapName == "") {
         Error("Initialise", "Magnetic Field set to be present, but Field Type has been set.");
         return kFALSE;
      }
      Info("Initialise", "Magnetic Field present. Creating...");
      /*
         Need to flesh out a MagFieldMap system
      */
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
	string name = "UniformMagField";
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
	string name = "ParabolicMagField";
	fMagField = new TUCNParabolicMagField(name, maxB, alpha, maxR);
	return fMagField;
}