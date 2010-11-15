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
#include "model_parameters.h"

Bool_t Build_Geom(const TGeoManager* geoManager);
Bool_t Draw_Geom(const TGeoManager* geoManager);

using namespace ModelParameters;

//__________________________________________________________________________
Int_t prevolume_geom()
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
   TGeoMedium* lithium6 = geoManager->GetMedium("Lithium6");
   
   // -------------------------------------
   // -- Making Top Volume
   TUCNGeoBBox* topShape = new TUCNGeoBBox("Top",100,100,100);
   TUCNBlackHole* top = new TUCNBlackHole("Top", topShape, blackhole);
   geoManager->SetTopVolume(top);
   top->SetVisibility(kFALSE);
   
   // -- Make the boundary volume in which all the others sit
   // -- This is what we will be reflecting off all the time
   Double_t surfaceRoughness = 0.1;
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
   // Source tube has 13 segments, all of which are identical 
   // (except one which has a hole in the top)
   
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
   // -- Valve volume front - this is what the valve press against
   TUCNGeoTube *valveVolFrontShape = new TUCNGeoTube("ValveVolFront", valveVolFrontRMin, valveVolFrontRMax, valveVolFrontHalfLength);
   TUCNTrackingVolume* valveVolFront = new TUCNTrackingVolume("ValveVolFront", valveVolFrontShape, heliumII);
   valveVolFront->SetLineColor(kTeal-4);
   valveVolFront->SetLineWidth(1);
   valveVolFront->SetVisibility(kTRUE);
   valveVolFront->SetTransparency(20);
   // -- Define the Valve volume front
   TGeoRotation valveVolFrontRot("ValveVolFrontRot",0,valveVolFrontAngle,0); // phi, theta, psi
   TGeoTranslation valveVolFrontTra("ValveVolFrontTra",0.,valveVolFrontYDisplacement,0.); // x, y, z
   TGeoCombiTrans valveVolFrontCom(valveVolFrontTra,valveVolFrontRot);
   TGeoHMatrix valveVolFrontMat = valveVolFrontCom;
   chamber->AddNode(valveVolFront, 1, new TGeoHMatrix(valveVolFrontMat));
   
   // -------------------------------------
   // -- Valve volume - this is what the valve sits in
   // -- This is joined together with the start of the bend volume to make a composite volume
   TUCNGeoTube *valveVolBackShape = new TUCNGeoTube("ValveVolBack", valveVolBackRMin, valveVolBackRMax, valveVolBackHalfLength);
   TUCNTrackingVolume* valveVolBack = new TUCNTrackingVolume("ValveVolBack", valveVolBackShape, heliumII);
   // -- Define the Valve volume back
   TGeoRotation valveVolBackRot("ValveVolBackRot",0,valveVolBackAngle,0); // phi, theta, psi
   TGeoTranslation valveVolBackTra("ValveVolBackTra",0.,0.,0.); // x, y, z
   TGeoCombiTrans valveVolBackCom(valveVolBackTra,valveVolBackRot);
   TGeoHMatrix valveVolBackMat = valveVolBackCom;
   TGeoHMatrix *valveVolBackMatrix = new TGeoHMatrix(valveVolBackMat);
   valveVolBackMatrix->SetName("ValveVolBackMatrix");
   valveVolBackMatrix->RegisterYourself();
   
   // -- BendEntrance - this is joined to the valve volume back  
   TUCNGeoBBox *bendEntranceShape = new TUCNGeoBBox("BendEntrance", bendEntranceHalfX, bendEntranceHalfY, bendEntranceHalfZ);
   TUCNTrackingVolume* bendEntrance = new TUCNTrackingVolume("BendEntrance", bendEntranceShape, heliumII);
   // -- Define the Bend Entrance segment
   TGeoRotation bendEntranceRot("BendEntranceRot",0,bendEntranceAngle,0); // phi, theta, psi
   TGeoTranslation bendEntranceTra("BendEntranceTra",0.,0.,-bendEntranceZDisplacement); // x, y, z
   TGeoCombiTrans bendEntranceCom(bendEntranceTra,bendEntranceRot);
   TGeoHMatrix bendEntranceMat = bendEntranceCom;
   TGeoHMatrix *bendEntranceMatrix = new TGeoHMatrix(bendEntranceMat);
   bendEntranceMatrix->SetName("BendEntranceMatrix");
   bendEntranceMatrix->RegisterYourself();
   
   // -- Create the composite Valve volume
   TUCNGeoCompositeShape *valveVolShape = new TUCNGeoCompositeShape("ValveVol", "(BendEntrance:BendEntranceMatrix + ValveVolBack:ValveVolBackMatrix)");
   TUCNTrackingVolume* valveVol = new TUCNTrackingVolume("ValveVol",valveVolShape,heliumII);
   valveVol->SetLineColor(kTeal-5);
   valveVol->SetLineWidth(1);
   valveVol->SetVisibility(kTRUE);
   valveVol->SetTransparency(20);// -- Define the Valve volume transformation
   TGeoRotation valveVolRot("ValveVolRot",0,valveVolAngle,0); // phi, theta, psi
   TGeoTranslation valveVolTra("ValveVolTra",0.,valveVolYDisplacement,0.); // x, y, z
   TGeoCombiTrans valveVolCom(valveVolTra,valveVolRot);
   TGeoHMatrix valveVolMat = valveVolCom;
   chamber->AddNode(valveVol, 1, new TGeoHMatrix(valveVolMat));
   
