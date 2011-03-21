// Run configuration class
// Author: Matthew Raso-Barnett  18/11/2010
#include <iostream>
#include <sstream>
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
          :TObject(), fNames(), fOptions(), fParams()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "RunConfig::Default Constructor" << endl;
   #endif
}

//__________________________________________________________________________
RunConfig::RunConfig(const ConfigFile& masterConfig, int runNumber)
          :TObject(), fNames(), fOptions(), fParams()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "RunConfig::Constructor" << endl;
   #endif
   //////////////////////////////////////////////////////////////////
   // -- Extract full path to directory containing configuration files 
   const string compressedfolderpath = masterConfig.GetString("Path","Folder");
   const string folderpath = Algorithms::FileSystem::ExpandFilePath(compressedfolderpath);
   //////////////////////////////////////////////////////////////////
   // -- Use full folder path to find and build the run-config file
   ostringstream runID;
   runID << "Run" << runNumber; 
   string runConfigFileName = folderpath + masterConfig.GetString("Config", runID.str());
   if (runConfigFileName.empty() == kTRUE) {
      cout << "Error: Run Configuration file name specified is either empty of invalid" << endl;
      throw runtime_error("Error fetching name of Run Config file");
   }
   ConfigFile runConfigFile(runConfigFileName);
   // -----------------------------------
   ReadInRunConfig(runConfigFile, folderpath);
   // -- Check for Override parameters
   // Get the section of the master config file containing this run's configfile and override params
   map<string,string> section = masterConfig.GetSection(runID.str());
   CheckForOverrideParameters(section);
   // -----------------------------------
   // Print properties
   this->Print();
}

