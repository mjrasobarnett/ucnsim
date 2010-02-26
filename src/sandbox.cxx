#ifndef __CINT__

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cassert>

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
#include "TUCNRunManager.h"
#include "TUCNMagField.h"
#include "TUCNUniformMagField.h"
#include "TUCNRun.h"

#include "Constants.h"
#include "Units.h"


using std::cout;
using std::endl;

Double_t total_energy = 200*neV;
Double_t f = 0.000263472;

Double_t lossFunc(Double_t* x, Double_t* par);
Double_t densityf(Double_t* x, Double_t* par);

Int_t main(Int_t argc,Char_t **argv)
{
	TRint *theApp = new TRint("UCNSimApp", &argc, argv);
#else 
Int_t ucnstandalone() {	
	gSystem->Load("libPhysics");
	gSystem->Load("libGeom");
	gSystem->Load("libEG");
	gSystem->Load("libUCN");
#endif
	
	TBenchmark benchmark;
	benchmark.Start("UCNSim");
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Geometry Creation
	
	TUCNRunManager* runManager = new TUCNRunManager();
	TUCNGeoManager* geoManager = runManager->GetGeoManager();
	
	Int_t numberOfRuns = 2;
	// Need to Add the runs before we initialise the geometry (because we create the navigators with each run). 
	// and the navigators need to be created before we close the geometry.
	runManager->CreateRuns(numberOfRuns);
	
	runManager->InitialiseGeometry();
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Run Simulation
	Double_t runTime = 100.*Units::s;
	Double_t maxStepTime = 0.05*Units::s;
	Int_t particles = 100;
	Double_t V = static_cast<TUCNGeoMaterial*>(geoManager->GetMaterial("Boundary Material"))->FermiPotential();
	Double_t f = static_cast<TUCNGeoMaterial*>(geoManager->GetMaterial("Boundary Material"))->Eta();
	Double_t totalEnergy = 0;
	cout << "V: " << V << "\t" << "f: " << f << endl;
	
	for (Int_t runNumber = 0; runNumber < numberOfRuns; runNumber++) {
		totalEnergy = 0.57*V; //(1.0/10.0)*(runNumber+1)*V; //(0.12*Units::m)*Constants::height_equivalent_conversion; 
		
		TUCNRun* run = runManager->GetRun(runNumber);
		cout << "Run number: " << runNumber << "\t" << "called: " << run->GetName() << endl;
		cout << "totalEnergy: " << totalEnergy << endl;
		
		run->Initialise(particles, totalEnergy);
	
		// -- Propagate the tracks according to the run parameters
		run->PropagateAllTracks(runTime, maxStepTime);	
	
		cout << "-------------------------------------------" << endl;
		cout << "Propagation Results: " << endl;
		cout << "Total Particles: " << geoManager->GetNtracks() << endl;
		cout << "Number Detected: " << geoManager->GetNumberDetected() << endl;
		cout << "Number Lost: " << geoManager->GetNumberLost() << endl;
		cout << "Number Decayed: " << geoManager->GetNumberDecayed() << endl;
		cout << "-------------------------------------------" << endl;
				
		// -- Write out data to file
		// Warning - introducing TFile here is causing program to crash on exit. Needs closer examination before we do this.
		///////////////////////////////////////////////////////////////////////////////////////
		// -- Create a file to store the tracks
	//	TFile* file = new TFile("tracks_test.root","RECREATE");
	//	if ( file->IsOpen() ) {
	//		cerr << "Opened tracks.root" << endl;
	//	}
	//	else {
	//		cerr << "Could not open tracks.root" << endl;
	//		return 1;
	//	}
	//	run->WriteOutData(file); 
		
		geoManager->ClearTracks();
		
		cout << "End of run" << endl << endl;
	}
	
	TFile* file = new TFile("tracks_test.root","RECREATE");
	runManager->WriteRunsToFile(file);
	
	TFile myfile("tracks_test.root");
	myfile.ls();
	
	TIter next(myfile.GetListOfKeys()); 
	TKey* key;
	while ((key=(TKey*)next())) {
		printf("key: %s points to an object of class: %s at %d \n", key->GetName(), key->GetClassName(),key->GetSeekKey());
	}
	
	TUCNRun* run = runManager->GetRun(0);
	
	///////////////////////////////////////////////////////////////////////////////////////
	// Draw Final Positions 
	TCanvas * canvas1 = new TCanvas("canvas1", "Final Particle Positions", 800, 10, 600, 600);
	TPolyMarker3D* finalPoints = new TPolyMarker3D(geoManager->GetNtracks(), 1);
	run->DrawParticles(canvas1, finalPoints);

	///////////////////////////////////////////////////////////////////////////////////////
	// -- FITTING 
	Int_t nbins = 50;
	Double_t maxlength = 0.12;
	
	// Plot Histogram
	TH1F * Histogram1 = new TH1F("Histogram1","Neutron Density versus height", nbins, 0.0, maxlength);	
	TH1F * Histogram2 = new TH1F("Histogram2","Avg Field Sampled by Neutron", nbins, 0.001, 0.002);	
	TH1F * Histogram3 = new TH1F("Histogram3","Distance Travelled", nbins, 500, 700.);	
	
	for (Int_t i = 0; i < particles; i++) {
		// Get each Track
		TUCNParticle* particle = run->GetParticle(i);
		if (particle->Lost() == kTRUE) continue;
		Histogram1->Fill(particle->Vz());
		Histogram2->Fill(particle->AvgMagField()/particle->Distance());
		Histogram3->Fill(particle->Distance());
	}

	// --------------------------------------------------------------------------------------
	// Fit Neutron Density versus Height
	TCanvas * histcanvas = new TCanvas("HistCanvas","Neutron Density versus height",20,20,800,800);
   histcanvas->Divide(1,2);
	histcanvas->SetGrid();
	histcanvas->cd(1);

	// -- Max height of neutrons
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

	// -------------------------------------------------------------------------------------- 
	// -- Plot Avg Field sampled by neutrons
	TCanvas * histcanvas2 = new TCanvas("HistCanvas2","Avg Field Sampled by Neutron",20,20,800,800);
	histcanvas2->cd();
	Histogram2->SetLineColor(kBlack);
	Histogram2->SetXTitle("Avg Field (T)");
	Histogram2->SetYTitle("Number of Neutrons");
	Histogram2->Draw("");
	
	cout << Histogram2->GetEntries() << endl;
	
	TCanvas * histcanvas3 = new TCanvas("HistCanvas3","Distance Travelled by Neutron",20,20,800,800);
	histcanvas3->cd();
	Histogram3->SetLineColor(kBlack);
	Histogram3->SetXTitle("Distance (m)");
	Histogram3->SetYTitle("Number of Neutrons");
	Histogram3->Draw("");
	
/*	///////////////////////////////////////////////////////////////////////////////////////
	// -- Fitting
	Double_t point_x[numberOfRuns];
	Double_t point_y[numberOfRuns];	
	Double_t error_x[numberOfRuns];
	Double_t error_y[numberOfRuns];
	
	
	TCanvas * histcanvas = new TCanvas("HistCanvas","CollisonsBeforeLoss",20,20,1000,1000);
//	histcanvas->Divide(5,2);
	for(Int_t i = 0; i < numberOfRuns; i++) {
		TUCNRun* run = runManager->GetRun(i);
		
		histcanvas->cd(i+1);
		Int_t nbins = 100;
		Int_t range = 100000;
*/		
/*		switch (i) {
			case 0:
				range = 60000;
				break;
			case 1:
				range = 40000;
				break;
			case 2:
				range = 30000;
				break;
			case 3:
				range = 20000;
				break;
			case 4:
				range = 20000;
				break;
			case 5:
				range = 20000;
				break;
			case 6:
				range = 15000;
				break;
			case 7:
				range = 15000;
				break;
			case 8:
				range = 12000;
				break;
			case 9:
				range = 10000;
				break;
		}
*/		
/*		TH1F * Histogram = new TH1F("Histogram","Number of collisions before loss", nbins, 0.0, range);
		TF1 *f1 = new TF1("f1", "expo", range);
		cout << "Filling Histogram..." << endl;
		
		for (Int_t j = 0; j < particles; j++) {
			// Get each Track
			TUCNParticle* particle = run->GetParticle(j);
			Histogram->Fill(particle->Bounces());
		}
		
		Histogram->Fit("f1", "R");
		
		Double_t p1 = f1->GetParameter(0);
		Double_t e1 = f1->GetParError(0);
		Double_t p2 = f1->GetParameter(1);
		Double_t e2 = f1->GetParError(1);
		cout <<  "After Fit: " << "\t" << "p1: " << p1 << "\t" << "p2: " << p2 << "\t" << "e1: " << e1 << "\t" << "e2: " << e2 << endl;

		point_x[i] = 0.01;
		point_y[i] = TMath::Abs(p2)/f;
		error_x[i] = 0.;
		error_y[i] = e2/f;

		cout << "Pointx: " << point_x[i] << "\t" << "Pointy: " << point_y[i] << endl;
		cout << "Errorx: " << error_x[i] << "\t" << "Errory: " << error_y[i] << endl;
	}

	//	Histogram->Draw();

	TCanvas * histcanvas4 = new TCanvas("HistCanvas4","Loss Function",20,20,600,600);
	histcanvas4->cd();
	
	TF1 *lossf = new TF1("lossf",lossFunc,0.0,1.0,0);
	lossf->SetRange(0.0, 1.0);
	lossf->Draw();
	cout << lossf->Eval(1.,0.,0.,0.) << endl;
	
	
//	Double_t point_x[13] = {0.01, 0.05, 0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,0.95,1.0};
//	Double_t point_y[13] = {0.134027*f, 7.64717e-05, 0.000111426, 0.000162566, 0.780407*f, 0.000252422, 0.000301221, 0.00034624, 0.000389121, 0.000449889, 0.000510026, 0.000552799, 0.000782759 };	
//	Double_t error_x[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
//	Double_t error_y[13] = {0.00578513*f, 2.97809e-06, 6.76548e-06, 6.31687e-06, 1.91486e-06, 1.00426e-05, 1.03125e-05, 1.1159e-05, 1.42658e-05, 1.58957e-05, 2.26283e-05, 2.96299e-05, 3.39148e-05};
//	Int_t numberOfRuns = 13;

	for (Int_t i = 0; i < numberOfRuns; i++) {
		point_y[i] = point_y[i]/f;
		error_y[i] = error_y[i]/f;
	}
	
	TGraphErrors* lossProb = new TGraphErrors(numberOfRuns, point_x, point_y, error_x, error_y);
	lossProb->SetTitle("TGraphErrors Example");
	lossProb->SetMarkerColor(4);
	lossProb->SetMarkerSize(1);
	lossProb->SetMarkerStyle(21);
	lossProb->Draw("PSame");
	
	
	lossf->GetXaxis()->SetRangeUser(0,1);
	lossf->GetYaxis()->SetRangeUser(0.,3.2);
	lossf->GetXaxis()->SetTitle("E/V");
	lossf->GetYaxis()->SetTitle("Loss Probability");
*/
	benchmark.Stop("UCNSim");
	benchmark.Show("UCNSim");
	
	#ifndef __CINT__
		theApp->Run();
	#endif
	///////////////////////////////////////////////////////////////////////////////////////
//	runManager->Delete();
	
	return 0;
}

// -------------------------------------------------------------------------------------- 
Double_t densityf(Double_t* x, Double_t* par)
{
	Double_t value = (total_energy - (Constants::neutron_mass)*(Constants::grav_acceleration)*x[0])/total_energy;
	assert(value >= 0.0);
	return par[0]*sqrt(value);
}

Double_t lossFunc(Double_t* x, Double_t* par) {
	Double_t value = (2.*((1./x[0])*TMath::ASin(TMath::Sqrt(x[0])) - TMath::Sqrt((1./x[0]) - 1.)));   
	return value;
}
