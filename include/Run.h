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
protected:
   RunConfig        fRunConfig;
   Data*            fData;
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
   Int_t                Neutrons() const        {return fData->InitialParticles();}
   Double_t             RunTime() const         {return fRunConfig.RunTime();}
   Double_t             MaxStepTime() const     {return fRunConfig.MaxStepTime();}
   
   TGeoManager*         GetGeoManager() const      {return fExperiment->GetGeoManager();}
   TGeoNavigator*       GetNavigator() const       {return fExperiment->GetNavigator();}
   FieldManager*        GetFieldManager() const    {return fExperiment->GetFieldManager();}
   
   Data*                GetData()            {return fData;}
   Experiment*          GetExperiment()      {return fExperiment;}
   const RunConfig&     GetRunConfig()       {return fRunConfig;}
   
   // Run Procedures
   Bool_t               Initialise();
   Bool_t               Start();
   Bool_t               Finish();
      
   ClassDef(Run, 1)
};

#endif
