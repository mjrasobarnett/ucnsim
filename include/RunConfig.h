// Run Configuration class
// Author: Matthew Raso-Barnett  
// 18/11/2010

#ifndef RUNCONFIG_H
#define RUNCONFIG_H

#include "TObject.h"
#include <string>
#include <map>

namespace RunParams {
   // -- Names
   static const std::string runName = "RunName";
   static const std::string geomFile = "GeomFile";
   static const std::string geomVisFile = "GeomVisFile";
   static const std::string inputFile = "InputDataFile";
   static const std::string outputFile = "OutputDataFile";
   static const std::string fieldsFile = "FieldsFile";
   static const std::string inputRunName = "InputRunName";
   static const std::string inputParticleState = "InputParticleState";
   // -- Options (Boolean or yes/no questions)
   static const std::string loadAllParticles = "AllParticles";
   static const std::string restartParticles = "RunFromBeginning";
   static const std::string gravField = "GravField";
   static const std::string magField = "MagFields";
   static const std::string elecField = "ElecFields";
   static const std::string wallLosses = "WallLosses";
   static const std::string recordSpin = "RecordSpin";
   static const std::string recordBounces = "RecordBounces";
   static const std::string recordTracks = "RecordTracks";
   static const std::string recordField = "RecordField";
   // -- Parameters (Allows values to be set by user)
   static const std::string runTime = "RunTime(s)";
   static const std::string maxStepTime = "MaxStepTime(s)";
   static const std::string spinStepTime = "SpinStepTime(s)";
   static const std::string trackMeasFreq = "TrackMeasureFrequency(Hz)";
   static const std::string spinMeasFreq = "SpinMeasureFrequency(Hz)";
   static const std::string fieldMeasFreq = "FieldMeasureFrequency(Hz)";
}

class ConfigFile;

class RunConfig : public TObject {

private:
   typedef std::pair<std::string, std::string> NamePair;
   typedef std::pair<std::string, bool> OptionPair;
   typedef std::pair<std::string, float> ParamPair;
   std::map<std::string, std::string> fNames;
   std::map<std::string, bool> fOptions;
   std::map<std::string, double> fParams;

   void ReadInRunConfig(const ConfigFile& runConfigFile, const std::string folderpath);
   void CheckForOverrideParameters(const std::map<std::string, std::string> section);

public:
   // Constructors
   RunConfig();
   RunConfig(const ConfigFile& masterConfig, int runNumber);
   RunConfig(const RunConfig&); 
   virtual ~RunConfig();
   
   // Methods
   std::string RunName() const;
   std::string GeomFileName() const;
   std::string GeomVisFileName() const;
   std::string FieldsFileName() const;
   std::string InputFileName() const;
   std::string OutputFileName() const;
   std::string InputRunName() const;
   std::string ParticlesToLoad() const;
   bool LoadAllParticles() const;
   bool RestartFromBeginning() const;
   double RunTime() const;
   double MaxStepTime() const;
   double SpinStepTime() const;
   bool GravFieldOn() const;
   bool MagFieldOn() const;
   bool ElecFieldOn() const;
   bool WallLossesOn() const;
   bool ObserveSpin() const;
   bool ObserveBounces() const;
   bool ObserveTracks() const;
   bool ObserveField() const;
   double TrackMeasureInterval() const;
   double SpinMeasureInterval() const;
   double FieldMeasureInterval() const;;
   
   virtual void Print(Option_t* option = "") const;

   ClassDef(RunConfig,1);
};
#endif /*RUNCONFIG_H*/