/* // -- Define the valve in its open state 
   Double_t openValveRMin = 0., openValveRMax = 36.0*Units::mm, openValveHalfLength = 1.375*Units::mm;
   TGeoVolume *openValve = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("OpenValve", boundary, openValveRMin, openValveRMax, openValveHalfLength);
   Double_t openValveAngle = 90.0;
   Double_t openValveYDisplacement = valveVolBackHalfLength - openValveHalfLength;
   TGeoRotation openValveRot("ValveVolBackRot",0,openValveAngle,0); // phi, theta, psi
   TGeoTranslation openValveTra("ValveVolBackTra",0.,openValveYDisplacement,0.); // x, y, z
   TGeoCombiTrans openValveCom(openValveTra,openValveRot);
   TGeoHMatrix openValveMat = openValveCom;
   openValve->SetLineColor(kBlue+3);
   openValve->SetLineWidth(1);
   openValve->SetVisibility(kTRUE);
   openValve->SetTransparency(20);
   
   // -- Define the valve in its closed state 
   Double_t closedValveRMin = 0., closedValveRMax = 36.0*Units::mm, closedValveHalfLength = 1.375*Units::mm;
   TGeoVolume *closedValve = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("OpenValve", vacuum, closedValveRMin, closedValveRMax, closedValveHalfLength);
   Double_t closedValveAngle = 90.0;
   Double_t closedValveYDisplacement = closedValveHalfLength - valveVolBackHalfLength;
   TGeoRotation closedValveRot("ValveVolBackRot",0,closedValveAngle,0); // phi, theta, psi
   TGeoTranslation closedValveTra("ValveVolBackTra",0.,closedValveYDisplacement,0.); // x, y, z
   TGeoCombiTrans closedValveCom(closedValveTra,closedValveRot);
   TGeoHMatrix closedValveMat = closedValveCom;
   closedValve->SetLineColor(kBlue+3);
   closedValve->SetLineWidth(1);
   closedValve->SetVisibility(kTRUE);
   closedValve->SetTransparency(20);
   valveVol->AddNode(closedValve,1,new TGeoHMatrix(closedValveMat));
   valveVol->AddNode(openValve,1,new TGeoHMatrix(openValveMat));
*/ 
   
   // -------------------------------------
   // -- BEND
   TUCNGeoTube *circleBendShape = new TUCNGeoTube("CircleBend", bendRMin, bendRMax, bendHalfLength);
   TUCNTrackingVolume* circleBend = new TUCNTrackingVolume("CircleBend", circleBendShape, heliumII);
      
   TUCNGeoBBox *bendBoxShape = new TUCNGeoBBox("BendBox", 2.0*bendHalfLength, bendRMax, bendRMax);
   TUCNTrackingVolume* bendBox = new TUCNTrackingVolume("BendBox", bendBoxShape, beryllium);
   
   // -- Define the transformation of bendbox
   TGeoRotation bendBoxRot("BendBoxRot",0,90,90); // phi, theta, psi
   TGeoTranslation bendBoxTra("BendBoxTra",bendRMax,-bendRMax,0.); // x, y, z
   TGeoCombiTrans bendBoxCom(bendBoxTra,bendBoxRot);
   TGeoHMatrix bendBoxMat = bendBoxCom;
   TGeoHMatrix *bendBoxMatrix = new TGeoHMatrix(bendBoxMat);
   bendBoxMatrix->SetName("BendBoxMatrix");
   bendBoxMatrix->RegisterYourself();
   // -- Create a composite bend
   TUCNGeoCompositeShape* bendShape = new TUCNGeoCompositeShape("BendShape","(CircleBend * BendBox:BendBoxMatrix)");
   TUCNTrackingVolume* bendVol = new TUCNTrackingVolume("BendVol",bendShape,heliumII);
   bendVol->SetLineColor(kYellow-3);
   bendVol->SetLineWidth(1);
   bendVol->SetVisibility(kTRUE);
   bendVol->SetTransparency(20);
   // -- Define the transformation of the bend
   TGeoRotation bendVolRot("BendVolRot",0,bendVolAngle,0); // phi, theta, psi
   TGeoTranslation bendVolTra("BendVolTra", -bendVolXDisplacement, bendVolYDisplacement, -bendVolZDisplacement); 
   TGeoCombiTrans bendVolCom(bendVolTra,bendVolRot);
   TGeoHMatrix bendVolMat = bendVolCom;
   chamber->AddNode(bendVol, 1, new TGeoHMatrix(bendVolMat));
   
   // -------------------------------------
   // -- DETECTOR VALVE
   // DetectorValveVol
   TUCNGeoBBox *detectorValveVolShape = new TUCNGeoBBox("DetectorValveVol", detectorValveVolHalfX, detectorValveVolHalfY, detectorValveVolHalfZ);
   TUCNTrackingVolume* detectorValveVol = new TUCNTrackingVolume("DetectorValveVol", detectorValveVolShape, heliumII);
   detectorValveVol->SetLineColor(kRed+3);
   detectorValveVol->SetLineWidth(1);
   detectorValveVol->SetVisibility(kTRUE);
   detectorValveVol->SetTransparency(0);
   // -- Define the Valve volume back
   TGeoRotation detectorValveVolRot("DetectorValveVolXDisplacementRot",0,detectorValveVolAngle,0); // phi, theta, psi
   TGeoTranslation detectorValveVolTra("DetectorValveVolXDisplacementTra", -detectorValveVolXDisplacement, detectorValveVolYDisplacement, -detectorValveVolZDisplacement);
   TGeoCombiTrans detectorValveVolCom(detectorValveVolTra,detectorValveVolRot);
   TGeoHMatrix detectorValveVolMat = detectorValveVolCom;
   chamber->AddNode(detectorValveVol, 1, new TGeoHMatrix(detectorValveVolMat));
   
