#ifndef TUCNDATA_H
#define TUCNDATA_H

#include "TNamed.h"
#include <vector>
#include <string>
#include "TFile.h"
#include "TDirectory.h"
#include "TUCNParticle.h"
#include "TUCNObserver.h"
#include "TUCNInitialConfig.h"
#include "TUCNRunConfig.h"

class TUCNData : public TNamed {
private:
   // -- Data Files
   TFile *fOutputFile;
   
   TDirectory *fInitialStatesFolder;
   TDirectory *fFinalStatesFolder;
   
   // -- Observers
   std::vector<TUCNObserver*> fObservers;
   void           PurgeObservers();
   
   void           AddObserver(TUCNObserver* observer);
   void           RegisterObservers(TUCNParticle* particle);
   void           PlotObservers(TTree* tree);
   
   Bool_t         LoadParticles(const TUCNRunConfig& runConfig);
   void           CopyDirectory(TDirectory * const sourceDir, TDirectory * const outputDir);
   void           CopyDirectoryContents(TDirectory * const sourceDir, TDirectory * const outputDir);
   
public:
   // -- Constructors
   TUCNData();
   TUCNData(const TUCNData& other);
   TUCNData& operator=(const TUCNData& other); 
   virtual ~TUCNData(void);
   
   Bool_t               Initialise(const TUCNInitialConfig& initialConfig);
   Bool_t               Initialise(const TUCNRunConfig& runConfig);
   
   // Add a Particle
   Bool_t               SaveParticle(TUCNParticle* particle, const std::string& state);
   
   // Get a Particle
   TUCNParticle* const  RetrieveParticle();
   
   // Particle Counters
   Bool_t               ChecksOut() const;
   Int_t                InitialParticles() const;
   Int_t                PropagatingParticles() const;
   Int_t                DetectedParticles() const;
   Int_t                DecayedParticles() const;
   Int_t                AbsorbedParticles() const;
   Int_t                LostParticles() const;
   Int_t                BadParticles() const;
   Int_t                FinalParticles() const;
   
   ClassDef(TUCNData, 1) // UCN Data Object
};
#endif
