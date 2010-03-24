// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#ifndef ROOT_TUCNRun
#define ROOT_TUCNRun

#include "TNamed.h"
#include "TUCNData.h"
#include "TUCNExperiment.h";

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
   TUCNData*            fData;
   TUCNExperiment*      fExperiment;
   
   Double_t             fRunTime;
   Double_t             fMaxStepTime;
   Bool_t               fWallLosses;
   
   Bool_t               LoadParticles(TUCNConfigFile& configFile);
   void                 PrintProgress(Int_t entry, Float_t nEntriesF, Int_t mintime=5);
   
public:
   // -- constructors
   TUCNRun();
   TUCNRun(const char *name, const char *title);
   TUCNRun(const TUCNRun&); 
   TUCNRun& operator=(const TUCNRun&);
   // -- destructor
   virtual ~TUCNRun();
   
   // -- methods
   
   // Get/Setters
   Int_t                Neutrons() const        {return fData->InitialParticles();}
   Double_t             RunTime() const         {return fRunTime;}
   Double_t             MaxStepTime() const     {return fMaxStepTime;}
   
   TGeoManager*         GeoManager() const      {return fExperiment->GeoManager();}
   TGeoNavigator*       Navigator() const       {return fExperiment->Navigator();}
   TUCNFieldManager*    FieldManager() const    {return fExperiment->FieldManager();}
   
   TUCNData*            GetData()            {return fData;}
   TUCNExperiment*      GetExperiment()      {return fExperiment;}
   
   // Data storage
   Bool_t               SaveInitialParticle(TUCNParticle* particle);
   Bool_t               SaveParticle(TUCNParticle* particle);
   TUCNParticle*        GetInitialParticle(Int_t particleID);
   TUCNParticle*        GetParticle(Int_t particleID);
   
   // Run Procedures
   Bool_t               Initialise(TUCNConfigFile& configFile);
   Bool_t               Start();
   Bool_t               Finish(TUCNConfigFile& configFile);
   
   ClassDef(TUCNRun, 1)
};

#endif
