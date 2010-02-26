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

#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"

#include "include/Constants.h"
#include "include/Units.h"
#include "include/FitSuite.h"

using std::vector;

Bool_t DrawInitialAndFinalPhaseSpace(const char* fileName); 
Bool_t DrawInitialAndFinalPositions(const char* fileName, TGeoManager* geoManager);
Bool_t DrawInitialAndFinalDirections(const char* fileName); 
Bool_t DrawTrack(const char* trackFileName, TGeoManager* geoManager); 

// -------------------------------------------------------------------------------------- 
Int_t draw_rundata(const char* geomFileName, const char* dataFileName, const char* badTrackFileName = 0) {
	
	// -- Import Geometry
	TGeoManager* geoManager = TGeoManager::Import(geomFileName); 
	// Needs to be imported first because we draw the volumes in certain histograms from it, so we do not want it
	// deleted in each function. 
	
	DrawInitialAndFinalDirections(dataFileName, geoManager);
   DrawInitialAndFinalPhaseSpace(dataFileName);
	
	DrawInitialAndFinalPositions(dataFileName, geoManager);
	if (badTrackFileName) {
		DrawTrack(badTrackFileName, geoManager);
	}
	
	return 0;
}

// -------------------------------------------------------------------------------------- 
Bool_t DrawInitialAndFinalPhaseSpace(const char* fileName) 
{
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(fileName, "read");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << fileName << endl;
	   return 0;
	}
	file->ls();
   
   // -- Extract Histogram Objects
   const char* initialEnergyHistName = "InitialEnergyHist;1";
   const char* initialPxHistName = "InitialPxHist;1";
   const char* initialPyHistName = "InitialPyHist;1";
   const char* initialPzHistName = "InitialPzHist;1";
   const char* initialTimeHistName = "InitialTimeHist;1";
   const char* initialDistHistName = "InitialDistHist;1";
   const char* finalEnergyHistName = "FinalEnergyHist;1";
   const char* finalPxHistName = "FinalPxHist;1";
   const char* finalPyHistName = "FinalPyHist;1";
   const char* finalPzHistName = "FinalPzHist;1";
   const char* finalTimeHistName = "FinalTimeHist;1";
   const char* finalDistHistName = "FinalDistHist;1";
   
   TH1F* initialEnergyHist = new TH1F();
   TH1F* initialPxHist = new TH1F();
   TH1F* initialPyHist = new TH1F();
   TH1F* initialPzHist = new TH1F();
   TH1F* initialTimeHist = new TH1F();
   TH1F* initialDistHist = new TH1F();
   TH1F* finalEnergyHist = new TH1F();
   TH1F* finalPxHist = new TH1F();
   TH1F* finalPyHist = new TH1F();
   TH1F* finalPzHist = new TH1F();
   TH1F* finalTimeHist = new TH1F();
   TH1F* finalDistHist = new TH1F();
   
   file->GetObject(initialEnergyHistName, initialEnergyHist);
   file->GetObject(initialPxHistName,initialPxHist);
   file->GetObject(initialPyHistName,initialPyHist);
   file->GetObject(initialPzHistName,initialPzHist);
   file->GetObject(initialTimeHistName,initialTimeHist);
   file->GetObject(initialDistHistName,initialDistHist);
   file->GetObject(finalEnergyHistName,finalEnergyHist);
   file->GetObject(finalPxHistName,finalPxHist);
   file->GetObject(finalPyHistName,finalPyHist);
   file->GetObject(finalPzHistName,finalPzHist);
   file->GetObject(finalTimeHistName,finalTimeHist);
   file->GetObject(finalDistHistName,finalDistHist);
   
   // -- Draw Histograms
	TCanvas *canvas1 = new TCanvas("InitialPhaseSpace","Initial Phase Space",60,0,1000,800);
	canvas1->Divide(3,2);
	canvas1->cd(1);
	initialEnergyHist->Draw();
	canvas1->cd(2);
	initialPxHist->Draw();
	canvas1->cd(3);
	initialPyHist->Draw();
	canvas1->cd(4);
	initialPzHist->Draw();
	canvas1->cd(5);
	initialTimeHist->Draw();
	canvas1->cd(6);
	initialDistHist->Draw();
	
	TCanvas *canvas2 = new TCanvas("FinalPhaseSpace","Final Phase Space",60,0,1000,800);
	canvas2->Divide(3,2);
	canvas2->cd(1);
	finalEnergyHist->Draw();
	canvas2->cd(2);
	finalPxHist->Draw();
	canvas2->cd(3);
	finalPyHist->Draw();
	canvas2->cd(4);
	finalPzHist->Draw();
	canvas2->cd(5);
	// -- Fit Emptying Time
	TF1* expo = new TF1();
   file->GetObject("Expo", expo);
   finalTimeHist->Fit("Expo","R");
	canvas2->cd(6);
	finalDistHist->Draw();
	
	return kTRUE;
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
   file->ls();
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
	file->ls();
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
Bool_t DrawTrack(const char* trackFileName, TGeoManager* geoManager) 
{
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(trackFileName, "read");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << trackFileName << endl;
	   return 0;
	}
	// -- Extract Initial and Final Positions in TPolyMarker3D
	const char* trackName = "TrackPoints;1";
	TPolyMarker3D* trackPoints = new TPolyMarker3D();
	file->GetObject(trackName, trackPoints);
	if (trackPoints == NULL) {
		cerr << "Could not find TPolyMarker3D: " << trackName << endl;
		return kFALSE;
	}
	
	const char* trackLineName = "TPolyLine3D;1";
	TPolyLine3D* trackLine = new TPolyLine3D();
	file->GetObject(trackLineName, trackLine);
	if (trackLine == NULL) {
		cerr << "Could not find TPolyMarker3D: " << trackLineName << endl;
		return kFALSE;
	}
	
	// -- Draw Track
	TCanvas *canvas = new TCanvas("TrackCanvas","Track Points",60,0,400,400);
	canvas->cd();
	geoManager->GetTopVolume()->Draw();
	geoManager->SetVisLevel(4);
	geoManager->SetVisOption(0);
	trackPoints->Draw();
	trackLine->Draw();
	
	cout << "Last 20 Track Points: " << endl;
	for (Int_t i = trackPoints->GetN() - 20; i < trackPoints->GetN(); i++) {
		Double_t point[3] = {0.,0.,0.};
		trackPoints->GetPoint(i,point[0],point[1],point[2]);
		cout << i << "\t" << "X: " << point[0] << "\t" << "Y: " << point[1] << "\t" << "Z: " << point[2] << endl;
	}
	
	return kTRUE;
}


