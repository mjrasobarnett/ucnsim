#include <iostream>
#include <fstream>
#include <cassert>

#include "TRint.h"
#include "TFile.h"
#include "TKey.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraphErrors.h"

#include "TGeoManager.h"
#include "TUCNRunManager.h"
#include "TUCNRun.h"

#include "Constants.h"
#include "Units.h"

using namespace std;

Double_t total_energy = 200*neV;
Double_t f = 0.000263472;

Double_t lossFunc(Double_t* x, Double_t* par);
Double_t densityf(Double_t* x, Double_t* par);

void		PlotFinalPositions(TCanvas* canvas, TUCNRunManager* runManager);

Int_t main(Int_t argc,Char_t **argv)
{
	// -- Read in File name
	TString filename;
	if (argc == 2) {
		filename = argv[1];
	} else {
		cerr << "Usage:" << endl;
		cerr << "fitdata <rundata-filename.root> " << endl;
		return -1;
	}
	
	// Start 'the app' -- this is so we are able to enter into a ROOT session after the program has run, instead of just quitting. Useful to play with histograms.
	TRint *theApp = new TRint("FittingApp", &argc, argv);
	
	// Read the file into memory
	cerr << "Reading runs from " << filename << endl;
	TFile * file = new TFile(filename,"UPDATE");
	if ( !file->IsOpen() ) {
		cerr << "Could not open file " << filename << endl;
		cerr << "Exiting..." << endl;
		exit(-1);
	}
	
	// Check contents of the file
	file->ls();	
	TIter next(file->GetListOfKeys()); 
	TKey* key;
	while ((key=(TKey*)next())) {
		printf("key: %s points to an object of class: %s at %i , with cycle number: %i \n", key->GetName(), key->GetClassName(),key->GetSeekKey(),key->GetCycle());
	}
	
	// Get the GeoManager and the RunManager From the File
	TUCNGeoManager* geoManager = 0;
	file->GetObject("GeoManager;1", geoManager);
	TUCNRunManager* runManager = 0;
	file->GetObject("RunManager;1", runManager);
	file->ls();	
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- FITTING 
		
	// Draw Final Positions 
	TCanvas * canvas1 = new TCanvas("canvas1", "Final Particle Positions", 800, 10, 600, 600);
	PlotFinalPositions(canvas1, runManager);
	
	///////////////////////////////////////////////////////////////////////////////////////
/*	// -- FITTING 
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
	
*/	///////////////////////////////////////////////////////////////////////////////////////
	// -- Fitting
//	void PlotWallLossFunction(TCanvas* canvas, TUCNRunManager* runManager) {
		
		// First we plot the number of bounces before particle was lost. Fitting this to an exponential gives the lifetime, and hence the probability of loss
		// averaged over all collisions (which should, for enough particles and uniform distributions..., be the probability of loss averaged over all angles-of-incidence)
		// Store this value for each energy. 
		 
		Int_t numberOfRuns = runManager->NumberOfRuns();
		Double_t point_x[numberOfRuns], point_y[numberOfRuns], error_x[numberOfRuns], error_y[numberOfRuns];
		TCanvas* histcanvas = new TCanvas("HistCanvas","CollisonsBeforeLoss",600,20,600,600);
		
		for(Int_t i = 0; i < numberOfRuns; i++) {
			histcanvas->cd();
			TUCNRun* run = runManager->GetRun(i);
			Int_t nbins = 100;
			Int_t range = 15000;
		
/*			switch (i) {
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
					range = 20000;
					break;
				case 7:
					range = 20000;
					break;
				case 8:
					range = 15000;
					break;
				case 9:
					range = 10000;
					break;
			}
*/		
			TH1F * Histogram = new TH1F("Histogram","Number of collisions before loss", nbins, 0.0, range);
			TF1 *f1 = new TF1("f1", "expo", range);
			cout << "Filling Histogram..." << endl;
		
			Int_t particles = 1000;
		
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

			point_x[i] = 0.95;
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
	//	for (Int_t i = 0; i < numberOfRuns; i++) {
	//		point_y[i] = point_y[i]/f;
	//		error_y[i] = error_y[i]/f;
	//	}
	
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
	
	file->ls(); 
//	file->Close();
	
	theApp->Run();
	
	return 0;
}	


// -------------------------------------------------------------------------------------- 
Double_t densityf(Double_t* x, Double_t* par)
{
	Double_t value = (total_energy - (Constants::neutron_mass)*(Constants::grav_acceleration)*x[0])/total_energy;
	assert(value >= 0.0);
	return par[0]*sqrt(value);
}

// -------------------------------------------------------------------------------------- 
Double_t lossFunc(Double_t* x, Double_t* par) {
	Double_t value = (2.*((1./x[0])*TMath::ASin(TMath::Sqrt(x[0])) - TMath::Sqrt((1./x[0]) - 1.)));   
	return value;
}

// -------------------------------------------------------------------------------------- 
void	PlotFinalPositions(TCanvas* canvas, TUCNRunManager* runManager) {
	// Take canvas, divide into segments proportional to number of runs. Then plot final positions onto canvas.
	Int_t nRuns = runManager->NumberOfRuns();
	// If more than one run, divide the canvas appropriately
	if (nRuns != 1) 	canvas->Divide((nRuns%2 == 0 ? nRuns/2:(nRuns/2+1)),2);
	// Draw the final positions of the particles on the canvas
	for (Int_t plotNumber = 1; plotNumber <= nRuns; plotNumber++) {
		canvas->cd(plotNumber);
		TPolyMarker3D* finalPoints = new TPolyMarker3D(gGeoManager->GetNtracks(), 1);
		runManager->GetRun(0)->DrawParticles(canvas, finalPoints);
	}
}
