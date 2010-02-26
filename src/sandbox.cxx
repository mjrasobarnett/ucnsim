#ifndef __CINT__

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "TCanvas.h"
#include "TBenchmark.h"
#include "TSystem.h"
#include "TRint.h"
#include "TRandom.h"
#include "TObjArray.h"

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


#include "Constants.h"
#include "Units.h"


using std::cout;
using std::endl;

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
	// -- Create a Run Manager
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
	
	TGeoVolume* volume = box;
	TGeoVolume* innerVolume = innerBox;
	
	// -- Create the nodes	
	volume->AddNode(innerVolume,1);
	chamber->AddNode(volume,1, matrix);
	
	cout << "Top Volume Nodes: " << chamber->GetNodes()->GetEntriesFast() << endl;
	cout << "Manager - No. of Volumes: " << geoManager->GetListOfVolumes()->GetEntriesFast() << endl;
	cout << "Manager - No. of Shapes: " << geoManager->GetListOfShapes()->GetEntriesFast() << endl;
	cout << "Manager - No. of Nodes: " << geoManager->GetListOfNodes()->GetEntriesFast() << endl;
		
	// -- Arrange and close geometry
   geoManager->CloseGeometry();
	geoManager->GetTopVolume()->Draw();
	geoManager->SetVisLevel(4);
	geoManager->SetVisOption(0);
	
	///////////////////////////////////////////////////////////////////////////////////////
	
	// -- Include/Don't include gravity
	runManager->TurnGravityOn();

	// -- Load / Define the parameters of the Run
	Double_t runTime = 100.*Units::s;
	Double_t maxStepTime = 0.10*Units::s;
	Int_t particles = 50000;
	Double_t totalEnergy = 200*Units::neV;
	
	// Generating mono-energetic particles inside the source volume
	cout << "Generating " << particles << " particles..."	<< endl;
	runManager->GenerateMonoEnergeticParticles(innerVolume, matrix, particles, totalEnergy);
	cout << "Particle's created. Preparing to Propagate..." << endl;
	
	///////////////////////////////////////////////////////////////////////////////////////
	// WRITE OUT DATA
	ofstream initialout("initialdata.txt");
	ofstream finalout("finaldata.txt");
	// --------------------------------------------------------------------------------------
	initialout << "Number" << "\t" << "Neutron Time" << "\t" << "Neutron Energy" << "\t";
	initialout << "X Position" << "\t" << "Y Position" << "\t" << "Z Position" << "\t" << "PX" << "\t" << "PY" << "\t" << "PZ" << "\t";
	initialout << "Detected" << "\t" << "Decayed" << "\t" << "Lost" << "\t" << endl;
	// --------------------------------------------------------------------------------------
	finalout << "Number" << "\t" << "Neutron Time" << "\t" << "Neutron Energy" << "\t";
	finalout << "X Position" << "\t" << "Y Position" << "\t" << "Z Position" << "\t" << "PX" << "\t" << "PY" << "\t" << "PZ" << "\t";
	finalout << "Detected" << "\t" << "Decayed" << "\t" << "Lost" << "\t" << endl;
	
	Int_t tracks = geoManager->GetNtracks();

	for (Int_t id = 0; id < tracks; id++) {
		// Get each Track
		TVirtualGeoTrack* track = geoManager->GetTrack(id);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
		// Write out the initial particle data
		initialout << id << "\t" << particle->T() /Units::s << "\t" << particle->Energy() /Units::neV << "\t";
		initialout << particle->Vx() /Units::m << "\t" << particle->Vy() /Units::m << "\t" << particle->Vz() /Units::m << "\t";
		initialout << particle->Px() /Units::eV << "\t" << particle->Py() /Units::eV << "\t" << particle->Pz() /Units::eV << "\t";
		initialout << particle->Detected() << "\t" << particle->Decayed() << "\t" << particle->Lost() << "\t" << endl;
	}
	
	// -- Propagate the tracks according to the run parameters
	runManager->PropagateAllTracks(runTime, maxStepTime);	
	
	cout << "-------------------------------------------" << endl;
	cout << "Propagation Results: " << endl;
	cout << "Total Particles: " << geoManager->GetNtracks() << endl;
	cout << "Number Detected: " << geoManager->GetNumberDetected() << endl;
	cout << "Number Lost: " << geoManager->GetNumberLost() << endl;
	cout << "Number Decayed: " << geoManager->GetNumberDecayed() << endl;
	cout << "-------------------------------------------" << endl;
	
	for (Int_t id = 0; id < tracks; id++) {
		// Get each Track
		TVirtualGeoTrack* track = geoManager->GetTrack(id);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
		// Write out the initial particle data
		finalout << id << "\t" << particle->T() /Units::s << "\t" << particle->Energy() /Units::neV << "\t";
		finalout << particle->Vx() /Units::m << "\t" << particle->Vy() /Units::m << "\t" << particle->Vz() /Units::m << "\t";
		finalout << particle->Px() /Units::eV << "\t" << particle->Py() /Units::eV << "\t" << particle->Pz() /Units::eV << "\t";
		finalout << particle->Detected() << "\t" << particle->Decayed() << "\t" << particle->Lost() << "\t" << endl;
	}
	///////////////////////////////////////////////////////////////////////////////////////
	
	// Draw Points
	geoManager->GetTopVolume()->Draw();
	geoManager->SetVisLevel(4);
	geoManager->SetVisOption(0);
	
	TPolyMarker3D FinalPoints(particles, 1);
	for (Int_t id = 0; id < tracks; id++) {
		// Get each Track
		TVirtualGeoTrack* track = geoManager->GetTrack(id);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
		FinalPoints.SetPoint(id, particle->Vx(), particle->Vy(), particle->Vz());
	}
	FinalPoints.SetMarkerColor(2);
	FinalPoints.SetMarkerStyle(6);
	FinalPoints.Draw();
				
#ifndef __CINT__
	theApp->Run();
#endif
	return 0;
}


