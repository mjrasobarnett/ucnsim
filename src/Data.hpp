#ifndef DATA_H
#define DATA_H

#include "TNamed.h"
#include <map>
#include <string>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TDirectory.h"
#include "Observer.hpp"
#include "ParticleManifest.hpp"

class TGeoManager;
class Experiment;
class RunConfig;
class Particle;

class Data : public TNamed {
private:
   // -- Data Files
   TFile *fInputFile;
   TFile *fOutputFile;
   TTree *fInputTree;
   TTree *fOutputTree;
   TBranch* fInputBranch;
   Particle* fCurrentParticle;
   ParticleManifest* fOutputManifest;
   
   // -- Observers
   typedef std::multimap<std::string, Observer*> ObserverList;
   typedef std::map<std::string, ObserverList> ObserverCategories;
   ObserverCategories fObservers;
   
   void           PurgeObservers();
   void           AddObserver(const std::string category, const std::string subject, Observer* observer);
   void           AttachObservers(ObserverList& observerList, Particle* particle);
      
         
   ParticleManifest* ReadInParticleManifest(TFile* file) const;
   TTree*            ReadInParticleTree(TFile* file) const;
   
   bool  CheckSelectedIndexList(std::vector<int>& selectedIndexes, std::vector<int>& availableIndexes) const;
   bool  CopyAllParticles(TBranch* inputBranch, TBranch* outputBranch);
   bool  CopySelectedParticles(const std::vector<int>& selected_IDs, TBranch* inputBranch, TBranch* outputBranch);
   
public:
   // -- Constructors
   Data();
   Data(const Data& other);
   virtual ~Data(void);
   
   Bool_t               Initialise(const RunConfig& runConfig);
   void                 CreateObservers(const RunConfig& runConfig, Experiment& experiment);
   std::vector<int>     GetSelectedParticleIndexes(const ParticleManifest& manifest, const RunConfig& runConfig) const;
   
   // Add a Particle
   Bool_t               SaveInitialParticle(Particle* particle);
   Bool_t               SaveFinalParticle(Particle* particle, const std::string& state);
   
   // Get a Particle
   std::vector<int>     GetListOfParticlesToLoad(const RunConfig& runConfig);
   Particle* const      RetrieveParticle(unsigned int index);
   
   // Observers
   void                 RegisterObservers(Particle* particle);
   void                 ResetObservers();
   
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
   
   // Saving
   void                 Export();
   int                  WriteObjectToFile(TObject* object);
   int                  WriteObjectToTree(TObject* object, const char* branchName);
   
   ClassDef(Data, 1) // UCN Data Object
};
#endif
