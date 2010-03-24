// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#ifndef ROOT_TUCNRun
#define ROOT_TUCNRun

#include "TNamed.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNRun															  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TGeoManager;
class TObjArray;
class TCanvas;
class TPolyMarker3D;

class TUCNConfigFile;
class TUCNFieldManager;
class TUCNData;
class TUCNParticle;
class TUCNGravField;
class TUCNMagField;
class TUCNFieldManager;

class TUCNRun : public TNamed 
{
protected:
   
   TObjArray*           fParticles;
   TUCNData*            fData;
   
   Int_t                fNeutrons;
   Double_t             fTotalEnergy;
   Double_t             fRunTime;
   Double_t             fMaxStepTime;
   Double_t             fDiffuseCoeff;
   Bool_t               fSampleMagField;
   Bool_t               fWallLosses;
   
   Int_t                fAbsorbedCounter; // Absorbed includes any loss to the boundary
   Int_t                fDetectedCounter;
   Int_t                fDecayedCounter;
   Int_t                fLostCounter;
   
   TObjArray*           GetParticles()    {return fParticles;}
   TUCNData*            GetData()         {return fData;}
   
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
   
   Int_t                Neutrons() const              {return fNeutrons;}
   Double_t             TotalEnergy() const           {return fTotalEnergy;}
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
   
   void                 DrawParticles(TCanvas* canvas, TPolyMarker3D* points);
   
//   Bool_t               AddTrack(TVirtualGeoTrack* track);
   Bool_t               AddParticle(TUCNParticle* particle);
   Bool_t               SaveInitialParticle(TUCNParticle* particle);
   Bool_t               SaveParticle(TUCNParticle* particle);
//   TGeoTrack*           GetTrack(Int_t trackID);
   TUCNParticle*        GetInitialParticle(Int_t particleID);
   TUCNParticle*        GetParticle(Int_t particleID);
   
   Bool_t               Propagate(TGeoManager* geoManager, TUCNFieldManager* fieldManager);
   
   void                 PrintProgress(Int_t entry, Float_t nEntriesF, Int_t mintime=10);
   
   ClassDef(TUCNRun, 1)
};

#endif
