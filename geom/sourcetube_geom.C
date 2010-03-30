// This is a model of the 'Real' cryoEDM sourcetube - 25/01/2010
#include "TGeoManager.h"
#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"

#include "../include/Units.h"
#include "../include/Constants.h"
#include "../include/Materials.h"

Bool_t Build_Geom(const TGeoManager* geoManager);
Bool_t Draw_Geom(const TGeoManager* geoManager);

namespace ModelParameters {
   // -- SourceTube Segment
   const Double_t sourceSegRMin = 0.; 
   const Double_t sourceSegRMax = 31.5*Units::mm;
   const Double_t sourceSegHalfLength = 125.*Units::mm;
   const Double_t sourceSegAngle = 90.0;
   const Double_t sourceSegYDisplacement = 125.*Units::mm;
   
   // -- Neutron Beam Area
   const Double_t neutronBeamAreaRMin = 0.;
   const Double_t neutronBeamAreaRMax = 15.*Units::mm;
   const Double_t neutronBeamAreaHalfLength = (13.*sourceSegHalfLength)/2.0;
   const Double_t neutronBeamAreaAngle = 90.0;
   const Double_t neutronBeamAreaYDisplacement = neutronBeamAreaHalfLength;
   
   // -- Valve Volume Entrance
   const Double_t valveVolEntranceRMin = 0.;
   const Double_t valveVolEntranceRMax = 31.5*Units::mm;
   const Double_t valveVolEntranceHalfLength = 46.*Units::mm;
   const Double_t valveVolEntranceAngle = 90.0; 
   const Double_t valveVolEntranceYDisplacement = 13.0*(2.0*sourceSegHalfLength) + valveVolEntranceHalfLength;
   
}

using namespace ModelParameters;

//__________________________________________________________________________
Int_t sourcetube_geom()
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
   TGeoMedium* blackhole = geoManager->GetMedium("BlackHole");
   TGeoMedium* heliumII = geoManager->GetMedium("HeliumII");
   
   // -------------------------------------
   // -- Making Top Volume
   TUCNGeoBBox* topShape = new TUCNGeoBBox("Top",100,100,100);
   TUCNBlackHole* top = new TUCNBlackHole("Top", topShape, blackhole);
   geoManager->SetTopVolume(top);
   top->SetVisibility(kFALSE);
   
   // -- Make the boundary volume in which all the others sit
   // -- This is what we will be reflecting off all the time
   Double_t surfaceRoughness = 0.05;
   TUCNGeoBBox* chamberShape = new TUCNGeoBBox("Chamber",10,10,10);
   TUCNBoundary* chamber = new TUCNBoundary("Chamber", chamberShape, beryllium, surfaceRoughness);
   chamber->SetLineColor(kOrange-7);
   chamber->SetLineWidth(1);
   chamber->SetVisibility(kFALSE);
   chamber->SetTransparency(80);
   // Add to geom
   top->AddNode(chamber,1);
   
   // -------------------------------------
   // -- SOURCE TUBE
   // -- Source tube has 13 segments, all of which are identical (except one which has a hole in the top)
   
   // -- Make a SourceTube Segment
   TUCNGeoTube *sourceSegShape = new TUCNGeoTube("SourceSeg", sourceSegRMin, sourceSegRMax, sourceSegHalfLength);
   TUCNTrackingVolume* sourceSeg = new TUCNTrackingVolume("SourceSeg", sourceSegShape, heliumII);
   sourceSeg->SetLineColor(kAzure-4);
   sourceSeg->SetLineWidth(1);
   sourceSeg->SetVisibility(kTRUE);
   sourceSeg->SetTransparency(20);
   
   Double_t segmentYPosition = sourceSegHalfLength;
   for (Int_t segNum = 1; segNum <= 13; segNum++) {
      // -- Define SourceTube matrix
      // Rotation seems to be applied before translation so bear that in mind when choosing which
      // coordinate to transform in translation
      TGeoRotation segmentRot("SegmentRot",0,sourceSegAngle,0); // phi, theta, psi
      TGeoTranslation segmentTra("SegmentTra",0.,segmentYPosition,0.); // x, y, z
      TGeoCombiTrans segmentCom(segmentTra,segmentRot);
      TGeoHMatrix segmentMat = segmentCom;
      Char_t sourceMatrixName[20];
      sprintf(sourceMatrixName, "SourceMatrix%d", segNum); 
      segmentMat.SetName(sourceMatrixName);
      chamber->AddNode(sourceSeg, segNum, new TGeoHMatrix(segmentMat));
      segmentYPosition += 2.*sourceSegHalfLength; // Shift next segment along by length of segment 
   }
   
   // -------------------------------------
   // -- SOURCE VALVE
   // Valve entrance volume is a shorter source-tube segment-like that connects
   // the valve volume to the source
   TUCNGeoTube *valveVolEntranceShape = new TUCNGeoTube("ValveVolEntrance", valveVolEntranceRMin, valveVolEntranceRMax, valveVolEntranceHalfLength);
   TUCNTrackingVolume* valveVolEntrance = new TUCNTrackingVolume("ValveVolEntrance", valveVolEntranceShape, heliumII);
   valveVolEntrance->SetLineColor(kTeal-3);
   valveVolEntrance->SetLineWidth(1);
   valveVolEntrance->SetVisibility(kTRUE);
   valveVolEntrance->SetTransparency(20);
   // -- Define the Valve volume entrance
   TGeoRotation valveVolEntranceRot("ValveVolEntranceRot",0,valveVolEntranceAngle,0);
   TGeoTranslation valveVolEntranceTra("ValveVolEntranceTra",0.,valveVolEntranceYDisplacement,0.);
   TGeoCombiTrans valveVolEntranceCom(valveVolEntranceTra,valveVolEntranceRot);
   TGeoHMatrix valveVolEntranceMat = valveVolEntranceCom;
   chamber->AddNode(valveVolEntrance, 1, new TGeoHMatrix(valveVolEntranceMat));
   
   // -------------------------------------
   // -- Close Geometry
   geoManager->CloseGeometry();
   
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "$(UCNSIM)/geom/sourcetube_geom.root";
   cerr << "Simulation Geometry Built... Writing to file: " << fileName << endl;
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
   geoManager->SetVisLevel(4); // Default draws 4 levels down volume heirarchy
   geoManager->SetVisOption(0); // Default is 1, but 0 draws all the intermediate volumes not just the final bottom layer
   geoManager->ViewLeaves(kTRUE);
   
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
   Double_t cameraCentre[3] = {0., 0., 0.};
   glViewer->SetPerspectiveCamera(camera,4,100,&cameraCentre[0],0,0);
   // -- Draw Reference Point, Axes
   Double_t refPoint[3] = {0.,0.,0.};
   // Int_t axesType = 0(Off), 1(EDGE), 2(ORIGIN), Bool_t axesDepthTest, Bool_t referenceOn, const Double_t referencePos[3]
   glViewer->SetGuideState(0, kFALSE, kFALSE, refPoint);
   glViewer->UpdateScene();
   return kTRUE;
}
