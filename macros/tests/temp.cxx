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
#include "TUCNParticleCloud.h"
#include "TUCNParticle.h"
#include "TUCNPolynomial.h"
#include "TUCNExperiment.h"
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
	gSystem->Load("libUCN"); // libUCN needs libGeom and libEG to be loaded first or it will complain. 
#endif

	TBenchmark benchmark;
	benchmark.Start("UCN Simulation");
	
	// ----------------------------------------------					
	// -- Create a UCNGeoManager
	TUCNGeoManager* myManager = new TUCNGeoManager("TestUCNGeoManager", "Derived class from TGeoManager");
			
	// -- Making Materials
   TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", 0,0,0);
   
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackhole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = myManager->MakeBox("TOP",blackHole,20,20,20);
	myManager->SetTopVolume(chamber);
			
	// -- Make a UCNTube 
	Double_t rMin = 0.0, rMax = 0.11, length = 0.4; 
	TGeoVolume* tube   = myManager->MakeTube("tube",boundary, rMin, rMax, length);
	TGeoVolume* innerTube  = myManager->MakeTube("innerTube",vacuum, rMin, rMax-0.01, length-0.01);
	
	tube->SetLineColor(36);
	tube->SetLineWidth(1);
	tube->SetVisibility(kTRUE);
	tube->SetTransparency(50);
	
	innerTube->SetLineColor(8);
	innerTube->SetLineWidth(1);
	innerTube->SetVisibility(kTRUE);
	innerTube->SetTransparency(80);
	
	// -- Define the transformation of the volume
	// Here is a general set-up of rotations and translations to be applied to a single volume. 
	TGeoRotation r1,r2; 
	r1.SetAngles(0,0,0);          	//	rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,0.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	
	// -- Create the nodes	
	tube->AddNode(innerTube,1);
	chamber->AddNode(tube,1, matrix);
		
	// -- Arrange and close geometry
   myManager->CloseGeometry();
	
	myManager->GetUCNNavigator()->BuildCache(kTRUE, kTRUE);
	myManager->GetUCNNavigator()->ResetAll();
	
	// -- Make a Grav Field
	Double_t fieldDir[3] = {0., 0., -1.};
	myManager->AddGravField(fieldDir[0], fieldDir[1], fieldDir[2]);
	// ----------------------------------------------					
	
	
	// ----------------------------------------------					
	// -- Create Particle Cloud
	Int_t particles = 100;
	Double_t totalEnergy = 100*Units::neV;
	TUCNParticleCloud* particleCloud = myManager->AddParticleCloud();
	particleCloud->UniformMonoEnergeticDistribution(particles, totalEnergy, innerTube, matrix);
		
	// -- Create Tracks
	myManager->MakeTracks(myManager->GetParticleCloud());


	TPolyLine3D* trackLine = new TPolyLine3D();
	

	for (Int_t id = 0; id < particles; id++) {
		TVirtualGeoTrack* track = myManager->GetTrack(id);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
		// Initialise track - Sets navigator's current point/direction/node to that of the particle
		myManager->SetCurrentTrack(track);
		myManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
		// Propagate track
		Double_t maxStepTime = 0.01*Units::s;
		Int_t steps = 100000;
//		Double_t runTime = 10000.0*Units::s;
		particle->Print();
		myManager->GetUCNNavigator()->PropagateTrack(track, steps, maxStepTime);
		
//		trackLine->SetPoint(0, particle->Vx(), particle->Vy(), particle->Vz());
//		myManager->GetUCNNavigator()->SetStep(1.0);
//		myManager->GetUCNNavigator()->Step();
//		myManager->GetUCNNavigator()->UpdateTrack(track, 0.0);
//		trackLine->SetPoint(1, particle->Vx(), particle->Vy(), particle->Vz());
		
	}

	cout << "NUMBER OF PARTICLES: " << particles << endl;
	cout << "NUMBER LOST: " << myManager->GetUCNNavigator()->GetNumberLost() << endl;
	cout << "NUMBER DETECTED: " << myManager->GetUCNNavigator()->GetNumberDetected() << endl;
	
	
	// Make track and add to list	
	benchmark.Stop("UCN Simulation");
	benchmark.Show("UCN Simulation");
			
	// ----------------------------------------------					
	// -- Draw Canvas
	Int_t width = 600;
	Int_t height = 600;
	Int_t positionX = 200;
	Int_t positionY = 10;
	TCanvas * canvas = new TCanvas("canvas", "UCNSIM: Final Particle Positions", positionX, positionY, width, height);
	canvas->Draw();
	
	// -- Draw Volumes
	myManager->GetTopVolume()->Draw();
	myManager->SetVisLevel(4);
	myManager->SetVisOption(0);
	
	// -- Draw Final Points
	TPolyMarker3D FinalPoints(particles, 1);
	for (Int_t i = 0; i < particleCloud->GetListOfParticles()->GetEntriesFast(); i++) {
	FinalPoints.SetPoint(i, particleCloud->GetParticle(i)->Vx(), particleCloud->GetParticle(i)->Vy(), particleCloud->GetParticle(i)->Vz());
	}
	
	FinalPoints.SetMarkerColor(2);
	FinalPoints.SetMarkerStyle(7);
	FinalPoints.Draw();
	
	// -- Draw Tracks
	// This will only work if the option to store tracks in the method PropagateTracks is turned on. 
	myManager->DrawTracks();
	
//	trackLine->Draw();
	
	
//	myManager->GetTrack(101)->Draw();
	// ----------------------------------------------					
	

				
#ifndef __CINT__
	theApp->Run();
#endif



	return 0;
}
