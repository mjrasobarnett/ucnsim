// Experiment
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>
#include <string>

#include "Experiment.h"

#include "Run.h"
#include "ConfigFile.h"

#include "TGeoManager.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;

//#define VERBOSE_MODE

ClassImp(Experiment)

//_____________________________________________________________________________
Experiment::Experiment()
               :TNamed("Experiment", "The Experimental Geometry")
{
// -- Default constructor
   Info("Experiment", "Default Constructor");
   fFieldManager = 0;
   fGeoManager = 0;
} 

//_____________________________________________________________________________
Experiment::Experiment(const Experiment& other)
               :TNamed(other),
                fFieldManager(other.fFieldManager),
                fGeoManager(other.fGeoManager)
{
// -- Copy Constructor
	Info("Experiment", "Copy Constructor");
}

//_____________________________________________________________________________
Experiment& Experiment::operator=(const Experiment& other)
{
// --assignment operator
   if(this!=&other) {
      TNamed::operator=(other);
      fFieldManager = other.fFieldManager;
      fGeoManager = other.fGeoManager;
   }
   return *this;
}

//______________________________________________________________________________
Experiment::~Experiment()
{ 
// -- Destructor
   Info("Experiment", "Destructor");
   if (fFieldManager) delete fFieldManager;
//   Leave GeoManager to be cleaned up by ROOT, otherwise there are problems
}

// -- METHODS --
//______________________________________________________________________________
Bool_t Experiment::BuildGeometry(const RunConfig& runConfig)
{
// -- Build Geomtery from file
   TString geomFileName = runConfig.GeomFileName();
   if (geomFileName == "") { 
      Error("BuildGeometry","No Geometry File has been specified");
      return kFALSE;
   }
   // Load the Geometry from the File
   fGeoManager = TGeoManager::Import(geomFileName);
   if (fGeoManager == NULL) {
      Error("BuildGeometry","Unable to load Geometry File");
      return kFALSE;
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t Experiment::Initialise(const RunConfig& runConfig)
{
// -- Initialise the Experimental Geometry with the Configuration File
   cout << "-------------------------------------------" << endl;
   cout << "Initialising the Experimental Geometry" << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build Geometry
   ///////////////////////////////////////////////////////////////////////////////////////
   new TGeoManager("GeoManager", "The Geometry Manager");
   if (!(this->BuildGeometry(runConfig))) { 
      Error("Initialise","Failed building geometry.");
      return kFALSE;
   }
   cout << "-------------------------------------------" << endl;
   cout << "Geometry has been created succesfully" << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Field Initialisation
   ///////////////////////////////////////////////////////////////////////////////////////
   fFieldManager = new FieldManager();
   if(!(fFieldManager->Initialise(runConfig))) {
      Error("Initialise","Failed in field initialisation.");
      return kFALSE;
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t Experiment::ExportGeometry(Run& run)
{
   // -- Write create the Visualisation Geometry and write it out to file if
   // -- it exists. Else just export the usual Geometry
   string geomFileName = run.GetRunConfig().GeomVisFileName();
   if (geomFileName.empty() == false) {
      fGeoManager = TGeoManager::Import(geomFileName.c_str());
      if (fGeoManager == NULL) return kFALSE;
   }
   // Write Geomanger to file
   run.GetData()->SaveGeometry(fGeoManager);
   return kTRUE;
}

