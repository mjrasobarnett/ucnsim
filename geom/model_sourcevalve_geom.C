// This is a model of the 'Real' cryoEDM experiment, using dimensions from the model created by
// Vishal Francis. 
// 9/01/2010 - Model covers the source tube down to the neutron detector
#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"

#include "../include/Units.h"
#include "../include/Constants.h"

Int_t simulation_geom();
Int_t visualisation_geom();

//__________________________________________________________________________
Int_t model_sourcevalve_geom()
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
   chamber->SetTransparency(20);
   // Add to geom
   top->AddNode(chamber,1);
   
   // -------------------------------------
   // -- SOURCE VALVE and BEND
   // Valve entrance volume is a shorter source-tube segment-like that connects the valve volume to the source
   Double_t valveVolEntranceRMin = 0., valveVolEntranceRMax = 31.5*Units::mm, valveVolEntranceHalfLength = 46.*Units::mm;
   TGeoVolume *valveVolEntrance = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("ValveVolEntrance", 
   											vacuum, valveVolEntranceRMin, valveVolEntranceRMax, valveVolEntranceHalfLength);
   valveVolEntrance->SetLineColor(kTeal-3);
   valveVolEntrance->SetLineWidth(1);
   valveVolEntrance->SetVisibility(kTRUE);
   valveVolEntrance->SetTransparency(20);
   // -- Define the Valve volume entrance
   Double_t valveVolEntranceAngle = 90.0;
   Double_t valveVolEntranceDisplacement = valveVolEntranceHalfLength;
   TGeoRotation valveVolEntranceRot("ValveVolEntranceRot",0,valveVolEntranceAngle,0); // phi, theta, psi
   TGeoTranslation valveVolEntranceTra("ValveVolEntranceTra",0.,valveVolEntranceDisplacement,0.); // x, y, z
   TGeoCombiTrans valveVolEntranceCom(valveVolEntranceTra,valveVolEntranceRot);
   TGeoHMatrix valveVolEntranceMat = valveVolEntranceCom;
   chamber->AddNode(valveVolEntrance, 1, new TGeoHMatrix(valveVolEntranceMat));
   
   // -------------------------------------
   // -- Valve volume front - this is what the valve press against
   Double_t valveVolFrontRMin = 0., valveVolFrontRMax = 31.5*Units::mm, valveVolFrontHalfLength = 12.75.*Units::mm;
   TGeoVolume *valveVolFront = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("ValveVolFront", 
   											vacuum, valveVolFrontRMin, valveVolFrontRMax, valveVolFrontHalfLength);
   valveVolFront->SetLineColor(kTeal-4);
   valveVolFront->SetLineWidth(1);
   valveVolFront->SetVisibility(kTRUE);
   valveVolFront->SetTransparency(20);
   // -- Define the Valve volume front
   Double_t valveVolFrontAngle = 90.0;
   Double_t valveVolFrontDisplacement = 2.0*valveVolEntranceHalfLength + valveVolFrontHalfLength;
   TGeoRotation valveVolFrontRot("ValveVolFrontRot",0,valveVolFrontAngle,0); // phi, theta, psi
   TGeoTranslation valveVolFrontTra("ValveVolFrontTra",0.,valveVolFrontDisplacement,0.); // x, y, z
   TGeoCombiTrans valveVolFrontCom(valveVolFrontTra,valveVolFrontRot);
   TGeoHMatrix valveVolFrontMat = valveVolFrontCom;
   chamber->AddNode(valveVolFront, 1, new TGeoHMatrix(valveVolFrontMat));
   
   // -------------------------------------
   // -- Valve volume - this is what the valve sits in
   // -- This is joined together with the start of the bend volume to make a composite volume
   Double_t valveVolBackRMin = 0., valveVolBackRMax = 36.0*Units::mm, valveVolBackHalfLength = 17.75*Units::mm;
   TGeoVolume *valveVolBack = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("ValveVolBack", 
   											vacuum, valveVolBackRMin, valveVolBackRMax, valveVolBackHalfLength);
   // -- Define the Valve volume back
   Double_t valveVolBackAngle = 90.0;
   TGeoRotation valveVolBackRot("ValveVolBackRot",0,valveVolBackAngle,0); // phi, theta, psi
   TGeoTranslation valveVolBackTra("ValveVolBackTra",0.,0.,0.); // x, y, z
   TGeoCombiTrans valveVolBackCom(valveVolBackTra,valveVolBackRot);
   TGeoHMatrix valveVolBackMat = valveVolBackCom;
   TGeoHMatrix *valveVolBackMatrix = new TGeoHMatrix(valveVolBackMat);
   valveVolBackMatrix->SetName("ValveVolBackMatrix");
   valveVolBackMatrix->RegisterYourself();
   // -- BendEntrance - this is joined to the valve volume back
   Double_t bendEntranceHalfX = 30.0*Units::mm, bendEntranceHalfY = 15.0*Units::mm, bendEntranceHalfZ = 55.0*Units::mm;
   TGeoVolume *bendEntrance = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("BendEntrance", 
   											vacuum, bendEntranceHalfX, bendEntranceHalfY, bendEntranceHalfZ);

   // -- Define the Bend Entrance segment
   Double_t bendEntranceAngle = 0.0;
   Double_t bendEntranceZDisplacement = bendEntranceHalfZ;
   TGeoRotation bendEntranceRot("ValveVolBackRot",0,bendEntranceAngle,0); // phi, theta, psi
   TGeoTranslation bendEntranceTra("ValveVolBackTra",0.,0.,-bendEntranceZDisplacement); // x, y, z
   TGeoCombiTrans bendEntranceCom(bendEntranceTra,bendEntranceRot);
   TGeoHMatrix bendEntranceMat = bendEntranceCom;
   TGeoHMatrix *bendEntranceMatrix = new TGeoHMatrix(bendEntranceMat);
   bendEntranceMatrix->SetName("BendEntranceMatrix");
   bendEntranceMatrix->RegisterYourself();
   // -- Create the composite Valve volume
   TUCNGeoCompositeShape *valveVolShape = new TUCNGeoCompositeShape("ValveVol","((BendEntrance:BendEntranceMatrix - ValveVolBack:ValveVolBackMatrix) + ValveVolBack:ValveVolBackMatrix)");
   TGeoVolume * valveVol = new TGeoVolume("ValveVol",valveVolShape,vacuum);
   valveVol->SetLineColor(kTeal-5);
   valveVol->SetLineWidth(1);
   valveVol->SetVisibility(kTRUE);
   valveVol->SetTransparency(20);// -- Define the Valve volume transformation
   Double_t valveVolAngle = 0.0;
   Double_t valveVolYDisplacement = 2.0*valveVolEntranceHalfLength + 
   											2.0*valveVolFrontHalfLength + valveVolBackHalfLength;
   TGeoRotation valveVolRot("ValveVolRot",0,valveVolAngle,0); // phi, theta, psi
   TGeoTranslation valveVolTra("ValveVolTra",0.,valveVolYDisplacement,0.); // x, y, z
   TGeoCombiTrans valveVolCom(valveVolTra,valveVolRot);
   TGeoHMatrix valveVolMat = valveVolCom;
   chamber->AddNode(valveVol, 1, new TGeoHMatrix(valveVolMat));

   // -------------------------------------
   geoManager->CloseGeometry();

   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "geom/model_sourcevalve_geom.root";
   cerr << "Simulation Geometry Built... Writing to file: " << fileName << endl;
   geoManager->Export(fileName);

   return 0;
}

