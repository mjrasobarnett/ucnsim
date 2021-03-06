// This is a model of the 'Real' cryoEDM sourcetube - 25/01/2010
#include "TGeoManager.h"
#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"

#include "../../include/Units.h"
#include "../../include/Constants.h"
#include "../../include/Materials.h"
#include "../../include/GeomParameters.h"

Bool_t Build_Geom(const TGeoManager* geoManager);
Bool_t Draw_Geom(const TGeoManager* geoManager);

using namespace GeomParameters;

//__________________________________________________________________________
Int_t sourcetube_gaps()
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
   // Materials - Define the materials used. Leave the neutron properties 
   // to be defined on a run-by-run basis
   Materials::BuildMaterials(geoManager);
   TGeoMedium* beryllium = geoManager->GetMedium("Beryllium");
   TGeoMedium* blackhole = geoManager->GetMedium("BlackHole");
   TGeoMedium* heliumII = geoManager->GetMedium("HeliumII");
   
   // -------------------------------------
   // -- Making Top Volume
   Box* topShape = new Box("Top",100,100,100);
   BlackHole* top = new BlackHole("Top", topShape, blackhole);
   geoManager->SetTopVolume(top);
   top->SetVisibility(kFALSE);
   
   // -- Make the boundary volume in which all the others sit
   // -- This is what we will be reflecting off all the time
   Double_t surfaceRoughness = 0.1;
   Box* chamberShape = new Box("Chamber",10,10,10);
   Boundary* chamber = new Boundary("Chamber", chamberShape, beryllium, surfaceRoughness);
   chamber->SetLineColor(kOrange-7);
   chamber->SetLineWidth(1);
   chamber->SetVisibility(kFALSE);
   chamber->SetTransparency(80);
   // Add to geom
   top->AddNode(chamber,1);
   
   // -------------------------------------
   // -- SOURCE TUBE
   // -- Source tube has 13 segments, all of which are identical 
   // -- (except one which has a hole in the top)
   Double_t segmentYPosition = sourceSegHalfLength;
   for (Int_t segNum = 1; segNum <= 13; segNum++) {
      // -- Make a SourceTube Segment
      Char_t sourceMatrixName[20], sourceSegName[20], sourceGapMatrixName[20];
      sprintf(sourceSegName, "SourceSeg%d", segNum); 
      Tube *sourceSegShape = new Tube(sourceSegName, sourceSegRMin, sourceSegRMax, sourceSegHalfLength);
      TrackingVolume* sourceSeg = new TrackingVolume(sourceSegName, sourceSegShape, heliumII);
      sourceSeg->SetLineColor(kAzure-4);
      sourceSeg->SetLineWidth(1);
      sourceSeg->SetVisibility(kTRUE);
      sourceSeg->SetTransparency(20);
      // -- Define SourceTube matrix
      TGeoRotation segmentRot("SegmentRot",0,sourceSegAngle,0); // phi, theta, psi
      TGeoTranslation segmentTra("SegmentTra",0.,segmentYPosition,0.); // x, y, z
      TGeoCombiTrans segmentCom(segmentTra,segmentRot);
      TGeoHMatrix segmentMat = segmentCom;
      sprintf(sourceMatrixName, "SourceMatrix%d", segNum); 
      segmentMat.SetName(sourceMatrixName);
      // Add SourceTube segment to geometry
      chamber->AddNode(sourceSeg, segNum, new TGeoHMatrix(segmentMat));
      // Shift next segment along by length of segment
      segmentYPosition += 2.0*sourceSegHalfLength;
   }
   
   // -------------------------------------
   // -- SOURCE TUBE GAPS
   // -- Create 12 gaps between each source tube segment 
   Double_t gapYPosition = 2.0*sourceSegHalfLength;
   for (Int_t segNum = 1; segNum <= 13; segNum++) {
      // -- Make a Gap between Segments
      Char_t sourceGapMatrixName[20], sourceSegGapName[20];
      sprintf(sourceSegGapName, "SourceSegGap%d", segNum); 
      Tube *sourceSegGapShape = new Tube(sourceSegGapName, sourceSegGapRMin, sourceSegGapRMax, sourceSegGapHalfLength);
      BlackHole* sourceSegGap = new BlackHole(sourceSegGapName, sourceSegGapShape, blackhole);
      sourceSegGap->SetLineColor(kGray);
      sourceSegGap->SetLineWidth(1);
      sourceSegGap->SetVisibility(kTRUE);
      sourceSegGap->SetTransparency(20);
      // -- Define SourceTube matrix
      TGeoRotation segmentGapRot("SegmentGapRot",0,sourceSegGapAngle,0); // phi, theta, psi
      TGeoTranslation segmentGapTra("SegmentGapTra",0.,gapYPosition,0.); // x, y, z
      TGeoCombiTrans segmentGapCom(segmentGapTra,segmentGapRot);
      TGeoHMatrix segmentGapMat = segmentGapCom;
      sprintf(sourceGapMatrixName, "SourceGapMatrix%d", segNum); 
      segmentGapMat.SetName(sourceGapMatrixName);
      // Add SourceTube Gap segment to geometry
      chamber->AddNode(sourceSegGap, segNum, new TGeoHMatrix(segmentGapMat));
      // Shift next segment along by length of segment
      gapYPosition += 2.0*sourceSegHalfLength;
   }
   
   // -------------------------------------
   // -- SOURCE VALVE
   // Valve entrance volume is a shorter source-tube segment-like that connects
   // the valve volume to the source
   Tube *valveVolEntranceShape = new Tube("ValveVolEntrance", valveVolEntranceRMin, valveVolEntranceRMax, valveVolEntranceHalfLength);
   TrackingVolume* valveVolEntrance = new TrackingVolume("ValveVolEntrance", valveVolEntranceShape, heliumII);
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
   const char *fileName = "$(UCN_GEOM)/sourcetube/sourcetube_gaps.root";
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
