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

#include "TUCNGeoManager.h"
#include "TUCNGeoNavigator.h"
#include "TUCNGeoBuilder.h"
#include "TUCNGeoBBox.h"
#include "TUCNGeoTube.h"
#include "TUCNGeoMaterial.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"
#include "TUCNExperiment.h"
#include "TUCNMagField.h"
#include "TUCNUniformMagField.h"
#include "TUCNRun.h"
#include "TUCNFieldManager.h"
#include "TUCNConfigFile.h"

#include "Constants.h"
#include "Units.h"
#include "FitSuite.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;

const Double_t total_energy = 50*neV;

Bool_t LoadFile(const string& configFileName, TFile* dataFile);
Double_t densityf(Double_t* x, Double_t* par);

Int_t main(void)
{
	///////////////////////////////////////////////////////////////////////////////////////
	// Read in Build Test ConfigFile
	///////////////////////////////////////////////////////////////////////////////////////
	string configFileName = "config/buildtest_config.cfg";

	///////////////////////////////////////////////////////////////////////////////////////
	// -- Initialise Simulation
	///////////////////////////////////////////////////////////////////////////////////////
	TUCNExperiment* experiment = new TUCNExperiment(configFileName);
	if (!(experiment->Initialise())) {
		cerr << "Failed to initialise the Experiment. Program aborting." << endl;
		return EXIT_FAILURE;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Run Simulation
	///////////////////////////////////////////////////////////////////////////////////////
	if (!(experiment->Run())) {
		cerr << "Experiment failed to run. Program aborting." << endl;
		return EXIT_FAILURE;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Export to File
	///////////////////////////////////////////////////////////////////////////////////////
	if (!(experiment->Export())) {
		cerr << "Experiment failed to write to File. Program aborting." << endl;
		return EXIT_FAILURE;
	}
	cout << endl << endl << "END OF SIMULATION" << endl << endl << endl;
	delete experiment;		
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Run Analysis of Data
	///////////////////////////////////////////////////////////////////////////////////////
	
	TUCNConfigFile configFile(configFileName);
	TString outputFile(configFile.GetString("OutputFile","I/O"));
	
	cout << endl << endl;
	cout << "-------------------------------------------" << endl;
	cout << "Run Build Test Analysis on: " << outputFile <<  endl;
	cout << "-------------------------------------------" << endl << endl;
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Open File
/*	TFile *old = gFile;
	TFile *f = 0;
	f = TFile::Open(outputFile);
	if (!f || f->IsZombie()) {
	   if (old) old->cd();
	   cerr << "Cannot open file: " << outputFile << endl;
	   return 0;
	}
	// -- Extract Experiment Object
	const char* name = 0; "Experiment;1";
	TUCNExperiment* testExperiment = new TUCNExperiment();
	if (name && strlen(name) > 0) {
	   f->GetObject(name, testExperiment);
	} else {
	   TIter next(f->GetListOfKeys());
	   TKey *key;
		while ((key = (TKey*)next())) {
			if (strcmp(key->GetClassName(),"TUCNExperiment") != 0) continue;
			key->Read(testExperiment);
	      break;
	   }
	}
	if (old) old->cd();
	delete f;
	cout << testExperiment << endl;
	gGeoManager->GetCurrentNavigator()->Print();
*/	///////////////////////////////////////////////////////////////////////////////////////
	
	// -- Import Geometry
	TGeoManager::Import(outputFile);
	gGeoManager->GetCurrentNavigator()->Print();
	gGeoManager->GetListOfNavigators()->Print();
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Open File
	TFile *old = gFile;
	TFile *f = 0;
	f = TFile::Open(outputFile);
	if (!f || f->IsZombie()) {
	   if (old) old->cd();
	   cerr << "Cannot open file: " << outputFile << endl;
	   return 0;
	}
	// -- Extract Run Object
	const char* name = "Run1;1";
	TUCNRun* testRun = new TUCNRun();
	if (name && strlen(name) > 0) {
	   f->GetObject(name, testRun);
	} else {
	   TIter next(f->GetListOfKeys());
	   TKey *key;
		while ((key = (TKey*)next())) {
			if (strcmp(key->GetClassName(),"TUCNRun") != 0) continue;
			key->Read(testRun);
	      break;
	   }
	}
	if (old) old->cd();
	delete f;	
	///////////////////////////////////////////////////////////////////////////////////////
	
	Int_t nbins = 50;
	Double_t tubeheight = 0.12;

	// Plot Histogram
	TH1F * Histogram1 = new TH1F("Histogram1","Neutron Density versus height", nbins, 0.0, tubeheight);	

	for (Int_t i = 0; i < testRun->Neutrons(); i++) {
		// Get each Track
		TUCNParticle* particle = testRun->GetParticle(i);
		if (particle->LostToBoundary() == kTRUE) continue;
		Histogram1->Fill(particle->Vz());
	}

	// --------------------------------------------------------------------------------------
	// Fit Neutron Density versus Height
	TCanvas * histcanvas = new TCanvas("HistCanvas","Neutron Density versus height",20,20,800,800);
   histcanvas->Divide(1,2);
	histcanvas->SetGrid();
	histcanvas->cd(1);

	// -- Max height of neutrons
	Double_t totalEnergy = testRun->TotalEnergy();
	Double_t maxheight = totalEnergy/(Constants::neutron_mass*(Constants::grav_acceleration));

	TF1 * fitdensf = new TF1("fitdensf", densityf, 0.0, maxheight, 1); 
	fitdensf->SetParName(0,"Const");
	fitdensf->SetParameter(0, 240);
	fitdensf->SetLineColor(kRed);

	Histogram1->SetLineColor(kBlack);
	Histogram1->SetXTitle("Height from bottom of Tube (m)");
	Histogram1->SetYTitle("Number of Neutrons");

	Histogram1->Fit("fitdensf", "R");
	Histogram1->Draw("E1");

	// -------------------------------------------------------------------------------------- 
	// -- Plot difference between bin content and fitted distribution of above histogram
	histcanvas->cd(2);	
	Int_t n = nbins;
  	Double_t ex[n], ey[n], x[n], y[n]; 
  	for (Int_t i=1;i<n;i++) { 
		x[i] = Histogram1->GetBinCenter(i); 
		y[i] = Histogram1->GetBinContent(i) - fitdensf->Eval(x[i]);
		ex[i] = 0.;
		ey[i] = Histogram1->GetBinError(i);
	} 
  	// create graph 
  	TGraphErrors* gr1  = new TGraphErrors(n,x,y,ex,ey); 
	gr1->SetTitle("Bin value minus fitted value versus height");
	gr1->Draw("AC*");
	
	
	
	cout << endl << endl << "ANALYSIS COMPLETE" << endl << endl << endl;
	
	return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------------------- 
Double_t densityf(Double_t* x, Double_t* par)
{
	Double_t value = (total_energy - (Constants::neutron_mass)*(Constants::grav_acceleration)*x[0])/total_energy;
	assert(value >= 0.0);
	return par[0]*sqrt(value);
}
