// FieldManager
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>
#include <string>

#include "TObjArray.h"
#include "TFile.h"
#include "TString.h"

#include "ConfigFile.h"
#include "Run.h"
#include "GravField.h"
#include "MagField.h"
#include "UniformMagField.h"
#include "ParabolicMagField.h"
#include "Point.h"

#include "FieldManager.h"

#include "Units.h"
#include "Constants.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;

//#define VERBOSE_MODE

ClassImp(FieldManager)

//_____________________________________________________________________________
FieldManager::FieldManager()
                 :TNamed("FieldManager", "Default Field Manager"),
                  fGravField(NULL),
                  fMagFieldArray(NULL),
                  fElecFieldArray(NULL)
{
// -- Default constructor
   Info("FieldManager", "Default Constructor");
} 

//_____________________________________________________________________________
FieldManager::FieldManager(const FieldManager& other)
                 :TNamed(other),
                  fGravField(other.fGravField),
                  fMagFieldArray(other.fMagFieldArray),
                  fElecFieldArray(other.fElecFieldArray)
{
// -- Copy Constructor
   Info("FieldManager", "Copy Constructor");
}

//_____________________________________________________________________________
FieldManager& FieldManager::operator=(const FieldManager& other)
{
// --assignment operator
   if(this!=&other) {
      TNamed::operator=(other);
      fGravField = other.fGravField;
      fMagFieldArray = other.fMagFieldArray;
      fElecFieldArray = other.fElecFieldArray;
   }
   return *this;
}

//______________________________________________________________________________
FieldManager::~FieldManager()
{ 
// -- Destructor
   Info("FieldManager", "Destructor");
   if (fGravField) delete fGravField;
   if (fMagFieldArray) delete fMagFieldArray;
   if (fElecFieldArray) delete fElecFieldArray;
}

// -- METHODS --

//______________________________________________________________________________
Bool_t FieldManager::Initialise(const RunConfig& runConfig)
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
   // Set-up the electric/magnetic fields
   if (runConfig.MagFieldOn() == kTRUE || runConfig.ElecFieldOn() == kTRUE) {
      // Look for Fields file
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
      if (runConfig.MagFieldOn() == kTRUE) {
         Info("Initialise","Magnetic Environment set to be ON. Creating...");
         TString magManagerName = "MagFieldArray";
         MagFieldArray* importedMagFieldArray = 0;
         f->GetObject(magManagerName,importedMagFieldArray);
         if (importedMagFieldArray == NULL) {
            Error("Initialise","Could not find: %s in file", magManagerName.Data());
            return kFALSE;
         }
         // Store MagField Manager
         fMagFieldArray = importedMagFieldArray;
         importedMagFieldArray = 0;
      } else {
         Info("Initialise","Magnetic Environment set to be OFF.");
      }
      // Search for Electric Field Manager
      if (runConfig.ElecFieldOn() == kTRUE) {
         Info("Initialise","Electric Fields Environment set to be ON. Creating...");
         TString elecFieldArrayName = "ElecFieldArray";
         ElecFieldArray* importedElecFieldArray = 0;
         f->GetObject(elecFieldArrayName,importedElecFieldArray);
         if (importedElecFieldArray == NULL) {
            Error("Initialise","Could not find: %s in file", elecFieldArrayName.Data());
            return kFALSE;
         }
         // Store ElecField Manager
         fElecFieldArray = importedElecFieldArray;
         importedElecFieldArray = 0;
      } else {
         Info("Initialise","Electric Environment set to be OFF.");
      }
      // Clean up file
      f->Close();
      delete f;
      f = 0;
   }
   
   cout << "-------------------------------------------" << endl;
   cout << "Field Environment initialised successfully" << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
}

//______________________________________________________________________________
GravField* FieldManager::AddGravField()
{
// Create and store reference to grav field object	
   if (fGravField) {
      Warning("AddGravField","GravField: %s already exists!", fGravField->GetName());
   } else {
      fGravField = new GravField();
      Info("AddGravField","GravField: %s created.", fGravField->GetName());
   }
   return fGravField;
}

//______________________________________________________________________________
const TVector3 FieldManager::GetMagField(const Point& point, const TVector3& velocity, const string volume) const
{
   // -- Calculate the total magnetic field at the given position
   TVector3 bfield(0.,0.,0.);
   if (fMagFieldArray != NULL) {
      bfield += fMagFieldArray->GetMagField(point,velocity,volume);
   }
   if (fElecFieldArray != NULL) {
      bfield += fElecFieldArray->GetMagField(point,velocity,volume);
   }
   return bfield;
}

