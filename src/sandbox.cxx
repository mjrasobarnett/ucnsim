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


#include "Constants.h"
#include "Units.h"


using std::cout;
using std::endl;

Double_t total_energy = 200*neV;
Double_t f = 0.000462963;

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
	
	Double_t height_equivalent_units = Constants::grav_acceleration*Constants::neutron_mass;
	
	Double_t observedLifetime = 150.*Units::s;
	Double_t fermiPotential = (0.91*Units::m)*height_equivalent_units; 
	Double_t totalEnergy = (0.52*Units::m)*height_equivalent_units;	
	Double_t initialVelocity = TMath::Sqrt(2*totalEnergy/Constants::neutron_mass);
	Double_t meanFreePath = 0.16*Units::m;
	
	Double_t V = fermiPotential;
	Double_t E = totalEnergy;
	cout << initialVelocity << endl;
	
	Double_t L = ((V/E)*TMath::ASin(TMath::Sqrt(E/V)) - TMath::Sqrt((V - E)/E));
	cout << "E: " << E/Units::neV << "\t" << "V: " << V/Units::neV << "\t" << "E/V: " << E/V << "\t"<< "L: " << L << endl;
	
	Double_t f = meanFreePath/(initialVelocity*observedLifetime*L);
	Double_t W = f*V;
	cout << "f: " << f << "\t" << "W: " << W/Units::neV << endl;
	
	Double_t estimatedProbOfLoss = 2*f*L;
	cout << "estimatedProbOfLoss: " << estimatedProbOfLoss << "\t" << "Av. number of collisions: " << 1./estimatedProbOfLoss << endl;
	
	
	totalEnergy = 0.2*V;	
	E = totalEnergy;
	
	
	// Materials
	TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", V, W);
	
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackHole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = geoManager->MakeBox("TOP",blackHole,20,20,20);
	geoManager->SetTopVolume(chamber);
			
	// -- Make a GeoBBox object via the UCNGeoManager
//	Double_t boxX = 0.11, boxY = 0.11, boxZ = 0.91; 
//	TGeoVolume* box   = geoManager->MakeUCNBox("box",boundary, boxX, boxY, boxZ);
//	TGeoVolume* innerBox  = geoManager->MakeUCNBox("innerbox",vacuum, boxX-0.01, boxY-0.01, boxZ-0.01);
	
	// -- Make a GeoTube object via the UCNGeoManager
	Double_t rMin = 0.0, rMax = 0.236, halfLength = 0.121; 
	TGeoVolume* tube   = geoManager->MakeUCNTube("tube",boundary, rMin, rMax, halfLength);
	TGeoVolume* innerTube  = geoManager->MakeUCNTube("innerTube",vacuum, rMin, rMax-0.001, halfLength-0.001);
	
	
	// -- Define the transformation of the volume
	TGeoRotation r1,r2; 
	r1.SetAngles(0,0,0);          //rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,0.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	
	TGeoVolume* volume = tube;
	TGeoVolume* innerVolume = innerTube;
	
	// -- Create the nodes	
	volume->AddNode(innerVolume,1);
	chamber->AddNode(volume,1, matrix);
	
	cout << "Top Volume Nodes: " << chamber->GetNodes()->GetEntriesFast() << endl;
	cout << "Manager - No. of Volumes: " << geoManager->GetListOfVolumes()->GetEntriesFast() << endl;
	cout << "Manager - No. of Shapes: " << geoManager->GetListOfShapes()->GetEntriesFast() << endl;
	cout << "Manager - No. of Nodes: " << geoManager->GetListOfNodes()->GetEntriesFast() << endl;
		
	// -- Arrange and close geometry
   geoManager->CloseGeometry();
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Run Simulation
	Double_t runTime = 800.*Units::s;
	Double_t maxStepTime = 1.00*Units::s;
	Int_t particles = 1000;
	
	runManager->TurnGravityOn();
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Mag Field
	TUCNUniformMagField* magfield = new TUCNUniformMagField("Uniform magnetic field", 0.0,1.0,1.0);
	
	geoManager->AddMagField(magfield);
	TUCNMagField* testfield = geoManager->GetMagField("Uniform magnetic field");
	cout << testfield << "\t" << magfield << endl;
	
	// Generating mono-energetic particles inside the source volume
	cout << "Generating " << particles << " particles..."	<< endl;
	runManager->GenerateMonoEnergeticParticles(innerVolume, matrix, particles, totalEnergy);
	cout << "Particle's created. Preparing to Propagate..." << endl;
	
	// -- Propagate the tracks according to the run parameters
	runManager->PropagateAllTracks(runTime, maxStepTime);	
	
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
//	runManager->WriteOutData(file); 

	
	///////////////////////////////////////////////////////////////////////////////////////
	// Draw Final Positions 
		
	TCanvas * canvas1 = new TCanvas("canvas1", "Final Particle Positions", 800, 10, 600, 600);
	TPolyMarker3D* finalPoints = new TPolyMarker3D(geoManager->GetNtracks(), 1);
	runManager->DrawParticles(canvas1, finalPoints);
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- FITTING 
	
	Int_t nbins = 50;
	Double_t maxlength = 0.12;
		
	// Plot Histogram
	TH1F * Histogram1 = new TH1F("Histogram1","Neutron Density versus height", nbins, 0.0, maxlength);	
	TH1F * Histogram2 = new TH1F("Histogram2","Avg Field Sampled by Neutron", nbins/5, 0.0, 2.);	
	TH1F * Histogram3a = new TH1F("Histogram3a","Number of collisions before loss", nbins, 0.0, 50000);	
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
	cout << E/V << endl;
	cout << mean << "\t" << meanerror << endl;
	
	Double_t estimatedLoss = 1./mean;
	Double_t estimatedLossError = TMath::Sqrt((-1./(mean*mean))*(-1./(mean*mean))*meanerror*meanerror);
	cout << estimatedLoss << "\t" << estimatedLossError << endl;
	
	TCanvas * histcanvas4 = new TCanvas("HistCanvas4","Loss Function",0,0,800,800);
	histcanvas4->cd();
	
//	Double_t point_x[9] = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
//	Double_t point_y[9] = {0.000518506, 0.000555893, 0.000542289, 0.000642828, 0.00065903, 0.000751564, 0.000830931, 0.000869, 0.00109959};	
//	Double_t error_x[9] = {0,0,0,0,0,0,0,0,0};
//	Double_t error_y[9] = {1.30748e-05, 1.39742e-05, 1.38661e-05, 1.70123e-05, 1.74454e-05, 2.02451e-05, 2.30679e-05, 2.47773e-05, 3.15899e-05};
	
	Double_t point_x[1];
	Double_t point_y[1];	
	Double_t error_x[1];
	Double_t error_y[1];
	
	point_x[0] = E/V;
	point_y[0] = estimatedLoss;
	error_x[0] = 0.;
	error_y[0] = estimatedLossError;
	
	TF1 *lossf = new TF1("lossf",lossFunc,0.,1.,1);
	lossf->SetParameter(0,f);
	lossf->GetXaxis()->SetTitle("E/V");
	lossf->GetYaxis()->SetTitle("Loss Probability");
	lossf->Draw();
	
	TGraphErrors* lossProb = new TGraphErrors(1, point_x, point_y, error_x, error_y);
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
