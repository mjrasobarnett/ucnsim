// Run Configuration class
// Author: Matthew Raso-Barnett  
// 18/11/2010

#ifndef RUNCONFIG_H
#define RUNCONFIG_H

#include <string>
#include "TVector3.h"

class RunConfig {
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
   bool fRestartParticles;
   
   bool fGravFieldOn;
   bool fMagFieldOn;
   bool fWallLossesOn;
   
   double fRunTime;
   double fMaxStepTime;
   
   bool fObsPolarisation;
   TVector3 fObsMeasAxis;
   
   bool fObsBounces;
   bool fObsTracks;
   
public:
   // Constructors
   RunConfig();
   RunConfig(const std::string& runConfigFileName);
   RunConfig(const RunConfig&); 
   RunConfig& operator=(const RunConfig&);
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
   bool GravFieldOn() const {return fGravFieldOn;}
   bool MagFieldOn() const {return fMagFieldOn;}
   bool WallLossesOn() const {return fWallLossesOn;}
   bool ObservePolarisation() const {return fObsPolarisation;}
   TVector3 PolarisationAxis() const {return fObsMeasAxis;}
   bool ObserveBounces() const {return fObsBounces;}
   bool ObserveTracks() const {return fObsTracks;}
   
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(RunConfig, 0)
};
#endif /*RUNCONFIG_H*/