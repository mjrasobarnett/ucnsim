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
   fGeoManager = 0;
} 

//_____________________________________________________________________________
TUCNExperiment::TUCNExperiment(const TUCNExperiment& other)
               :TNamed(other),
                fFieldManager(other.fFieldManager),
                fGeoManager(other.fGeoManager)
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
      fGeoManager = other.fGeoManager;
   }
   return *this;
}

//______________________________________________________________________________
TUCNExperiment::~TUCNExperiment()
{ 
// -- Destructor
   Info("TUCNExperiment", "Destructor");
   if (fFieldManager) delete fFieldManager;
//   Leave GeoManager to be cleaned up by ROOT, otherwise there are problems
}

// -- METHODS --
//______________________________________________________________________________
Bool_t TUCNExperiment::BuildGeometry(const TUCNRunConfig& runConfig)
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
Bool_t TUCNExperiment::Initialise(const TUCNRunConfig& runConfig)
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
   fFieldManager = new TUCNFieldManager();
   if(!(this->FieldManager()->Initialise(runConfig))) {
      Error("Initialise","Failed in field initialisation.");
      return kFALSE;
   }
   return kTRUE;
}
