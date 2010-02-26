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
#include "TGeoVolume.h"
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

Bool_t PlotInitialAndFinalPositions(const TString& dataFileName, const TString& runName);
Bool_t PlotInitialAndFinalAngularDistribution(const TString& dataFileName, const TString& runName); 
Bool_t PlotFinalHeightDistribution(const TString& dataFileName);
Bool_t PlotLossFunction(const TString& dataFileName);
Bool_t PlotAvgMagField(const TString& dataFileName);

Int_t main(Int_t argc,Char_t ** argv)
{
	
	TRint *theApp = new TRint("FittingApp", &argc, argv);
	
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
	TString dataFile(configFile.GetString("OutputFile","I/O"));
	
	cout << endl << endl;
	cout << "-------------------------------------------" << endl;
	cout << "Run Build Test Analysis on: " << dataFile <<  endl;
	cout << "-------------------------------------------" << endl << endl;
	
	PlotInitialAndFinalPositions(dataFile, "Run1;1");
	PlotInitialAndFinalAngularDistribution(dataFile, "Run1;1");
	PlotFinalHeightDistribution(dataFile);
	PlotLossFunction(dataFile);
	PlotAvgMagField(dataFile);
	
	cout << endl << endl << "ANALYSIS COMPLETE" << endl << endl << endl;
	theApp->Run();
	
	return EXIT_SUCCESS;
}


// -------------------------------------------------------------------------------------- 
Bool_t PlotInitialAndFinalPositions(const TString& dataFileName, const TString& runName) 
{
// -- Create a TPolyMarker3D object to store the final positions of the neutrons and write this to file. 
	cout << "-------------------------------------------" << endl;
	cout << "DrawInitialAndFinalPositions" <<  endl;
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
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Create the initial points
	TPolyMarker3D* initialPoints = new TPolyMarker3D(run->Neutrons(), 1); // 1 is marker style
	for (Int_t i = 0; i < run->Neutrons(); i++) {
		TUCNParticle* particle = run->GetInitialParticle(i);
		assert(particle != NULL);
		initialPoints->SetPoint(i, particle->Vx(), particle->Vy(), particle->Vz());
	}
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
		assert(particle != NULL);
		finalPoints->SetPoint(i, particle->Vx(), particle->Vy(), particle->Vz());
	}
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
	cout << "DrawInitialAndFinalAngularDistribution" <<  endl;
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
		TUCNParticle* finalParticle = run->GetParticle(i);
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

