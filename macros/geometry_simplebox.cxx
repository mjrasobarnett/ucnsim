#ifndef __CINT__

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "TCanvas.h"
#include "TBenchmark.h"
#include "TFile.h"
#include "TSystem.h"
#include "TRint.h"
#include "TRandom.h"
#include "TObjArray.h"

#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoTrack.h"
#include "TNtuple.h"
#include "TParticle.h"
#include "TParticlePDG.h"
#include "TGeoMatrix.h"
#include "TPolyMarker3D.h"

#include "TUCNData.h"
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
	Int_t geometry_simplebox() {
	gSystem->Load("libPhysics");
	gSystem->Load("libGeom");
	gSystem->Load("libEG");
	gSystem->Load("libUCN"); // libUCN needs libGeom and libEG to be loaded first or it will complain. 
#endif
	// _________________________________________________________________________________________
	// This program creates a basic cuboid volume and propagates a particle (or particles)
	// around it for a set period of time. There is the option to write the final particle data
	// to a simple text file which can be analysed with the fitdata program. 
	// _________________________________________________________________________________________ 
	
	// ----------------------------------------------					
	// -- Create a UCNGeoManager with the name UCNGeom
	// The UCNManager is the primary object so every program should start by building one and storing a
	// pointer to it. You can use gGeoManager as well, however then you must cast down to access new methods.
	TUCNGeoManager* myManager = new TUCNGeoManager("UCNGeom", "Simple box geometry");
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
	TGeoMedium* vacuum    = new TGeoMedium("Vacuum",   1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary  = new TGeoMedium("Boundary", 3, matBoundary);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = myManager->MakeBox("TOP",blackHole,20,20,20);
	myManager->SetTopVolume(chamber);
			
	// -- Make a GeoBBox object via the UCNGeoManager
	//	MakeBox has been overridden here. If you can tell me how this is possible without throwing a warning
	// or anything (seeing as this is a non-virtual method) I would be really interested to know. 
	// In any case, ALL make Volume methods have been overridden in the UCNManager to only build volumes from 
	// UCNShapes which contain the new tracking methods.
	Double_t boxX = 0.1, boxY = 0.1, boxZ = 0.5; 
	TGeoVolume* box      = myManager->MakeBox("box",boundary, boxX, boxY, boxZ);
	TGeoVolume* innerBox = myManager->MakeBox("innerbox",vacuum, boxX-0.01, boxY-0.01, boxZ-0.01);
	
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
	TGeoTranslation t1(0.,0.,0.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	
	// -- Create the nodes	
	box->AddNode(innerBox,1);
	box->Print();
	chamber->AddNode(box,1, matrix);
	chamber->Print();
		
	// -- Arrange and close geometry
  myManager->CloseGeometry();
	// -----------------------------------------------
	// Store geometry
	// -----------------------------------------------
	TString ucngeom = gSystem->Getenv("UCN_GEOM");
	if ( ucngeom.Length() == 0 ) {
		cerr << "Could not find environment variable UCN_GEOM" << endl;
		ucngeom = gSystem->WorkingDirectory();
		cerr << "Saving geometry in current directory:" << endl << ucngeom << endl;
	}
	TString filename = ucngeom + "/geometry_simplebox.root";
	cerr << "Saving geometry as " << filename << endl;
	gGeoManager->Export(filename);
#ifndef __CINT__
	//theApp->Run();
#endif		
	return 0;
}
