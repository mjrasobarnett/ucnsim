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
#include "TUCNRunManager.h"
#include "TUCNMagField.h"
#include "TUCNUniformMagField.h"
#include "TUCNRun.h"
#include "TUCNFieldManager.h"
#include "TUCNConfigFile.h"

#include "Constants.h"
#include "Units.h"

using std::cout;
using std::endl;
using std::string;

Bool_t BuildGeometry(TUCNGeoManager* geoManager, TUCNConfigFile& configFile);
Double_t ReadFermiPotential(TUCNConfigFile& configFile);


Int_t main(Int_t argc,Char_t **argv)
{
	
	string configFileName;
	if (argc == 2) {
		configFileName = argv[1];
	} else {
		cerr << "Usage:" << endl;
		cerr << "sandbox <configFile.txt>" << endl;
		return -1;
	}
	
	TUCNConfigFile configFile(configFileName);
	cout << argv[1] << endl;
	
//	TRint* theApp = new TRint("UCNSimApp", &argc, argv);
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Geometry Creation
	///////////////////////////////////////////////////////////////////////////////////////
	// Create the geoManager
	new TGeoManager("GeoManager", "Geometry Manager");
	// Create the UCNGeoManager
	TUCNGeoManager* geoManager = new TUCNGeoManager("UCNGeoManager", "UCNGeometry Manager");
	// Create the UCNNavigator and initialise in the UCNManager
	TUCNGeoNavigator* navigator = new TUCNGeoNavigator(gGeoManager);
	Int_t navigatorIndex = gGeoManager->AddNavigator(navigator);
	gGeoManager->SetCurrentNavigator(navigatorIndex);
	
	if(!BuildGeometry(geoManager, configFile)) {
		cerr << "Failed building geometry. Program aborting." << endl;
		return EXIT_FAILURE;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Field Initialisation
	///////////////////////////////////////////////////////////////////////////////////////
	TUCNFieldManager* fieldManager = new TUCNFieldManager();
	if(!(fieldManager->Initialise(configFile))) {
		cerr << "Failed in field initialisation. Program aborting." << endl;
		return EXIT_FAILURE;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Run Simulation
	///////////////////////////////////////////////////////////////////////////////////////
	TUCNRunManager* runManager = new TUCNRunManager();
	if (!(runManager->Initialise(configFile))) {
		cerr << "Failed to initialise the Runs. Program aborting." << endl;
		return EXIT_FAILURE;
	}
	
	navigator->DiffuseCoefficient(0.1);
	Double_t V = static_cast<TUCNGeoMaterial*>(gGeoManager->GetMaterial("Boundary Material"))->FermiPotential();
	Double_t f = static_cast<TUCNGeoMaterial*>(gGeoManager->GetMaterial("Boundary Material"))->Eta();
	Double_t totalEnergy = 0;
	cout << "V: " << V << "\t" << "f: " << f << endl;
	
	
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
	
	return EXIT_SUCCESS;
}

Bool_t BuildGeometry(TUCNGeoManager* geoManager, TUCNConfigFile& configFile)
{
	// -------------------------------------
	// BUILDING GEOMETRY
	cout << "-------------------------------------------" << endl;
	cout << "Building Geometry..." << endl;
	cout << "-------------------------------------------" << endl;
	
	// Read in value of FermiPotential
	Double_t V = ReadFermiPotential(configFile);
	// Read in value of f
	Double_t f = configFile.GetFloat("f", "Geometry");
	// Check values were set
	if (f == 0.0 || V == 0.0) {
		cout << "Boundary Material properties, f or V, have not been set! Check ConfigFile and try again." << endl;
		return kFALSE;
	}
	// Determine W
	Double_t W = f*V;
	
	// Materials
	TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", V, W);
	cout << "Boundary Material has been defined with V (neV): " << V/Units::neV << "  W (neV): " << W/Units::neV << endl;
	
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackHole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
	
	// -- Making Top Volume
	TGeoVolume* chamber = geoManager->MakeUCNBox("TOP",blackHole,20,20,20);
	gGeoManager->SetTopVolume(chamber);
	
	// -- Make a GeoTube object via the UCNGeoManager
	Double_t rMin = 0.0, rMax = 0.236, length = 0.121; 
	TGeoVolume* tube = geoManager->MakeUCNTube("tube",boundary, rMin, rMax, length/2.);
	TGeoVolume* innerTube = geoManager->MakeUCNTube("innerTube",vacuum, rMin, rMax-0.001, (length-0.001)/2.);
	
	
	// -- Define the transformation of the volume
	TGeoRotation r1,r2; 
	r1.SetAngles(0,0,0);          //rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,length/2.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	
	TGeoVolume* volume = tube;
	TGeoVolume* innerVolume = innerTube;
	
	// -- Create the nodes	
	volume->AddNode(innerVolume, 1);
	chamber->AddNode(volume, 1, matrix);
	
	// -- Define the Source in our geometry where we will create the particles
	geoManager->SetSourceVolume(innerVolume);
	geoManager->SetSourceMatrix(matrix);
	
	// -- Arrange and close geometry
	gGeoManager->CloseGeometry();
	cout << "-------------------------------------------" << endl;
	cout << "Geometry has been created succesfully" << endl;
	cout << "-------------------------------------------" << endl;
	
	return kTRUE;
}

Double_t ReadFermiPotential(TUCNConfigFile& configFile) {
	// Read ConfigFile for the fermiPotential. Two possible ways this can be stored (depending on units). Function checks for both
	// and returns the one it finds. 
	Double_t value = 0.0;
	Double_t fermiPotentialneV  = configFile.GetFloat("FermiPotential(neV)", "Geometry")*Units::neV;
	Double_t fermiPotentialm    = configFile.GetFloat("FermiPotential(m)", "Geometry")*Constants::height_equivalent_conversion;
	cout << "Reading ConfigFile to determine FermiPotential..." << endl;
	// Determine which of these two values is zero (if any) and returning the remaining value.
	if (fermiPotentialneV == 0.0) {
		if (fermiPotentialm == 0.0) {
			cout << "No Fermi-potential has been set!" << endl;
		} else {
			value = fermiPotentialm; 
		}
	} else {
		if (fermiPotentialm == 0.0) {
			value = fermiPotentialneV;
		} else {
			cout << "Both possible definitions the Fermi-potential have been set! Checking to see if they differ..." << endl;
			if (fermiPotentialneV == fermiPotentialm) {
				cout << "Both have the same value." << endl;
				value = fermiPotentialneV;
			} else {
				cout << "Both are different. No value will be set." << endl;
			}
		}
	}
	return value;
}

