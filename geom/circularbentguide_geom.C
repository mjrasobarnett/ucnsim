#include "../include/Units.h"
#include "../include/Constants.h"

Int_t circularbentguide_geom()
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
	TGeoVolume *top = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("Top",blackHole,20,20,20);
	geoManager->SetTopVolume(top);
	
	// -- Make the boundary volume in which all the others sit
	TGeoVolume *chamber = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("Chamber",boundary,2,2,2);
	
	// -- Make a GeoTube object via the UCNGeoManager
	Double_t rmin = 0.3, rmax = 0.5, bendwidth = 0.1;
	TGeoVolume *circlebend = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("CircleBend", vacuum, rmin, rmax, bendwidth);
	TGeoVolume *bendbox = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("BendBox", vacuum, 2.0*bendwidth, rmax, rmax);
	
	Double_t guideX = 0.1, guideY = 0.1, guideZ = 0.5;
	TGeoVolume *sourceguide = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("SourceGuide", vacuum, guideX, guideY, guideZ);
	TGeoVolume *detectorguide = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("DetectorGuide", vacuum, guideX, guideY, guideZ);
	
	// -- Define the transformation of bendbox
	TGeoRotation rbendbox;
	rbendbox.SetAngles(0,90,90);
	TGeoTranslation tbendbox(0.5,-0.5,0.);
	TGeoCombiTrans cbendbox(tbendbox,rbendbox);
	TGeoHMatrix bendboxm = cbendbox;
	TGeoHMatrix *bendboxmatrix = new TGeoHMatrix(bendboxm);
	bendboxmatrix->SetName("BendBoxMatrix");
	bendboxmatrix->RegisterYourself();
	
	// -- Create a composite bend
	TUCNGeoCompositeShape *bendshape = new TUCNGeoCompositeShape("BendShape","(CircleBend * BendBox:BendBoxMatrix)");
	TGeoVolume * bend = new TGeoVolume("Bend",bendshape,vacuum);
	
	// -- Define the transformation of the bend
	TGeoRotation r1;
	r1.SetAngles(0,90,0);
	TGeoTranslation t1(-0.4,-0.1,0.);
	TGeoCombiTrans c1(t1,r1);
	TGeoHMatrix bendm = c1;
	TGeoHMatrix *bendmatrix = new TGeoHMatrix(bendm);
	bendmatrix->SetName("BendMatrix");
	// -- Define the transformation of the guide
	TGeoRotation r2;
	r2.SetAngles(0,90,0);
	TGeoTranslation t2(0.,-0.5.,0.1);
	TGeoCombiTrans c2(t2,r2);
	TGeoHMatrix sourceguidem = c2;
	TGeoHMatrix *sourceguidematrix = new TGeoHMatrix(sourceguidem);
	sourceguidematrix->SetName("SourceGuideMatrix");
	// -- Define the transformation of the detector guide
	TGeoRotation r3;
	r3.SetAngles(90,90,0);
	TGeoTranslation t3(-0.9,-0.1,-0.4);
	TGeoCombiTrans c3(t3,r3);
	TGeoHMatrix detectorguidem = c3;
	TGeoHMatrix *detectorguidematrix = new TGeoHMatrix(detectorguidem);
	detectorguidematrix->SetName("DetectorGuideMatrix");
	
	// -- Create the nodes	
	top->AddNode(chamber,1);
	chamber->AddNode(sourceguide,1,sourceguidematrix);
	chamber->AddNode(detectorguide,1,detectorguidematrix);
	chamber->AddNode(bend,1,bendmatrix);
	
	// -- Define the general transformation of the source volume
	TGeoRotation r1,r2;
	r1.SetAngles(0,90,0); // rotation defined by Euler angles
	r2.SetAngles(0,0,0);
	TGeoTranslation t1(0.,-0.5.,0.1);
	TGeoTranslation t2(0.,0.,0.);
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
	
	// -- Define the visibility of the volumes
	sourceguide->SetLineColor(kBlue);
	sourceguide->SetLineWidth(1);
	sourceguide->SetVisibility(kTRUE);
	sourceguide->SetTransparency(50);
	
	detectorguide->SetLineColor(kBlue);
	detectorguide->SetLineWidth(1);
	detectorguide->SetVisibility(kTRUE);
	detectorguide->SetTransparency(50);
	
	bend->SetLineColor(kGreen);
	bend->SetLineWidth(1);
	bend->SetVisibility(kTRUE);
	bend->SetTransparency(50);
	
	chamber->SetLineColor(kYellow);
	chamber->SetLineWidth(1);
	chamber->SetVisibility(kTRUE);
	chamber->SetTransparency(80);
	
	// -- Write out geometry to file
	const char *fileName = "geom/circularbentguide_geom.root";
	cerr << "Geometry Built... Writing to file: " << fileName << endl;
	geoManager->Export(fileName);
	
	return 0;
}
