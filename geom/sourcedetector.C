// This is a model of the cryoEDM experiment, using dimensions from the model created by
// Vishal Francis. 
// 14/11/2010 - Model covers whole neutron facing geometry to a first approximation. 
// Things missing currently includes valves
#include "TGeoManager.h"
#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"

#include "../include/Units.h"
#include "../include/Constants.h"
#include "../include/Materials.h"
#include "../include/GeomParameters.h"

Bool_t Build_Geom(const TGeoManager* geoManager);
Bool_t Draw_Geom(const TGeoManager* geoManager);

//__________________________________________________________________________
Int_t sourcedetector()
{
   // Create the geoManager
   TGeoManager* geoManager = new TGeoManager("GeoManager","Geometry Manager");
   // Build and write to file the simulation and visualisation geoms
   Build_Geom(geoManager);
   Draw_Geom(geoManager);
   
   return 0;
}

//__________________________________________________________________________
Bool_t Build_Geom(const TGeoManager* geoManager) 
{
   // -------------------------------------
   // BUILDING GEOMETRY
   // Materials - Define the materials used. Leave the neutron properties to be defined on a run-by-run basis
   
   Materials::BuildMaterials(geoManager);
   TGeoMedium* beryllium = geoManager->GetMedium("Beryllium");
   TGeoMedium* copper = geoManager->GetMedium("Copper");
   TGeoMedium* blackhole = geoManager->GetMedium("BlackHole");
   TGeoMedium* heliumII = geoManager->GetMedium("HeliumII");
   TGeoMedium* lithium6 = geoManager->GetMedium("Lithium6");
   
   // -------------------------------------
   // -- Making Top Volume
   Box* topShape = new Box("Top",100,100,100);
   BlackHole* top = new BlackHole("Top", topShape, blackhole);
   geoManager->SetTopVolume(top);
   top->SetVisibility(kFALSE);
   
   // -- Make the boundary volume in which all the others sit
   // -- This is what we will be reflecting off all the time
   Double_t surfaceRoughness = 1.0;
   Box* chamberShape = new Box("Chamber",10,10,10);
   Boundary* chamber = new Boundary("Chamber", chamberShape, copper, surfaceRoughness);
   chamber->SetLineColor(kOrange-7);
   chamber->SetLineWidth(1);
   chamber->SetVisibility(kFALSE);
   chamber->SetTransparency(80);
   // Add to geom
   top->AddNode(chamber,1);
   
   // -------------------------------------
   // -- Hole in Source Tube Dimensions
   const Double_t sourceHoleRMin = 0.; 
   const Double_t sourceHoleRMax = 1.5*Units::mm;
   const Double_t sourceHoleHalfZ = 1.25*Units::mm;
   const Double_t sourceHolePhi = 0.0;
   const Double_t sourceHoleTheta = 0.0;
   const Double_t sourceHolePsi = 0.0;
   const Double_t sourceHoleXPos = 0.*Units::mm;
   const Double_t sourceHoleYPos = 0.*Units::mm;
   const Double_t sourceHoleZPos = 0.*Units::mm;
   // -- Hole in Source tube
   Tube *sourceHoleShape = new Tube("SourceHole", sourceHoleRMin, sourceHoleRMax, sourceHoleHalfZ);
   TrackingVolume* sourceHole = new TrackingVolume("SourceHole", sourceHoleShape, heliumII);
   sourceHole->SetLineColor(kAzure-4);
   sourceHole->SetLineWidth(1);
   sourceHole->SetVisibility(kTRUE);
   sourceHole->SetTransparency(20);
   // -- Define SourceTube matrix
   TGeoRotation sourceHoleRot("SourceHoleRot",sourceHolePhi,sourceHoleTheta,sourceHolePsi);
   TGeoTranslation sourceHoleTra("SegmentTra",sourceHoleXPos,sourceHoleYPos,sourceHoleZPos);
   TGeoCombiTrans sourceHoleCom(sourceHoleTra,sourceHoleRot);
   TGeoHMatrix sourceHoleMat = sourceHoleCom;
   chamber->AddNode(sourceHole, 1, new TGeoHMatrix(sourceHoleMat));
   
   // -------------------------------------
   // -- Detector Dimensions
   const Double_t detectorRMin = 0.;
   const Double_t detectorRMax = 1.5*Units::mm;
   const Double_t detectorHalfLength = 0.1*Units::mm;
   const Double_t detectorPhi = 0.0;
   const Double_t detectorTheta = 0.0;
   const Double_t detectorPsi = 0.0;
   const Double_t detectorXDisplacement = 0.0;
   const Double_t detectorYDisplacement = 0.0;
   const Double_t detectorZDisplacement = sourceHoleHalfZ + detectorHalfLength;
   const Double_t detectorEfficiency = 1.0;
   // -- Detector
   Tube *detectorShape = new Tube("Detector", detectorRMin, detectorRMax, detectorHalfLength);
   Detector* detector = new Detector("Detector",detectorShape,lithium6,detectorEfficiency);
   detector->SetLineColor(kRed+3);
   detector->SetLineWidth(1);
   detector->SetVisibility(kTRUE);
   detector->SetTransparency(0);
   // -- Detector Matrix
   TGeoRotation detectorRot("DetectorRot",detectorPhi,detectorTheta,detectorPsi);
   TGeoTranslation detectorTra("DetectorTra",detectorXDisplacement, detectorYDisplacement, detectorZDisplacement);
   TGeoCombiTrans detectorCom(detectorTra,detectorRot);
   TGeoHMatrix detectorMat = detectorCom;
   chamber->AddNode(detector, 1, new TGeoHMatrix(detectorMat));
   
   
   // -------------------------------------
   // -- Hole Dimensions
   const Double_t holeRMin = 0.;
   const Double_t holeRMax = 1.5*Units::mm;
   const Double_t holeHalfLength = 0.1*Units::mm;
   const Double_t holePhi = 0.0;
   const Double_t holeTheta = 0.0;
   const Double_t holePsi = 0.0;
   const Double_t holeXDisplacement = 0.0;
   const Double_t holeYDisplacement = 0.0;
   const Double_t holeZDisplacement = -sourceHoleHalfZ - holeHalfLength;
   // -- hole
   Tube *holeShape = new Tube("hole", holeRMin, holeRMax, holeHalfLength);
   BlackHole* hole = new BlackHole("hole", holeShape, blackhole);
   hole->SetLineColor(kGray+3);
   hole->SetLineWidth(1);
   hole->SetVisibility(kTRUE);
   hole->SetTransparency(0);
   // -- hole Matrix
   TGeoRotation holeRot("holeRot",holePhi,holeTheta,holePsi);
   TGeoTranslation holeTra("holeTra",holeXDisplacement, holeYDisplacement, holeZDisplacement);
   TGeoCombiTrans holeCom(holeTra,holeRot);
   TGeoHMatrix holeMat = holeCom;
   chamber->AddNode(hole, 1, new TGeoHMatrix(holeMat));
   
   // -------------------------------------
   // -- Close Geometry
   geoManager->CloseGeometry();
   
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "$(UCN_GEOM)/sourcedetector_geom.root";
   cout << "Simulation Geometry Built... Writing to file: " << fileName << endl;
   geoManager->Export(fileName);
   
   return kTRUE;
}

