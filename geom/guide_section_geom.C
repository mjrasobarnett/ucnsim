// This is a model of the cryoEDM experiment, using dimensions from the model created by
// Vishal Francis. 
// 9/01/2010 - Model covers the source tube down to the neutron detector
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
   
   // -- Valve Volume Front
   const Double_t valveVolFrontRMin = 0.;
   const Double_t valveVolFrontRMax = 31.5*Units::mm;
   const Double_t valveVolFrontHalfLength = 12.75.*Units::mm;
   const Double_t valveVolFrontAngle = 90.0; 
   const Double_t valveVolFrontYDisplacement = valveVolEntranceYDisplacement + valveVolEntranceHalfLength + valveVolFrontHalfLength;
   
   // -- Valve Volume
   // Valve Volume Back
   const Double_t valveVolBackRMin = 0.;
   const Double_t valveVolBackRMax = 36.0*Units::mm;
   const Double_t valveVolBackHalfLength = 17.75*Units::mm;
   const Double_t valveVolBackAngle = 90.0;
   // Bend Entrance
   const Double_t bendEntranceHalfX = 30.0*Units::mm;
   const Double_t bendEntranceHalfY = 15.0*Units::mm;
   const Double_t bendEntranceHalfZ = 55.0*Units::mm;
   const Double_t bendEntranceAngle = 0.0;
   const Double_t bendEntranceZDisplacement = bendEntranceHalfZ;
   // Composite
   const Double_t valveVolAngle = 0.0;
   const Double_t valveVolYDisplacement = valveVolFrontYDisplacement + valveVolFrontHalfLength + valveVolBackHalfLength;
   
   // -- Bend
   const Double_t bendRMin = 160.0*Units::mm;
   const Double_t bendRMax = 220.0*Units::mm;
   const Double_t bendHalfLength = 15.0*Units::mm;
   const Double_t bendVolAngle = 90.0;
   const Double_t bendVolYDisplacement = valveVolYDisplacement;
   const Double_t bendVolXDisplacement = (bendRMin + bendRMax)/2.0;
   const Double_t bendVolZDisplacement = 2.0*bendEntranceHalfZ;							
   
   // -- Detector Valve Volume
   const Double_t detectorValveVolHalfX = 55.0*Units::mm;
   const Double_t detectorValveVolHalfY = 30.0*Units::mm;
   const Double_t detectorValveVolHalfZ = 30.0*Units::mm;
   const Double_t detectorValveVolAngle = 0.0;
   const Double_t detectorValveVolXDisplacement = bendVolXDisplacement + detectorValveVolHalfX;
   const Double_t detectorValveVolYDisplacement = valveVolYDisplacement;
   const Double_t detectorValveVolZDisplacement = bendVolZDisplacement + bendVolXDisplacement;
   
   // -- Detector Tube Top
   const Double_t detectorTubeTopRMin = 0.;
   const Double_t detectorTubeTopRMax = 25.0*Units::mm;
   const Double_t detectorTubeTopHalfLength = 15.75*Units::mm;
   const Double_t detectorTubeTopAngle = 0.0;
   const Double_t detectorTubeTopXDisplacement = detectorValveVolXDisplacement;
   const Double_t detectorTubeTopYDisplacement = valveVolYDisplacement;
   const Double_t detectorTubeTopZDisplacement = detectorValveVolZDisplacement + detectorValveVolHalfZ + detectorTubeTopHalfLength;
   
   // -- Detector Tube
   const Double_t detectorTubeRMin = 0.;
   const Double_t detectorTubeRMax = 27.85*Units::mm;
   const Double_t detectorTubeHalfLength = 250.75*Units::mm;
   const Double_t detectorTubeAngle = 0.0;
   const Double_t detectorTubeXDisplacement = detectorValveVolXDisplacement;
   const Double_t detectorTubeYDisplacement = valveVolYDisplacement;
   const Double_t detectorTubeZDisplacement = detectorTubeTopZDisplacement + detectorTubeTopHalfLength + detectorTubeHalfLength;
   
   // -- Detector
   const Double_t detectorRMin = 0.;
   const Double_t detectorRMax = 27.85*Units::mm;
   const Double_t detectorHalfLength = 5.*Units::mm;
   const Double_t detectorAngle = 0.0;
   const Double_t detectorXDisplacement = detectorValveVolXDisplacement;
   const Double_t detectorYDisplacement = valveVolYDisplacement;
   const Double_t detectorZDisplacement = detectorTubeZDisplacement + detectorTubeHalfLength + detectorHalfLength;
   
   // -- Guide Sections Segment
   // Note that I am unsure of the exact number and lengths of each individual section. The total
   // length of the whole assembly is definitely 2890.6mm and is either made of 4 or 5 sections.
   // For now we will use 5 equal sections.
   const Double_t guideSegHalfX = 25.0*Units::mm;
   const Double_t guideSegHalfY = 25.0*Units::mm;
   const Double_t guideSegHalfZ = 289.0*Units::mm;
   const Double_t guidePhi = -90;
   const Double_t guideTheta = 90;
   const Double_t guideXDisplacement = -detectorValveVolXDisplacement - detectorValveVolHalfX - guideSegHalfZ;
   const Double_t guideYDisplacement = detectorValveVolYDisplacement;
   const Double_t guideZDisplacement = -detectorValveVolZDisplacement;
}

