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
   fSurvivedParticles = 0;
   fDetectedParticles = 0;
   fDecayedParticles = 0;
   fAbsorbedParticles = 0;
   fLostParticles = 0;
   fBadParticles = 0;
}

//_____________________________________________________________________________
TUCNData::TUCNData(const char * name, const char * title)
         :TNamed(name,title)
{
   // -- Constructor
   Info("TUCNData","Constructor");
   // -- Create the Trees
   fInitialParticles = new TTree("InitialParticles","Tree of Initial Particle States");
   fSurvivedParticles = new TTree("SurvivedParticles","Tree of Survived Particles");
   fDetectedParticles = new TTree("DetectedParticles","Tree of Detected Particles");
   fDecayedParticles = new TTree("DecayedParticles","Tree of Decayed Particles");
   fAbsorbedParticles = new TTree("AbsorbedParticles","Tree of Absorbed Particles");
   fLostParticles = new TTree("LostParticles","Tree of Lost Particles");
   fBadParticles = new TTree("BadParticles","Tree of Bad Particles"); 
}

//_____________________________________________________________________________
TUCNData::TUCNData(const TUCNData& d)
         :TNamed(d), 
          fInitialParticles(d.fInitialParticles),
          fSurvivedParticles(d.fSurvivedParticles),
          fDetectedParticles(d.fDetectedParticles),
          fDecayedParticles(d.fDecayedParticles),
          fAbsorbedParticles(d.fAbsorbedParticles),
          fLostParticles(d.fLostParticles),
          fBadParticles(d.fBadParticles)
{
   // Copy Constructor
   Info("TUCNData","Copy Constructor");
}

