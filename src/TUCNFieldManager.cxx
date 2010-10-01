// TUCNFieldManager
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>
#include <string>

#include "TObjArray.h"
#include "TFile.h"
#include "TString.h"
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
                 :TNamed("FieldManager", "Default Field Manager"), fGravField(NULL),
                  fMagFieldManager(NULL)
{
// -- Default constructor
   Info("TUCNFieldManager", "Default Constructor");
} 

//_____________________________________________________________________________
TUCNFieldManager::TUCNFieldManager(const TUCNFieldManager& other)
                 :TNamed(other), fGravField(other.fGravField),
                  fMagFieldManager(other.fMagFieldManager)
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
      fGravField = other.fGravField;
      fMagFieldManager = other.fMagFieldManager;
   }
   return *this;
}

//______________________________________________________________________________
TUCNFieldManager::~TUCNFieldManager()
{ 
// -- Destructor
   Info("TUCNFieldManager", "Destructor");
   if (fGravField) delete fGravField;
   if (fMagFieldManager) delete fMagFieldManager;
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
