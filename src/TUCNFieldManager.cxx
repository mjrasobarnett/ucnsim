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
Bool_t TUCNFieldManager::Initialise(const TUCNRunConfig& runConfig)
{
// -- Read in the field environment for the experiment from configFile.
   cout << "-------------------------------------------" << endl;
   cout << "Initialising the Field Environment" << endl;
   cout << "-------------------------------------------" << endl;
   // Set-up the Grav Field
   if (runConfig.GravFieldOn() == kTRUE) {
      Info("Initialise","Gravitational Field set to be ON. Creating...");
      this->AddGravField();
   } else {
      Info("Initialise","Gravitational Field set to be OFF.");
   }
   // Set-up the magnetic fields
   if (runConfig.MagFieldOn() == kTRUE) {
      // Look for Fields file
      Info("Initialise","Magnetic Environment set to be ON. Creating...");
      TString fieldsFileName = runConfig.FieldsFileName();
      if (fieldsFileName == "") { 
         Error("Initialise","No Fields File has been specified");
         return kFALSE;
      }
      cout << "Loading Electromagnetic Field environment from file: " << fieldsFileName << endl;
      // Open Electro-magnetic Field file
      TFile *f = TFile::Open(fieldsFileName,"read");
      if (!f || f->IsZombie()) {
         Error("Initialise","Cannot open file: %s", fieldsFileName.Data());
         return kFALSE;
      }
      f->ls();
      // Search for Magnetic Field Manager
      TString magManagerName = "MagFieldManager";
      TUCNMagFieldManager* importedMagFieldManager = 0;
      f->GetObject(magManagerName,importedMagFieldManager);
      if (importedMagFieldManager == NULL) {
         Error("Initialise","Could not find MagFieldManager: %s in file", magManagerName.Data());
         return kFALSE;
      }
      // Store MagField Manager
      fMagFieldManager = importedMagFieldManager;
      // Clean up
      importedMagFieldManager = 0;
      f->Close();
      delete f;
      f = 0;
      
   } else {
      Info("Initialise","Magnetic Environment set to be OFF.");
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
const TUCNMagField* const TUCNFieldManager::GetMagField(const TVector3& position, const string& volume) const
{
   return fMagFieldManager->GetMagField(position, volume);
}

