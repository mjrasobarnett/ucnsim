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

Bool_t PlotInitialAndFinalPositions(const TString& dataFileName, const TString& runName);
Bool_t PlotInitialAndFinalAngularDistribution(const TString& dataFileName, const TString& runName);

Int_t main(Int_t argc,Char_t **argv)
{
	///////////////////////////////////////////////////////////////////////////////////////
	// Read in ConfigFile
	///////////////////////////////////////////////////////////////////////////////////////
	string configFileName;
	if (argc == 2) {
		configFileName= argv[1];
	} else {
		cerr << "Usage:" << endl;
		cerr << "sandbox <configFile.cfg>" << endl;
		return -1;
	}
	
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
	
/*	TUCNConfigFile configFile(configFileName);
	TString dataFile(configFile.GetString("OutputFile","I/O"));
	
	cout << endl << endl;
	cout << "-------------------------------------------" << endl;
	cout << "Run Build Test Analysis on: " << dataFile <<  endl;
	cout << "-------------------------------------------" << endl << endl;
	
	PlotInitialAndFinalPositions(dataFile, "Run1;1");
	PlotInitialAndFinalAngularDistribution(dataFile, "Run1;1");
*/	
	return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotInitialAndFinalPositions(const TString& dataFileName, const TString& runName) 
{
// -- Create a TPolyMarker3D object to store the final positions of the neutrons and write this to file. 
	cout << "-------------------------------------------" << endl;
	cout << "PlotInitialAndFinalPositions" <<  endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(dataFileName, "update");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << dataFileName << endl;
	   return 0;
	}
	// -- Extract Run Object
	cout << "Fetching run: " << runName << endl;
	TUCNRun* run = new TUCNRun();
   file->GetObject(static_cast<const char*>(runName), run);
	if (run == NULL) {
		cerr << "Could not find run: " << runName << endl;
		return kFALSE;
	}
	cout << "Successfully Loaded Run: " << runName << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Create the initial points
	TPolyMarker3D* initialPoints = new TPolyMarker3D(run->Neutrons(), 1); // 1 is marker style
	for (Int_t i = 0; i < run->Neutrons(); i++) {
		TUCNParticle* particle = run->GetInitialParticle(i);
		if (particle == NULL) continue;
		initialPoints->SetPoint(i, particle->Vx(), particle->Vy(), particle->Vz());
	}
	cout << "Created Initial Points" << endl;
	initialPoints->SetMarkerColor(2);
	initialPoints->SetMarkerStyle(6);
	// -- Write the points to the File
	initialPoints->SetName("NeutronInitialPositions");
	initialPoints->Write();
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Create the final points
	TPolyMarker3D* finalPoints = new TPolyMarker3D(run->Neutrons(), 1); // 1 is marker style
	for (Int_t i = 0; i < run->Neutrons(); i++) {
		TUCNParticle* particle = run->GetParticle(i);
		if (particle == NULL) continue;
		finalPoints->SetPoint(i, particle->Vx(), particle->Vy(), particle->Vz());
	}
	cout << "Created Final Points" << endl;
	finalPoints->SetMarkerColor(2);
	finalPoints->SetMarkerStyle(6);
	// -- Write the points to the File
	finalPoints->SetName("NeutronFinalPositions");
	finalPoints->Write();
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Clean Up
	delete run;
	delete initialPoints; 
	delete finalPoints;
	delete file;
	
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotInitialAndFinalAngularDistribution(const TString& dataFileName, const TString& runName) 
{
// -- Create a Histogram object to store the angular distribution (as in, their initial and final directions about the origin). 
	cout << "-------------------------------------------" << endl;
	cout << "PlotInitialAndFinalAngularDistribution" <<  endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(dataFileName, "update");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << dataFileName << endl;
	   return 0;
	}
	// -- Extract Run Object
	TUCNRun* run = new TUCNRun();
   file->GetObject(static_cast<const char*>(runName), run);
	if (run == NULL) {
		cerr << "Could not find run: " << runName << endl;
		return kFALSE;
	}
	// -- Plot the Initial and Final Directions
	Int_t nbins = 50;
	TH1F* initialThetaHist = new TH1F("InitialThetaHist","Initial Direction: Theta component, Units of Pi", nbins, 0.0, 1.0);
	TH1F* initialPhiHist = new TH1F("InitialPhiHist","Initial Direction: Phi component, Units of Pi", nbins, 0.0, 2.0);
	TH1F* finalThetaHist = new TH1F("FinalThetaHist","Final Direction: Theta component, Units of Pi", nbins, 0.0, 1.0);
	TH1F* finalPhiHist = new TH1F("FinalPhiHist","Final Direction: Phi component, Units of Pi", nbins, 0.0, 2.0);
	// Axis Titles
	initialThetaHist->SetXTitle("Height from bottom of Tube (m)");
	initialThetaHist->SetYTitle("Number of Neutrons");
	
	for (Int_t i = 0; i < run->Neutrons(); i++) {
		TUCNParticle* initialParticle = run->GetInitialParticle(i);
		if (initialParticle == NULL) continue;
		TUCNParticle* finalParticle = run->GetParticle(i);
		if (finalParticle == NULL) continue;
		initialThetaHist->Fill(initialParticle->Theta()/TMath::Pi());
		initialPhiHist->Fill(initialParticle->Phi()/TMath::Pi());
		finalThetaHist->Fill(finalParticle->Theta()/TMath::Pi());
		finalPhiHist->Fill(finalParticle->Phi()/TMath::Pi());
	}
	// -- Write the points to the File
	initialThetaHist->Write();
	initialPhiHist->Write();
	finalThetaHist->Write();
	finalPhiHist->Write();
	
	delete initialThetaHist;
	delete initialPhiHist;
	delete finalThetaHist;
	delete finalPhiHist;
	delete run;
	delete file;
	
	return kTRUE;
}




