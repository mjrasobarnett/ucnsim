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

const Double_t total_energy = 50*Units::neV;

Bool_t DrawFinalPositions(TString& dataFileName);
Bool_t PlotFinalHeightDistribution(TString& dataFileName);
Bool_t PlotLossFunction(TString& dataFileName);
Bool_t PlotAvgMagField(TString& dataFileName);

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
	
	
	gGeoManager->GetTopVolume()->Draw();
	gGeoManager->GetTrack(0)->Draw();
	theApp->Run();
	
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
	
	DrawFinalPositions(dataFile);
	PlotFinalHeightDistribution(dataFile);
	PlotLossFunction(dataFile);
	PlotAvgMagField(dataFile);
	
	cout << endl << endl << "ANALYSIS COMPLETE" << endl << endl << endl;
	theApp->Run();
	
	return EXIT_SUCCESS;
}


// -------------------------------------------------------------------------------------- 
Bool_t DrawFinalPositions(TString& dataFileName) 
{
// -- Create a TPolyMarker3D object to store the final positions of the neutrons and write this to file. 
	cout << "-------------------------------------------" << endl;
	cout << "DrawFinalPositions" <<  endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Open File
	TFile *f = 0;
	f = TFile::Open(dataFileName, "update");
	if (!f || f->IsZombie()) {
	   cerr << "Cannot open file: " << dataFileName << endl;
	   return 0;
	}
	// -- Extract Run Object
	const char* name = "Run1;1";
	TUCNRun* run = new TUCNRun();
   f->GetObject(name, run);
	if (run == NULL) {
		cerr << "Could not find run: " << name << endl;
		return kFALSE;
	}
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Create the points
	TPolyMarker3D* finalPoints = new TPolyMarker3D(run->Neutrons(), 1); // 1 is marker style
	for (Int_t i = 0; i < run->Neutrons(); i++) {
		TUCNParticle* particle = run->GetParticle(i);
		assert(particle != NULL);
		finalPoints->SetPoint(i, particle->Vx(), particle->Vy(), particle->Vz());
	}
	finalPoints->SetMarkerColor(2);
	finalPoints->SetMarkerStyle(6);
	// -- Write the points to the File
	finalPoints->SetName("NeutronPositions");
	finalPoints->Write();
	// -- Clean Up
	delete run; 
	delete finalPoints;
	delete f;
	
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotFinalHeightDistribution(TString& dataFileName)
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
	
	// Fill Histogram
	TH1F* histogram = new TH1F("NeutronDensity","Neutron Density versus height", nbins, 0.0, tubeheight);	
	for (Int_t i = 0; i < run->Neutrons(); i++) {
		// Get each Track
		TUCNParticle* particle = run->GetParticle(i);
		if (particle->LostToBoundary() == kTRUE) {
			continue;
		} else {
			histogram->Fill(particle->Vz());
		}
	}
	histogram->Write();
	
	// Clean up
	delete histogram;
	delete experiment;
	delete run;
	delete f;
	
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotLossFunction(TString& dataFileName)
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
		TH1F *histogram = new TH1F("CollisionsBeforeLost","Number of collisions before loss", nbins, 0.0, range);
		for (Int_t i = 0; i < particles; i++) {
			// Get each Track
			TUCNParticle* particle = run->GetParticle(i);
			histogram->Fill(particle->Bounces());
		}
		// -- Fit to exponential
		TF1 *expntl = new TF1("expntl", "expo", range);
		histogram->Fit("expntl", "R");
//		Double_t p1 = expntl->GetParameter(0);
//		Double_t e1 = expntl->GetParError(0);
		Double_t lossParameter = expntl->GetParameter(1);
		Double_t error = expntl->GetParError(1);
		
		// -- Fill array with fitted neutron lifetime for each run
		point_x[runNumber] = totalEnergy/V; // Total energy of the neutrons in units of the fermi potential V
		point_y[runNumber] = TMath::Abs(lossParameter)/eta; // Loss probability in units of f, averaged over all angles of incidence.
		error_x[runNumber] = 0.;
		error_y[runNumber] = error/eta; // Error on the loss probability
		
		delete run; run = 0;
		delete histogram; histogram = 0;
		delete expntl; expntl = 0;
	}
	
	// -- Plot the angle averaged loss probabilities for each energy
	TGraphErrors* lossProb = new TGraphErrors(numberOfRuns, point_x, point_y, error_x, error_y);
	lossProb->SetTitle("Angle-averaged loss probability of monochromatic UCN as a function of the total initial energy");
	lossProb->SetMarkerColor(4);
	lossProb->SetMarkerSize(1);
	lossProb->SetMarkerStyle(21);
	
	// -- Write to file
	lossProb->Write();
	file->ls();
	// Clean up
	delete lossProb;
	delete file;
	delete experiment; 
	// Note that deleting the experiment will delete the geoManager as well, even though we store
	// (sort of needed to store) the geoManager seperately in the file.
	
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotAvgMagField(TString& dataFileName)
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
	// -- Extract Run Object
	const char* runName = "Run11;1";
	TUCNRun* run = new TUCNRun();
   file->GetObject(runName, run);
	if (run == NULL) {
		cerr << "Could not find run: " << runName << endl;
		return kFALSE;
	}
	// -- Extract Experiment Object
	const char* expName = "Experiment;1";
	TUCNExperiment* experiment = new TUCNExperiment();
   file->GetObject(expName, experiment);
	if (experiment == NULL) {
		cerr << "Could not find object: " << expName << endl;
		return kFALSE;
	}
	
	// -- Get Run Parameters
	Int_t particles = run->Neutrons();
	Int_t nbins = 100;
	Double_t fieldMax = 1.0;
	Double_t fieldMin = 0.9;
	TH1F *histogram = new TH1F("CollisionsBeforeLoss","Number of collisions before loss", nbins, fieldMin, fieldMax);
	
	for (Int_t j = 0; j < particles; j++) {
		// Get each Track
		TUCNParticle* particle = run->GetParticle(j);
		if (particle->LostToBoundary()) { 
			continue; 
		} else {
			histogram->Fill(particle->AvgMagField());
		}
	}

	// Write out Histogram
	histogram->Write();
	
	// Clean Up
	delete run;
	delete experiment;
	delete histogram;
	delete file;

	return kTRUE;
}
