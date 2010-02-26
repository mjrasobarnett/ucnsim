#include "TGLViewer.h"
#include "TGLPerspectiveCamera.h"

#include "../include/Units.h"
#include "../include/Constants.h"

Int_t simulation_geom();
Int_t visualisation_geom();

Int_t model_edm_geom()
{
	// Build and write to file the simulation geom
	simulation_geom();
	// Build and write to file the same geom, but designed for visualisation
//	visualisation_geom();
	return 0;
}

//__________________________________________________________________________
Int_t simulation_geom() {
	// This is a model of the 'Real' cryoEDM experiment, using dimensions from the model created by
	// Vishal Francis. 
	// 9/01/2010 - Model covers the source tube down to the neutron detector
	
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
	
	// -------------------------------------
	// -- Making Top Volume
	TGeoVolume *top = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("Top",blackHole,100,100,100);
	geoManager->SetTopVolume(top);
	top->SetVisibility(kFALSE);
	
	// -- Make the boundary volume in which all the others sit
	// -- This is what we will be reflecting off all the time
	TGeoVolume *chamber = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("Chamber",boundary,10,10,10);
	chamber->SetLineColor(kOrange-7);
	chamber->SetLineWidth(1);
	chamber->SetVisibility(kFALSE);
	chamber->SetTransparency(80);
	// Add to geom
	top->AddNode(chamber,1);
		
	// -------------------------------------
	// -- SOURCE TUBE
	// -- Source tube has 13 segments, all of which are identical (except one which has a hole in the top)
	
	// -- Make SourceTube Segment
	Double_t sourceSegRMin = 0., sourceSegRMax = 31.5*Units::mm, sourceSegHalfLength = 125.*Units::mm;
	TGeoVolume *sourceSeg = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("SourceTubeSeg", vacuum, sourceSegRMin, sourceSegRMax, sourceSegHalfLength);
	sourceSeg->SetLineColor(kAzure-4);
	sourceSeg->SetLineWidth(1);
	sourceSeg->SetVisibility(kTRUE);
	sourceSeg->SetTransparency(60);
	
	Double_t sourceAngle = 90.0;
	Double_t segmentDisplacement = 125.*Units::mm;
	for (Int_t segNum = 1; segNum <= 13; segNum++) {
		// -- Define SourceTube matrix
		// Rotation seems to be applied before translation so bear that in mind when choosing which
		// coordinate to transform in translation
		TGeoRotation segmentRot("SegmentRot",0,sourceAngle,0); // phi, theta, psi
		TGeoTranslation segmentTra("SegmentTra",0.,segmentDisplacement,0.); // x, y, z
		TGeoCombiTrans segmentCom(segmentTra,segmentRot);
		TGeoHMatrix segmentMat = segmentCom;
		Char_t sourceMatrixName[20];
		sprintf(sourceMatrixName, "SourceMatrix%d", segNum); 
		segmentMat.SetName(sourceMatrixName);
		segmentMat.RegisterYourself();
		chamber->AddNode(sourceSeg, segNum, new TGeoHMatrix(segmentMat));
		segmentDisplacement += 2.*sourceSegHalfLength; // Shift next segment along by length of segment
	}
	
	// -------------------------------------
	// -- SOURCE VALVE and BEND
	// Valve entrance volume is a shorter source-tube segment-like that connects the valve volume to the source
	Double_t valveVolEntranceRMin = 0., valveVolEntranceRMax = 31.5*Units::mm, valveVolEntranceHalfLength = 46.*Units::mm;
	TGeoVolume *valveVolEntrance = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("ValveVolEntrance", 
												vacuum, valveVolEntranceRMin, valveVolEntranceRMax, valveVolEntranceHalfLength);
	valveVolEntrance->SetLineColor(kTeal-3);
	valveVolEntrance->SetLineWidth(1);
	valveVolEntrance->SetVisibility(kTRUE);
	valveVolEntrance->SetTransparency(60);
	
	Double_t valveVolEntranceAngle = 90.0;
	Double_t valveVolEntranceDisplacement = 13.0*2.0*sourceSegHalfLength 
												/*translates origin to the end of the source tube*/ + valveVolEntranceHalfLength;
	TGeoRotation valveVolEntranceRot("ValveVolEntranceRot",0,valveVolEntranceAngle,0); // phi, theta, psi
	TGeoTranslation valveVolEntranceTra("ValveVolEntranceTra",0.,valveVolEntranceDisplacement,0.); // x, y, z
	TGeoCombiTrans valveVolEntranceCom(valveVolEntranceTra,valveVolEntranceRot);
	TGeoHMatrix valveVolEntranceMat = valveVolEntranceCom;
	chamber->AddNode(valveVolEntrance, 1, new TGeoHMatrix(valveVolEntranceMat));
	
	// Valve volume front - this is what the valve press against
	Double_t valveVolFrontRMin = 0., valveVolFrontRMax = 31.5*Units::mm, valveVolFrontHalfLength = 12.75.*Units::mm;
	TGeoVolume *valveVolFront = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("ValveVolFront", 
												vacuum, valveVolFrontRMin, valveVolFrontRMax, valveVolFrontHalfLength);
	valveVolFront->SetLineColor(kTeal-4);
	valveVolFront->SetLineWidth(1);
	valveVolFront->SetVisibility(kTRUE);
	valveVolFront->SetTransparency(60);
	
	Double_t valveVolFrontAngle = 90.0;
	Double_t valveVolFrontDisplacement = 13.0*2.0*sourceSegHalfLength + 2.0*valveVolEntranceHalfLength + valveVolFrontHalfLength;
	TGeoRotation valveVolFrontRot("ValveVolFrontRot",0,valveVolFrontAngle,0); // phi, theta, psi
	TGeoTranslation valveVolFrontTra("ValveVolFrontTra",0.,valveVolFrontDisplacement,0.); // x, y, z
	TGeoCombiTrans valveVolFrontCom(valveVolFrontTra,valveVolFrontRot);
	TGeoHMatrix valveVolFrontMat = valveVolFrontCom;
	chamber->AddNode(valveVolFront, 1, new TGeoHMatrix(valveVolFrontMat));
	
	// Valve volume back - this is what the valve sits in
	Double_t valveVolBackRMin = 0., valveVolBackRMax = 36.0*Units::mm, valveVolBackHalfLength = 17.75.*Units::mm;
	TGeoVolume *valveVolBack = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("ValveVolBack", 
												vacuum, valveVolBackRMin, valveVolBackRMax, valveVolBackHalfLength);
	valveVolBack->SetLineColor(kTeal-5);
	valveVolBack->SetLineWidth(1);
	valveVolBack->SetVisibility(kTRUE);
	valveVolBack->SetTransparency(60);
	
	Double_t valveVolBackAngle = 90.0;
	Double_t valveVolBackDisplacement = 13.0*2.0*sourceSegHalfLength + 2.0*valveVolEntranceHalfLength + 
													2.0*valveVolFrontHalfLength + valveVolBackHalfLength;
	TGeoRotation valveVolBackRot("ValveVolBackRot",0,valveVolBackAngle,0); // phi, theta, psi
	TGeoTranslation valveVolBackTra("ValveVolBackTra",0.,valveVolBackDisplacement,0.); // x, y, z
	TGeoCombiTrans valveVolBackCom(valveVolBackTra,valveVolBackRot);
	TGeoHMatrix valveVolBackMat = valveVolBackCom;
	chamber->AddNode(valveVolBack, 1, new TGeoHMatrix(valveVolBackMat));
	
	
	
	geoManager->CloseGeometry();
	
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
	glViewer->SetClearColor(TColor::kWhite);

	// -- Set Lights - turn some off if you wish
	TGLLightSet* lightSet = glViewer->GetLightSet();
	lightSet->SetLight(TGLLightSet::kLightLeft, kFALSE);

	// -- Set Camera type
	// kCameraPerspXOZ, kCameraPerspYOZ, kCameraPerspXOY, kCameraOrthoXOY
	// kCameraOrthoXOZ, kCameraOrthoZOY, kCameraOrthoXnOY, kCameraOrthoXnOZ, kCameraOrthoZnOY
	TGLViewer::ECameraType camera = 2;
	glViewer->SetCurrentCamera(camera);
   glViewer->CurrentCamera().SetExternalCenter(kTRUE);
   
	// -- Draw Reference Point, Axes
	Double_t refPoint[3] = {1.,2.,1.};
	// Int_t axesType = 0(Off), 1(EDGE), 2(ORIGIN), Bool_t axesDepthTest, Bool_t referenceOn, const Double_t referencePos[3]
	glViewer->SetGuideState(1, kFALSE, kTRUE, refPoint);
	
	
	
	return 0;
}

Int_t visualisation_geom() {
	// -- Same geometry as above but without TUCNCompositeShape
	// -- This geometry is to be used exclusively for visualisation purposes as the current implementation
	// -- of TUCNCompositeShape cannot be drawn due to known casting problems in TGeoPainter 
	
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
	const char *fileName = "geom/circularbentguide_visualisation.root";
	cerr << "Geometry Built... Writing to file: " << fileName << endl;
	geoManager->Export(fileName);
	geoManager->Delete();
	return 0;
}