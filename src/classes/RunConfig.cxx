// Run configuration class
// Author: Matthew Raso-Barnett  18/11/2010
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Constants.h"
#include "Units.h"

#include "RunConfig.h"
#include "ConfigFile.h"

using namespace std;

ClassImp(RunConfig);

//__________________________________________________________________________
RunConfig::RunConfig()
          :TObject(), fRunName(""), fGeomFile(""), fGeomVisFile(""), fInputDataFile(""),
           fOutputDataFile(""), fFieldsFile(""), fInputRunName(""),
           fParticlesToLoad(""), fLoadAllParticles(true), fRestartParticles(false),
           fGravFieldOn(true), fMagFieldOn(false), fWallLossesOn(true), fRunTime(0.),
           fMaxStepTime(0.), fSpinStepTime(0.), fObsSpin(false), fObsBounces(false),
           fObsTracks(false), fObsField(false), fSpinMeasFreq(0.), fFieldMeasFreq(0.)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "RunConfig::Default Constructor" << endl;
   #endif
}

//__________________________________________________________________________
RunConfig::RunConfig(const ConfigFile& masterConfig, int runNumber)
          :TObject()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "RunConfig::Constructor" << endl;
   #endif
   // Extract full path to config files 
   const string compressedfolderpath = masterConfig.GetString("Path","Folder");
   const string folderpath = masterConfig.ExpandFilePath(compressedfolderpath);
   // Use full folder path to fetch the run config file
   Char_t runID[6];
   sprintf(runID,"Run%d",runNumber);
   string runConfigFileName = folderpath + masterConfig.GetString("Config", runID);
   if (runConfigFileName.empty() == kTRUE) {
      cout << "Unable to read in Run Configuration file name" << endl;
      throw runtime_error("Error fetching name of Run Config file");
   }

   ConfigFile runConfigFile(runConfigFileName);
   fRunName = runConfigFile.GetString("RunName","Name");
   
   fGeomFile = folderpath + runConfigFile.GetString("GeomFile","Files");
   string geomVisFileName = runConfigFile.GetString("GeomVisFile","Files");
   if (geomVisFileName.empty() == false) {fGeomVisFile = folderpath + geomVisFileName;}
   
   fInputDataFile = folderpath + runConfigFile.GetString("InputDataFile","Files");
   fOutputDataFile = folderpath + runConfigFile.GetString("OutputDataFile","Files");
   
   fInputRunName = runConfigFile.GetString("InputRunName","Particles");
   fParticlesToLoad = runConfigFile.GetString("WhichParticles","Particles");
   fLoadAllParticles = runConfigFile.GetBool("AllParticles","Particles");
   fRestartParticles = runConfigFile.GetBool("RunFromBeginning","Particles");
   
   fGravFieldOn = runConfigFile.GetBool("GravField","Properties");
   fWallLossesOn = runConfigFile.GetBool("WallLosses","Properties");
   
   fRunTime = runConfigFile.GetFloat("RunTime(s)","Properties");
   fMaxStepTime = runConfigFile.GetFloat("MaxStepTime(s)","Properties");
   
   fObsBounces = runConfigFile.GetBool("RecordBounces","Observables");
   fObsTracks = runConfigFile.GetBool("RecordTracks","Observables");
   
   fFieldsFile = folderpath + runConfigFile.GetString("FieldsFile","Files");
   fMagFieldOn = runConfigFile.GetBool("MagFields","Properties");
   fSpinStepTime = runConfigFile.GetFloat("SpinStepTime(s)","Properties");
   fObsSpin = runConfigFile.GetBool("RecordSpin","Observables");
   fSpinMeasFreq = runConfigFile.GetFloat("SpinMeasurementFrequency(s)","Observables");
   fObsField = runConfigFile.GetBool("RecordField","Observables");
   fFieldMeasFreq = runConfigFile.GetFloat("FieldMeasurementFrequency(s)","Observables");
   // Check for inconsistencies in RunConfig File
   if (fFieldsFile.empty() && fMagFieldOn == true) {
      throw runtime_error("Incompatible options in RunConfig: Check FieldsFile and MagFieldOn");
   } else if (fMagFieldOn == true && fSpinStepTime == 0.0) {
      throw runtime_error("Incompatible options in RunConfig: Check MagFieldOn and SpinStepTime");
   } else if (fObsSpin == true && fSpinMeasFreq == 0.0) {
      throw runtime_error("Incompatible options in RunConfig: Check RecordSpin and SpinMeasFreq");
   } else if (fObsField == true && fFieldMeasFreq == 0.0) {
      throw runtime_error("Incompatible options in RunConfig: Check RecordField and FieldMeasFreq");
   } else if (fMagFieldOn == false) {
      fSpinStepTime = 0.0;
      fObsSpin = false;
      fObsField = false;
      fSpinMeasFreq = 0.0;
      fFieldMeasFreq = 0.0;
   }
   this->Print();
}