//__________________________________________________________________________
RunConfig::RunConfig(const RunConfig& other)
          :TObject(other),
           fNames(other.fNames),
           fOptions(other.fOptions),
           fParams(other.fParams)
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
void RunConfig::ReadInRunConfig(const ConfigFile& runConfigFile, const string folderpath)
{
   //////////////////////////////////////////////////////////////////
   // -- Search the Run Config file for parameters
   // -----------------------------------
   // Name of Run object
   string runName = runConfigFile.GetString(RunParams::runName,"Name");
   fNames.insert(NamePair(RunParams::runName, runName));
   // -----------------------------------
   // -- Files to Load
   // Name of Input Geometry
   string geomFileName = runConfigFile.GetString(RunParams::geomFile,"Files");
   if (geomFileName.empty() == true) {
      throw runtime_error("No GeomFile specified in runconfig");
   }
   fNames.insert(NamePair(RunParams::geomFile, folderpath + geomFileName));
   // Name of Visualisation Geometry
   string geomVisFileName = runConfigFile.GetString(RunParams::geomVisFile,"Files");
   if (geomVisFileName.empty() == true) {
      geomVisFileName = geomFileName;
   }
   fNames.insert(NamePair(RunParams::geomVisFile, folderpath + geomVisFileName));
   // Name of Input particle data file 
   string inputDataFileName = runConfigFile.GetString(RunParams::inputFile,"Files");
   if (inputDataFileName.empty() == true) {
      throw runtime_error("No InputDataFile specified in runconfig");
   }
   fNames.insert(NamePair(RunParams::inputFile, folderpath + inputDataFileName));
   // Name of Output data file
   string outputDataFileName = runConfigFile.GetString(RunParams::outputFile,"Files");
   if (outputDataFileName.empty() == true) {
      throw runtime_error("No OutputDataFile specified in runconfig");
   }
   fNames.insert(NamePair(RunParams::outputFile, folderpath + outputDataFileName));
   // Name of Fields input file -- Not required to be set
   string fieldsFileName = runConfigFile.GetString(RunParams::fieldsFile,"Files");
   if (fieldsFileName.empty() == false) {
      fNames.insert(NamePair(RunParams::fieldsFile, folderpath + fieldsFileName));
   }
   // -----------------------------------
   // -- Input Particles options
   // Name of state folder to load particles from
   string particlesToLoad = runConfigFile.GetString(RunParams::inputParticleState,"Particles");
   if (Algorithms::DataFile::ValidateStateNames(particlesToLoad) == false) {
      throw runtime_error("Invalid InputParticleState specified in runconfig");
   }
   fNames.insert(NamePair(RunParams::inputParticleState, particlesToLoad));
   // Option for whether to load all particles in input folder
   bool loadAllParticles = runConfigFile.GetBool(RunParams::loadAllParticles,"Particles");
   fOptions.insert(OptionPair(RunParams::loadAllParticles, loadAllParticles));
   // Option for whether to restart particles from their initial states (obviously this is
   // only relevant if ParticlesToLoad is not already the initial states) 
   bool restartParticles = runConfigFile.GetBool(RunParams::restartParticles,"Particles");
   fOptions.insert(OptionPair(RunParams::restartParticles, restartParticles));
   // -----------------------------------
   // -- Run Options
   // Option for whether gravity is turned on
   bool gravField = runConfigFile.GetBool(RunParams::gravField,"Properties");
   fOptions.insert(OptionPair(RunParams::gravField, gravField));
   // Option for whether wall losses are turned on
   bool wallLosses = runConfigFile.GetBool(RunParams::wallLosses,"Properties");
   fOptions.insert(OptionPair(RunParams::wallLosses, wallLosses));
   // Option for whether magnetic field is turned on
   bool magField = runConfigFile.GetBool(RunParams::magField,"Properties");
   fOptions.insert(OptionPair(RunParams::magField, magField));
   // Option for whether magnetic field is turned on
   bool elecField = runConfigFile.GetBool(RunParams::elecField,"Properties");
   fOptions.insert(OptionPair(RunParams::elecField, elecField));
   // Parameter to be set; Specifies time to simulate neutrons to
   double runTime = runConfigFile.GetFloat(RunParams::runTime,"Properties");
   if (runTime <= 0.) {throw runtime_error("Invalid RunTime specified in runconfig");}
   fParams.insert(ParamPair(RunParams::runTime, runTime));
   // Parameter to be set; Maximum geometrical step allowed in simulation
   double maxStepTime = runConfigFile.GetFloat(RunParams::maxStepTime,"Properties");
   if (maxStepTime <= 0.) {throw runtime_error("Invalid MaxStepTime specified in runconfig");}
   fParams.insert(ParamPair(RunParams::maxStepTime, maxStepTime));
   // Parameter to be set; Maximum spin step allowed in simulation
   double spinStepTime = runConfigFile.GetFloat(RunParams::spinStepTime,"Properties");
   if (spinStepTime <= 0.) {throw runtime_error("Invalid SpinStepTime specified in runconfig");}
   fParams.insert(ParamPair(RunParams::spinStepTime, spinStepTime));
   // -----------------------------------
   // -- Observer Options
   // Option for whether to record numbers of Bounces
   bool recordBounces = runConfigFile.GetBool(RunParams::recordBounces,"Observables");
   fOptions.insert(OptionPair(RunParams::recordBounces, recordBounces));
   // Option for whether to record particle tracks
   bool recordTracks = runConfigFile.GetBool(RunParams::recordTracks,"Observables");
   fOptions.insert(OptionPair(RunParams::recordTracks, recordTracks));
   // Parameter to be set; Specify frequency of track measurements (if desired -
   // by default it will measure all track points)
   double trackMeasFreq = runConfigFile.GetFloat("TrackMeasureFrequency(Hz)","Observables");
   double trackMeasInterval = (trackMeasFreq == 0.0 ? 0.0 : (1.0/trackMeasFreq));
   fParams.insert(ParamPair(RunParams::trackMeasFreq, trackMeasInterval));
   // Option for whether to record spin states
   bool recordSpin = runConfigFile.GetBool(RunParams::recordSpin,"Observables");
   fOptions.insert(OptionPair(RunParams::recordSpin, recordSpin));
   // Parameter to be set; Specify frequency of spin measurements
   double spinMeasFreq = runConfigFile.GetFloat("SpinMeasureFrequency(Hz)","Observables");
   double spinMeasInterval = (spinMeasFreq == 0.0 ? 0.0 : (1.0/spinMeasFreq));
   fParams.insert(ParamPair(RunParams::spinMeasFreq, spinMeasInterval));
   // Option for whether to record field seen by particles
   bool recordField = runConfigFile.GetBool(RunParams::recordField,"Observables");
   fOptions.insert(OptionPair(RunParams::recordField, recordField));
   // Parameter to be set; Specify frequency of field measurements
   double fieldMeasFreq = runConfigFile.GetFloat("FieldMeasureFrequency(Hz)","Observables");
   double fieldMeasInterval = (fieldMeasFreq == 0.0 ? 0.0 : (1.0/fieldMeasFreq));
   fParams.insert(ParamPair(RunParams::fieldMeasFreq, fieldMeasInterval));
   // -----------------------------------
   // -----------------------------------
   // Check for inconsistencies in RunConfig File
   if (fieldsFileName.empty() && (magField == true || elecField == true)) {
      throw runtime_error("Incompatible options in RunConfig: Check FieldsFile and Mag/ElecField");
   }
   if (magField == true && spinStepTime == 0.0) {
      throw runtime_error("Incompatible options in RunConfig: Check MagFieldOn and SpinStepTime");
   }
   if (recordSpin == true && spinMeasFreq == 0.0) {
      throw runtime_error("Incompatible options in RunConfig: Check RecordSpin and SpinMeasFreq");
   }
   if (recordField == true && fieldMeasInterval == 0.0) {
      throw runtime_error("Incompatible options in RunConfig: Check RecordField and FieldMeasFreq");
   }
}