using namespace ModelParameters;

//__________________________________________________________________________
Int_t guide_section_geom()
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
   TUCNGeoCompositeShape *valveVolShape = new TUCNGeoCompositeShape("ValveVol","(BendEntrance:BendEntranceMatrix + ValveVolBack:ValveVolBackMatrix)");
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
   TUCNGeoTube *guideSegShape = new TUCNGeoBBox("GuideSeg", guideSegHalfX, guideSegHalfY, guideSegHalfZ);
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
   // -- Close Geometry
   geoManager->CloseGeometry();
   
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "$(UCN_GEOM)/guide_section_geom.root";
   cerr << "Simulation Geometry Built... Writing to file: " << fileName << endl;
   geoManager->Export(fileName);
   
   // -------------------------------------
   // -- VISUALISATION GEOM
   // -- Here we swap out TUCNCompositeShape for TGeoCompositeShape due to explicit TGeoShape downcasting
   // -- problems in TGeoPainter. The geometry that is left becomes the visualisation geometry used for drawing
   
   // -- First replace the composite shapes in the geometry
   TGeoCompositeShape *valveVolShapeVis = new TGeoCompositeShape("ValveVolVis",                                    "(BendEntrance:BendEntranceMatrix + ValveVolBack:ValveVolBackMatrix)");
   TGeoCompositeShape* bendShapeVis = new TGeoCompositeShape("BendShapeVis","(CircleBend * BendBox:BendBoxMatrix)");
   valveVol->SetShape(valveVolShapeVis);
   bendVol->SetShape(bendShapeVis);
   
   // -------------------------------------
   // -- Write out visualisation geometry to file
   const char *visFileName = "$(UCN_GEOM)/guide_section_vis.root";
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
   Double_t cameraCentre[3] = {detectorValveVolXDisplacement, detectorValveVolYDisplacement, detectorValveVolZDisplacement};
   glViewer->SetPerspectiveCamera(camera,4,100,&cameraCentre[0],0,0);
   // -- Draw Reference Point, Axes
   Double_t refPoint[3] = {0.,0.,0.};
   // Int_t axesType = 0(Off), 1(EDGE), 2(ORIGIN), Bool_t axesDepthTest, Bool_t referenceOn, const Double_t referencePos[3]
   glViewer->SetGuideState(0, kFALSE, kFALSE, refPoint);
   glViewer->UpdateScene();
   return kTRUE;
}
