// A geometry to test the construction of a circular bend with square cross-section like that found in the EDM experiment
#include "../include/Units.h"
#include "../include/Constants.h"

Int_t simulation_geom();
Int_t visualisation_geom();

//__________________________________________________________________________
Int_t test_circularbend_geom()
{
	// Build and write to file the simulation geom
	simulation_geom();
	// Build and write to file the same geom, but designed for visualisation
	visualisation_geom();
	return 0;
}

//__________________________________________________________________________
Int_t simulation_geom() {
	// Create the geoManager
	TGeoManager* geoManager = new TGeoManager("GeoManager", "Geometry Manager");

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
	TGeoVolume *bendbox = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("BendBox", boundary, 2.0*bendwidth, rmax, rmax);
	
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
	sourceguide->SetTransparency(20);
	
	detectorguide->SetLineColor(kBlue);
	detectorguide->SetLineWidth(1);
	detectorguide->SetVisibility(kTRUE);
	detectorguide->SetTransparency(20);
	
	bend->SetLineColor(kGreen);
	bend->SetLineWidth(1);
	bend->SetVisibility(kTRUE);
	bend->SetTransparency(20);
	
	chamber->SetLineColor(kYellow);
	chamber->SetLineWidth(1);
	chamber->SetVisibility(kTRUE);
	chamber->SetTransparency(80);
	
	// -- Write out geometry to file
	const char *fileName = "geom/test_circularbend_geom.root";
	cerr << "Geometry Built... Writing to file: " << fileName << endl;
	geoManager->Export(fileName);
	geoManager->Delete();
	return 0;
}

Int_t visualisation_geom() {
	// -- Same geometry as above but without TUCNCompositeShape
	// -- This geometry is to be used exclusively for visualisation purposes as the current implementation
	// -- of TUCNCompositeShape cannot be drawn due to known casting problems in TGeoPainter 
	
	// Create the geoManager
	TGeoManager* geoManager = new TGeoManager("GeoManager", "Geometry Manager");

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
	TGeoVolume *bendbox = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("BendBox", boundary, 2.0*bendwidth, rmax, rmax);
	
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
	TGeoCompositeShape *bendshape = new TGeoCompositeShape("BendShape","(CircleBend * BendBox:BendBoxMatrix)");
	TGeoVolume * bend = new TGeoVolume("Bend",bendshape,vacuum);
	
	// -- Define the transformation of the bend
	TGeoRotation r1;
	r1.SetAngles(0,90,0);
	TGeoTranslation t1(-0.4,-0.1,0.);
	TGeoCombiTrans c1(t1,r1);
	TGeoHMatrix bendm = c1;
	TGeoHMatrix *bendmatrix = new TGeoHMatrix(bendm);
	bendmatrix->SetName("BendMatrix");
	// -- Define the transformation of the source
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
	
	// -- Arrange and close geometry
	geoManager->CloseGeometry();
	
	geoManager->SetVisLevel(4); // Default draws 4 levels down volume heirarchy
	geoManager->SetVisOption(0); // Default is 1, but 0 draws all the intermediate volumes not just the final bottom layer
	
	// -- Define the visibility of the volumes
	sourceguide->SetLineColor(kBlue);
	sourceguide->SetLineWidth(1);
	sourceguide->SetVisibility(kTRUE);
	sourceguide->SetTransparency(20);
	
	detectorguide->SetLineColor(kBlue);
	detectorguide->SetLineWidth(1);
	detectorguide->SetVisibility(kTRUE);
	detectorguide->SetTransparency(20);
	
	bend->SetLineColor(kGreen);
	bend->SetLineWidth(1);
	bend->SetVisibility(kTRUE);
	bend->SetTransparency(20);
	
	chamber->SetLineColor(kYellow);
	chamber->SetLineWidth(1);
	chamber->SetVisibility(kTRUE);
	chamber->SetTransparency(80);
	
   // -------------------------------------
   // -- Draw the geometry in OpenGLViewer
   TCanvas* canvas = new TCanvas("GeomCanvas","Canvas for visualisation of EDM Geom",60,40,600,600);
   canvas->cd();
   geoManager->SetVisLevel(3); // Default draws 4 levels down volume heirarchy
   geoManager->SetVisOption(0); // Default is 1, but 0 draws all the intermediate volumes not just the final bottom layer
   geoManager->GetTopVolume()->Draw("ogl");

   // -- Get the GLViewer so we can manipulate the camera
   TGLViewer * glViewer = dynamic_cast<TGLViewer*>(gPad->GetViewer3D());

   // -- Select Draw style 
   glViewer->SetStyle(TGLRnrCtx::kFill); // TGLRnrCtx::kFill, TGLRnrCtx::kOutline, TGLRnrCtx::kWireFrame

   // -- Set Background colour
   glViewer->SetClearColor(TColor::kBlack);

   // -- Set Lights - turn some off if you wish
   //	TGLLightSet* lightSet = glViewer->GetLightSet();
   //	lightSet->SetLight(TGLLightSet::kLightLeft, kFALSE);

   // -- Set Camera type
   // kCameraPerspXOZ, kCameraPerspYOZ, kCameraPerspXOY, kCameraOrthoXOY
   // kCameraOrthoXOZ, kCameraOrthoZOY, kCameraOrthoXnOY, kCameraOrthoXnOZ, kCameraOrthoZnOY
   TGLViewer::ECameraType camera = 2;
   glViewer->SetCurrentCamera(camera);
// glViewer->CurrentCamera().SetExternalCenter(kTRUE);
   Double_t cameraCentre[3] = {0., 0., 0.};
   glViewer->SetPerspectiveCamera(camera,4,100,&cameraCentre[0],0,0);

   // -- Draw Reference Point, Axes
   Double_t refPoint[3] = {0.,0.,0.};
   // Int_t axesType = 0(Off), 1(EDGE), 2(ORIGIN), Bool_t axesDepthTest, Bool_t referenceOn, const Double_t referencePos[3]
   glViewer->SetGuideState(0, kFALSE, kFALSE, refPoint);

   glViewer->UpdateScene();
	
	// -------------------------------------
   // -- Write out geometry to file
	const char *fileName = "geom/test_circularbend_vis.root";
	cerr << "Geometry Built... Writing to file: " << fileName << endl;
	geoManager->Export(fileName);
	return 0;
}