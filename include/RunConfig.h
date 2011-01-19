// Run Configuration class
// Author: Matthew Raso-Barnett  
// 18/11/2010

#ifndef RUNCONFIG_H
#define RUNCONFIG_H

#include "TObject.h"
#include <string>
#include "TVector3.h"

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
   Bool_t fLoadAllParticles;
//   std::vector<Int_t> fParticleIDs;
   Bool_t fRestartParticles;
   
   Bool_t fGravFieldOn;
   Bool_t fMagFieldOn;
   Bool_t fWallLossesOn;
   
   Double_t fRunTime;
   Double_t fMaxStepTime;
   Double_t fSpinStepTime;
   
   Bool_t fObsPolarisation;   
   Bool_t fObsBounces;
   Bool_t fObsTracks;
   Bool_t fObsField;
   
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
   Bool_t LoadAllParticles() const {return fLoadAllParticles;}
   Bool_t RestartFromBeginning() const {return fRestartParticles;}
   Double_t RunTime() const {return fRunTime;}
   Double_t MaxStepTime() const {return fMaxStepTime;}
   Double_t SpinStepTime() const {return fSpinStepTime;}
   Bool_t GravFieldOn() const {return fGravFieldOn;}
   Bool_t MagFieldOn() const {return fMagFieldOn;}
   Bool_t WallLossesOn() const {return fWallLossesOn;}
   Bool_t ObservePolarisation() const {return fObsPolarisation;}
   Bool_t ObserveBounces() const {return fObsBounces;}
   Bool_t ObserveTracks() const {return fObsTracks;}
   Bool_t ObserveField() const {return fObsField;}
   
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(RunConfig, 1)
};
#endif /*RUNCONFIG_H*/