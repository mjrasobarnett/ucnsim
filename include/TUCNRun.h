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
class TGeoVolume;
class TGeoMatrix;
class TPolyMarker3D;
class TFile;
class TGeoTrack;
class TGeoNode;
class TGeoVolume;
class TGeoMatrix;
class TGeoHMatrix;
class TCanvas;
class TVirtualGeoTrack;

class TUCNConfigFile;
class TUCNFieldManager;
class TUCNData;
class TUCNParticle;
class TUCNGravField;
class TUCNMagField;
class TUCNGeoManager;
class TUCNGeoMaterial;
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
   
   Int_t                fBoundaryLossCounter;
   Int_t                fDetectedCounter;
   Int_t                fDecayedCounter;
   
   
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
   Int_t                LostToBoundary() const        {return fBoundaryLossCounter;}
   Int_t                Decayed() const               {return fDecayedCounter;}
   
   void                 IncrementDetected()           {fDetectedCounter++;}
   void                 IncrementLostToBoundary()     {fBoundaryLossCounter++;}
   void                 IncrementDecayed()            {fDecayedCounter++;}
   
   void                 DrawParticles(TCanvas* canvas, TPolyMarker3D* points);
   void                 DrawTrack(TCanvas* canvas, Int_t trackID);
   
   Bool_t               AddTrack(TVirtualGeoTrack* track);
   Bool_t               AddParticle(TUCNParticle* particle);
   Bool_t               SaveInitialParticle(TUCNParticle* particle);
   Bool_t               SaveParticle(TUCNParticle* particle);
   TGeoTrack*           GetTrack(Int_t trackID);
   TUCNParticle*        GetInitialParticle(Int_t particleID);
   TUCNParticle*        GetParticle(Int_t particleID);
   
   Bool_t               Propagate(TGeoManager* geoManager, TUCNFieldManager* fieldManager);
   
   void                 PrintProgress(Int_t entry, Float_t nEntriesF, Int_t mintime=10);
   
   ClassDef(TUCNRun, 1)
};

#endif
