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

Double_t gTotalEnergy = 0.0*Units::neV; // Global required by the densityf function. Value defined by DrawNeutronHeightDist...

Bool_t DrawInitialAndFinalPositions(const char* fileName, TGeoManager* geoManager);
Bool_t DrawInitialAndFinalDirections(const char* fileName, TGeoManager* geoManager); 
Bool_t DrawNeutronHeightDistribution(const char* fileName);
Bool_t DrawLossFunction(const char* fileName);
Bool_t DrawVolumeAveragedMagField(const char* fileName);

Double_t densityf(Double_t* x, Double_t* par);
Double_t lossFunc(Double_t* x, Double_t* par);
 

Int_t buildtest_plot(const char* fileName) {
	
	// -- Import Geometry
	TGeoManager* geoManager = TGeoManager::Import(fileName); 
	// Needs to be imported first because we draw the volumes in certain histograms from it, so we do not want it
	// deleted in each function. 
	
	DrawInitialAndFinalPositions(fileName, geoManager);
	DrawInitialAndFinalDirections(fileName, geoManager);
	DrawNeutronHeightDistribution(fileName);
	DrawLossFunction(fileName);
	DrawVolumeAveragedMagField(fileName);
	
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
	TCanvas *canvas = new TCanvas("InitialAndFinalPositionsCanvas","Neutron Initial and Final Positions",60,0,400,400);
	canvas->Divide(2,1);
	canvas->cd(1);
	geoManager->GetTopVolume()->Draw();
	geoManager->SetVisLevel(4);
	geoManager->SetVisOption(0);
	initialPositions->Draw();
	// -- Draw Final Points
	canvas->cd(2);
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
Bool_t DrawNeutronHeightDistribution(const char* fileName)
{
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(fileName, "read");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << fileName << endl;
	   return 0;
	}
	// -- Extract Run Object
	const char* runName = "Run1;1";
	TUCNRun* run = new TUCNRun();
   file->GetObject(runName, run);
	if (run == NULL) {
		cerr << "Could not find required object" << endl;
		return kFALSE;
	}
	// -- Extract Histogram Object
	const char* histogramName = "NeutronDensity;1";
	TH1F* histogram = new TH1F();
   file->GetObject(histogramName, histogram);
	if (histogram == NULL) {
		cerr << "Could not find required histogram" << endl;
		return kFALSE;
	}
	// --------------------------------------------------------------------------------------
	// Fit Neutron Density versus Height
	
	TCanvas * histcanvas = new TCanvas("NeutronDensityCanvas","Neutron Density versus height",60,0,800,800);
   histcanvas->Divide(1,2);
	histcanvas->SetGrid();
	histcanvas->cd(1);

	// -- Max height of neutrons
	gTotalEnergy = run->TotalEnergy();
	Double_t maxheight = gTotalEnergy/(Constants::neutron_mass*(Constants::grav_acceleration));
	
	if (gTotalEnergy == 0.0) {
		cerr << "Total Energy undefined" << endl;
		return EXIT_FAILURE;
	}
		
	TF1* analyticNeutronDensity = new TF1("AnalyticNeutronDensity", densityf, 0.0, maxheight, 1); 
	analyticNeutronDensity->SetParName(0,"Const");
	analyticNeutronDensity->SetParameter(0, 240);
	analyticNeutronDensity->SetLineColor(kRed);

	histogram->Fit("AnalyticNeutronDensity", "R");
	histogram->Draw("E1");

	// -------------------------------------------------------------------------------------- 
	// -- Plot difference between bin content and fitted distribution of above histogram
	histcanvas->cd(2);	
	Int_t numberOfPoints = histogram->GetNbinsX();
	const Int_t arrayStorage = 10000;
	Double_t ex[arrayStorage];
	Double_t ey[arrayStorage];
	Double_t x[arrayStorage];
	Double_t y[arrayStorage];
	 
	for (Int_t i = 1; i < numberOfPoints; i++) { 
		x[i] = (histogram->GetBinCenter(i)); 
		y[i] = (histogram->GetBinContent(i) - analyticNeutronDensity->Eval(x[i]));
		ex[i] = (0.);
		ey[i] = (histogram->GetBinError(i));
	} 
  	// create graph 
  	TGraphErrors* residuals  = new TGraphErrors(numberOfPoints, x, y, ex, ey); 
	residuals->SetTitle("Residuals");
	residuals->Draw("AC*");
	
	// Clean up
	delete run;
		
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t DrawLossFunction(const char* fileName)
{
// -- Function reads in the Loss Function Graph from file and fits it to the analytical expression	
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(fileName, "read");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << fileName << endl;
	   return 0;
	}
	// -- Extract LossFunctionGraph
	const char* graphName = "LossProbGraph;1";
	TGraphErrors* graph = new TGraphErrors();
   file->GetObject(graphName, graph);
	if (graph == NULL) {
		cerr << "Could not find required graph: " << graphName << endl;
		return kFALSE;
	}
	
	// -- Draw analytic curve
	TCanvas* canvas = new TCanvas("LossProbabilityCanvas","Loss Probability as a function of Energy",60,0,600,600);
	canvas->cd();
	TF1* analyticCurve = new TF1("AnalyticLossFunc",lossFunc,0.0,1.0,0);
	analyticCurve->Draw();
	// -- Draw the loss graph data on top
	graph->Draw("PSame");
	
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t DrawVolumeAveragedMagField(const char* fileName)
{
// -- Read in the Histograms for the Volume-Averaged Mag field from file and draw.
// -- Should be two histograms in the file - one for the field when neutron losses at boundary are present,
// -- one for when they are not. 	
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(fileName, "read");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << fileName << endl;
	   return 0;
	}
	// -- Extract Histogram of the no losses case
	const char* noLossesHistName = "VolumeAveragedFieldNoLosses;1";
	TH1F *noLossesHistogram = new TH1F();
   file->GetObject(noLossesHistName, noLossesHistogram);
	if (noLossesHistogram == NULL) {
		cerr << "Could not find required histogram: " << noLossesHistName << endl;
		return kFALSE;
	}
	// -- Extract Histogram of case with losses
	const char* lossesHistName = "VolumeAveragedFieldLosses;1";
	TH1F *lossesHistogram = new TH1F();
   file->GetObject(lossesHistName, lossesHistogram);
	if (lossesHistogram == NULL) {
		cerr << "Could not find required histogram: " << lossesHistName << endl;
		return kFALSE;
	}
	// -- Draw Histograms
	TCanvas* canvas = new TCanvas("VolumeAveragedFieldCanvas","Volume Averaged Field",60,0,600,600);
	canvas->Divide(1,2);
	canvas->cd(1);
	noLossesHistogram->Draw("");
	canvas->cd(2);
	lossesHistogram->Draw("");
	
	return kTRUE;
}


// -------------------------------------------------------------------------------------- 
Double_t densityf(Double_t* x, Double_t* par)
{
	assert(gTotalEnergy > 0.0);
	Double_t value = (gTotalEnergy - (Constants::neutron_mass)*(Constants::grav_acceleration)*x[0])/gTotalEnergy;
	assert(value >= 0.0);
	return par[0]*sqrt(value);
}

// -------------------------------------------------------------------------------------- 
Double_t lossFunc(Double_t* x, Double_t* /*par*/) {
	Double_t value = (2.*((1./x[0])*TMath::ASin(TMath::Sqrt(x[0])) - TMath::Sqrt((1./x[0]) - 1.)));   
	return value;
}

