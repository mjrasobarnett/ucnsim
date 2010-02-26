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
Double_t densityf(Double_t*x, Double_t*par);

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
	
	// Materials
	TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", 0,0,0);

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
	Double_t boxX = 0.11, boxY = 0.11, boxZ = 0.91; 
	TGeoVolume* box   = geoManager->MakeUCNBox("box",boundary, boxX, boxY, boxZ);
	TGeoVolume* innerBox  = geoManager->MakeUCNBox("innerbox",vacuum, boxX-0.01, boxY-0.01, boxZ-0.01);
	
	// -- Make a GeoTube object via the UCNGeoManager
	Double_t rMin = 0.0, rMax = 0.11, length = 0.91; 
	TGeoVolume* tube   = geoManager->MakeUCNTube("tube",boundary, rMin, rMax, length);
	TGeoVolume* innerTube  = geoManager->MakeUCNTube("innerTube",vacuum, rMin, rMax-0.01, length-0.01);
	
	
	// -- Define the transformation of the volume
	TGeoRotation r1,r2; 
	r1.SetAngles(0,0,0);          //rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,0.); 
	TGeoTranslation t2(0.,0.,0.9); 
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
	
	runManager->TurnGravityOn();

	///////////////////////////////////////////////////////////////////////////////////////
	// -- Mag Field
	TUCNUniformMagField* magfield = new TUCNUniformMagField("Uniform magnetic field", 0.0,1.0,1.0);
	
	geoManager->AddMagField(magfield);
	TUCNMagField* testfield = geoManager->GetMagField("Uniform magnetic field");
	cout << testfield << "\t" << magfield << endl;
	
	// -- Load / Define the parameters of the Run
	Double_t runTime = 10.*Units::s;
	Double_t maxStepTime = 1.00*Units::s;
	Int_t particles = 1000;
	Double_t totalEnergy = 200*Units::neV;
	
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
/*	// -- Create a file to store the tracks
	TFile* file = new TFile("tracks_test.root","RECREATE");
	if ( file->IsOpen() ) {
		cerr << "Opened tracks.root" << endl;
	}
	else {
		cerr << "Could not open tracks.root" << endl;
		return 1;
	}
	runManager->WriteOutData(file); 
*/
	
	///////////////////////////////////////////////////////////////////////////////////////
	// Draw Final Positions 
		
	TCanvas * canvas1 = new TCanvas("canvas1", "Final Particle Positions", 800, 10, 600, 600);
	TPolyMarker3D* finalPoints = new TPolyMarker3D(geoManager->GetNtracks(), 1);
	runManager->DrawParticles(canvas1, finalPoints);
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- FITTING 
	
	Int_t nbins = 50;
	Double_t maxlength = 0.9;
		
	// Plot Histogram
	TH1F * Histogram1 = new TH1F("Histogram1","Neutron Density versus height", nbins, 0.0, 2.*maxlength);	
	TH1F * Histogram2 = new TH1F("Histogram2","Avg Field Sampled by Neutron", nbins/5, 0.0, 2.);	
	
	for (Int_t i = 0; i < particles; i++) {
		// Get each Track
		TVirtualGeoTrack* track = geoManager->GetTrack(i);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
		Histogram1->Fill(particle->Vz());
		Histogram2->Fill(particle->AvgMagField());
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
	
	#ifndef __CINT__
		theApp->Run();
	#endif
	///////////////////////////////////////////////////////////////////////////////////////
	runManager->Delete();
	
	return 0;
}

// -------------------------------------------------------------------------------------- 
Double_t densityf(Double_t*x, Double_t*par)
{
	Double_t value = (total_energy - (Constants::neutron_mass)*(Constants::grav_acceleration)*x[0])/total_energy;
	assert(value >= 0.0);
	return par[0]*sqrt(value);
}


