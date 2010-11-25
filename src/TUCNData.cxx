// UCNData.cpp
// Author: Simon JM Peeters
// Created July 2009

#include "TUCNData.h"

#include "TTree.h"
#include "TUCNParticle.h"

#include <iostream>
#include <cassert>

using namespace std;

ClassImp(TUCNData)

//_____________________________________________________________________________
TUCNData::TUCNData() 
			:TNamed()
{
	// -- Default Constructor
	Info("TUCNData","Default Constructor");
	// -- Create the Trees
	fInitialParticles = 0;
   fPropagatingParticles = 0;
   fDetectedParticles = 0;
   fDecayedParticles = 0;
   fAbsorbedParticles = 0;
   fLostParticles = 0;
   fBadParticles = 0;
}

//_____________________________________________________________________________
TUCNData::TUCNData(const string& name)
         :TNamed(name.c_str(), "")
{
   // -- Constructor
   Info("TUCNData","Constructor");
   // -- Create the Trees
   fInitialParticles = new TTree("Initial","Tree of Initial Particle States");
   fPropagatingParticles = new TTree("Propagating","Tree of Propagating Particles");
   fDetectedParticles = new TTree("Detected","Tree of Detected Particles");
   fDecayedParticles = new TTree("Decayed","Tree of Decayed Particles");
   fAbsorbedParticles = new TTree("Absorbed","Tree of Absorbed Particles");
   fLostParticles = new TTree("Lost","Tree of Lost Particles");
   fBadParticles = new TTree("Bad","Tree of Bad Particles"); 
}

//_____________________________________________________________________________
TUCNData::TUCNData(const TUCNData& other)
         :TNamed(other), 
          fInitialParticles(other.fInitialParticles),
          fPropagatingParticles(other.fPropagatingParticles),
          fDetectedParticles(other.fDetectedParticles),
          fDecayedParticles(other.fDecayedParticles),
          fAbsorbedParticles(other.fAbsorbedParticles),
          fLostParticles(other.fLostParticles),
          fBadParticles(other.fBadParticles),
          fObservers(other.fObservers)
{
   // Copy Constructor
   Info("TUCNData","Copy Constructor");
}