//__________________________________________________________________________
Bool_t Draw_Geom(const TGeoManager* geoManager) 
{
   // -------------------------------------
   // -- Draw the vis-geometry in OpenGLViewer
   TCanvas* canvas = new TCanvas("GeomCanvas","Canvas for visualisation of EDM Geom",60,40,600,600);
   canvas->cd();
   geoManager->GetTopVolume()->Draw("ogl");
   geoManager->SetVisOption(0); // Default is 1, but 0 draws all the intermediate volumes not just the final bottom layer
   
   // -- Get the GLViewer so we can manipulate the camera
   TGLViewer * glViewer = dynamic_cast<TGLViewer*>(gPad->GetViewer3D());
   // -- Select Draw style 
   glViewer->SetStyle(TGLRnrCtx::kFill); // TGLRnrCtx::kFill, TGLRnrCtx::kOutline, TGLRnrCtx::kWireFrame
   // -- Set Background colour
   glViewer->SetClearColor(TColor::kWhite);
   // -- Set Lights - turn some off if you wish
// TGLLightSet* lightSet = glViewer->GetLightSet();
// lightSet->SetLight(TGLLightSet::kLightLeft, kFALSE);
   // -- Set Camera type
   // kCameraPerspXOZ, kCameraPerspYOZ, kCameraPerspXOY, kCameraOrthoXOY
   // kCameraOrthoXOZ, kCameraOrthoZOY, kCameraOrthoXnOY, kCameraOrthoXnOZ, kCameraOrthoZnOY
   TGLViewer::ECameraType camera = 2;
   glViewer->SetCurrentCamera(camera);
   glViewer->CurrentCamera().SetExternalCenter(kTRUE);
   Double_t cameraCentre[3] = {0.,0.,0.};
   glViewer->SetPerspectiveCamera(camera,4,100,&cameraCentre[0],0,0);
   // -- Draw Reference Point, Axes
   Double_t refPoint[3] = {0.,0.,0.};
   // Int_t axesType = 0(Off), 1(EDGE), 2(ORIGIN), Bool_t axesDepthTest, Bool_t referenceOn, const Double_t referencePos[3]
   glViewer->SetGuideState(0, kFALSE, kFALSE, refPoint);
   glViewer->UpdateScene();
   return kTRUE;
}
