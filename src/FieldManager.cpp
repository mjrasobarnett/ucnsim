// FieldManager
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>
#include <string>

#include "TObjArray.h"
#include "TFile.h"
#include "TString.h"

#include "ConfigFile.hpp"
#include "Run.hpp"
#include "GravField.hpp"
#include "MagField.hpp"
#include "UniformMagField.hpp"
#include "ParabolicMagField.hpp"
#include "Point.hpp"

#include "FieldManager.hpp"

#include "Units.hpp"
#include "Constants.hpp"

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
   // -- Check what fields to read in from the configFile and import them from
   // -- Fields file.
   if (runConfig.GravFieldOn() == false && runConfig.MagFieldOn() == false && runConfig.ElecFieldOn() == false) {
      // No fields active
      return kTRUE;
   }
   // Read in the gravitational & electric/magnetic fields
   cout << "-------------------------------------------" << endl;
   cout << "Initialising the Field Environment" << endl;
   cout << "-------------------------------------------" << endl;
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
   // Search for the Gravitational Field
   if (runConfig.GravFieldOn() == kTRUE) {
      Info("Initialise","Gravitational Field set to be ON. Creating...");
      TString gravFieldName = "GravField";
      f->GetObject(gravFieldName,fGravField);
      if (fGravField == NULL) {
         Error("Initialise","Could not find: %s in file", gravFieldName.Data());
         return kFALSE;
      }
   } else {
      Info("Initialise","Gravitational Field set to be OFF.");
   }
   // Search for Magnetic Field Manager
   if (runConfig.MagFieldOn() == kTRUE) {
      Info("Initialise","Magnetic Environment set to be ON. Creating...");
      TString magManagerName = "MagFieldArray";
      f->GetObject(magManagerName,fMagFieldArray);
      if (fMagFieldArray == NULL) {
         Error("Initialise","Could not find: %s in file", magManagerName.Data());
         return kFALSE;
      }
   } else {
      Info("Initialise","Magnetic Environment set to be OFF.");
   }
   // Search for Electric Field Manager
   if (runConfig.ElecFieldOn() == kTRUE) {
      Info("Initialise","Electric Fields Environment set to be ON. Creating...");
      TString elecFieldArrayName = "ElecFieldArray";
      f->GetObject(elecFieldArrayName,fElecFieldArray);
      if (fElecFieldArray == NULL) {
         Error("Initialise","Could not find: %s in file", elecFieldArrayName.Data());
         return kFALSE;
      }
   } else {
      Info("Initialise","Electric Environment set to be OFF.");
   }
   // Clean up file
   f->Close();
   cout << "-------------------------------------------" << endl;
   cout << "Field Environment initialised successfully" << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
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

