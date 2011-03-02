// Run Configuration class
// Author: Matthew Raso-Barnett  
// 18/11/2010

#ifndef RUNCONFIG_H
#define RUNCONFIG_H

#include "TObject.h"
#include <string>
#include "TVector3.h"

class ConfigFile;

class RunConfig : public TObject {
private:
   std::string fRunName;
   
   std::string fGeomFile;
   std::string fGeomVisFile;
   std::string fInputDataFile;
   std::string fOutputDataFile;
   std::string fFieldsFile;
   
   std::string fInputRunName;
   std::string fParticlesToLoad;
   bool fLoadAllParticles;
//   std::vector<Int_t> fParticleIDs;
   bool fRestartParticles;
   
   bool fGravFieldOn;
   bool fMagFieldOn;
   bool fWallLossesOn;
   
   double fRunTime;
   double fMaxStepTime;
   double fSpinStepTime;
   
   bool fObsSpin;   
   bool fObsBounces;
   bool fObsTracks;
   bool fObsField;
   
   double fTrackMeasInterval;
   double fSpinMeasInterval;
   double fFieldMeasInterval;
   
public:
   // Constructors
   RunConfig();
   RunConfig(const ConfigFile& masterConfig, int runNumber);
   RunConfig(const RunConfig&); 
   virtual ~RunConfig();
   
   // Methods
   std::string RunName() const {return fRunName;}
   std::string GeomFileName() const {return fGeomFile;}
   std::string GeomVisFileName() const {return fGeomVisFile;}
   std::string FieldsFileName() const {return fFieldsFile;}
   std::string InputFileName() const {return fInputDataFile;}
   std::string OutputFileName() const {return fOutputDataFile;}
   std::string InputRunName() const {return fInputRunName;}
   std::string ParticlesToLoad() const {return fParticlesToLoad;}
   bool LoadAllParticles() const {return fLoadAllParticles;}
   bool RestartFromBeginning() const {return fRestartParticles;}
   double RunTime() const {return fRunTime;}
   double MaxStepTime() const {return fMaxStepTime;}
   double SpinStepTime() const {return fSpinStepTime;}
   bool GravFieldOn() const {return fGravFieldOn;}
   bool MagFieldOn() const {return fMagFieldOn;}
   bool WallLossesOn() const {return fWallLossesOn;}
   bool ObserveSpin() const {return fObsSpin;}
   bool ObserveBounces() const {return fObsBounces;}
   bool ObserveTracks() const {return fObsTracks;}
   bool ObserveField() const {return fObsField;}
   double TrackMeasureInterval() const {return fTrackMeasInterval;}
   double SpinMeasureInterval() const {return fSpinMeasInterval;}
   double FieldMeasureInterval() const {return fFieldMeasInterval;}
   
   virtual void Print(Option_t* option = "") const;

   ClassDef(RunConfig,1);
};
#endif /*RUNCONFIG_H*/
