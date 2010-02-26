// UCNData.cpp
// Author: Simon JM Peeters
// Created July 2009

#include "TUCNData.h"
#include "TUCNGeoManager.h"

#include "TFile.h"
#include "TGeoTrack.h"
#include "TSystem.h"

#include <iostream>
#include <string>
#include <vector>


using namespace std;

ClassImp(TUCNData)

TUCNData::TUCNData(void) : TNamed("ucnData","Default data"), fTracks("Neutrons","Neutron tracks")
{
	// -- Default Constructor
	Info("TUCNData","Default Constructor");
	fCurrentTrack    = new TGeoTrack();
	fCurrentParticle = new TUCNParticle();
	fTracks.Branch("Tracks","TGeoTrack",&fCurrentTrack);
	fTracks.Branch("Particles","TUCNParticle",&fCurrentParticle);
}


TUCNData::TUCNData(const char * name, const char * title) : TNamed(name,title), fTracks("Neutrons","Neutron tracks")
{
	// -- Constructor
	Info("TUCNData","Constructor");
	fCurrentTrack    = new TGeoTrack();
	fCurrentParticle = new TUCNParticle();
	fTracks.Branch("Tracks","TGeoTrack",&fCurrentTrack);
	fTracks.Branch("Particles","TUCNParticle",&fCurrentParticle);
}

TUCNData::~TUCNData(void)
{
	// -- Destructor
	Info("TUCNData","Destructor");
	if(fCurrentTrack) delete fCurrentTrack;
	if(fCurrentParticle) delete fCurrentParticle;
}

Bool_t TUCNData::LoadGeometry(TString filename)
{	
	TString ucngeom = gSystem->Getenv("UCN_GEOM");
	if ( ucngeom.Length() == 0 ) {
		cerr << "Could not find environment variable UCN_GEOM" << endl;
		return 1;
	}
	TString fullfilename = ucngeom + "/" + filename;
	Info("TUCNData","Reading geometry from %s",fullfilename.Data());
	TFile * file = new TFile(fullfilename,"READ");
	if ( !file->IsOpen() ) {
		Info("TUCNData","Could not open file %s\nExiting...",fullfilename.Data());
		return kFALSE;
	}
	TUCNGeoManager * myManager = (TUCNGeoManager*)file->Get("UCNGeom");
	if ( myManager == NULL ) {
		Info("TUCNData","Could not find UCNGeom in file %s\nExiting...",fullfilename.Data());
		return kFALSE;
	}
	myManager->GetTopVolume()->Draw();
	myManager->SetVisLevel(4);
	myManager->SetVisOption(0);
	return kTRUE;

/*	
	TFile * file = new TFile(filename,"READ");
	if ( (TUCNGeoManager*)file->Get("UCNGeom") == NULL )  {
		return kFALSE;
	} 
	else {
		return kTRUE;
		}
*/
}

void TUCNData::AddTrack(TVirtualGeoTrack* track)
{
	// Add the current track from the geomanager
//	cerr << "Adding track: " << track->GetId() << " to tree" << endl;
	TGeoTrack    * tmptr = fCurrentTrack;
	TUCNParticle * tmppa = fCurrentParticle;
	fCurrentTrack    = static_cast<TGeoTrack*>(track);
	fCurrentParticle = static_cast<TUCNParticle*>(fCurrentTrack->GetParticle());
	fTracks.Fill();
	fCurrentTrack = tmptr;
	fCurrentParticle = tmppa;
}

void TUCNData::AddParticle(TUCNParticle* particle)
{
	// Add the current track from the geomanager
//	cerr << "Adding track: " << track->GetId() << " to tree" << endl;
	TUCNParticle * tmppa = fCurrentParticle;
	fCurrentParticle = particle;
	fTracks.Fill();
	fCurrentParticle = tmppa;
}