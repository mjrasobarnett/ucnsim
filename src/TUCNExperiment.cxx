// TUCNExperiment
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>

#include "TUCNExperiment.h"

#include "TUCNRun.h"
#include "TUCNFieldManager.h"
#include "TUCNConfigFile.h"

#include "TGeoManager.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;

//#define VERBOSE_MODE

ClassImp(TUCNExperiment)

//_____________________________________________________________________________
TUCNExperiment::TUCNExperiment()
               :TNamed("Experiment", "The Experimental Geometry")
{
// -- Default constructor
   Info("TUCNExperiment", "Default Constructor");
   fFieldManager = 0;
} 

//_____________________________________________________________________________
TUCNExperiment::TUCNExperiment(const TUCNExperiment& other)
               :TNamed(other),
                fFieldManager(other.fFieldManager)
{
// -- Copy Constructor
	Info("TUCNExperiment", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNExperiment& TUCNExperiment::operator=(const TUCNExperiment& other)
{
// --assignment operator
   if(this!=&other) {
      TNamed::operator=(other);
      fFieldManager = other.fFieldManager;
   }
   return *this;
}

//______________________________________________________________________________
TUCNExperiment::~TUCNExperiment()
{ 
// -- Destructor
   Info("TUCNExperiment", "Destructor");
   if (fFieldManager) delete fFieldManager;
   if (gGeoManager) delete gGeoManager;
}

// -- METHODS --
//______________________________________________________________________________
Bool_t TUCNExperiment::BuildGeometry(TUCNConfigFile& configFile, const TUCNRun& run)
{
// -- Build Geomtery from file
   TString geomFileName = configFile.GetString("GeomFile",run.GetName());
   if (geomFileName == "") { 
      Error("BuildGeometry","No Geometry File has been specified");
      return kFALSE;
   }
   // Load the Geometry from the File
   TGeoManager::Import(geomFileName);
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TUCNExperiment::Initialise(TUCNConfigFile& configFile, const TUCNRun& run)
{
// -- Initialise the Experimental Geometry with the Configuration File
   cout << "-------------------------------------------" << endl;
   cout << "Initialising the Experimental Geometry" << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build Geometry
   ///////////////////////////////////////////////////////////////////////////////////////
   new TGeoManager("GeoManager", "The Geometry Manager");
   if (!(this->BuildGeometry(configFile, run))) { 
      Error("Initialise","Failed building geometry.");
      return kFALSE;
   }
   cout << "-------------------------------------------" << endl;
   cout << "Geometry has been created succesfully" << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Field Initialisation
   ///////////////////////////////////////////////////////////////////////////////////////
   fFieldManager = new TUCNFieldManager();
   if(!(this->FieldManager()->Initialise(configFile, run))) {
      Error("Initialise","Failed in field initialisation.");
      return kFALSE;
   }
   return kTRUE;
}
