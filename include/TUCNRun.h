// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#ifndef ROOT_TUCNRun
#define ROOT_TUCNRun

#include "TNamed.h"
#include "TUCNData.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNRun															  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNConfigFile;
class TGeoManager;
class TUCNFieldManager;
class TUCNParticle;

class TUCNRun : public TNamed 
{
protected:
   TUCNData*            fData;
   
   Double_t             fRunTime;
   Double_t             fMaxStepTime;
   Double_t             fDiffuseCoeff;
   Bool_t               fSampleMagField;
   Bool_t               fWallLosses;
   
   Int_t                fAbsorbedCounter; // Absorbed includes any loss to the boundary
   Int_t                fDetectedCounter;
   Int_t                fDecayedCounter;
   Int_t                fLostCounter;
   
   TUCNData*            GetData()         {return fData;}
   void                 PrintProgress(Int_t entry, Float_t nEntriesF, Int_t mintime=10);
   
public:
   // -- constructors
   TUCNRun();
   TUCNRun(const TUCNRun&); 
   TUCNRun& operator=(const TUCNRun&);
   TUCNRun(const char *name, const char *title);
   // -- destructor
   virtual ~TUCNRun();
   
   Bool_t               Initialise(TUCNConfigFile* configFile);
   Bool_t               Export(TString& outputFile);
   
   Int_t                Neutrons() const              {return fData->InitialParticles();}
   Double_t             RunTime() const               {return fRunTime;}
   Double_t             MaxStepTime()                 {return fMaxStepTime;}
   Int_t                Detected() const              {return fDetectedCounter;}
   Int_t                Absorbed() const              {return fAbsorbedCounter;}
   Int_t                Decayed() const               {return fDecayedCounter;}
   Int_t                Lost() const                  {return fLostCounter;}
   
   void                 IncrementDetected()           {fDetectedCounter++;}
   void                 IncrementAbsorbed()           {fAbsorbedCounter++;}
   void                 IncrementDecayed()            {fDecayedCounter++;}
   void                 IncrementLost()               {fLostCounter++;}
      
//   Bool_t               AddTrack(TVirtualGeoTrack* track);
   Bool_t               SaveInitialParticle(TUCNParticle* particle);
   Bool_t               SaveParticle(TUCNParticle* particle);
//   TGeoTrack*           GetTrack(Int_t trackID);
   TUCNParticle*        GetInitialParticle(Int_t particleID);
   TUCNParticle*        GetParticle(Int_t particleID);
   
   Bool_t               Propagate(TGeoManager* geoManager, TUCNFieldManager* fieldManager);
   
   
   ClassDef(TUCNRun, 1)
};

#endif
