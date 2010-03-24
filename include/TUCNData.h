#ifndef TUCNDATA_H
#define TUCNDATA_H

#include "TNamed.h"
#include "TTree.h"
#include "TGeoTrack.h"

class TUCNParticle;

class TUCNData : public TNamed {
protected:
   TTree             *fInitialParticleStates;
   TTree             *fFinalParticleStates;
   
private:   
   TTree*            GetInitialParticlesTree()     {return fInitialParticleStates;}
   TTree*            GetFinalParticlesTree()       {return fFinalParticleStates;}
   
public:
   TUCNData(void); 
   TUCNData(const char *name, const char *title);
   TUCNData(const TUCNData&);
   TUCNData& operator=(const TUCNData&);
   virtual ~TUCNData(void);
   
   Bool_t               ChecksOut();
   
   // Storage of Tracks and Particles
   Bool_t               AddInitialParticleState(TUCNParticle* particle);
   Bool_t               AddFinalParticleState(TUCNParticle* particle);
   
   TUCNParticle*        GetInitialParticleState(Int_t particleID);
   TUCNParticle*        GetFinalParticleState(Int_t particleID);
   
   Int_t                InitialParticles() {return fInitialParticleStates->GetEntriesFast();}
   
   ClassDef(TUCNData, 1) // UCN Data Object
};
#endif