//_____________________________________________________________________________
TUCNData& TUCNData::operator=(const TUCNData& other)
{
// --assignment operator
   Info("TUCNData","Assignment");
   if(this!=&other) {
      TNamed::operator=(other);
      if (fInitialParticles) delete fInitialParticles; fInitialParticles = NULL;
      fInitialParticles = other.fInitialParticles;
      if (fPropagatingParticles) delete fPropagatingParticles; fPropagatingParticles = NULL;
      fPropagatingParticles = other.fPropagatingParticles;
      if (fDetectedParticles) delete fDetectedParticles; fDetectedParticles = NULL;
      fDetectedParticles = other.fDetectedParticles;
      if (fDecayedParticles) delete fDecayedParticles; fDecayedParticles = NULL;
      fDecayedParticles = other.fDecayedParticles;
      if (fAbsorbedParticles) delete fAbsorbedParticles; fAbsorbedParticles = NULL;
      fAbsorbedParticles = other.fAbsorbedParticles;
      if (fLostParticles) delete fLostParticles; fLostParticles = NULL;
      fLostParticles = other.fLostParticles;
      if (fBadParticles) delete fBadParticles; fBadParticles = NULL;
      fBadParticles = other.fBadParticles;
      // Clear list of observers before copying
      PurgeObservers();
      fObservers = other.fObservers;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNData::~TUCNData()
{
   // -- Destructor
   Info("TUCNData","Destructor");
   if(fInitialParticles) delete fInitialParticles;
   if(fPropagatingParticles) delete fPropagatingParticles;
   if(fDetectedParticles) delete fDetectedParticles;
   if(fDecayedParticles) delete fDecayedParticles;
   if(fAbsorbedParticles) delete fAbsorbedParticles;
   if(fLostParticles) delete fLostParticles;
   if(fBadParticles) delete fBadParticles;
   PurgeObservers();
}

//_____________________________________________________________________________
void TUCNData::PurgeObservers()
{
   // -- Delete all observers held
   if (fObservers.empty() == kFALSE) {
      vector<TUCNObserver*>::iterator it;
      for(it = fObservers.begin(); it != fObservers.end(); ++it) {
         delete *it;
         *it = 0;
      }
   }
}

//_____________________________________________________________________________
Bool_t TUCNData::ChecksOut()
{
// -- Perform a check out routine, to ensure data is consistent
   // Sum initial and final states
   const Int_t initialParticles = this->InitialParticles();
   const Int_t finalParticles = this->FinalParticles();
   // Check that initial and final states match
   if (initialParticles != finalParticles) {
      Error("ChecksOut", "Initial particle states: %i, is not equal to final states: %i", initialParticles, finalParticles);
      return kFALSE;
   } 
   Info("ChecksOut","Initial particle states: %i. Number of Final States: %i.", initialParticles, finalParticles);
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddParticleState(TString treeName, TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   TTree* tree = this->FetchTree(treeName);
   if (tree == NULL) {
      Error("AddParticleState","Failed to fetch tree with name %s",treeName.Data());
      return 0;
   }
   return this->AddParticleState(tree, particle);
}

//_____________________________________________________________________________
Bool_t TUCNData::AddParticleState(TTree* tree, TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (tree->GetBranch(tree->GetName()) == NULL) {
      tree->Branch(tree->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the InitialParticleState Tree
   tree->SetBranchAddress(tree->GetName(),&particle);
   tree->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddInitialParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fInitialParticles->GetBranch(fInitialParticles->GetName()) == NULL) {
      fInitialParticles->Branch(fInitialParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the InitialParticleState Tree
   fInitialParticles->SetBranchAddress(fInitialParticles->GetName(),&particle);
   fInitialParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddPropagatingParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fPropagatingParticles->GetBranch(fPropagatingParticles->GetName()) == NULL) {
      fPropagatingParticles->Branch(fPropagatingParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the PropagatingParticleState Tree
   fPropagatingParticles->SetBranchAddress(fPropagatingParticles->GetName(),&particle);
   fPropagatingParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddDetectedParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fDetectedParticles->GetBranch(fDetectedParticles->GetName()) == NULL) {
      fDetectedParticles->Branch(fDetectedParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the Detected ParticleState Tree
   fDetectedParticles->SetBranchAddress(fDetectedParticles->GetName(),&particle);
   fDetectedParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddDecayedParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fDecayedParticles->GetBranch(fDecayedParticles->GetName()) == NULL) {
      fDecayedParticles->Branch(fDecayedParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fDecayedParticles->SetBranchAddress(fDecayedParticles->GetName(),&particle);
   fDecayedParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddAbsorbedParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fAbsorbedParticles->GetBranch(fAbsorbedParticles->GetName()) == NULL) {
      fAbsorbedParticles->Branch(fAbsorbedParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fAbsorbedParticles->SetBranchAddress(fAbsorbedParticles->GetName(),&particle);
   fAbsorbedParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddLostParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fLostParticles->GetBranch(fLostParticles->GetName()) == NULL) {
      fLostParticles->Branch(fLostParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fLostParticles->SetBranchAddress(fLostParticles->GetName(),&particle);
   fLostParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddBadParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fBadParticles->GetBranch(fBadParticles->GetName()) == NULL) {
      fBadParticles->Branch(fBadParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fBadParticles->SetBranchAddress(fBadParticles->GetName(),&particle);
   fBadParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
TUCNParticle*  TUCNData::GetParticleState(TString treeName, Int_t index)
{
// -- Retrieve the particle from the TTree	
   TTree* tree = this->FetchTree(treeName);
   if (tree == NULL) {
      Error("GetParticleState","Failed to fetch tree with name %s",treeName.Data());
      return 0;
   }
   return this->GetParticleState(tree, index);
}

//_____________________________________________________________________________
TUCNParticle*  TUCNData::GetParticleState(TTree* tree, Int_t index)
{
// -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > tree->GetEntries() || index < 0) {
      Error("GetParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   tree->SetBranchAddress(tree->GetName(), &particle);
   tree->GetEntry(index);
   if (particle == NULL) {
      Error("GetParticleState","Failed to Get Particle: %i from Tree: %s",index,tree->GetName());
      return 0;
   }
   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetInitialParticleState(Int_t index)
{
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fInitialParticles->GetEntries() || index < 0) {
      Error("GetInitialParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fInitialParticles->SetBranchAddress(fInitialParticles->GetName(), &particle);
   fInitialParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetInitialParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }
   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetPropagatingParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fPropagatingParticles->GetEntries() || index < 0) {
      Error("GetPropagatingParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fPropagatingParticles->SetBranchAddress(fPropagatingParticles->GetName(), &particle);
   fPropagatingParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetPropagatingParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetDetectedParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fDetectedParticles->GetEntries() || index < 0) {
      Error("GetDetectedParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fDetectedParticles->SetBranchAddress(fDetectedParticles->GetName(), &particle);
   fDetectedParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetDetectedParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetDecayedParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fDecayedParticles->GetEntries() || index < 0) {
      Error("GetDecayedParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fDecayedParticles->SetBranchAddress(fDecayedParticles->GetName(), &particle);
   fDecayedParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetDecayedParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetAbsorbedParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fAbsorbedParticles->GetEntries() || index < 0) {
      Error("GetAbsorbedParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fAbsorbedParticles->SetBranchAddress(fAbsorbedParticles->GetName(), &particle);
   fAbsorbedParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetAbsorbedParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetLostParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fLostParticles->GetEntries() || index < 0) {
      Error("GetLostParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fLostParticles->SetBranchAddress(fLostParticles->GetName(), &particle);
   fLostParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetLostParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetBadParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fBadParticles->GetEntries() || index < 0) {
      Error("GetBadParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fBadParticles->SetBranchAddress(fBadParticles->GetName(), &particle);
   fBadParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetBadParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TTree* TUCNData::FetchTree(TString treeName) {
// -- Return pointer to a particular Tree based on the name given
// -- This could be converted to a switch statement if this is felt to be more efficient,
// -- however the number of calls to this function would suggest this is unimportant
   // Convert to Lower Case
   treeName.ToLower();
   // Determine which Tree is trying to be selected
   if (treeName == "initial" || treeName == "initialparticles" || treeName == "initialstates") {
      Info("FetchTree","Returning the InitialParticles Tree");
      return fInitialParticles;
   } else if (treeName == "propagating" || treeName == "propagatingparticles" || treeName == "propagatingstates") {
      Info("FetchTree","Returning the PropagatingParticles Tree");
      return fPropagatingParticles;
   } else if (treeName == "decayed" || treeName == "decayedparticles" || treeName == "decayedstates") {
      Info("FetchTree","Returning the DecayedParticles Tree");
      return fDecayedParticles;
   } else if (treeName == "detected" || treeName == "detectedparticles" || treeName == "detectedstates") {
      Info("FetchTree","Returning the DetectedParticles Tree");
      return fDetectedParticles;
   } else if (treeName == "absorbed" || treeName == "absorbedparticles" || treeName == "absorbedstates") {
      Info("FetchTree","Returning the AbsorbedParticles Tree");
      return fAbsorbedParticles;
   } else if (treeName == "lost" || treeName == "lostparticles" || treeName == "loststates") {
      Info("FetchTree","Returning the LostParticles Tree");
      return fLostParticles;
   } else if (treeName == "bad" || treeName == "badparticles" || treeName == "badstates") {
      Info("FetchTree","Returning the BadParticles Tree");
      return fBadParticles;
   } else {
      Error("FetchTree","Input TreeName: %s does not match any listed in Data",treeName.Data());
      return 0;
   }
}

//_____________________________________________________________________________
void TUCNData::AddObserver(TUCNObserver* observer)
{
   // -- Add observer to internal list
   fObservers.push_back(observer);
}

//_____________________________________________________________________________
void TUCNData::RegisterObservers(TUCNParticle* particle)
{
   // -- Register all observers in list with the particle
   vector<TUCNObserver*>::iterator obsIter;
   for (obsIter = fObservers.begin(); obsIter != fObservers.end(); obsIter++) {
      (*obsIter)->RegisterInterest(*particle);
   }
}

//_____________________________________________________________________________
void TUCNData::PlotObservers(TTree* tree)
{
   // Pass Tree of particles to each observer so that they can make a plot of their results
   vector<TUCNObserver*>::iterator obsIter;
   for (obsIter = fObservers.begin(); obsIter != fObservers.end(); obsIter++) {
      (*obsIter)->Plot(this, tree);
   }
}



ClassImp(TUCNDataTest)

//_____________________________________________________________________________
TUCNDataTest::TUCNDataTest()
             :TObject()
{
   // -- Default Constructor
   Info("TUCNDataTest","Default Constructor");
}

//_____________________________________________________________________________
TUCNDataTest::~TUCNDataTest(void)
{
   // -- Destructor
   Info("TUCNDataTest","Destructor");
}


ClassImp(TUCNDataBranch)

//_____________________________________________________________________________
TUCNDataBranch::TUCNDataBranch() 
			      :std::vector<TUCNParticle*>()
{
   // -- Default Constructor
   Info("TUCNDataBranch","Default Constructor");
}

//_____________________________________________________________________________
TUCNDataBranch::TUCNDataBranch(const string& /*name*/)
               :std::vector<TUCNParticle*>()
{
   // -- Constructor
   Info("TUCNDataBranch","Constructor");
}

//_____________________________________________________________________________
TUCNDataBranch::TUCNDataBranch(const TUCNDataBranch& other)
               :std::vector<TUCNParticle*>(other)
{
   // Copy Constructor
   Info("TUCNDataBranch","Copy Constructor");
}

//_____________________________________________________________________________
TUCNDataBranch& TUCNDataBranch::operator=(const TUCNDataBranch& other)
{
// --assignment operator
   Info("TUCNDataBranch","Assignment");
   if(this!=&other) {
      std::vector<TUCNParticle*>::operator=(other);
   }
   return *this;
}

//_____________________________________________________________________________
TUCNDataBranch::~TUCNDataBranch(void)
{
   // -- Destructor
   Info("TUCNDataBranch","Destructor");
}
