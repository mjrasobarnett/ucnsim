#include "../include/Units.h"
#include "../include/Constants.h"

Int_t VerticalTubeParaMagField()
{
	// -- Simple Tube example for Mike P, that includes Wall Losses and finding the average field in a parabolic mag field
	
	// Loading useful libraries (if they aren't already loaded)
	gSystem->Load("libPhysics");
	gSystem->Load("libGeom");
	gSystem->Load("libGeomPainter");
	gSystem->Load("libEG");
	gSystem->Load("libUCN");
	
	// Create the geoManager
	TGeoManager* geoManager = new TGeoManager("GeoManager", "Geometry Manager");
	// Create the UCNNavigator and initialise in the UCNManager
	Info("TUCNRun", "Creating a new Navigator...");

	// -------------------------------------
	// BUILDING GEOMETRY
	
		// Materials
	TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", 0, 0);
	
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackHole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
	
	// -- Making Top Volume
	TGeoVolume* chamber = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("TOP",blackHole,20,20,20);
	geoManager->SetTopVolume(chamber);
	
	// -- Make a GeoTube object via the UCNGeoManager
	Double_t rMin = 0.0, rMax = 0.236, length = 0.121;
	Double_t innerR = rMax - 0.001;
	Double_t innerLength = length - 0.001; 
	TGeoVolume* tube = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("tube",boundary, rMin, rMax, length/2.);
	TGeoVolume* innerTube = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("innerTube",vacuum, rMin, innerR, innerLength/2.);
	
	
	// -- Define the transformation of the volume
	TGeoRotation r1,r2; 
	r1.SetAngles(0,0,0); //rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,innerLength/2.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	matrix->SetName("SourceMatrix");
	
	TGeoVolume* volume = tube;
	TGeoVolume* innerVolume = innerTube;
	
	// -- Create the nodes	
	volume->AddNode(innerVolume, 1);
	chamber->AddNode(volume, 1, matrix);
	
	// -- Define the Source in our geometry where we will create the particles
//	this->SetSourceVolume(innerVolume);
//	this->SetSourceMatrix(matrix);
	
	// -- Arrange and close geometry
	geoManager->CloseGeometry();
	
	// -- Write out geometry to file
	cerr << "Geometry Built... Writing to file: " << endl;
	geoManager->Export("geom/verticaltube_geom.root");
	
	return 0;
}