//_____________________________________________________________________________
TUCNData& TUCNData::operator=(const TUCNData& d)
{
// --assignment operator
   Info("TUCNData","Assignment");
   if(this!=&d) {
      TNamed::operator=(d);
      fInitialParticleStates = d.fInitialParticleStates;
      fSurvivedParticleStates = d.fSurvivedParticleStates;
      fDetectedParticles = d.fDetectedParticles;
      fDecayedParticles = d.fDecayedParticles;
      fAbsorbedParticles = d.fAbsorbedParticles;
      fLostParticles = d.fLostParticles;
      fBadParticles = d.fBadParticles;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNData::~TUCNData(void)
{
   // -- Destructor
   Info("TUCNData","Destructor");
   if(fInitialParticleStates) delete fInitialParticleStates;
   if(fSurvivedParticleStates) delete fSurvivedParticleStates;
   if(fDetectedParticles) delete fDetectedParticles;
   if(fDecayedParticles) delete fDecayedParticles;
   if(fAbsorbedParticles) delete fAbsorbedParticles;
   if(fLostParticles) delete fLostParticles;
   if(fBadParticles) delete fBadParticles;
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
Bool_t TUCNData::AddInitialParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fInitialParticles->GetBranch("InitialParticles") == NULL) {
      fInitialParticles->Branch("InitialParticles","TUCNParticle",&particle);
   }
   // Add the particle to the InitialParticleState Tree
   fInitialParticles->SetBranchAddress("InitialParticles",&particle);
   fInitialParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddSurvivedParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fSurvivedParticles->GetBranch("SurvivedParticles") == NULL) {
      fSurvivedParticles->Branch("SurvivedParticles","TUCNParticle",&particle);
   }
   // Add the particle to the SurvivedParticleState Tree
   fSurvivedParticles->SetBranchAddress("SurvivedParticles",&particle);
   fSurvivedParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddDetectedParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fDetectedParticles->GetBranch("DetectedParticles") == NULL) {
      fDetectedParticles->Branch("DetectedParticles","TUCNParticle",&particle);
   }
   // Add the particle to the SurvivedParticleState Tree
   fDetectedParticles->SetBranchAddress("DetectedParticles",&particle);
   fDetectedParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddDecayedParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fDecayedParticles->GetBranch("DecayedParticles") == NULL) {
      fDecayedParticles->Branch("DecayedParticles","TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fDecayedParticles->SetBranchAddress("DecayedParticles",&particle);
   fDecayedParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddAbsorbedParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fAbsorbedParticles->GetBranch("AbsorbedParticles") == NULL) {
      fAbsorbedParticles->Branch("AbsorbedParticles","TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fAbsorbedParticles->SetBranchAddress("AbsorbedParticles",&particle);
   fAbsorbedParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddLostParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fLostParticles->GetBranch("LostParticles") == NULL) {
      fLostParticles->Branch("LostParticles","TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fLostParticles->SetBranchAddress("LostParticles",&particle);
   fLostParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddBadParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fBadParticles->GetBranch("BadParticles") == NULL) {
      fBadParticles->Branch("BadParticles","TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fBadParticles->SetBranchAddress("BadParticles",&particle);
   fBadParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetInitialParticleState(Int_t particleID) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (particleID > fInitialParticles->GetEntries() || particleID < 0) {
      Error("GetInitialParticleState","No Entry exists for Particle: %i", particleID);
      return 0;
   }
   fInitialParticles->SetBranchAddress("InitialParticles", &particle);
   fInitialParticles->GetEntry(particleID);
   if (particle == NULL) {
      Error("GetInitialParticleState","Failed to Get Particle: %i from Tree",particleID);
      return 0;
   }
   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetSurvivedParticleState(Int_t particleID) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (particleID > fSurvivedParticles->GetEntries() || particleID < 0) {
      Error("GetSurvivedParticleState","No Entry exists for Particle: %i", particleID);
      return 0;
   }
   fSurvivedParticles->SetBranchAddress("SurvivedParticles", &particle);
   fSurvivedParticles->GetEntry(particleID);
   if (particle == NULL) {
      Error("GetSurvivedParticleState","Failed to Get Particle: %i from Tree",particleID);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetDetectedParticleState(Int_t particleID) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (particleID > fDetectedParticles->GetEntries() || particleID < 0) {
      Error("GetDetectedParticleState","No Entry exists for Particle: %i", particleID);
      return 0;
   }
   fDetectedParticles->SetBranchAddress("DetectedParticles", &particle);
   fDetectedParticles->GetEntry(particleID);
   if (particle == NULL) {
      Error("GetDetectedParticleState","Failed to Get Particle: %i from Tree",particleID);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetDecayedParticleState(Int_t particleID) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (particleID > fDecayedParticles->GetEntries() || particleID < 0) {
      Error("GetDecayedParticleState","No Entry exists for Particle: %i", particleID);
      return 0;
   }
   fDecayedParticles->SetBranchAddress("DecayedParticles", &particle);
   fDecayedParticles->GetEntry(particleID);
   if (particle == NULL) {
      Error("GetDecayedParticleState","Failed to Get Particle: %i from Tree",particleID);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetAbsorbedParticleState(Int_t particleID) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (particleID > fAbsorbedParticles->GetEntries() || particleID < 0) {
      Error("GetAbsorbedParticleState","No Entry exists for Particle: %i", particleID);
      return 0;
   }
   fAbsorbedParticles->SetBranchAddress("AbsorbedParticles", &particle);
   fAbsorbedParticles->GetEntry(particleID);
   if (particle == NULL) {
      Error("GetAbsorbedParticleState","Failed to Get Particle: %i from Tree",particleID);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetLostParticleState(Int_t particleID) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (particleID > fLostParticles->GetEntries() || particleID < 0) {
      Error("GetLostParticleState","No Entry exists for Particle: %i", particleID);
      return 0;
   }
   fLostParticles->SetBranchAddress("LostParticles", &particle);
   fLostParticles->GetEntry(particleID);
   if (particle == NULL) {
      Error("GetLostParticleState","Failed to Get Particle: %i from Tree",particleID);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetBadParticleState(Int_t particleID) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (particleID > fBadParticles->GetEntries() || particleID < 0) {
      Error("GetBadParticleState","No Entry exists for Particle: %i", particleID);
      return 0;
   }
   fBadParticles->SetBranchAddress("BadParticles", &particle);
   fBadParticles->GetEntry(particleID);
   if (particle == NULL) {
      Error("GetBadParticleState","Failed to Get Particle: %i from Tree",particleID);
      return 0;
   }   return particle;
}