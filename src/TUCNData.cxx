// UCNData.cpp
// Author: Simon JM Peeters
// Created July 2009

#include "TUCNData.h"

#include "TTree.h"
#include "TGeoTrack.h"
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
	fInitialParticleStates = 0;
	fFinalParticleStates = 0;
}

//_____________________________________________________________________________
TUCNData::TUCNData(const char * name, const char * title) 
			: TNamed(name,title)
{
	// -- Constructor
	Info("TUCNData","Constructor");
	// -- Create the Trees
	fInitialParticleStates = new TTree("InitialParticleStates","Tree of Initial Particles");
   fFinalParticleStates = new TTree("FinalParticleStates","Tree of Final Particles");
}

//_____________________________________________________________________________
TUCNData::TUCNData(const TUCNData& d)
         :TNamed(d), fInitialParticleStates(d.fInitialParticleStates),
          fFinalParticleStates(d.fFinalParticleStates)
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
      fFinalParticleStates = d.fFinalParticleStates;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNData::~TUCNData(void)
{
	// -- Destructor
	Info("TUCNData","Destructor");
	if(fInitialParticleStates) delete fInitialParticleStates;
	if(fFinalParticleStates) delete fFinalParticleStates;
}

//_____________________________________________________________________________
Bool_t TUCNData::ChecksOut()
{
// -- Perform a check out routine, to ensure data is consistent
	Int_t initialParticles = this->GetInitialParticlesTree()->GetEntries();
	Int_t finalParticles = this->GetFinalParticlesTree()->GetEntries();
	if (initialParticles != finalParticles) {
		Error("ChecksOut", "Number of initial particle states: %i, does not match number of final states: %i",initialParticles,finalParticles);
		return kFALSE;
	} 
	Info("ChecksOut","Number of Initial States: %i. Number of Final States: %i.",initialParticles,finalParticles);
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddInitialParticleState(TUCNParticle* particle)
{
	// -- Create the Branches in the Trees where we will store the objects
   if (fInitialParticleStates->GetBranch("InitialParticles") == NULL) {
      fInitialParticleStates->Branch("InitialParticles","TUCNParticle",&particle);
   }
   // Add the particle to the InitialParticleState Tree
	this->GetInitialParticlesTree()->SetBranchAddress("InitialParticles",&particle);
	this->GetInitialParticlesTree()->Fill();
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddFinalParticleState(TUCNParticle* particle)
{
	// -- Create the Branches in the Trees where we will store the objects
   if (fFinalParticleStates->GetBranch("FinalParticles") == NULL) {
      fFinalParticleStates->Branch("FinalParticles","TUCNParticle",&particle);
   }
   // Add the particle to the FinalParticleState Tree
	this->GetFinalParticlesTree()->SetBranchAddress("FinalParticles",&particle);
	this->GetFinalParticlesTree()->Fill();
	return kTRUE;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetInitialParticleState(Int_t particleID) {
	// -- Retrieve the particle from the TTree	
	TUCNParticle* particle = 0;
	if (particleID > this->GetInitialParticlesTree()->GetEntries() || particleID < 0) {
		Error("GetInitialParticleState","No Entry exists for Particle: %i", particleID);
		return 0;
	}
	this->GetInitialParticlesTree()->SetBranchAddress("InitialParticles", &particle);
	this->GetInitialParticlesTree()->GetEntry(particleID);
	assert(particle != NULL);
	return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetFinalParticleState(Int_t particleID) {
	// -- Retrieve the particle from the TTree	
	TUCNParticle* particle = 0;
	if (particleID > this->GetFinalParticlesTree()->GetEntries() || particleID < 0) {
		Error("GetFinalParticleState","No Entry exists for Particle: %i", particleID);
		return 0;
	}
	this->GetFinalParticlesTree()->SetBranchAddress("FinalParticles", &particle);
	this->GetFinalParticlesTree()->GetEntry(particleID);
	assert(particle != NULL);
	return particle;
}
