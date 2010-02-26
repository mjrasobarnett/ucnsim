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
	gSystem->Load("libPhysics");
	gSystem->Load("libGeom");
	gSystem->Load("libEG");
	gSystem->Load("libUCN");
#endif
	
	TBenchmark benchmark;
	benchmark.Start("UCN Simulation");
		
	// -- Create a UCNGeoManager
	TUCNGeoManager* myManager = new TUCNGeoManager("TestUCNGeoManager", "Derived class from TGeoManager");
	
	TUCNExperiment* experiment = new TUCNExperiment();
		
	// -- Making Materials
   TUCNGeoMaterial* matTracking = new TUCNGeoMaterial("Tracking Material", 0,0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0,0);
	matTracking->IsTrackingMaterial(kTRUE);
//	matBlackHole->IsBlackhole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = myManager->MakeBox("TOP",blackHole,20,20,20);
	myManager->SetTopVolume(chamber);
			
	// -- Make a GeoBBox object via the UCNGeoManager
	TGeoVolume* myUCNBox   = static_cast<TUCNGeoManager*>(gGeoManager)->MakeBox("UCNBox",vacuum, 0.2,0.2,0.9);
//	TGeoVolume* myUCNTube  = static_cast<TUCNGeoManager*>(gGeoManager)->MakeTube("UCNTube",vacuum, 0.,0.5,1.);
	
	myUCNBox->SetLineColor(1);
	myUCNBox->SetLineWidth(1);
	myUCNBox->SetVisibility(kTRUE);
	myUCNBox->SetTransparency(50);
	
//	myUCNTube->SetLineColor(35);
//	myUCNTube->SetLineWidth(1);
//	myUCNTube->SetVisibility(kFALSE);
//	myUCNTube->SetTransparency(50);
	
	// -- Define the transformation of the volume
	TGeoRotation r1,r2; 
	r1.SetAngles(0,0,0);          //rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,0.); 
	TGeoTranslation t2(2.,-2.,0.9); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	
	// -- Create the nodes	
//	myUCNBox->AddNode(myUCNTube,1);
	chamber->AddNode(myUCNBox,1, matrix);
	
	cout << "Top Volume Nodes: " << chamber->GetNodes()->GetEntriesFast() << endl;
	cout << "Manager - No. of Volumes: " << myManager->GetListOfVolumes()->GetEntriesFast() << endl;
	cout << "Manager - No. of Shapes: " << myManager->GetListOfShapes()->GetEntriesFast() << endl;
	cout << "Manager - No. of Nodes: " << myManager->GetListOfNodes()->GetEntriesFast() << endl;
		
	// -- Arrange and close geometry
   myManager->CloseGeometry();
	myManager->GetTopVolume()->Draw();
	myManager->SetVisLevel(4);
	myManager->SetVisOption(0);
//	myManager->SetTopVisible();
	
	// -- Make a Grav Field
	Double_t fieldDir[3] = {0., 0., -1.};
	myManager->AddGravField(fieldDir[0], fieldDir[1], fieldDir[2]);

	// -- Create Particle Cloud
	Double_t totalEnergy = 200*Units::neV;
	UInt_t particles = 1000;
	TUCNParticleCloud* particleCloud = myManager->AddParticleCloud();
//	myManager->RandomPoints(myUCNTube);
	particleCloud->UniformMonoEnergeticDistribution(particles, totalEnergy, myUCNBox, matrix);
			
	// Create Tracks
	myManager->MakeTracks(myManager->GetParticleCloud());

	// Propagate Tracks
	Double_t runtime = 10.*Units::s;
	Double_t maxStepTime = 0.01*Units::s;
	myManager->PropagateTracks(runtime, maxStepTime, kTRUE, kFALSE);
			
	// Draw Points
	TPolyMarker3D FinalPoints(particles, 1);
	for (Int_t i = 0; i < particleCloud->GetListOfParticles()->GetEntriesFast(); i++) {
		FinalPoints.SetPoint(i, particleCloud->GetParticle(i)->Vx(), particleCloud->GetParticle(i)->Vy(), particleCloud->GetParticle(i)->Vz());
	}
	FinalPoints.SetMarkerColor(2);
	FinalPoints.SetMarkerStyle(6);
	FinalPoints.Draw();
				
#ifndef __CINT__
	theApp->Run();
#endif
	return 0;
}


