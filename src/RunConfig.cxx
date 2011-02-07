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

//__________________________________________________________________________
RunConfig::RunConfig()
          :fRunName(""), fGeomFile(""), fGeomVisFile(""), fInputDataFile(""),
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
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "RunConfig::Constructor" << endl;
   #endif
   const string folderpath = masterConfig.GetString("Path","Folder");
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
   fGeomVisFile = folderpath + runConfigFile.GetString("GeomVisFile","Files");
   fInputDataFile = folderpath + runConfigFile.GetString("InputDataFile","Files");
   fOutputDataFile = folderpath + runConfigFile.GetString("OutputDataFile","Files");
   fFieldsFile = folderpath + runConfigFile.GetString("FieldsFile","Files");
   
   fInputRunName = runConfigFile.GetString("InputRunName","Particles");
   fParticlesToLoad = runConfigFile.GetString("WhichParticles","Particles");
   fLoadAllParticles = runConfigFile.GetBool("AllParticles","Particles");
   fRestartParticles = runConfigFile.GetBool("RunFromBeginning","Particles");
   
   fGravFieldOn = runConfigFile.GetBool("GravField","Properties");
   fMagFieldOn = runConfigFile.GetBool("MagFields","Properties");
   fWallLossesOn = runConfigFile.GetBool("WallLosses","Properties");
   
   fRunTime = runConfigFile.GetFloat("RunTime(s)","Properties");
   fMaxStepTime = runConfigFile.GetFloat("MaxStepTime(s)","Properties");
   fSpinStepTime = runConfigFile.GetFloat("SpinStepTime(s)","Properties");
   
   fObsSpin = runConfigFile.GetBool("RecordSpin","Observables");
   fObsBounces = runConfigFile.GetBool("RecordBounces","Observables");
   fObsTracks = runConfigFile.GetBool("RecordTracks","Observables");
   fObsField = runConfigFile.GetBool("RecordField","Observables");
   
   fSpinMeasFreq = runConfigFile.GetFloat("SpinMeasurementFrequency(s)","Observables");
   fFieldMeasFreq = runConfigFile.GetFloat("FieldMeasurementFrequency(s)","Observables");
   this->Print();
}

//__________________________________________________________________________
RunConfig::RunConfig(const RunConfig& other)
              :fRunName(other.fRunName),
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
RunConfig& RunConfig::operator=(const RunConfig& other)
{
   if(this!=&other) {
      fRunName = other.fRunName;
      fGeomFile=other.fGeomFile;
      fGeomVisFile=other.fGeomVisFile;
      fInputDataFile=other.fInputDataFile;
      fOutputDataFile=other.fOutputDataFile;
      fFieldsFile=other.fFieldsFile;
      fInputRunName=other.fInputRunName;
      fParticlesToLoad=other.fParticlesToLoad;
      fLoadAllParticles=other.fLoadAllParticles;
      fRestartParticles=other.fRestartParticles;
      fGravFieldOn=other.fGravFieldOn;
      fMagFieldOn=other.fMagFieldOn;
      fWallLossesOn=other.fWallLossesOn;
      fRunTime=other.fRunTime;
      fMaxStepTime=other.fMaxStepTime;
      fSpinStepTime=other.fSpinStepTime;
      fObsSpin=other.fObsSpin;
      fObsBounces=other.fObsBounces;
      fObsTracks=other.fObsTracks;
      fObsField=other.fObsField;
      fSpinMeasFreq=other.fSpinMeasFreq;
      fFieldMeasFreq=other.fFieldMeasFreq;
   }
   return *this;
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
