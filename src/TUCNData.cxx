// UCNData.cpp
// Author: Simon JM Peeters
// Created July 2009

#include "TUCNData.h"

#include "TTree.h"
#include "TGeoTrack.h"
#include "TUCNParticle.h"

#include <cassert>

ClassImp(TUCNData)

//_____________________________________________________________________________
TUCNData::TUCNData() 
			:TNamed()
{
	// -- Default Constructor
	Info("TUCNData","Default Constructor");
	// -- Create the Trees
	fTracks = 0;
	fInitialParticleStates = 0;
	fFinalParticleStates = 0;
	// -- Create storage for filling the trees
	fCurrentTrack    = 0;
	fInitialParticle = 0;
	fCurrentParticle = 0;
}

//_____________________________________________________________________________
TUCNData::TUCNData(const char * name, const char * title) 
			: TNamed(name,title), 
			  fTracks()
{
	// -- Constructor
	Info("TUCNData","Constructor");
	// -- Create the Trees
	fTracks = new TTree("ParticleTracks","Tree of Particle Tracks");
	fInitialParticleStates = new TTree("InitialParticleStates","Tree of Initial Particles");
	fFinalParticleStates = new TTree("FinalParticleStates","Tree of Final Particles");
	// -- Create storage for filling the trees
	fCurrentTrack    = new TGeoTrack();
	fInitialParticle = new TUCNParticle();
	fCurrentParticle = new TUCNParticle();
	// -- Create the Branches in the Trees where we will store the objects
	fTracks->Branch("Tracks","TGeoTrack",&fCurrentTrack);
	fInitialParticleStates->Branch("InitialParticles","TUCNParticle",&fInitialParticle);
	fFinalParticleStates->Branch("FinalParticles","TUCNParticle",&fCurrentParticle);
}

//_____________________________________________________________________________
TUCNData::~TUCNData(void)
{
	// -- Destructor
	Info("TUCNData","Destructor");
	if(fTracks) delete fTracks;
	if(fInitialParticleStates) delete fInitialParticleStates;
	if(fFinalParticleStates) delete fFinalParticleStates;
	if(fCurrentTrack) delete fCurrentTrack;
	if(fInitialParticle) delete fInitialParticle;
	if(fCurrentParticle) delete fCurrentParticle;
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
Bool_t TUCNData::AddTrack(TGeoTrack* track)
{
	// Add the Track to the the Track Tree
	TGeoTrack *tmptr = fCurrentTrack;
	fCurrentTrack = track;
	this->GetTracksTree()->Fill();
	fCurrentTrack = tmptr;
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddInitialParticleState(TUCNParticle* particle)
{
	// Add the particle to the InitialParticleState Tree
	TUCNParticle *tmppa = fInitialParticle;
	fInitialParticle = particle;
	this->GetInitialParticlesTree()->Fill();
	fInitialParticle = tmppa;
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddFinalParticleState(TUCNParticle* particle)
{
	// Add the particle to the FinalParticleState Tree
	TUCNParticle *tmppa = fCurrentParticle;
	fCurrentParticle = particle;
	this->GetFinalParticlesTree()->Fill();
	fCurrentParticle = tmppa;
	return kTRUE;
}

//_____________________________________________________________________________
TGeoTrack* TUCNData::GetTrack(Int_t trackID) {
	// -- Retrieve the track from the TTree
	TGeoTrack* track = 0;
	if (trackID > this->GetTracksTree()->GetEntries() || trackID < 0) {
		Error("GetTrack","No Entry exists for Track: %i",trackID);
		return 0;
	}
	this->GetTracksTree()->SetBranchAddress("Tracks", &track);
	this->GetTracksTree()->GetEntry(trackID);
	assert(track != NULL);
	return track;
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
