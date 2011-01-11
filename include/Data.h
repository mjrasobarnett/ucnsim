#ifndef DATA_H
#define DATA_H

#include "TNamed.h"
#include <map>
#include <string>
#include "TFile.h"
#include "TDirectory.h"
#include "Particle.h"
#include "Observer.h"
#include "InitialConfig.h"
#include "RunConfig.h"

class Data : public TNamed {
private:
   // -- Data Files
   TFile *fOutputFile;
   
   TDirectory *fInitialStatesFolder;
   TDirectory *fFinalStatesFolder;
   
   TIter *fCurrentParticleDir;
   
   // -- Observers
   std::multimap<std::string, Observer*> fObservers;
   
   void           PurgeObservers();
   void           InitialiseObservers(const RunConfig& runConfig);
   void           AddObserver(std::string subject, Observer* observer);
   void           RegisterObservers(Particle* particle);
   
   Bool_t         LoadParticles(const RunConfig& runConfig);
   Particle*      LocateParticle(TDirectory * const particleDir);
   
   void           CopyDirectory(TDirectory * const sourceDir, TDirectory * const outputDir);
   void           CopyDirectoryContents(TDirectory * const sourceDir, TDirectory * const outputDir);
   
public:
   // -- Constructors
   Data();
   Data(const Data& other);
   Data& operator=(const Data& other); 
   virtual ~Data(void);
   
   Bool_t               Initialise(const InitialConfig& initialConfig);
   Bool_t               Initialise(const RunConfig& runConfig);
   
   // Add a Particle
   Bool_t               SaveParticle(Particle* particle, const std::string& state);
   
   // Get a Particle
   Particle* const  RetrieveParticle();
   
   // Particle Counters
   Bool_t               ChecksOut() const;
   Int_t                InitialParticles() const;
   Int_t                PropagatingParticles() const;
   Int_t                DetectedParticles() const;
   Int_t                DecayedParticles() const;
   Int_t                AbsorbedParticles() const;
   Int_t                LostParticles() const;
   Int_t                AnomalousParticles() const;
   Int_t                FinalParticles() const;
   
   ClassDef(Data, 1) // UCN Data Object
};
#endif
