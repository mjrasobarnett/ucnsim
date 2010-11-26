// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#ifndef ROOT_TUCNRun
#define ROOT_TUCNRun

#include "TNamed.h"
#include <string>
#include <list>
#include "TUCNData.h"
#include "TUCNExperiment.h"
#include "TUCNRunConfig.h"
#include "TUCNObserver.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNRun															  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TGeoManager;
class TUCNConfigFile;
class TUCNParticle;

class TUCNRun : public TNamed 
{
protected:
   TUCNRunConfig        fRunConfig;
   TUCNData*            fData;
   TUCNExperiment*      fExperiment;
   
   Bool_t               LoadParticles(const TUCNRunConfig& runConfig);
   void                 PrintProgress(Int_t entry, Float_t nEntriesF, Int_t mintime=2);
   Bool_t               InitialiseObservers(const TUCNRunConfig& runConfig);
   
public:
   // -- constructors
   TUCNRun();
   TUCNRun(const TUCNRunConfig& runConfig);
   TUCNRun(const TUCNRun&); 
   TUCNRun& operator=(const TUCNRun&);
   // -- destructor
   virtual ~TUCNRun();
   
   // -- methods
   
   // Get/Setters
   Int_t                Neutrons() const        {return fData->InitialParticles();}
   Double_t             RunTime() const         {return fRunConfig.RunTime();}
   Double_t             MaxStepTime() const     {return fRunConfig.MaxStepTime();}
   
   TGeoManager*         GeoManager() const      {return fExperiment->GeoManager();}
   TGeoNavigator*       Navigator() const       {return fExperiment->Navigator();}
   TUCNFieldManager*    FieldManager() const    {return fExperiment->FieldManager();}
   
   TUCNData*            GetData()            {return fData;}
   TUCNExperiment*      GetExperiment()      {return fExperiment;}
   const TUCNRunConfig& GetRunConfig()       {return fRunConfig;}
   
   // Data storage
   
   // Run Procedures
   Bool_t               Initialise();
   Bool_t               Start();
   Bool_t               Finish();
      
   ClassDef(TUCNRun, 1)
};

#endif
