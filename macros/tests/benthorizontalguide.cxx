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

#include "TH1.h"
#include "TF1.h"
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
#include "TUCNGeoMaterial.h"
#include "TUCNParticleCloud.h"
#include "TUCNParticle.h"
#include "TUCNExperiment.h"
#include "Constants.h"
#include "Units.h"


using std::cout;
using std::endl;

// Global
Double_t neutronmass = (Constants::neutron_mass_c2/Constants::c_squared);
Double_t densityf(Double_t*x, Double_t*par);

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
	Double_t boxX = 0.1, boxY = 0.1, boxZ = 0.2; 
	TGeoVolume* box   = myManager->MakeBox("box",boundary, boxX, boxY, boxZ);
	TGeoVolume* innerBox  = myManager->MakeBox("innerbox",vacuum, boxX-0.01, boxY-0.01, boxZ-0.01);
	
	TGeoVolume* box2   = myManager->MakeBox("box2",boundary, boxX, boxY, boxZ);
	TGeoVolume* innerBox2  = myManager->MakeBox("innerbox2",vacuum, boxX-0.01, boxY-0.01, boxZ);
	
	TGeoVolume* box3   = myManager->MakeBox("box3",boundary, boxX, boxY, boxZ);
	TGeoVolume* innerBox3  = myManager->MakeBox("innerbox3",vacuum, boxX-0.01, boxY-0.01, boxZ);
	
	TGeoVolume* box4   = myManager->MakeBox("box4",boundary, boxX, boxY, boxZ);
	TGeoVolume* innerBox4  = myManager->MakeBox("innerbox4",vacuum, boxX-0.01, boxY-0.01, boxZ-0.01);
	
	innerBox->SetTransparency(80);
	innerBox2->SetTransparency(80);
	innerBox3->SetTransparency(80);
	innerBox4->SetTransparency(80);
	
	innerBox->SetTransparency(80);
	innerBox2->SetTransparency(80);
	innerBox3->SetTransparency(80);
	innerBox4->SetTransparency(80);
	
	box->SetLineColor(38);
	box2->SetLineColor(38);
	box3->SetLineColor(38);
	box4->SetLineColor(38);
	
	box->SetLineColor(38);
	box2->SetLineColor(38);
	box3->SetLineColor(38);
	box4->SetLineColor(38);
	
	
	// -- Define the transformation of the volume
	// Here is a general set-up of rotations and translations to be applied to a single volume. 
	TGeoRotation r1,r2; 
	r1.SetAngles(0,90,0);          	//	rotation defined by Euler angles 
	r2.SetAngles(20,90,0); 	 
	TGeoTranslation t1(0.,0.,0.); 
	TGeoTranslation t2(-0.04,0.3,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm1 = c1;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix hm2 = c2;
	TGeoHMatrix *matrix1 = new TGeoHMatrix(hm1);
	TGeoHMatrix *matrix2 = new TGeoHMatrix(hm2);
	
	TGeoRotation r3,r4; 
	r3.SetAngles(0,0,0);          	//	rotation defined by Euler angles 
	r4.SetAngles(0,0,0); 	 
	TGeoTranslation t3(0.,0.,-0.01); 
	TGeoTranslation t4(0.,0.,0.0); 
	TGeoCombiTrans c3(t3,r3); 
	TGeoCombiTrans c4(t4,r4); 
	TGeoHMatrix hm3 = c3;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix hm4 = c4;
	TGeoHMatrix *matrix3 = new TGeoHMatrix(hm3);
	TGeoHMatrix *matrix4 = new TGeoHMatrix(hm4);
	
	TGeoRotation r5,r6; 
	r5.SetAngles(40,90,0);          	//	rotation defined by Euler angles 
	r6.SetAngles(60,90,0); 	 
	TGeoTranslation t5(-0.19,0.6,0.); 
	TGeoTranslation t6(-0.4,0.78,0.); 
	TGeoCombiTrans c5(t5,r5); 
	TGeoCombiTrans c6(t6,r6); 
	TGeoHMatrix hm5 = c5;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix hm6 = c6;
	TGeoHMatrix *matrix5 = new TGeoHMatrix(hm5);
	TGeoHMatrix *matrix6 = new TGeoHMatrix(hm6);
	
	TGeoRotation r7,r8; 
	r7.SetAngles(0,0,0);          	//	rotation defined by Euler angles 
	r8.SetAngles(0,0,0); 	 
	TGeoTranslation t7(0.,0.,0.); 
	TGeoTranslation t8(0.,0.,0.01); 
	TGeoCombiTrans c7(t7,r7); 
	TGeoCombiTrans c8(t8,r8); 
	TGeoHMatrix hm7 = c7;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix hm8 = c8;
	TGeoHMatrix *matrix7 = new TGeoHMatrix(hm7);
	TGeoHMatrix *matrix8 = new TGeoHMatrix(hm8);
	
	// -- Create the nodes	
	box->AddNode(innerBox,1,matrix3);
	box2->AddNode(innerBox2,1, matrix4);
	box3->AddNode(innerBox3,1, matrix7);
	box4->AddNode(innerBox4,1, matrix8);
	
	chamber->AddNode(box,1,matrix1);
	chamber->AddNode(box2,1,matrix2);
	chamber->AddNode(box3,1,matrix5);
	chamber->AddNode(box4,1,matrix6);
		
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
	Int_t particles = 1;
	Double_t totalEnergy = 100*Units::neV;
	TUCNParticleCloud* particleCloud = myManager->AddParticleCloud();
	particleCloud->UniformMonoEnergeticDistribution(particles, totalEnergy, innerBox, matrix1);
	
	// -- Create Tracks
	myManager->MakeTracks(myManager->GetParticleCloud());
	
	// -- Propagate Tracks
	// Propagate Tracks takes three arguments - the runtime of the simulation, whether to write-out the
	// final particle data in a text file, and whether to store the track objects (ie: for drawing the tracks)
	// If you plan to do much more than 10000 neutrons make sure you turn off storing the tracks, or your RAM will quickly be gone.  
	Double_t runtime = 10.*Units::s;
	Double_t maxStepTime = 0.01*Units::s;
	myManager->PropagateTracks(runtime, maxStepTime, kFALSE, kTRUE);
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
						
#ifndef __CINT__
	theApp->Run();
#endif
	return 0;
}