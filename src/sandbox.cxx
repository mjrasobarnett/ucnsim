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

using std::cout;
using std::endl;
using std::cerr;
using std::string;

Bool_t BuildGeometry(TUCNGeoManager* geoManager, TUCNConfigFile& configFile);
Double_t ReadFermiPotential(TUCNConfigFile& configFile);


Int_t main(Int_t argc,Char_t **argv)
{
	///////////////////////////////////////////////////////////////////////////////////////
	// Read in ConfigFile
	///////////////////////////////////////////////////////////////////////////////////////
	string configFile;
	if (argc == 2) {
		configFile= argv[1];
	} else {
		cerr << "Usage:" << endl;
		cerr << "sandbox <configFile.txt>" << endl;
		return -1;
	}
	
	
//	TRint* theApp = new TRint("UCNSimApp", &argc, argv);
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Initialise Simulation
	///////////////////////////////////////////////////////////////////////////////////////
	TUCNExperiment* experiment = new TUCNExperiment(configFile);
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
//	navigator->DiffuseCoefficient(0.1);
	
/*	for (Int_t runNumber = 0; runNumber < numberOfRuns; runNumber++) {
		totalEnergy = 0.57*V; // (1.0/10.0)*(runNumber+1)*V; //(0.12*Units::m)*Constants::height_equivalent_conversion; 
		
		TUCNRun* run = runManager->GetRun(runNumber);
		cout << "Run number: " << runNumber << "\t" << "called: " << run->GetName() << endl;
		cout << "totalEnergy: " << totalEnergy << endl;
		
		run->Initialise(particles, totalEnergy, runTime, maxStepTime, geoManager, gravField);
	
		// -- Propagate the tracks according to the run parameters
		run->PropagateTracks(fieldManager);	
	
		cout << "-------------------------------------------" << endl;
		cout << "Propagation Results: " << endl;
		cout << "Total Particles: " << gGeoManager->GetNtracks() << endl;
		cout << "Number Detected: " << run->Detected() << endl;
		cout << "Number Lost To Boundary: " << run->LostToBoundary() << endl;
		cout << "Number Decayed: " << run->Decayed() << endl;
		cout << "-------------------------------------------" << endl;

		gGeoManager->ClearTracks();
		
		cout << "End of run" << endl << endl;
	}
	
	TString outputData = "data/magfield-0.01Diff-0.5-noloss.root";
	cerr << "Writing out data to file: " << outputData << endl;
	TFile* outputFile = new TFile(outputData,"RECREATE");
	if ( !outputFile->IsOpen() ) {
		cerr << "Could not open output file " << endl;
		cerr << "Exiting..." << endl;
		exit(-1);
	}
	
	runManager->Write();
	fieldManager->Write();
	outputFile->ls();
	outputFile->Close();
	
	TString outputGeom = "geom/magfieldgeom.root";
	cerr << "Writing out geometry to file: " << outputGeom << endl;
	gGeoManager->Export(outputGeom);
	
	
//	benchmark.Stop("UCNSim");
//	benchmark.Show("UCNSim");
*/	
//	theApp->Run();
	
	delete experiment;
		
	return EXIT_SUCCESS;
}

