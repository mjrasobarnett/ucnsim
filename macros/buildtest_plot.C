#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cassert>
#include <cstdlib>
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

#include "include/Constants.h"
#include "include/Units.h"
#include "include/FitSuite.h"

Double_t gTotalEnergy = 0.0*Units::neV;

Bool_t DrawInitialPositions(const char* fileName, TGeoManager* geoManager);
Bool_t DrawFinalPositions(const char* fileName, TGeoManager* geoManager);
Bool_t DrawNeutronHeightDistribution(const char* fileName);
Double_t densityf(Double_t* x, Double_t* par); 

Int_t buildtest_plot(const char* fileName) {
	
	// -- Import Geometry
	TGeoManager* geoManager = TGeoManager::Import(fileName);
	
	DrawInitialPositions(fileName, geoManager);
	DrawFinalPositions(fileName, geoManager);
	DrawNeutronHeightDistribution(fileName);
	
	return 0;
}

// -------------------------------------------------------------------------------------- 
Bool_t DrawInitialPositions(const char* fileName, TGeoManager* geoManager) 
{
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(fileName, "read");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << fileName << endl;
	   return 0;
	}
	// -- Extract Initial Positions in TPolyMarker3D
	const char* positions = "NeutronInitialPositions;1";
	TPolyMarker3D* initialPositions = new TPolyMarker3D();
	file->GetObject(positions, initialPositions);
	if (initialPositions == NULL) {
		cerr << "Could not find TPolyMarker3D: " << positions << endl;
		return kFALSE;
	}
	// -- Draw Points
	TCanvas *canvas = new TCanvas("InitialPositionsCanvas","Neutron InitialPositions",60,0,400,400);
	canvas->cd();
	geoManager->GetTopVolume()->Draw();
	geoManager->SetVisLevel(4);
	geoManager->SetVisOption(0);
	initialPositions->Draw();
	
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t DrawFinalPositions(const char* fileName, TGeoManager* geoManager) 
{
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(fileName, "read");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << fileName << endl;
	   return 0;
	}
	// -- Extract Final Positions in TPolyMarker3D
	const char* positions = "NeutronFinalPositions;1";
	TPolyMarker3D* finalPositions = new TPolyMarker3D();
	file->GetObject(positions, finalPositions);
	if (finalPositions == NULL) {
		cerr << "Could not find TPolyMarker3D: " << positions << endl;
		return kFALSE;
	}
	// -- Draw Points
	TCanvas * canvas = new TCanvas("FinalPositionsCanvas","Neutron Final Positions",500,0,400,400);
	canvas->cd();
	geoManager->GetTopVolume()->Draw();
	geoManager->SetVisLevel(4);
	geoManager->SetVisOption(0);
	finalPositions->Draw();
	
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

	histogram->SetLineColor(kBlack);
	histogram->SetXTitle("Height from bottom of Tube (m)");
	histogram->SetYTitle("Number of Neutrons");
	histogram->Fit("AnalyticNeutronDensity", "R");
	histogram->Draw("E1");

	// -------------------------------------------------------------------------------------- 
	// -- Plot difference between bin content and fitted distribution of above histogram
	histcanvas->cd(2);	
	static const Int_t n = histogram->GetNbinsX();
  	Double_t ex[n], ey[n], x[n], y[n]; 
  	for (Int_t i=1; i< n; i++) { 
		x[i] = histogram->GetBinCenter(i); 
		y[i] = histogram->GetBinContent(i) - analyticNeutronDensity->Eval(x[i]);
		ex[i] = 0.;
		ey[i] = histogram->GetBinError(i);
	} 
  	// create graph 
  	TGraphErrors* residuals  = new TGraphErrors(n,x,y,ex,ey); 
	residuals->SetTitle("Residuals");
	residuals->Draw("AC*");
	
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
