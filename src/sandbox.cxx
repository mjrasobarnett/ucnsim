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
	// -- Read in File name
	TString geomfile;
/*	if (argc == 2) {
		geomfile = argv[1];
	} else {
		cerr << "Usage:" << endl;
		cerr << "sandbox <geom.root> " << endl;
		return -1;
	}
*/	
	TRint *theApp = new TRint("UCNSimApp", &argc, argv);
	TBenchmark benchmark;
	benchmark.Start("UCNSim");
	
	geomfile = "geom/geom.root";
	
	// -- Read the file into memory
	cerr << "Reading geometry from " << geomfile << endl;
	TFile * inputFile = new TFile(geomfile,"UPDATE");
	if ( !inputFile->IsOpen() ) {
		cerr << "Could not open file " << geomfile << endl;
		cerr << "Exiting..." << endl;
		exit(-1);
	}
	// -- Check contents of the file
	inputFile->ls();	
	TIter next(inputFile->GetListOfKeys()); 
	TKey* key;
	while ((key=(TKey*)next())) {
		printf("key: %s points to an object of class: %s at %i , with cycle number: %i \n", key->GetName(), key->GetClassName(),key->GetSeekKey(),key->GetCycle());
	}
	
	// Get the GeoManager From the File
	TUCNGeoManager* geoManager = 0;
	inputFile->GetObject("GeoManager;1", geoManager);
	inputFile->ls();
//	inputFile->Close();
	
	geoManager->GetCurrentNode()->Print();
	cout << geoManager->GetListOfNavigators()->GetEntries() << endl;
	geoManager->GetCurrentNode()->Print();
	
	inputFile->ls();
	
	geoManager->GetSourceMatrix()->GetTranslation();
	
	cout << geoManager->GetListOfMatrices()->GetEntries() << endl;
	TObjArray* objarray = geoManager->GetListOfMatrices();
	for (Int_t i = 0; i < geoManager->GetListOfMatrices()->GetEntries(); i++) {
		static_cast<TGeoMatrix*>(objarray->At(i))->Print();
	}
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Geometry Creation
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Run Simulation
	TUCNRunManager* runManager = new TUCNRunManager();
	
	Int_t numberOfRuns = 1;
	// Need to Add the runs before we initialise the geometry (because we create the navigators with each run). 
	// and the navigators need to be created before we close the geometry.
	runManager->CreateRuns(numberOfRuns);
	
	Double_t runTime = 10000.*Units::s;
	Double_t maxStepTime = 1.00*Units::s;
	Int_t particles = 1000;
	Double_t V = static_cast<TUCNGeoMaterial*>(geoManager->GetMaterial("Boundary Material"))->FermiPotential();
	Double_t f = static_cast<TUCNGeoMaterial*>(geoManager->GetMaterial("Boundary Material"))->Eta();
	Double_t totalEnergy = 0;
	cout << "V: " << V << "\t" << "f: " << f << endl;
	
	for (Int_t runNumber = 0; runNumber < numberOfRuns; runNumber++) {
		totalEnergy = 0.95*V; // (1.0/10.0)*(runNumber+1)*V; //(0.12*Units::m)*Constants::height_equivalent_conversion; 
		
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

		geoManager->ClearTracks();
		
		cout << "End of run" << endl << endl;
	}
	
	geoManager->GetCurrentNode()->Print();
	cout << geoManager->GetListOfNavigators()->GetEntries() << endl;
	cout << geoManager->GetCurrentNavigator() << endl;
	
/*	TFile* outputFile = new TFile("runs-0.95V.root","RECREATE");
	if ( !outputFile->IsOpen() ) {
		cerr << "Could not open output file " << endl;
		cerr << "Exiting..." << endl;
		exit(-1);
	}
	
	runManager->Write();
//	outputFile->ls();
	inputFile->ls();
	
	geoManager->Write();
//	outputFile->ls();
	inputFile->ls();
	inputFile->Close();
//	outputFile->Close();
*/
	benchmark.Stop("UCNSim");
	benchmark.Show("UCNSim");
	
	theApp->Run();
	
	return 0;
}
