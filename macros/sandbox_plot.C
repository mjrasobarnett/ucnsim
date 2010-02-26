#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <stdio.h> // sprintf

#include "TCanvas.h"
#include "TBenchmark.h"
#include "TSystem.h"
#include "TRint.h"
#include "TRandom.h"
#include "TObjArray.h"
#include "TFile.h"
#include "TKey.h"

#include "TF1.h"
#include "TH1.h"
#include "TStyle.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraphErrors.h"

#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoTrack.h"
#include "TParticle.h"
#include "TParticlePDG.h"
#include "TGeoMatrix.h"
#include "TPolyMarker3D.h"
#include "TVectorD.h"

#include "include/Constants.h"
#include "include/Units.h"
#include "include/FitSuite.h"

using std::vector;

Bool_t DrawInitialAndFinalPositions(const char* fileName, TGeoManager* geoManager);
Bool_t DrawInitialAndFinalDirections(const char* fileName, TGeoManager* geoManager); 
Bool_t DrawBadTrack(const char* badTrackFileName, TGeoManager* geoManager); 

Int_t sandbox_plot(const char* geomFileName, const char* dataFileName, const char* badTrackFileName = 0) {
	
	// -- Import Geometry
	TGeoManager* geoManager = TGeoManager::Import(geomFileName); 
	// Needs to be imported first because we draw the volumes in certain histograms from it, so we do not want it
	// deleted in each function. 
	
	DrawInitialAndFinalDirections(dataFileName, geoManager);
	DrawInitialAndFinalPositions(dataFileName, geoManager);
	
	if (badTrackFileName) {
		DrawBadTrack(badTrackFileName, geoManager);
	}
	
	return 0;
}

// -------------------------------------------------------------------------------------- 
Bool_t DrawInitialAndFinalPositions(const char* fileName, TGeoManager* geoManager) 
{
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(fileName, "read");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << fileName << endl;
	   return 0;
	}
	// -- Extract Initial and Final Positions in TPolyMarker3D
	const char* initialFileName = "NeutronInitialPositions;1";
	const char* finalFileName = "NeutronFinalPositions;1";
	TPolyMarker3D* initialPositions = new TPolyMarker3D();
	TPolyMarker3D* finalPositions = new TPolyMarker3D();
	file->GetObject(initialFileName, initialPositions);
	if (initialPositions == NULL) {
		cerr << "Could not find TPolyMarker3D: " << initialFileName << endl;
		return kFALSE;
	}
	file->GetObject(finalFileName, finalPositions);
	if (finalPositions == NULL) {
		cerr << "Could not find TPolyMarker3D: " << finalFileName << endl;
		return kFALSE;
	}
	
	// -- Draw Initial Points
	TCanvas *canvas1 = new TCanvas("InitialPositionsCanvas","Neutron Initial Positions",60,0,400,400);
	canvas1->cd();
	geoManager->GetTopVolume()->Draw();
	geoManager->SetVisLevel(4);
	geoManager->SetVisOption(0);
	initialPositions->Draw();
	// -- Draw Final Points
	TCanvas *canvas2 = new TCanvas("FinalPositionsCanvas","Neutron Final Positions",460,0,400,400);
	canvas2->cd();
	geoManager->GetTopVolume()->Draw();
	geoManager->SetVisLevel(4);
	geoManager->SetVisOption(0);
	finalPositions->Draw();
	
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t DrawInitialAndFinalDirections(const char* fileName, TGeoManager* geoManager) 
{
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(fileName, "read");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << fileName << endl;
	   return 0;
	}
	// -- Extract Histogram Objects
	const char* initialThetaHistName = "InitialThetaHist;1";
	const char* initialPhiHistName = "InitialPhiHist;1";
	const char* finalThetaHistName = "FinalThetaHist;1";
	const char* finalPhiHistName = "FinalPhiHist;1";
	TH1F* initialThetaHist = new TH1F();
	TH1F* initialPhiHist = new TH1F();
	TH1F* finalThetaHist = new TH1F();
	TH1F* finalPhiHist = new TH1F();
   file->GetObject(initialThetaHistName, initialThetaHist);
	file->GetObject(initialPhiHistName, initialPhiHist);
   file->GetObject(finalThetaHistName, finalThetaHist);
   file->GetObject(finalPhiHistName, finalPhiHist);
	if (initialThetaHist == NULL || initialPhiHist == NULL || finalThetaHist == NULL || finalPhiHist == NULL) {
		cerr << "Could not find required histogram" << endl;
		return kFALSE;
	}
	// -- Draw Histograms
	TCanvas *canvas = new TCanvas("InitialAndFinalDirections","Neutron Initial and Final Directions",60,0,800,800);
	canvas->Divide(2,2);
	canvas->cd(1);
	initialThetaHist->Draw();
	canvas->cd(2);
	initialPhiHist->Draw();
	canvas->cd(3);
	finalThetaHist->Draw();
	canvas->cd(4);
	finalPhiHist->Draw();
	
	return kTRUE;
}


// -------------------------------------------------------------------------------------- 
Bool_t DrawBadTrack(const char* badTrackFileName, TGeoManager* geoManager) 
{
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(badTrackFileName, "read");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << fileName << endl;
	   return 0;
	}
	// -- Extract Initial and Final Positions in TPolyMarker3D
	const char* badTrackName = "BadTrackPoints;1";
	TPolyMarker3D* trackPoints = new TPolyMarker3D();
	file->GetObject(badTrackName, trackPoints);
	if (trackPoints == NULL) {
		cerr << "Could not find TPolyMarker3D: " << badTrackName << endl;
		return kFALSE;
	}
	
	const char* badTrackLineName = "TPolyLine3D;1";
	TPolyLine3D* trackLine = new TPolyLine3D();
	file->GetObject(badTrackLineName, trackLine);
	if (trackLine == NULL) {
		cerr << "Could not find TPolyMarker3D: " << badTrackLineName << endl;
		return kFALSE;
	}
	
	// -- Draw Track
	TCanvas *canvas = new TCanvas("BadTrackCanvas","Bad Track Points",60,0,400,400);
	canvas->cd();
	geoManager->GetTopVolume()->Draw();
	geoManager->SetVisLevel(4);
	geoManager->SetVisOption(0);
	trackPoints->Draw();
	trackLine->Draw();
	
//	cout << "Track Points: " << endl;
//	for (Int_t i = 0; i < trackPoints->GetN(); i++) {
//		Double_t point[3] = {0.,0.,0.};
//		trackPoints->GetPoint(i,point[0],point[1],point[2]);
//		cout << i << "\t" << "X: " << point[0] << "\t" << "Y: " << point[1] << "\t" << "Z: " << point[2] << endl;
//	}
	
	return kTRUE;
}


