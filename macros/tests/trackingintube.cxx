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
	
	// _________________________________________________________________________________________
	// This program creates a tube nested inside a rotated box, and propagates a particle (or particles)
	// around it for a set period of time. THe aim is to test the methods related to reflecting off of the inside
	// and outside of the tube.  
	// _________________________________________________________________________________________ 
	
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
			
	// -- Make a UCNBox
	Double_t boxX = 0.5, boxY = 0.5, boxZ = 0.5; 
	TGeoVolume* box   = myManager->MakeBox("box",boundary, boxX, boxY, boxZ);
	TGeoVolume* innerBox  = myManager->MakeBox("innerbox",vacuum, boxX-0.01, boxY-0.01, boxZ-0.01);
	
	// -- Make a UCNTube 
	Double_t rMin = 0.0, rMax = 0.11, length = 0.6; 
	TGeoVolume* tube   = myManager->MakeTube("tube",boundary, rMin, rMax, length);
	TGeoVolume* innertube  = myManager->MakeTube("innertube",vacuum, rMin, rMax-0.01, length-0.01);
	
	tube->SetLineColor(36);
	tube->SetLineWidth(1);
	tube->SetVisibility(kTRUE);
	tube->SetTransparency(50);
	
	innertube->SetLineColor(38);
	innertube->SetLineWidth(1);
	innertube->SetVisibility(kTRUE);
	innertube->SetTransparency(80);
	
	// -- Define the transformation of the volume
	// Here is a general set-up of rotations and translations to be applied to a single volume. 
	TGeoRotation r1,r2; 
	r1.SetAngles(0,30,0);          	//	rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,0.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	
	// -- Create the nodes	
	tube->AddNode(innertube,1);
	innerBox->AddNode(tube,1);
	box->AddNode(innerBox,1);
	chamber->AddNode(box,1, matrix);
		
	// -- Arrange and close geometry
   myManager->CloseGeometry();

	// -- Make a Grav Field
	Double_t fieldDir[3] = {0., 0., -1.};
	myManager->AddGravField(fieldDir[0], fieldDir[1], fieldDir[2]);
	// ----------------------------------------------					
	
	
	// ----------------------------------------------					
	// -- Create Particle Cloud
	Int_t particles = 10;
	Double_t totalEnergy = 100*Units::neV;
	TUCNParticleCloud* particleCloud = myManager->AddParticleCloud();
	particleCloud->UniformMonoEnergeticDistribution(particles, totalEnergy, innerBox, matrix);
		
	// -- Create Tracks
	myManager->MakeTracks(myManager->GetParticleCloud());
	
	// -- Propagate Tracks
	Double_t maxStepTime = 0.01*Units::s;

	Int_t steps = 1000;
	myManager->PropagateTracks(steps, maxStepTime, kTRUE);
	
//	Double_t runtime = 1.*Units::s;
//	myManager->PropagateTracks(runtime, maxStepTime, kFALSE, kTRUE);
	// ----------------------------------------------					
	
	
	// -- Testing method IsNextPointOnTube
//	TUCNParticle* myParticle = particleCloud->GetParticle(0);
//	myParticle->Print();
//	Double_t point[3] = {0., 0.1, 0.25};
//	Double_t velocity[3] = {myParticle->VelocityX(), myParticle->VelocityY(), myParticle->VelocityZ()};
//	Double_t field[3] = {myManager->GetGravField()->Gx(), myManager->GetGravField()->Gy(), myManager->GetGravField()->Gz()};
//	TUCNGeoTube* myTube = static_cast<TUCNGeoTube*>(innertube->GetShape());
//	cout <<  myTube->IsNextPointOnTube(point, velocity, field, myTube->GetRmax(), myTube->GetDz(), 0.) << endl;
	
			
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
	FinalPoints.SetMarkerStyle(6);
	FinalPoints.Draw();
	
	// -- Draw Tracks
	// This will only work if the option to store tracks in the method PropagateTracks is turned on. 
	myManager->DrawTracks();
	// ----------------------------------------------					
	
	benchmark.Stop("UCN Simulation");
	benchmark.Show("UCN Simulation");
				
#ifndef __CINT__
	theApp->Run();
#endif
	return 0;
}