//__________________________________________________________________________
RunConfig::RunConfig(const RunConfig& other)
              :TObject(other),
               fRunName(other.fRunName),
               fGeomFile(other.fGeomFile),
               fGeomVisFile(other.fGeomVisFile),
               fInputDataFile(other.fInputDataFile),
               fOutputDataFile(other.fOutputDataFile),
               fFieldsFile(other.fFieldsFile),
               fInputRunName(other.fInputRunName),
               fParticlesToLoad(other.fParticlesToLoad),
               fLoadAllParticles(other.fLoadAllParticles),
               fRestartParticles(other.fRestartParticles),
               fGravFieldOn(other.fGravFieldOn),
               fMagFieldOn(other.fMagFieldOn),
               fWallLossesOn(other.fWallLossesOn),
               fRunTime(other.fRunTime),
               fMaxStepTime(other.fMaxStepTime),
               fSpinStepTime(other.fSpinStepTime),
               fObsSpin(other.fObsSpin),
               fObsBounces(other.fObsBounces),
               fObsTracks(other.fObsTracks),
               fObsField(other.fObsField),
               fSpinMeasFreq(other.fSpinMeasFreq),
               fFieldMeasFreq(other.fFieldMeasFreq)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "RunConfig::Copy Constructor" << endl;
   #endif
}

//__________________________________________________________________________
RunConfig::~RunConfig()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "RunConfig::Destructor" << endl;
   #endif
}

//__________________________________________________________________________
void RunConfig::Print(Option_t* /*option*/) const
{
   cout << "-------------------------------------------" << endl;
   cout << "Run Configuration Settings" << endl;
   cout << "Name: " << fRunName << endl;
   cout << "GeomFile: " << fGeomFile << endl;
   cout << "GeomVisFile: " << fGeomVisFile << endl;
   cout << "InputDataFile: " << fInputDataFile << endl;
   cout << "OutputDataFile: " << fOutputDataFile << endl;
   cout << "FieldsFile: " << fFieldsFile << endl;
   cout << "InputRunName: " << fInputRunName << endl;
   cout << "InitialParticleBranch: " << fParticlesToLoad << endl;
   cout << "LoadAllParticles: " << fLoadAllParticles << endl;
   cout << "RestartParticles: " << fRestartParticles << endl;
   cout << "GravFieldOn: " << fGravFieldOn << endl;
   cout << "MagFieldOn: " << fMagFieldOn << endl;
   cout << "WallLossesOn: " << fWallLossesOn << endl;
   cout << "RunTime: " << fRunTime << " s"<< endl;
   cout << "MaxStepTime: " << fMaxStepTime << " s"<< endl;
   cout << "SpinStepTime: " << fSpinStepTime << " s"<< endl;
   cout << "Observe Spin: " << fObsSpin << endl;
   cout << "Observe Bounces: " << fObsBounces << endl;
   cout << "Observe Tracks: " << fObsTracks << endl;
   cout << "Observe Field: " << fObsField << endl;
   cout << "Spin Measurement Frequency: " << fSpinMeasFreq << endl;
   cout << "Field Measurement Frequency: " << fFieldMeasFreq << endl;
   cout << "-------------------------------------------" << endl;
}