/* // -------------------------------------
   // -- DetectorValve
   Double_t detectorValveHalfX = 2.0*Units::mm, detectorValveHalfY = 30.0*Units::mm, detectorValveHalfZ = 30.0*Units::mm;
   TGeoVolume *detectorValve = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("DetectorValve", vacuum, detectorValveHalfX, detectorValveHalfY, detectorValveHalfZ);
   detectorValve->SetLineColor(kBlue+3);
   detectorValve->SetLineWidth(1);
   detectorValve->SetVisibility(kTRUE);
   detectorValve->SetTransparency(0);
   // -- Define the Valve volume back
   Double_t detectorValveAngle = -50.0;
   Double_t detectorValveXDisplacement = 0.;
   Double_t detectorValveYDisplacement = 0.;
   Double_t detectorValveZDisplacement = 9.0*Units::mm;
   TGeoRotation detectorValveRot("DetectorValveXDisplacementRot",90,detectorValveAngle,90); // phi, theta, psi
   TGeoTranslation detectorValveTra("DetectorValveXDisplacementTra", -detectorValveXDisplacement, detectorValveYDisplacement, -detectorValveZDisplacement);
   TGeoCombiTrans detectorValveCom(detectorValveTra,detectorValveRot);
   TGeoHMatrix detectorValveMat = detectorValveCom;
   detectorValveVol->AddNode(detectorValve, 1, new TGeoHMatrix(detectorValveMat));
*/ 
   // -------------------------------------
   // -- DETECTOR TUBE
   // DetectorTubeTop - Entrance into the detector tube
   TUCNGeoTube *detectorTubeTopShape = new TUCNGeoTube("DetectorTubeTop", detectorTubeTopRMin, detectorTubeTopRMax, detectorTubeTopHalfLength);
   TUCNTrackingVolume* detectorTubeTop = new TUCNTrackingVolume("DetectorTubeTop", detectorTubeTopShape, heliumII);
   detectorTubeTop->SetLineColor(kOrange+1);
   detectorTubeTop->SetLineWidth(1);
   detectorTubeTop->SetVisibility(kTRUE);
   detectorTubeTop->SetTransparency(20);
   // -- Define the Valve volume back
   TGeoRotation detectorTubeTopRot("DetectorTubeTopRot",0,detectorTubeTopAngle,0);
   TGeoTranslation detectorTubeTopTra("DetectorTubeTopTra", -detectorTubeTopXDisplacement, detectorTubeTopYDisplacement, -detectorTubeTopZDisplacement);
   TGeoCombiTrans detectorTubeTopCom(detectorTubeTopTra,detectorTubeTopRot);
   TGeoHMatrix detectorTubeTopMat = detectorTubeTopCom; 
   chamber->AddNode(detectorTubeTop, 1, new TGeoHMatrix(detectorTubeTopMat));
   
   // -------------------------------------
   // -- DetectorTube
   TUCNGeoTube *detectorTubeShape = new TUCNGeoTube("DetectorTube", detectorTubeRMin, detectorTubeRMax, detectorTubeHalfLength);
   TUCNTrackingVolume* detectorTube = new TUCNTrackingVolume("DetectorTube", detectorTubeShape, heliumII);
   detectorTube->SetLineColor(kOrange+1);
   detectorTube->SetLineWidth(1);
   detectorTube->SetVisibility(kTRUE);
   detectorTube->SetTransparency(20);
   // -- Define the Valve volume back
   TGeoRotation detectorTubeRot("DetectorTubeRot",0,detectorTubeAngle,0);
   TGeoTranslation detectorTubeTra("DetectorTubeTra", -detectorTubeXDisplacement, detectorTubeYDisplacement, -detectorTubeZDisplacement);
   TGeoCombiTrans detectorTubeCom(detectorTubeTra,detectorTubeRot);
   TGeoHMatrix detectorTubeMat = detectorTubeCom;
   chamber->AddNode(detectorTube, 1, new TGeoHMatrix(detectorTubeMat));
   
   // -------------------------------------
   // -- Detector
   Double_t detectorEfficiency = 1.0;
   TUCNGeoTube *detectorShape = new TUCNGeoTube("Detector", detectorRMin, detectorRMax, detectorHalfLength);
   TUCNDetector* detector = new TUCNDetector("Detector",detectorShape,lithium6,detectorEfficiency);
   detector->SetLineColor(kGray+3);
   detector->SetLineWidth(1);
   detector->SetVisibility(kTRUE);
   detector->SetTransparency(0);
   // -- Define the Valve volume back
   TGeoRotation detectorRot("DetectorRot",0,detectorTubeAngle,0);
   TGeoTranslation detectorTra("DetectorTra", -detectorXDisplacement,detectorYDisplacement,-detectorZDisplacement);
   TGeoCombiTrans detectorCom(detectorTra,detectorRot);
   TGeoHMatrix detectorMat = detectorCom;
   chamber->AddNode(detector, 1, new TGeoHMatrix(detectorMat));
   
   // -------------------------------------
   // -- GUIDE SECTION
   // Guide section has *probably* 4 segments. 
   TUCNGeoBBox *guideSegShape = new TUCNGeoBBox("GuideSeg", guideSegHalfX, guideSegHalfY, guideSegHalfZ);
   TUCNTrackingVolume* guideSeg = new TUCNTrackingVolume("GuideSeg", guideSegShape, heliumII);
   guideSeg->SetLineColor(kCyan-8);
   guideSeg->SetLineWidth(1);
   guideSeg->SetVisibility(kTRUE);
   guideSeg->SetTransparency(20);
   Double_t guideSegXPos = guideXDisplacement;
   for (Int_t segNum = 1; segNum <= 5; segNum++) {
      // Define Guide Seg matrix
      TGeoRotation segmentRot("SegmentRot",guidePhi,guideTheta,0); // phi, theta, psi
      TGeoTranslation segmentTra("SegmentTra",guideSegXPos, guideYDisplacement, guideZDisplacement);
      TGeoCombiTrans segmentCom(segmentTra,segmentRot);
      TGeoHMatrix segmentMat = segmentCom;
      Char_t sourceMatrixName[20];
      sprintf(sourceMatrixName, "GuideSegMatrix%d", segNum);
      segmentMat.SetName(sourceMatrixName);
      chamber->AddNode(guideSeg, segNum, new TGeoHMatrix(segmentMat));
      // Shift next segment along by length of segment
      guideSegXPos = guideSegXPos - 2.0*guideSegHalfZ;
   }
   
   // -------------------------------------
   // -- RAMSEY CELL PRE-VOLUME SECTION
   // -- Define General Box shape dimensions
   TUCNGeoBBox *preVolumeBoxShape = new TUCNGeoBBox("PreVolumeBoxShape", preVolumeBoxHalfX, preVolumeBoxHalfY, preVolumeBoxHalfZ);
   TGeoRotation preVolumeBoxRot("PreVolumeBoxRot", preVolumeBoxPhi, preVolumeBoxTheta, preVolumeBoxPsi);
   TGeoTranslation preVolumeBoxTra("PreVolumeBoxTra", preVolumeBoxXDisplacement, preVolumeBoxYDisplacement, preVolumeBoxZDisplacement);
   TGeoCombiTrans preVolumeBoxCom(preVolumeBoxTra,preVolumeBoxRot);
   TGeoHMatrix preVolumeBoxMat = preVolumeBoxCom;
   TGeoHMatrix *preVolumeBoxMatrix = new TGeoHMatrix(preVolumeBoxMat);
   preVolumeBoxMatrix->SetName("PreVolumeBoxMatrix");
   preVolumeBoxMatrix->RegisterYourself();
   
   // -- Define the pieces to make the rounded corners of the pre-volume
   // Make the square pieces to be subtracted from the corners of the box
   TUCNGeoBBox *preVolumeBoxCornerShape = new TUCNGeoBBox("PreVolumeBoxCornerShape", preVolumeBoxCornerHalfX, preVolumeBoxCornerHalfY, preVolumeBoxCornerHalfZ);
   // Define Corner Box 1 matrix
   TGeoRotation preVolumeBoxCornerRot("PreVolumeBoxCornerRot", preVolumeBoxCornerPhi, preVolumeBoxCornerTheta, preVolumeBoxCornerPsi);
   TGeoTranslation preVolumeBoxCorner1Tra("PreVolumeBoxCorner1Tra",preVolumeBoxCorner1XDisplacement, preVolumeBoxCorner1YDisplacement, preVolumeBoxCorner1ZDisplacement);
   TGeoCombiTrans preVolumeBoxCorner1Com(preVolumeBoxCorner1Tra, preVolumeBoxCornerRot);
   TGeoHMatrix preVolumeBoxCorner1Mat = preVolumeBoxCorner1Com;
   TGeoHMatrix *preVolumeBoxCorner1Matrix = new TGeoHMatrix(preVolumeBoxCorner1Mat);
   preVolumeBoxCorner1Matrix->SetName("PreVolumeBoxCorner1Matrix");
   preVolumeBoxCorner1Matrix->RegisterYourself();
   // Define Corner Box 2 matrix
   TGeoTranslation preVolumeBoxCorner2Tra("PreVolumeBoxCorner2Tra",preVolumeBoxCorner2XDisplacement, preVolumeBoxCorner2YDisplacement, preVolumeBoxCorner2ZDisplacement);
   TGeoCombiTrans preVolumeBoxCorner2Com(preVolumeBoxCorner2Tra, preVolumeBoxCornerRot);
   TGeoHMatrix preVolumeBoxCorner2Mat = preVolumeBoxCorner2Com;
   TGeoHMatrix *preVolumeBoxCorner2Matrix = new TGeoHMatrix(preVolumeBoxCorner2Mat);
   preVolumeBoxCorner2Matrix->SetName("PreVolumeBoxCorner2Matrix");
   preVolumeBoxCorner2Matrix->RegisterYourself();
   // Define Corner Box 3 matrix
   TGeoTranslation preVolumeBoxCorner3Tra("PreVolumeBoxCorner3Tra",preVolumeBoxCorner3XDisplacement, preVolumeBoxCorner3YDisplacement, preVolumeBoxCorner3ZDisplacement);
   TGeoCombiTrans preVolumeBoxCorner3Com(preVolumeBoxCorner3Tra, preVolumeBoxCornerRot);
   TGeoHMatrix preVolumeBoxCorner3Mat = preVolumeBoxCorner3Com;
   TGeoHMatrix *preVolumeBoxCorner3Matrix = new TGeoHMatrix(preVolumeBoxCorner3Mat);
   preVolumeBoxCorner3Matrix->SetName("PreVolumeBoxCorner3Matrix");
   preVolumeBoxCorner3Matrix->RegisterYourself();
   // Define Corner Box 4 matrix
   TGeoTranslation preVolumeBoxCorner4Tra("PreVolumeBoxCorner4Tra",preVolumeBoxCorner4XDisplacement, preVolumeBoxCorner4YDisplacement, preVolumeBoxCorner4ZDisplacement);
   TGeoCombiTrans preVolumeBoxCorner4Com(preVolumeBoxCorner4Tra, preVolumeBoxCornerRot);
   TGeoHMatrix preVolumeBoxCorner4Mat = preVolumeBoxCorner4Com;
   TGeoHMatrix *preVolumeBoxCorner4Matrix = new TGeoHMatrix(preVolumeBoxCorner4Mat);
   preVolumeBoxCorner4Matrix->SetName("PreVolumeBoxCorner4Matrix");
   preVolumeBoxCorner4Matrix->RegisterYourself();
   
   // Make the rounded corners of the pre-volume
   TUCNGeoTube *preVolumeCornerShape = new TUCNGeoTube("PreVolumeCornerShape", preVolumeCornerRMin, preVolumeCornerRMax, preVolumeCornerHalfZ);
   // Define Corner 1 Matrix
   TGeoRotation preVolumeCornerRot("PreVolumeCornerRot", preVolumeCornerPhi, preVolumeCornerTheta, preVolumeCornerPsi);
   TGeoTranslation preVolumeCorner1Tra("PreVolumeCorner1Tra", preVolumeCorner1XDisplacement, preVolumeCorner1YDisplacement, preVolumeCorner1ZDisplacement);
   TGeoCombiTrans preVolumeCorner1Com(preVolumeCorner1Tra, preVolumeCornerRot);
   TGeoHMatrix preVolumeCorner1Mat = preVolumeCorner1Com;
   TGeoHMatrix *preVolumeCorner1Matrix = new TGeoHMatrix(preVolumeCorner1Mat);
   preVolumeCorner1Matrix->SetName("PreVolumeCorner1Matrix");
   preVolumeCorner1Matrix->RegisterYourself();
   // Define Corner 2 Matrix
   TGeoTranslation preVolumeCorner2Tra("PreVolumeCorner2Tra", preVolumeCorner2XDisplacement, preVolumeCorner2YDisplacement, preVolumeCorner2ZDisplacement);
   TGeoCombiTrans preVolumeCorner2Com(preVolumeCorner2Tra, preVolumeCornerRot);
   TGeoHMatrix preVolumeCorner2Mat = preVolumeCorner2Com;
   TGeoHMatrix *preVolumeCorner2Matrix = new TGeoHMatrix(preVolumeCorner2Mat);
   preVolumeCorner2Matrix->SetName("PreVolumeCorner2Matrix");
   preVolumeCorner2Matrix->RegisterYourself();
   // Define Corner 3 Matrix
   TGeoTranslation preVolumeCorner3Tra("PreVolumeCorner3Tra", preVolumeCorner3XDisplacement, preVolumeCorner3YDisplacement, preVolumeCorner3ZDisplacement);
   TGeoCombiTrans preVolumeCorner3Com(preVolumeCorner3Tra, preVolumeCornerRot);
   TGeoHMatrix preVolumeCorner3Mat = preVolumeCorner3Com;
   TGeoHMatrix *preVolumeCorner3Matrix = new TGeoHMatrix(preVolumeCorner3Mat);
   preVolumeCorner3Matrix->SetName("PreVolumeCorner3Matrix");
   preVolumeCorner3Matrix->RegisterYourself();
   // Define Corner 4 Matrix
   TGeoTranslation preVolumeCorner4Tra("PreVolumeCorner4Tra", preVolumeCorner4XDisplacement, preVolumeCorner4YDisplacement, preVolumeCorner4ZDisplacement);
   TGeoCombiTrans preVolumeCorner4Com(preVolumeCorner4Tra, preVolumeCornerRot);
   TGeoHMatrix preVolumeCorner4Mat = preVolumeCorner4Com;
   TGeoHMatrix *preVolumeCorner4Matrix = new TGeoHMatrix(preVolumeCorner4Mat);
   preVolumeCorner4Matrix->SetName("PreVolumeCorner4Matrix");
   preVolumeCorner4Matrix->RegisterYourself();
   
   // -- Create the composite Ramsey Cell Pre-Volume
   TUCNGeoCompositeShape *preVolumeShape = new TUCNGeoCompositeShape("PreVolume", "((((((((PreVolumeBoxShape:PreVolumeBoxMatrix - PreVolumeBoxCornerShape:PreVolumeBoxCorner1Matrix) - PreVolumeBoxCornerShape:PreVolumeBoxCorner2Matrix) - PreVolumeBoxCornerShape:PreVolumeBoxCorner3Matrix) - PreVolumeBoxCornerShape:PreVolumeBoxCorner4Matrix) + PreVolumeCornerShape:PreVolumeCorner1Matrix) + PreVolumeCornerShape:PreVolumeCorner2Matrix) + PreVolumeCornerShape:PreVolumeCorner3Matrix) + PreVolumeCornerShape:PreVolumeCorner4Matrix)");
   TUCNTrackingVolume* preVolume = new TUCNTrackingVolume("PreVolume",preVolumeShape,heliumII);
   preVolume->SetLineColor(kMagenta-5);
   preVolume->SetLineWidth(1);
   preVolume->SetVisibility(kTRUE);
   preVolume->SetTransparency(20);// -- Define the Valve volume transformation
   TGeoRotation preVolumeRot("ValveVolRot",preVolumePhi, preVolumeTheta, preVolumePsi);
   TGeoTranslation preVolumeTra("ValveVolTra", preVolumeXDisplacement, preVolumeYDisplacement, preVolumeZDisplacement);
   TGeoCombiTrans preVolumeCom(preVolumeTra,preVolumeRot);
   TGeoHMatrix preVolumeMat = preVolumeCom;
   chamber->AddNode(preVolume, 1, new TGeoHMatrix(preVolumeMat));
   
   
   // -------------------------------------
   // -- Close Geometry
   geoManager->CloseGeometry();
   
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "$(UCN_GEOM)/prevolume_geom.root";
   cerr << "Simulation Geometry Built... Writing to file: " << fileName << endl;
   geoManager->Export(fileName);
   
   // -------------------------------------
   // -- VISUALISATION GEOM
   // -- Here we swap out TUCNCompositeShape for TGeoCompositeShape due to explicit
   // -- TGeoShape downcasting problems in TGeoPainter. The geometry that is left becomes
   // --  the visualisation geometry used for drawing
   // First replace the composite shapes in the geometry
   TGeoCompositeShape *valveVolShapeVis = new TGeoCompositeShape("ValveVolVis",                                    "(BendEntrance:BendEntranceMatrix + ValveVolBack:ValveVolBackMatrix)");
   valveVol->SetShape(valveVolShapeVis);
   
   TGeoCompositeShape* bendShapeVis = new TGeoCompositeShape("BendShapeVis","(CircleBend * BendBox:BendBoxMatrix)");
   bendVol->SetShape(bendShapeVis);
   
   TGeoCompositeShape* preVolumeShapeVis = new TGeoCompositeShape("PreVolumeShapeVis", "((((((((PreVolumeBoxShape:PreVolumeBoxMatrix - PreVolumeBoxCornerShape:PreVolumeBoxCorner1Matrix) - PreVolumeBoxCornerShape:PreVolumeBoxCorner2Matrix) - PreVolumeBoxCornerShape:PreVolumeBoxCorner3Matrix) - PreVolumeBoxCornerShape:PreVolumeBoxCorner4Matrix) + PreVolumeCornerShape:PreVolumeCorner1Matrix) + PreVolumeCornerShape:PreVolumeCorner2Matrix) + PreVolumeCornerShape:PreVolumeCorner3Matrix) + PreVolumeCornerShape:PreVolumeCorner4Matrix)");
   preVolume->SetShape(preVolumeShapeVis);
   
   // -------------------------------------
   // -- Write out visualisation geometry to file
   const char *visFileName = "$(UCN_GEOM)/prevolume_vis.root";
   cerr << "Visualisation Geometry Built... Writing to file: " << visFileName << endl;
   geoManager->Export(visFileName);
   
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
   geoManager->SetVisOption(1); // Default is 1, but 0 draws all the intermediate volumes not just the final bottom layer
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
   Double_t cameraCentre[3] = {preVolumeXDisplacement, preVolumeYDisplacement, preVolumeZDisplacement};
   glViewer->SetPerspectiveCamera(camera,4,100,&cameraCentre[0],0,0);
   // -- Draw Reference Point, Axes
   Double_t refPoint[3] = {0.,0.,0.};
   // Int_t axesType = 0(Off), 1(EDGE), 2(ORIGIN), Bool_t axesDepthTest, Bool_t referenceOn, const Double_t referencePos[3]
   glViewer->SetGuideState(0, kFALSE, kFALSE, refPoint);
   glViewer->UpdateScene();
   return kTRUE;
}
