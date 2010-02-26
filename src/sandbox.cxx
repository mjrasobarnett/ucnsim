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
	
	Int_t numberOfRuns = 1;
	// Need to Add the runs before we initialise the geometry (because we create the navigators with each run). 
	// and the navigators need to be created before we close the geometry.
	runManager->CreateRuns(numberOfRuns);
	
	runManager->InitialiseGeometry();
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Run Simulation
	Double_t runTime = 10.*Units::s;
	Double_t maxStepTime = 1.00*Units::s;
	Int_t particles = 1000;
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
	
	TFile* file = new TFile("runs.root","RECREATE");
	runManager->WriteToFile(file);
	

	benchmark.Stop("UCNSim");
	benchmark.Show("UCNSim");
	
	#ifndef __CINT__
		theApp->Run();
	#endif
	///////////////////////////////////////////////////////////////////////////////////////
//	runManager->Delete();
	
	return 0;
}