// -------------------------------------------------------------------------------------- 
Bool_t PlotFinalHeightDistribution(const TString& dataFileName)
{
	// -- Write a histogram to the output file that contains the final height distribution of the run: Run1
	// -- This is a test of the neutron's real space density as a function of height under gravity. There is a clear analytic result for this.
	
	cout << "-------------------------------------------" << endl;
	cout << "PlotFinalHeightDistribution" <<  endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Import Geometry
	TGeoManager* geoManager = TGeoManager::Import(dataFileName);
	// -- (Re)Open File
	TFile *f = 0;
	f = TFile::Open(dataFileName, "update");
	if (!f || f->IsZombie()) {
	   cerr << "Cannot open file: " << dataFileName << endl;
	   return 0;
	}
	// -- Extract Run Object
	const char* runName = "Run1;1";
	TUCNRun* run = new TUCNRun();
   f->GetObject(runName, run);
	if (run == NULL) {
		cerr << "Could not find run: " << runName << endl;
		return kFALSE;
	}
	
	// -- Extract Experiment Object
	const char* expName = "Experiment;1";
	TUCNExperiment* experiment = new TUCNExperiment();
   f->GetObject(expName, experiment);
	if (experiment == NULL) {
		cerr << "Could not find object: " << expName << endl;
		return kFALSE;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////
	// Get the height of the tube we are in.
	Int_t nbins = 50;
	const char* tubeName = "innerTube";
	TGeoVolume* tube = geoManager->GetVolume(tubeName);
	if (tube == NULL) {
		cerr << "Error: Attempt to get volume: " << tubeName << " from geoManager failed!" << endl;
		return kFALSE;
	}
	Double_t tubeheight = 2.0*dynamic_cast<TUCNGeoTube*>(tube->GetShape())->GetDz();
	
	// Create and Fill Histogram
	TH1F* histogram = new TH1F("NeutronDensity","Neutron Density versus height", nbins, 0.0, tubeheight);	
	histogram->SetLineColor(kBlack);
	histogram->SetXTitle("Height from bottom of Tube (m)");
	histogram->SetYTitle("Number of Neutrons");
	
	for (Int_t i = 0; i < run->Neutrons(); i++) {
		// Get each Track
		TUCNParticle* particle = run->GetParticle(i);
		if (particle->LostToBoundary() == kTRUE) {
			continue;
		} else {
			histogram->Fill(particle->Vz());
		}
	}
	
	// Write Histogram
	histogram->Write();
	
	// Clean up
	delete histogram;
	delete experiment;
	delete run;
	delete f;
	
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotLossFunction(const TString& dataFileName)
{
	cout << "-------------------------------------------" << endl;
	cout << "PlotLossFunction" <<  endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Import Geometry
	TGeoManager* geoManager = TGeoManager::Import(dataFileName);
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(dataFileName, "update");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << dataFileName << endl;
	   return 0;
	}
	// -- Extract Experiment Object
	const char* expName = "Experiment;1";
	TUCNExperiment* experiment = new TUCNExperiment();
   file->GetObject(expName, experiment);
	if (experiment == NULL) {
		cerr << "Could not find object: " << expName << endl;
		return kFALSE;
	}
	///////////////////////////////////////////////////////////////////////////////////////
	
	Int_t numberOfRuns = 9; // We must ensure that only runs 2-10 are for losses //experiment->NumberOfRuns();
	static const Double_t V = static_cast<TUCNGeoMaterial*>(geoManager->GetMaterial("Boundary Material"))->FermiPotential();
	static const Double_t eta = static_cast<TUCNGeoMaterial*>(geoManager->GetMaterial("Boundary Material"))->Eta();
//	if (eta == 0.0 || V == 0.0) {
//		cerr << "V or Eta are read to be zero from geoManager stored in File - Exiting" << endl;
//		return kFALSE;
//	}
	
	Double_t point_x[numberOfRuns], point_y[numberOfRuns], error_x[numberOfRuns], error_y[numberOfRuns];
		
	for (Int_t runNumber = 2; runNumber <= 10; runNumber++) {
		// -- Extract Each Run Object
		Char_t name[20];
		sprintf(name,"Run%d;1",runNumber);
		const char* runName = name;
		TUCNRun* run = new TUCNRun();
	   file->GetObject(runName, run);
		if (run == NULL) {
			cerr << "Could not find run: " << runName << endl;
			return kFALSE;
		}
		// check that we found the run
		
		// -- Fit Parameters
		Int_t particles = run->Neutrons(); 
		Double_t totalEnergy = run->TotalEnergy();
		Int_t nbins = 100;
		Int_t range = 100000;
		// -- Fill Histogram
		
		TCanvas canvas;
		canvas.cd();
		TH1F histogram("CollisionsBeforeLost","Number of collisions before loss", nbins, 0.0, range);
		for (Int_t i = 0; i < particles; i++) {
			// Get each Track
			TUCNParticle* particle = run->GetParticle(i);
			histogram.Fill(particle->Bounces());
		}
		// -- Fit to exponential
		TF1 expntl("expntl", "expo", range);
		histogram.Fit("expntl", "R");
//		Double_t p1 = expntl->GetParameter(0);
//		Double_t e1 = expntl->GetParError(0);
		Double_t lossParameter = expntl.GetParameter(1);
		Double_t error = expntl.GetParError(1);
		
		// -- Fill array with fitted neutron lifetime for each run
		point_x[runNumber-2] = totalEnergy/V; // Total energy of the neutrons in units of the fermi potential V
		point_y[runNumber-2] = TMath::Abs(lossParameter)/eta; // Loss probability in units of f, averaged over all angles of incidence.
		error_x[runNumber-2] = 0.;
		error_y[runNumber-2] = error/eta; // Error on the loss probability
		
		cout << point_x[runNumber] << "\t" << point_y[runNumber] << "\t" << error_x[runNumber] << "\t" << error_y[runNumber] << endl;
		
		delete run; run = 0;
	}
	
	// -- Plot the angle averaged loss probabilities for each energy
	TGraphErrors* lossProb = new TGraphErrors(numberOfRuns, point_x, point_y, error_x, error_y);
	lossProb->SetName("LossProbGraph");
	lossProb->SetTitle("Angle-averaged loss probability");
	lossProb->SetMarkerColor(4);
	lossProb->SetMarkerSize(1);
	lossProb->SetMarkerStyle(21);
	
	// -- Write to file
	lossProb->Write();
	// Clean up
	delete lossProb;
	delete file;
	delete experiment; 
	// Note that deleting the experiment will delete the geoManager as well, even though we store
	// (sort of needed to store) the geoManager seperately in the file.
	
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotAvgMagField(const TString& dataFileName)
{
	// -- Plot the Average Mag Field
	cout << "-------------------------------------------" << endl;
	cout << "PlotAvgMagField" <<  endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(dataFileName, "update");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << dataFileName << endl;
	   return 0;
	}
	// -- Extract Experiment Object
	const char* expName = "Experiment;1";
	TUCNExperiment* experiment = new TUCNExperiment();
   file->GetObject(expName, experiment);
	if (experiment == NULL) {
		cerr << "Could not find object: " << expName << endl;
		return kFALSE;
	}
	// -- Extract Run Object
	const char* noLossesRunName = "Run11;1";
	TUCNRun* noLossesRun = new TUCNRun();
   file->GetObject(noLossesRunName, noLossesRun);
	if (noLossesRun == NULL) {
		cerr << "Could not find run: " << noLossesRunName << endl;
		return kFALSE;
	}
	// -- Extract Run Object
	const char* lossesRunName = "Run12;1";
	TUCNRun* lossesRun = new TUCNRun();
   file->GetObject(lossesRunName, lossesRun);
	if (lossesRun == NULL) {
		cerr << "Could not find run: " << lossesRunName << endl;
		return kFALSE;
	}
	
	// -- Get Run Parameters
	Int_t nbins = 100;
	Double_t fieldMax = 1.0;
	Double_t fieldMin = 0.9;
	TH1F *noLossesHistogram = new TH1F("VolumeAveragedFieldNoLosses","VolumeAveragedField: No Neutron losses", nbins, fieldMin, fieldMax);
	TH1F *lossesHistogram = new TH1F("VolumeAveragedFieldLosses","VolumeAveragedField: Neutron Losses at Boundary", nbins, fieldMin, fieldMax);
	
	// Loop over the surviving neutrons
	for (Int_t j = 0; j < noLossesRun->Neutrons(); j++) {
		TUCNParticle* noLossesParticle = noLossesRun->GetParticle(j);
		if (noLossesParticle->LostToBoundary() == kFALSE) { 
			noLossesHistogram->Fill(noLossesParticle->AvgMagField());
		}
		
	}

	// Loop over the surviving neutrons
	for (Int_t j = 0; j < lossesRun->Neutrons(); j++) {
		TUCNParticle* lossesParticle = lossesRun->GetParticle(j);
		if (lossesParticle->LostToBoundary() == kFALSE) {
			lossesHistogram->Fill(lossesParticle->AvgMagField());
		}
	}

	// Write out Histogram
	noLossesHistogram->Write();
	lossesHistogram->Write();
	
	// Clean Up
	delete noLossesRun;
	delete lossesRun;
	delete experiment;
	delete noLossesHistogram;
	delete lossesHistogram;
	delete file;

	return kTRUE;
}
