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
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Export to File
	///////////////////////////////////////////////////////////////////////////////////////
	if (!(experiment->Export())) {
		cerr << "Experiment failed to write to File. Program aborting." << endl;
		return EXIT_FAILURE;
	}
	
//	navigator->DiffuseCoefficient(0.1);
//	benchmark.Stop("UCNSim");
//	benchmark.Show("UCNSim");
//	theApp->Run();

	cout << endl << endl << "END OF SIMULATION" << endl << endl << endl;
	delete experiment;		
	return EXIT_SUCCESS;
}

