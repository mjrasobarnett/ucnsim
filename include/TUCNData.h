#ifndef TUCNDATA_H
#define TUCNDATA_H

#include "TNamed.h"
#include <vector>
#include <string>
#include "TFile.h"
#include "TDirectory.h"
#include "TIter.h"
#include "TUCNParticle.h"
#include "TUCNObserver.h"
#include "TUCNInitialConfig.h"
#include "TUCNRunConfig.h"

class TUCNData : public TNamed {
private:
   // -- Data Files
   TFile *fInputFile;
   TFile *fOutputFile;
   
   TDirectory *fInitialStatesFolder;
   TDirectory *fFinalStatesFolder;
   TIter      *fNextKey;
   
   // -- Observers
   std::vector<TUCNObserver*> fObservers;
   void           PurgeObservers();
   
   void           AddObserver(TUCNObserver* observer);
   void           RegisterObservers(TUCNParticle* particle);
   void           PlotObservers(TTree* tree);
   
public:
   // -- Constructors
   TUCNData();
   TUCNData(const TUCNInitialConfig& initialConfig);
   TUCNData(const TUCNRunConfig& runConfig);
   TUCNData(const TUCNData& other);
   TUCNData& operator=(const TUCNData& other); 
   virtual ~TUCNData(void);
   
   // Add a Particle
   Bool_t               SaveParticle(const string& state, TUCNParticle* particle);
   
   // Get a Particle
   TUCNParticle* const  RetrieveParticle();
   
   // Particle Counters
   Bool_t         ChecksOut() const;
   Int_t          InitialParticles() const;
   Int_t          PropagatingParticles() const;
   Int_t          DetectedParticles() const;
   Int_t          DecayedParticles() const;
   Int_t          AbsorbedParticles() const;
   Int_t          LostParticles() const;
   Int_t          BadParticles() const;
   Int_t          FinalParticles() const;
   
   ClassDef(TUCNData, 1) // UCN Data Object
};
#endif
