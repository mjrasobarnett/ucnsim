#include "../include/Units.h"
#include "../include/Constants.h"

Int_t horizontalguidetube_geom()
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
	
	// Materials - Define the materials used. Leave the neutron properties to be defined on a run-by-run basis
	TUCNGeoMaterial *matTracking  = new TUCNGeoMaterial("TrackingMaterial",0,0);
	TUCNGeoMaterial *matBlackHole = new TUCNGeoMaterial("BlackHole",0,0);
	TUCNGeoMaterial *matBoundary  = new TUCNGeoMaterial("BoundaryMaterial",0,0);
	
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackHole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium *vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium *blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium *boundary = new TGeoMedium("Boundary",3, matBoundary);
	
	// -- Making Top Volume
	TGeoVolume *chamber = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("Top",blackHole,20,20,20);
	geoManager->SetTopVolume(chamber);
	
	// -- Make a GeoTube object via the UCNGeoManager
	Double_t sourceX = 0.2, sourceY = 0.2, sourceZ = 0.5;
	TGeoVolume *guideWalls = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("GuideWalls", boundary, sourceX+0.01, sourceY+0.01, 3.0);
	TGeoVolume *sourceVolume = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("Source", vacuum, sourceX, sourceY, sourceZ);
	
	Double_t guideX = 0.1, guideY = 0.1, guideZ = 2.0;
	TGeoVolume *guide = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("Guide", vacuum, guideX, guideY, guideZ);
	
	// -- Define the visibility of the volumes
	guideWalls->SetLineColor(kBlue);
	guideWalls->SetLineWidth(1);
	guideWalls->SetVisibility(kTRUE);
	guideWalls->SetTransparency(50);
	
	guide->SetLineColor(kGreen);
	guide->SetLineWidth(1);
	guide->SetVisibility(kTRUE);
	guide->SetTransparency(50);
	
	sourceVolume->SetLineColor(kGreen);
	sourceVolume->SetLineWidth(1);
	sourceVolume->SetVisibility(kTRUE);
	sourceVolume->SetTransparency(50);
	
	// -- Define the transformations
	TGeoRotation* rotation = new TGeoRotation("Experiment Rotation",0.0,90.0,0.0);
	TGeoTranslation* sourcetranslation = new TGeoTranslation("Source Translation",0.0,0.0,-2.0);
	TGeoTranslation* guidetranslation = new TGeoTranslation("Guide Translation",0.0,0.0,0.5);
	
	// -- Create the nodes	
	guideWalls->AddNode(guide, 1, guidetranslation);
	guideWalls->AddNode(sourceVolume, 1, sourcetranslation);
	chamber->AddNode(guideWalls, 1, rotation);
	
	// -- Define the general transformation of the source volume
	TGeoRotation r1,r2;
	r1.SetAngles(0,90,0); // rotation defined by Euler angles
	r2.SetAngles(0,0,0);
	TGeoTranslation t1(0.,0.,0.);
	TGeoTranslation t2(0.,0.,-2.);
	TGeoCombiTrans c1(t1,r1);
	TGeoCombiTrans c2(t2,r2);
	TGeoHMatrix hm = c1 * c2; // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	matrix->SetName("SourceMatrix");
	matrix->RegisterYourself();
	
	// -- Arrange and close geometry
	geoManager->CloseGeometry();
	
	geoManager->SetVisLevel(4); // Default draws 4 levels down volume heirarchy
	geoManager->SetVisOption(0); // Default is 1, but 0 draws all the intermediate volumes not just the final bottom layer
	
	// -- Write out geometry to file
	const char *fileName = "geom/horizontalguidetube_geom.root";
	cerr << "Geometry Built... Writing to file: " << fileName << endl;
	geoManager->Export(fileName);
	
	return 0;
}
