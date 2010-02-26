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
#include "TUCNFieldManager.h"

#include "Constants.h"
#include "Units.h"

using std::cout;
using std::endl;

void BuildGeometry(TUCNGeoManager* geoManager);

Int_t main(Int_t argc,Char_t **argv)
{
	TRint* theApp = new TRint("UCNSimApp", &argc, argv);
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Geometry Creation
	///////////////////////////////////////////////////////////////////////////////////////
	// Create the geoManager
	TUCNGeoManager* geoManager = new TUCNGeoManager("GeoManager", "Geometry Manager");
	// Create the UCNNavigator and initialise in the UCNManager
	Info("TUCNRun", "Creating a new Navigator...");
	TUCNGeoNavigator* navigator = new TUCNGeoNavigator(geoManager);
	Int_t navigatorIndex = geoManager->AddNavigator(navigator);
	geoManager->SetCurrentNavigator(navigatorIndex);
	
	BuildGeometry(geoManager);
	
	cout << static_cast<TUCNGeoManager*>(gGeoManager)->GetSourceVolume()->GetName() << endl;
	cout << static_cast<TUCNGeoManager*>(gGeoManager)->GetSourceMatrix()->GetName() << endl;
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Field Creation
	///////////////////////////////////////////////////////////////////////////////////////
	TUCNFieldManager* fieldManager = new TUCNFieldManager();
	TUCNGravField* gravField =	fieldManager->AddGravField();
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Run Simulation
	///////////////////////////////////////////////////////////////////////////////////////
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
		
		run->Initialise(particles, totalEnergy, gravField);
	
		// -- Propagate the tracks according to the run parameters
		run->PropagateAllTracks(runTime, maxStepTime, gravField);	
	
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
	
//	benchmark.Stop("UCNSim");
//	benchmark.Show("UCNSim");
	
	theApp->Run();
	
	return 0;
}

void BuildGeometry(TUCNGeoManager* geoManager)
{
	// -------------------------------------
	// BUILDING GEOMETRY
	cerr << "Building Geometry..." << endl;
	// Input parameters to calculate f  -- All numbers come from Mike P (presumable from data)
	// for a specific energy group, from which we can calculate f. 
	Double_t observedLifetime = 150.*Units::s;
	Double_t fermiPotential = (0.91*Units::m)*Constants::height_equivalent_conversion;
	Double_t totalEnergy = (0.52*Units::m)*Constants::height_equivalent_conversion;
	Double_t initialVelocity = TMath::Sqrt(2.*totalEnergy/Constants::neutron_mass);
	Double_t meanFreePath = 0.16*Units::m;
	
	// Calculate f = W/V and hence W
	Double_t X = totalEnergy/fermiPotential;
	Double_t L = 2.0*((1./X)*TMath::ASin(TMath::Sqrt(X)) - TMath::Sqrt((1./X) - 1.));
	Double_t f = meanFreePath/(initialVelocity*observedLifetime*L);
	Double_t W = f*fermiPotential;
	cerr << "Input Parameters: " << endl;
	cerr << "E: " << totalEnergy/Units::neV << "\t" << "V: " << fermiPotential/Units::neV << "\t" << "E/V: " << X << "\t"<< "L: " << L << endl;
	cerr << "f: " << f << "\t" << "W: " << W/Units::neV << endl;
	
	// Materials
	TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", fermiPotential, W);
	
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackHole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = geoManager->MakeUCNBox("TOP",blackHole,20,20,20);
	geoManager->SetTopVolume(chamber);
				
	// -- Make a GeoTube object via the UCNGeoManager
	Double_t rMin = 0.0, rMax = 0.236, length = 0.121; 
	TGeoVolume* tube   = geoManager->MakeUCNTube("tube",boundary, rMin, rMax, length/2.);
	TGeoVolume* innerTube  = geoManager->MakeUCNTube("innerTube",vacuum, rMin, rMax-0.001, (length-0.001)/2.);
	
	
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
	volume->AddNode(innerVolume,1);
	chamber->AddNode(volume,1, matrix);
	
	// -- Define the Source in our geometry where we will create the particles
	geoManager->SetSourceVolume(innerVolume);
	geoManager->SetSourceMatrix(matrix);
	
	// -- Arrange and close geometry
   geoManager->CloseGeometry();
}
