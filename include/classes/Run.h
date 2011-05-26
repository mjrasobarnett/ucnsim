// Run
// Author: Matthew Raso-Barnett  08/10/2009

#ifndef ROOT_Run
#define ROOT_Run

#include "TNamed.h"
#include "Data.h"
#include "Experiment.h"
#include "RunConfig.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						Run															  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TGeoManager;
class ConfigFile;
class Particle;

class Run : public TNamed 
{
private:
   RunConfig        fRunConfig;
   Data             fData;
   Experiment*      fExperiment;
   
public:
   // -- constructors
   Run();
   Run(const RunConfig& runConfig);
   Run(const Run&); 
   Run& operator=(const Run&);
   // -- destructor
   virtual ~Run();
   
   // -- methods
   
   // Get/Setters
   Int_t                Neutrons() const        {return fData.InitialParticles();}
   Double_t             RunTime() const         {return fRunConfig.RunTime();}
   Double_t             MaxStepTime() const     {return fRunConfig.MaxStepTime();}
   
   Data&                GetData()             {return fData;}
   const Experiment&    GetExperiment() const {return *fExperiment;}
   const RunConfig&     GetRunConfig() const {return fRunConfig;}
   
   // Run Procedures
   Bool_t               Initialise();
   Bool_t               Start();
   Bool_t               Finish();
      
   ClassDef(Run, 1)
};

#endif
