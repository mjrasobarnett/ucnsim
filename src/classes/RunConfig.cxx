// Run configuration class
// Author: Matthew Raso-Barnett  18/11/2010
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Algorithms.h"
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
           fObsTracks(false), fObsField(false), fTrackMeasInterval(0.), fSpinMeasInterval(0.),
           fFieldMeasInterval(0.)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "RunConfig::Default Constructor" << endl;
   #endif
}

//__________________________________________________________________________
RunConfig::RunConfig(const ConfigFile& masterConfig, int runNumber)
          :TObject(), fRunName(""), fGeomFile(""), fGeomVisFile(""), fInputDataFile(""),
           fOutputDataFile(""), fFieldsFile(""), fInputRunName(""),
           fParticlesToLoad(""), fLoadAllParticles(true), fRestartParticles(false),
           fGravFieldOn(true), fMagFieldOn(false), fWallLossesOn(true), fRunTime(0.),
           fMaxStepTime(0.), fSpinStepTime(0.), fObsSpin(false), fObsBounces(false),
           fObsTracks(false), fObsField(false), fTrackMeasInterval(0.), fSpinMeasInterval(0.),
           fFieldMeasInterval(0.)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "RunConfig::Constructor" << endl;
   #endif
   //////////////////////////////////////////////////////////////////
   // -- Extract full path to directory containing configuration files 
   const string compressedfolderpath = masterConfig.GetString("Path","Folder");
   const string folderpath = masterConfig.ExpandFilePath(compressedfolderpath);
   //////////////////////////////////////////////////////////////////
   // -- Use full folder path to find and build the run-config file
   Char_t runID[6];
   sprintf(runID,"Run%d",runNumber);
   string runConfigFileName = folderpath + masterConfig.GetString("Config", runID);
   if (runConfigFileName.empty() == kTRUE) {
      cout << "Error: Run Configuration file name specified is either empty of invalid" << endl;
      throw runtime_error("Error fetching name of Run Config file");
   }
   ConfigFile runConfigFile(runConfigFileName);
   //////////////////////////////////////////////////////////////////
   // -- Search the Run Config file for parameters
   // -----------------------------------
   // Name of Run object
   fInputRunName = runConfigFile.GetString("InputRunName","Particles");
   // -----------------------------------
   // -- Files to Load
   // Name of Run
   fRunName = runConfigFile.GetString("RunName","Name");
   // Name of Input Geometry
   string geomFileName = runConfigFile.GetString("GeomFile","Files");
   if (geomFileName.empty() == true) {
      throw runtime_error("No GeomFile specified in runconfig");
   }
   fGeomFile = folderpath + geomFileName;
   // Name of Visualisation Geometry
   string geomVisFileName = runConfigFile.GetString("GeomVisFile","Files");
   if (geomVisFileName.empty() == true) {
      geomVisFileName = geomFileName;
   }
   fGeomVisFile = folderpath + geomVisFileName;
   // Name of Input particle data file 
   string inputDataFileName = runConfigFile.GetString("InputDataFile","Files");
   if (inputDataFileName.empty() == true) {
      throw runtime_error("No InputDataFile specified in runconfig");
   }
   fInputDataFile = folderpath + inputDataFileName;
   // Name of Output data file
   string outputDataFileName = runConfigFile.GetString("OutputDataFile","Files");
   if (outputDataFileName.empty() == true) {
      throw runtime_error("No OutputDataFile specified in runconfig");
   }
   fOutputDataFile = folderpath + outputDataFileName;
   // Name of Fields input file -- Not required to be set
   string fieldsFileName = runConfigFile.GetString("FieldsFile","Files");
   if (fieldsFileName.empty() == false) {fFieldsFile = folderpath + fieldsFileName;}
   else {fFieldsFile = "";}
   // -----------------------------------
   // -- Input Particles options
   // Name of state folder to load particles from
   fParticlesToLoad = runConfigFile.GetString("InputParticleState","Particles");
   if (Algorithms::DataFile::ValidateStateNames(fParticlesToLoad) == false) {
      throw runtime_error("Invalid InputParticleState specified in runconfig");
   }
   // Option for whether to load all particles in input folder 
   fLoadAllParticles = runConfigFile.GetBool("AllParticles","Particles");
   // Option for whether to restart particles from their initial states (obviously this is
   // only relevant if ParticlesToLoad is not already the initial states) 
   fRestartParticles = runConfigFile.GetBool("RunFromBeginning","Particles");
   // -----------------------------------
   // -- Run Options
   // Option for whether gravity is turned on
   fGravFieldOn = runConfigFile.GetBool("GravField","Properties");
   // Option for whether wall losses are turned on
   fWallLossesOn = runConfigFile.GetBool("WallLosses","Properties");
   // Option for whether magnetic field is turned on
   fMagFieldOn = runConfigFile.GetBool("MagFields","Properties");
   // Parameter to be set; Specifies time to simulate neutrons to  
   fRunTime = runConfigFile.GetFloat("RunTime(s)","Properties");
   if (fRunTime <= 0.) {throw runtime_error("Invalid RunTime specified in runconfig");}
   // Parameter to be set; Maximum geometrical step allowed in simulation 
   fMaxStepTime = runConfigFile.GetFloat("MaxStepTime(s)","Properties");
   if (fMaxStepTime <= 0.) {fMaxStepTime = 1.0*Units::s;}
   // Parameter to be set; Maximum spin step allowed in simulation 
   fSpinStepTime = runConfigFile.GetFloat("SpinStepTime(s)","Properties");
   if (fSpinStepTime <= 0.) {throw runtime_error("Invalid SpinStepTime specified in runconfig");}
   // -----------------------------------
   // -- Observer Options
   // Option for whether to record numbers of Bounces
   fObsBounces = runConfigFile.GetBool("RecordBounces","Observables");
   // Option for whether to record particle tracks
   fObsTracks = runConfigFile.GetBool("RecordTracks","Observables");
   // Parameter to be set; Specify frequency of track measurements (if desired - 
   // by default it will measure all track points)
   double trackMeasFreq = runConfigFile.GetFloat("TrackMeasureFrequency(Hz)","Observables");
   fTrackMeasInterval = (trackMeasFreq == 0.0 ? 0.0 : (1.0/trackMeasFreq)); 
   // Option for whether to record spin states
   fObsSpin = runConfigFile.GetBool("RecordSpin","Observables");
   // Parameter to be set; Specify frequency of spin measurements
   double spinMeasFreq = runConfigFile.GetFloat("SpinMeasureFrequency(Hz)","Observables");
   fSpinMeasInterval = (spinMeasFreq == 0.0 ? 0.0 : (1.0/spinMeasFreq));
   // Option for whether to record field seen by particles
   fObsField = runConfigFile.GetBool("RecordField","Observables");
   // Parameter to be set; Specify frequency of field measurements
   double fieldMeasFreq = runConfigFile.GetFloat("FieldMeasureFrequency(Hz)","Observables");
   fFieldMeasInterval = (fieldMeasFreq == 0.0 ? 0.0 : (1.0/fieldMeasFreq)); 
   // -----------------------------------
   // -----------------------------------
   // Check for inconsistencies in RunConfig File
   if (fFieldsFile.empty() && fMagFieldOn == true) {
      throw runtime_error("Incompatible options in RunConfig: Check FieldsFile and MagFieldOn");
   } else if (fMagFieldOn == true && fSpinStepTime == 0.0) {
      throw runtime_error("Incompatible options in RunConfig: Check MagFieldOn and SpinStepTime");
   } else if (fObsSpin == true && fSpinMeasInterval == 0.0) {
      throw runtime_error("Incompatible options in RunConfig: Check RecordSpin and SpinMeasFreq");
   } else if (fObsField == true && fFieldMeasInterval == 0.0) {
      throw runtime_error("Incompatible options in RunConfig: Check RecordField and FieldMeasFreq");
   } else if (fMagFieldOn == false) {
      fSpinStepTime = 0.0;
      fObsSpin = false;
      fObsField = false;
      fSpinMeasInterval = 0.0;
      fFieldMeasInterval = 0.0;
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
               fTrackMeasInterval(other.fTrackMeasInterval),
               fSpinMeasInterval(other.fSpinMeasInterval),
               fFieldMeasInterval(other.fFieldMeasInterval)
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
   cout << "Track Measurement Interval: " << fTrackMeasInterval << endl;
   cout << "Spin Measurement Interval: " << fSpinMeasInterval << endl;
   cout << "Field Measurement Interval: " << fFieldMeasInterval << endl;
   cout << "-------------------------------------------" << endl;
}
