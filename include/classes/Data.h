#ifndef DATA_H
#define DATA_H

#include "TNamed.h"
#include <map>
#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "Observer.h"
#include "ParticleManifest.h"

class TGeoManager;
class Experiment;
class RunConfig;
class Particle;

class Data : public TNamed {
private:
   // -- Data Files
   TFile *fOutputFile;
   TTree *fOutputTree;
   Particle* fCurrentParticle;
   ParticleManifest* fOutputManifest;
   
   // -- Observers
   std::multimap<std::string, Observer*> fObservers;
   
   void           PurgeObservers();
   void           AddObserver(std::string subject, Observer* observer);
   void           RegisterObservers(Particle* particle);
   
   Bool_t         LoadParticles(const RunConfig& runConfig);
      
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
   void                 CreateObservers(const RunConfig& runConfig, const Experiment& experiment);
   
   // Add a Particle
   Bool_t               SaveParticle(Particle* particle, const std::string& state);
   
   // Get a Particle
   Particle* const      RetrieveParticle(unsigned int index);
   
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
   
   // Geometry
   void                 SaveGeometry(TGeoManager* const geoManager);
   
   void                 ExportData();
   
   ClassDef(Data, 1) // UCN Data Object
};
#endif
