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
#include "TUCNGeoMaterial.h"
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
	
#endif
	
	gSystem->Load("libPhysics");
	gSystem->Load("libGeom");
	gSystem->Load("libEG");
	gSystem->Load("libUCN"); // libUCN needs libGeom and libEG to be loaded first or it will complain. 
	
	
	// _________________________________________________________________________________________
	// This program creates a basic cuboid volume and propagates a particle (or particles)
	// around it for a set period of time. There is the option to write the final particle data
	// to a simple text file which can be analysed with the fitdata program. 
	// _________________________________________________________________________________________ 
	
	// ----------------------------------------------					
	// -- Create a UCNGeoManager
	// The UCNManager is the primary object so every program should start by building one and storing a
	// pointer to it. You can use gGeoManager as well, however then you must cast down to access new methods.
	TUCNGeoManager* myManager = new TUCNGeoManager("TestUCNGeoManager", "Derived class from TGeoManager");
			
	// -- Making Materials
	// Every material made should be a TUCNMaterial. This class includes specific methods that identify the 
	// material as a one of three types: Tracking Material, Boundary Material, and BlackHole Material. All the
	// usual properties of materials can be set, but at the moment are not current required for navigation. In
	// the future expect to set the material element type, scattering length (or effective potential) and other
	// quantities. 
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
			
	// -- Make a GeoBBox object via the UCNGeoManager
	//	MakeBox has been overridden here. If you can tell me how this is possible without throwing a warning
	// or anything (seeing as this is a non-virtual method) I would be really interested to know. 
	// In any case, ALL make Volume methods have been overridden in the UCNManager to only build volumes from 
	// UCNShapes which contain the new tracking methods.
	Double_t boxX = 0.1, boxY = 0.1, boxZ = 0.5; 
	TGeoVolume* box   = myManager->MakeBox("box",boundary, boxX, boxY, boxZ);
	TGeoVolume* innerBox  = myManager->MakeBox("innerbox",vacuum, boxX-0.01, boxY-0.01, boxZ-0.01);
	
	box->SetLineColor(36);
	box->SetLineWidth(1);
	box->SetVisibility(kTRUE);
	box->SetTransparency(50);
	
	innerBox->SetLineColor(38);
	innerBox->SetLineWidth(1);
	innerBox->SetVisibility(kTRUE);
	innerBox->SetTransparency(80);
	
	// -- Define the transformation of the volume
	// Here is a general set-up of rotations and translations to be applied to a single volume. 
	TGeoRotation r1,r2; 
	r1.SetAngles(45,138,30);          	//	rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(-0.5,+0.5,0.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	
	// -- Create the nodes	
	box->AddNode(innerBox,1);
	chamber->AddNode(box,1, matrix);
		
	// -- Arrange and close geometry
   myManager->CloseGeometry();

	// -- Make a Grav Field
	// GravField class is really just a TVector3 object with a few accessor methods. 
	// Field Direction will be normalised by the GravField class.
	Double_t fieldDir[3] = {0., 0., -1.};
	myManager->AddGravField(fieldDir[0], fieldDir[1], fieldDir[2]);
	// ----------------------------------------------					
	
	
	// ----------------------------------------------					
	// -- Create Particle Cloud
	// Particle cloud creates an array of particles inside a given volume with random positions and random
	// directions. The matrix supplied MUST be the total transformation matrix of the volume supplied in the
	// global reference frame. Currently there is only one way of filling the volume - a monoenergetic
	// distribution. The Particle cloud actually owns the particles (not the tracks).  
	Int_t particles = 1000;
	Double_t totalEnergy = 100*Units::neV;
	TUCNParticleCloud* particleCloud = myManager->AddParticleCloud();
	particleCloud->UniformMonoEnergeticDistribution(particles, totalEnergy, innerBox, matrix);
		
	// -- Create Tracks
	myManager->MakeTracks(myManager->GetParticleCloud());
	
	// -- Propagate Tracks
	// Propagate Tracks takes three arguments - the runtime of the simulation, whether to write-out the
	// final particle data in a text file, and whether to store the track objects (ie: for drawing the tracks)
	// If you plan to do much more than 10000 neutrons make sure you turn off storing the tracks, or your RAM will quickly be gone.  
	Double_t runtime = 10.*Units::s;
	Double_t maxStepTime = 0.01*Units::s;
	myManager->PropagateTracks(runtime, maxStepTime, kFALSE, kFALSE);
	// ----------------------------------------------					
	
			
	// ----------------------------------------------					
	// -- Draw Canvas
	Double_t width = 600;
	Double_t height = 600;
	Double_t positionX = 200;
	Double_t positionY = 10;
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
	
	
				
#ifndef __CINT__
	theApp->Run();
#endif
	return 0;
}