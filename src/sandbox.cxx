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
	TRint *theApp = new TRint("UCN App", &argc, argv);
#else 
Int_t ucnstandalone() {	
	gSystem->Load("libPhysics");
	gSystem->Load("libGeom");
	gSystem->Load("libEG");
	gSystem->Load("libUCN");
#endif
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Geometry Creation
	
	TUCNRunManager* runManager = new TUCNRunManager();
	TUCNGeoManager* geoManager = runManager->GetGeoManager();
	
	Int_t numberOfRuns = 9;
	// Need to Add the runs before we initialise the geometry (because we create the navigators with each run). 
	// and the navigators need to be created before we close the geometry.
	runManager->AddRuns(numberOfRuns);
	
	runManager->InitialiseGeometry();
	
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Run Simulation
	Double_t runTime = 5000.*Units::s;
	Double_t maxStepTime = 1.00*Units::s;
	Int_t particles = 6000;
	Double_t V = static_cast<TUCNGeoMaterial*>(geoManager->GetMaterial("Boundary Material"))->FermiPotential();
	Double_t f = static_cast<TUCNGeoMaterial*>(geoManager->GetMaterial("Boundary Material"))->Eta();
	Double_t totalEnergy = 0;
	cout << "V: " << V << "\t" << "f: " << f << endl;
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Fitting
	Double_t point_x[numberOfRuns];
	Double_t point_y[numberOfRuns];	
	Double_t error_x[numberOfRuns];
	Double_t error_y[numberOfRuns];
	
//	TCanvas* canvas = new TCanvas("canvas", "NumberOfCollisionsBeforeLoss", 800, 10, 600, 600);
//	canvas->cd();
	
	for (Int_t runNumber = 0; runNumber < numberOfRuns; runNumber++) {
		totalEnergy = (1.0/10.0)*(runNumber+1)*V;
		
		TUCNRun* run = runManager->GetRun(runNumber);
		cout << "Run number: " << runNumber << endl;
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
		
		Int_t nbins = 50;
		TH1F * Histogram = new TH1F("Histogram","Number of collisions before loss", nbins, 0.0, 10000);
		TF1 *f1 = new TF1("f1", "expo", 0, 10000);	
		cout << "Filling Histogram..." << endl;
		for (Int_t i = 0; i < particles; i++) {
			// Get each Track
			TVirtualGeoTrack* track = geoManager->GetTrack(i);
			TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
			Histogram->Fill(particle->Bounces());
		}
		
		Double_t mean = Histogram->GetMean();
		Double_t meanerror = Histogram->GetMeanError();
		cout << "Mean: " << mean << "\t" << "Mean error: " << meanerror << endl;
		Double_t estimatedLoss = 1./mean;
		Double_t estimatedLossError = TMath::Sqrt((-1./(mean*mean))*(-1./(mean*mean))*meanerror*meanerror);
		cout << "Estimated Loss: " << estimatedLoss << "\t" << "Estimated Loss Error: " << estimatedLossError << endl;
		
		point_x[runNumber] = totalEnergy/V;
		point_y[runNumber] = estimatedLoss;
		error_x[runNumber] = 0.;
		error_y[runNumber] = estimatedLossError;
		cout << "pointx: " << point_x[runNumber] << "\t" << "pointy: " << point_y[runNumber] << endl;
		cout << "errorx: " << error_x[runNumber] << "\t" << "errory: " << error_y[runNumber] << endl;
		
		Double_t p1 = f1->GetParameter(0);
		Double_t e1 = f1->GetParError(0);
		Double_t p2 = f1->GetParameter(1);
		Double_t e2 = f1->GetParError(1);
		cout << "p1: " << p1 << "\t" << "p2: " << p2 << "\t" << "e1: " << e1 << "\t" << "e2: " << e2 << endl;
		
		Histogram->Fit("f1", "R");
//		Histogram->Draw();
		
		p1 = f1->GetParameter(0);
		e1 = f1->GetParError(0);
		p2 = f1->GetParameter(1);
		e2 = f1->GetParError(1);
		cout << "p1: " << p1 << "\t" << "p2: " << p2 << "\t" << "e1: " << e1 << "\t" << "e2: " << e2 << endl;
		
		point_y[runNumber] = TMath::Abs(p2);
		error_y[runNumber] = e2;
		cout << "pointx: " << point_x[runNumber] << "\t" << "pointy: " << point_y[runNumber] << endl;
		cout << "errorx: " << error_x[runNumber] << "\t" << "errory: " << error_y[runNumber] << endl;
				
		geoManager->ClearTracks();
		
		cout << "End of run" << endl << endl;
	}
	
/*	
	{
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
		TH1F * Histogram2 = new TH1F("Histogram2","Avg Field Sampled by Neutron", nbins/5, 0.0, 2.);	
		TH1F * Histogram3a = new TH1F("Histogram3a","Number of collisions before loss", nbins, 0.0, 10000);	
		TH1F * Histogram3b = new TH1F("Histogram3b","Lifetime of neutron", nbins, 0.0, runTime);	
		TH1F * Histogram3c = new TH1F("Histogram3c","Percentage Diffuse", nbins, 0.0, 0.2);	
		TH1F * Histogram3d = new TH1F("Histogram3d","Percentage Specular", nbins, 0.8, 1.);	
	
		for (Int_t i = 0; i < particles; i++) {
			// Get each Track
			TVirtualGeoTrack* track = geoManager->GetTrack(i);
			TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
			Histogram1->Fill(particle->Vz());
			Histogram2->Fill(particle->AvgMagField());
			Histogram3a->Fill(particle->Bounces());
			Histogram3b->Fill(particle->T());
		
			Double_t diffusePercentage = 0;
			Double_t specularPercentage = 0;
			if (particle->Bounces() != 0) {
				diffusePercentage = static_cast<Double_t>(particle->DiffuseBounces())/particle->Bounces();
				specularPercentage = static_cast<Double_t>(particle->SpecularBounces())/particle->Bounces();
			} else {
				diffusePercentage = particle->DiffuseBounces();
				specularPercentage = particle->SpecularBounces();
			}
			Histogram3c->Fill(diffusePercentage);
			Histogram3d->Fill(specularPercentage);
		}
	
		// --------------------------------------------------------------------------------------
		// Fit Neutron Density versus Height
		TCanvas * histcanvas = new TCanvas("HistCanvas","Neutron Density versus height",0,0,800,800);
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
		TCanvas * histcanvas2 = new TCanvas("HistCanvas2","Avg Field Sampled by Neutron",0,0,800,800);
		histcanvas2->cd();
		Histogram2->SetLineColor(kBlack);
		Histogram2->SetXTitle("Avg Field (T)");
		Histogram2->SetYTitle("Number of Neutrons");
		Histogram2->Draw("");
	
		// -------------------------------------------------------------------------------------- 
		// -- Plot Bounces 
		TCanvas * histcanvas3 = new TCanvas("HistCanvas3","Number of bounces before loss",0,0,800,800);
		histcanvas3->Divide(2,2);
		histcanvas3->SetGrid();
		histcanvas3->cd(1);
		Histogram3a->SetLineColor(kBlack);
		Histogram3a->SetXTitle("Bounces before loss");
		Histogram3a->SetYTitle("Number of Neutrons");
		Histogram3a->Draw("");
		histcanvas3->cd(2);
		Histogram3b->SetXTitle("LifeTime (s)");
		Histogram3b->SetYTitle("Number of Neutrons");
		Histogram3b->Draw("");
		histcanvas3->cd(3);
		Histogram3c->SetXTitle("Percentage of Diffuse Reflections");
		Histogram3c->SetYTitle("Number of Neutrons");
		Histogram3c->Draw("");
		histcanvas3->cd(4);
		Histogram3d->SetXTitle("Percentage of Specular Reflections");
		Histogram3d->SetYTitle("Number of Neutrons");
		Histogram3d->Draw("");
	
		Double_t mean = Histogram3a->GetMean();
		Double_t meanerror = Histogram3a->GetMeanError();
		cout << mean << "\t" << meanerror << endl;
	
		Double_t estimatedLoss = 1./mean;
		Double_t estimatedLossError = TMath::Sqrt((-1./(mean*mean))*(-1./(mean*mean))*meanerror*meanerror);
		cout << estimatedLoss << "\t" << estimatedLossError << endl;
	}
*/	
	
	TCanvas * histcanvas4 = new TCanvas("HistCanvas4","Loss Function",0,0,800,800);
	histcanvas4->cd();
	
	TF1 *lossf = new TF1("lossf",lossFunc,0.,1.,1);
	lossf->SetParameter(0,f);
	lossf->GetXaxis()->SetTitle("E/V");
	lossf->GetYaxis()->SetTitle("Loss Probability");
	lossf->Draw();
	
	TGraphErrors* lossProb = new TGraphErrors(numberOfRuns, point_x, point_y, error_x, error_y);
	lossProb->SetTitle("TGraphErrors Example");
	lossProb->SetMarkerColor(4);
	lossProb->SetMarkerSize(1);
	lossProb->SetMarkerStyle(21);
	lossProb->Draw("PSame");

	
	#ifndef __CINT__
		theApp->Run();
	#endif
	///////////////////////////////////////////////////////////////////////////////////////
	runManager->Delete();
	
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
      return (2.*par[0]*((1./x[0])*TMath::ASin(TMath::Sqrt(x[0])) - TMath::Sqrt((1./x[0]) - 1.)));
}