//__________________________________________________________________________
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
   chamber->SetTransparency(20);
   // Add to geom
   top->AddNode(chamber,1);

   // -------------------------------------
   // -- SOURCE VALVE and BEND
   // Valve entrance volume is a shorter source-tube segment-like that connects the valve volume to the source
   Double_t valveVolEntranceRMin = 0., valveVolEntranceRMax = 31.5*Units::mm, valveVolEntranceHalfLength = 46.*Units::mm;
   TGeoVolume *valveVolEntrance = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("ValveVolEntrance", 
   											vacuum, valveVolEntranceRMin, valveVolEntranceRMax, valveVolEntranceHalfLength);
   valveVolEntrance->SetLineColor(kTeal-3);
   valveVolEntrance->SetLineWidth(1);
   valveVolEntrance->SetVisibility(kTRUE);
   valveVolEntrance->SetTransparency(20);
   // -- Define the Valve volume entrance
   Double_t valveVolEntranceAngle = 90.0;
   Double_t valveVolEntranceDisplacement = valveVolEntranceHalfLength;
   TGeoRotation valveVolEntranceRot("ValveVolEntranceRot",0,valveVolEntranceAngle,0); // phi, theta, psi
   TGeoTranslation valveVolEntranceTra("ValveVolEntranceTra",0.,valveVolEntranceDisplacement,0.); // x, y, z
   TGeoCombiTrans valveVolEntranceCom(valveVolEntranceTra,valveVolEntranceRot);
   TGeoHMatrix valveVolEntranceMat = valveVolEntranceCom;
   chamber->AddNode(valveVolEntrance, 1, new TGeoHMatrix(valveVolEntranceMat));

   // -------------------------------------
   // -- Valve volume front - this is what the valve press against
   Double_t valveVolFrontRMin = 0., valveVolFrontRMax = 31.5*Units::mm, valveVolFrontHalfLength = 12.75.*Units::mm;
   TGeoVolume *valveVolFront = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("ValveVolFront", 
   											vacuum, valveVolFrontRMin, valveVolFrontRMax, valveVolFrontHalfLength);
   valveVolFront->SetLineColor(kTeal-4);
   valveVolFront->SetLineWidth(1);
   valveVolFront->SetVisibility(kTRUE);
   valveVolFront->SetTransparency(20);
   // -- Define the Valve volume front
   Double_t valveVolFrontAngle = 90.0;
   Double_t valveVolFrontDisplacement = 2.0*valveVolEntranceHalfLength + valveVolFrontHalfLength;
   TGeoRotation valveVolFrontRot("ValveVolFrontRot",0,valveVolFrontAngle,0); // phi, theta, psi
   TGeoTranslation valveVolFrontTra("ValveVolFrontTra",0.,valveVolFrontDisplacement,0.); // x, y, z
   TGeoCombiTrans valveVolFrontCom(valveVolFrontTra,valveVolFrontRot);
   TGeoHMatrix valveVolFrontMat = valveVolFrontCom;
   chamber->AddNode(valveVolFront, 1, new TGeoHMatrix(valveVolFrontMat));

   // -------------------------------------
   // -- Valve volume - this is what the valve sits in
   // -- This is joined together with the start of the bend volume to make a composite volume
   Double_t valveVolBackRMin = 0., valveVolBackRMax = 36.0*Units::mm, valveVolBackHalfLength = 17.75*Units::mm;
   TGeoVolume *valveVolBack = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("ValveVolBack", 
   											vacuum, valveVolBackRMin, valveVolBackRMax, valveVolBackHalfLength);
   // -- Define the Valve volume back
   Double_t valveVolBackAngle = 90.0;
   TGeoRotation valveVolBackRot("ValveVolBackRot",0,valveVolBackAngle,0); // phi, theta, psi
   TGeoTranslation valveVolBackTra("ValveVolBackTra",0.,0.,0.); // x, y, z
   TGeoCombiTrans valveVolBackCom(valveVolBackTra,valveVolBackRot);
   TGeoHMatrix valveVolBackMat = valveVolBackCom;
   TGeoHMatrix *valveVolBackMatrix = new TGeoHMatrix(valveVolBackMat);
   valveVolBackMatrix->SetName("ValveVolBackMatrix");
   valveVolBackMatrix->RegisterYourself();
   // -- BendEntrance - this is joined to the valve volume back
   Double_t bendEntranceHalfX = 30.0*Units::mm, bendEntranceHalfY = 15.0*Units::mm, bendEntranceHalfZ = 55.0*Units::mm;
   TGeoVolume *bendEntrance = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("BendEntrance", 
   											vacuum, bendEntranceHalfX, bendEntranceHalfY, bendEntranceHalfZ);

   // -- Define the Bend Entrance segment
   Double_t bendEntranceAngle = 0.0;
   Double_t bendEntranceZDisplacement = bendEntranceHalfZ;
   TGeoRotation bendEntranceRot("ValveVolBackRot",0,bendEntranceAngle,0); // phi, theta, psi
   TGeoTranslation bendEntranceTra("ValveVolBackTra",0.,0.,-bendEntranceZDisplacement); // x, y, z
   TGeoCombiTrans bendEntranceCom(bendEntranceTra,bendEntranceRot);
   TGeoHMatrix bendEntranceMat = bendEntranceCom;
   TGeoHMatrix *bendEntranceMatrix = new TGeoHMatrix(bendEntranceMat);
   bendEntranceMatrix->SetName("BendEntranceMatrix");
   bendEntranceMatrix->RegisterYourself();
   // -- Create the composite Valve volume
   TGeoCompositeShape *valveVolShape = new TGeoCompositeShape("ValveVol","((BendEntrance:BendEntranceMatrix - ValveVolBack:ValveVolBackMatrix) + ValveVolBack:ValveVolBackMatrix)");
   TGeoVolume * valveVol = new TGeoVolume("ValveVol",valveVolShape,vacuum);
   valveVol->SetLineColor(kTeal-5);
   valveVol->SetLineWidth(1);
   valveVol->SetVisibility(kTRUE);
   valveVol->SetTransparency(20);// -- Define the Valve volume transformation
   Double_t valveVolAngle = 0.0;
   Double_t valveVolYDisplacement = 2.0*valveVolEntranceHalfLength + 
   											2.0*valveVolFrontHalfLength + valveVolBackHalfLength;
   TGeoRotation valveVolRot("ValveVolRot",0,valveVolAngle,0); // phi, theta, psi
   TGeoTranslation valveVolTra("ValveVolTra",0.,valveVolYDisplacement,0.); // x, y, z
   TGeoCombiTrans valveVolCom(valveVolTra,valveVolRot);
   TGeoHMatrix valveVolMat = valveVolCom;
   chamber->AddNode(valveVol, 1, new TGeoHMatrix(valveVolMat));


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
   const char *fileName = "geom/model_sourcevalve_vis.root";
   cerr << "Visualisation Geometry Built... Writing to file: " << fileName << endl;
   geoManager->Export(fileName);

	return 0;
}