//__________________________________________________________________________
void RunConfig::CheckForOverrideParameters(const std::map<std::string, std::string> section)
{
   // -- Look for any variables within the provided section from the master config file,
   // -- and if any of these variables match the name of a variable in the run, replace its value
   // -- with that from the master config
   // Loop over all variables in this section
   map<string,string>::const_iterator secIt;
   for (secIt = section.begin(); secIt != section.end(); secIt++) {
      // Check if variable's name matches a variable from the 'Names'
      // section of the RunConfig file 
      map<string,string>::iterator nameIt = fNames.find(secIt->first);
      if (nameIt != fNames.end()) {
         // If there is a match, override the runconfig value with that from the masterconfig
         string new_value = secIt->second;
         cout << "Overriding RunConfig Parameter: " << secIt->first;
         cout << " from: " << nameIt->second << " to: " << new_value << endl;
         nameIt->second = new_value;
         continue;
      }
      // Check if variable's name matches a variable from the 'Options'
      // section of the RunConfig file 
      map<string,bool>::iterator optIt = fOptions.find(secIt->first);
      if (optIt != fOptions.end()) {
         // If there is a match, override the runconfig value with that from the masterconfig
         string s_value = secIt->second;
         bool new_value = false;
         if (Algorithms::String::ConvertToBool(s_value, new_value) == true) {
            cout << "Overriding RunConfig Parameter: " << secIt->first;
            cout << " from: " << optIt->second << " to: " << new_value << endl;
            optIt->second = new_value;
         }
         continue;
      }
      // Check if variable's name matches a variable from the 'Parameters'
      // section of the RunConfig file 
      map<string,double>::iterator parIt = fParams.find(secIt->first);
      if (parIt != fParams.end()) {
         // If there is a match, override the runconfig value with that from the masterconfig
         string s_value = secIt->second;
         double new_value = 0.;
         if (Algorithms::String::ConvertToDouble(s_value, new_value) == true) {
            cout << "Overriding RunConfig Parameter: " << secIt->first;
            cout << " from: " << parIt->second << " to: " << new_value << endl;
            parIt->second = new_value;
         }
         continue;
      }
   }
}

//__________________________________________________________________________
string RunConfig::RunName() const
{
   map<string, string>::const_iterator it = fNames.find(RunParams::runName);
   return (it == fNames.end()) ? "" : it->second;
}

//__________________________________________________________________________
string RunConfig::GeomFileName() const
{
   map<string, string>::const_iterator it = fNames.find(RunParams::geomFile);
   return (it == fNames.end()) ? "" : it->second;
}

//__________________________________________________________________________
string RunConfig::GeomVisFileName() const
{
   map<string, string>::const_iterator it = fNames.find(RunParams::geomVisFile);
   return (it == fNames.end()) ? "" : it->second;
}

//__________________________________________________________________________
string RunConfig::FieldsFileName() const
{
   map<string, string>::const_iterator it = fNames.find(RunParams::fieldsFile);
   return (it == fNames.end()) ? "" : it->second;
}

//__________________________________________________________________________
string RunConfig::InputFileName() const
{
   map<string, string>::const_iterator it = fNames.find(RunParams::inputFile);
   return (it == fNames.end()) ? "" : it->second;
}

//__________________________________________________________________________
string RunConfig::OutputFileName() const
{
   map<string, string>::const_iterator it = fNames.find(RunParams::outputFile);
   return (it == fNames.end()) ? "" : it->second;
}

//__________________________________________________________________________
string RunConfig::InputRunName() const
{
   map<string, string>::const_iterator it = fNames.find(RunParams::inputRunName);
   return (it == fNames.end()) ? "" : it->second;
}

//__________________________________________________________________________
string RunConfig::ParticlesToLoad() const
{
   map<string, string>::const_iterator it = fNames.find(RunParams::inputParticleState);
   return (it == fNames.end()) ? "" : it->second;
}

//__________________________________________________________________________
bool RunConfig::LoadAllParticles() const
{
   map<string, bool>::const_iterator it = fOptions.find(RunParams::loadAllParticles);
   return (it == fOptions.end()) ? true : it->second;
}

//__________________________________________________________________________
bool RunConfig::RestartFromBeginning() const
{
   map<string, bool>::const_iterator it = fOptions.find(RunParams::restartParticles);
   return (it == fOptions.end()) ? false : it->second;
}

//__________________________________________________________________________
double RunConfig::RunTime() const
{
   map<string, double>::const_iterator it = fParams.find(RunParams::runTime);
   return (it == fParams.end()) ? 0. : it->second;
}

//__________________________________________________________________________
double RunConfig::MaxStepTime() const
{
   map<string, double>::const_iterator it = fParams.find(RunParams::maxStepTime);
   return (it == fParams.end()) ? 0. : it->second;
}

//__________________________________________________________________________
double RunConfig::SpinStepTime() const
{
   map<string, double>::const_iterator it = fParams.find(RunParams::spinStepTime);
   return (it == fParams.end()) ? 0. : it->second;
}

//__________________________________________________________________________
bool RunConfig::GravFieldOn() const
{
   map<string, bool>::const_iterator it = fOptions.find(RunParams::gravField);
   return (it == fOptions.end()) ? true : it->second;
}

//__________________________________________________________________________
bool RunConfig::MagFieldOn() const
{
   map<string, bool>::const_iterator it = fOptions.find(RunParams::magField);
   return (it == fOptions.end()) ? false : it->second;
}

//__________________________________________________________________________
bool RunConfig::WallLossesOn() const
{
   map<string, bool>::const_iterator it = fOptions.find(RunParams::wallLosses);
   return (it == fOptions.end()) ? true : it->second;
}

//__________________________________________________________________________
bool RunConfig::ObserveSpin() const
{
   map<string, bool>::const_iterator it = fOptions.find(RunParams::recordSpin);
   return (it == fOptions.end()) ? false : it->second;
}

//__________________________________________________________________________
bool RunConfig::ObserveBounces() const
{
   map<string, bool>::const_iterator it = fOptions.find(RunParams::recordBounces);
   return (it == fOptions.end()) ? false : it->second;
}

//__________________________________________________________________________
bool RunConfig::ObserveTracks() const
{
   map<string, bool>::const_iterator it = fOptions.find(RunParams::recordTracks);
   return (it == fOptions.end()) ? false : it->second;
}

//__________________________________________________________________________
bool RunConfig::ObserveField() const
{
   map<string, bool>::const_iterator it = fOptions.find(RunParams::recordField);
   return (it == fOptions.end()) ? false : it->second;
}

//__________________________________________________________________________
double RunConfig::TrackMeasureInterval() const
{
   map<string, double>::const_iterator it = fParams.find(RunParams::trackMeasFreq);
   return (it == fParams.end()) ? 0. : it->second;
}

//__________________________________________________________________________
double RunConfig::SpinMeasureInterval() const
{
   map<string, double>::const_iterator it = fParams.find(RunParams::spinMeasFreq);
   return (it == fParams.end()) ? 0. : it->second;
}

//__________________________________________________________________________
double RunConfig::FieldMeasureInterval() const
{
   map<string, double>::const_iterator it = fParams.find(RunParams::fieldMeasFreq);
   return (it == fParams.end()) ? 0. : it->second;
}

//__________________________________________________________________________
void RunConfig::Print(Option_t* /*option*/) const
{
   cout << "-------------------------------------------" << endl;
   cout << "Run Configuration Settings" << endl;
   cout << "Name: " << RunName() << endl;
   cout << "GeomFile: " << GeomFileName() << endl;
   cout << "GeomVisFile: " << GeomVisFileName() << endl;
   cout << "InputDataFile: " << InputFileName() << endl;
   cout << "OutputDataFile: " << OutputFileName() << endl;
   cout << "FieldsFile: " << FieldsFileName() << endl;
   cout << "InputRunName: " << InputRunName() << endl;
   cout << "ParticlesToLoad: " << ParticlesToLoad() << endl;
   cout << "LoadAllParticles: " << LoadAllParticles() << endl;
   cout << "RestartParticles: " << RestartFromBeginning() << endl;
   cout << "GravFieldOn: " << GravFieldOn() << endl;
   cout << "MagFieldOn: " << MagFieldOn() << endl;
   cout << "WallLossesOn: " << WallLossesOn() << endl;
   cout << "RunTime (s): " << RunTime() << endl;
   cout << "MaxStepTime (s): " << MaxStepTime() << endl;
   cout << "SpinStepTime (s): " << SpinStepTime() << endl;
   cout << "Observe Spin: " << ObserveSpin() << endl;
   cout << "Observe Bounces: " << ObserveBounces() << endl;
   cout << "Observe Tracks: " << ObserveTracks() << endl;
   cout << "Observe Field: " << ObserveField() << endl;
   cout << "Track Measurement Interval: " << TrackMeasureInterval() << endl;
   cout << "Spin Measurement Interval: " << SpinMeasureInterval() << endl;
   cout << "Field Measurement Interval: " << FieldMeasureInterval() << endl;
   cout << "-------------------------------------------" << endl;
}
