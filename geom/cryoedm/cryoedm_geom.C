// This is a model of the cryoEDM experiment, using dimensions from the model created by
// Vishal Francis. 
// 14/11/2010 - Model covers whole neutron facing geometry to a first approximation. 
// Things missing currently includes valves
#include "TGeoManager.h"
#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"

#include "include/Units.h"
#include "include/Constants.h"
#include "include/Materials.h"
#include "include/GeomParameters.h"

Bool_t Build_Geom(const TGeoManager* geoManager);
Bool_t Draw_Geom(const TGeoManager* geoManager);

using namespace GeomParameters;

//__________________________________________________________________________
Int_t cryoedm_geom()
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
   // Source tube has 13 segments, all of which are identical 
   // (except one which has a hole in the top)

   // -- Make a SourceTube Segment
   Tube *sourceSegShape = new Tube("SourceSeg", sourceSegRMin, sourceSegRMax, sourceSegHalfLength);
   TrackingVolume* sourceSeg = new TrackingVolume("SourceSeg", sourceSegShape, heliumII);
   sourceSeg->SetLineColor(kAzure-4);
   sourceSeg->SetLineWidth(1);
   sourceSeg->SetVisibility(kTRUE);
   sourceSeg->SetTransparency(20);
   Double_t sourceCapacity = 0.0;
   Double_t segmentYPos = sourceSegHalfLength;
   for (Int_t segNum = 1; segNum <= sourceSegments; segNum++) {
      // -- Define SourceTube matrix
      // Rotation seems to be applied before translation so bear that in mind when choosing which
      // coordinate to transform in translation
      TGeoRotation segmentRot("SegmentRot",sourceSegPhi,sourceSegTheta,sourceSegPsi);
      TGeoTranslation segmentTra("SegmentTra",sourceSegXPos, segmentYPos, sourceSegZPos);
      TGeoCombiTrans segmentCom(segmentTra,segmentRot);
      TGeoHMatrix segmentMat = segmentCom;
      Char_t sourceMatrixName[20];
      sprintf(sourceMatrixName, "SourceMatrix%d", segNum); 
      segmentMat.SetName(sourceMatrixName);
      chamber->AddNode(sourceSeg, segNum, new TGeoHMatrix(segmentMat));
      segmentYPos += 2.*sourceSegHalfLength; // Shift next segment along by length of segment 
      sourceCapacity += sourceSegShape->Capacity();
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
   TGeoRotation valveVolEntranceRot("ValveVolEntranceRot",valveVolEntrancePhi,valveVolEntranceTheta, valveVolEntrancePsi);
   TGeoTranslation valveVolEntranceTra("ValveVolEntranceTra",valveVolEntranceXPos, valveVolEntranceYPos, valveVolEntranceZPos);
   TGeoCombiTrans valveVolEntranceCom(valveVolEntranceTra,valveVolEntranceRot);
   TGeoHMatrix valveVolEntranceMat = valveVolEntranceCom;
   chamber->AddNode(valveVolEntrance, 1, new TGeoHMatrix(valveVolEntranceMat));
   Double_t valveVolEntraceCapacity = valveVolEntranceShape->Capacity();

   // -------------------------------------
   // -- Valve volume front - this is what the valve press against
   Tube *valveVolFrontShape = new Tube("ValveVolFront", valveVolFrontRMin, valveVolFrontRMax, valveVolFrontHalfLength);
   TrackingVolume* valveVolFront = new TrackingVolume("ValveVolFront", valveVolFrontShape, heliumII);
   valveVolFront->SetLineColor(kTeal-4);
   valveVolFront->SetLineWidth(1);
   valveVolFront->SetVisibility(kTRUE);
   valveVolFront->SetTransparency(20);
   // -- Define the Valve volume front
   TGeoRotation valveVolFrontRot("ValveVolFrontRot",valveVolFrontPhi, valveVolFrontTheta, valveVolFrontPsi);
   TGeoTranslation valveVolFrontTra("ValveVolFrontTra",valveVolFrontXPos, valveVolFrontYPos, valveVolFrontZPos);
   TGeoCombiTrans valveVolFrontCom(valveVolFrontTra,valveVolFrontRot);
   TGeoHMatrix valveVolFrontMat = valveVolFrontCom;
   chamber->AddNode(valveVolFront, 1, new TGeoHMatrix(valveVolFrontMat));
   Double_t valveVolFrontCapacity = valveVolFrontShape->Capacity();

   // -------------------------------------
   // -- Valve volume - this is what the valve sits in
   // -- This is joined together with the start of the bend volume to make a composite volume
   Tube *valveVolBackShape = new Tube("ValveVolBack", valveVolBackRMin, valveVolBackRMax, valveVolBackHalfLength);
   TrackingVolume* valveVolBack = new TrackingVolume("ValveVolBack", valveVolBackShape, heliumII);
   TGeoRotation valveVolBackRot("ValveVolBackRot",valveVolBackPhi, valveVolBackTheta, valveVolBackPsi);
   TGeoTranslation valveVolBackTra("ValveVolBackTra",valveVolBackXPos, valveVolBackXPos, valveVolBackXPos);
   TGeoCombiTrans valveVolBackCom(valveVolBackTra,valveVolBackRot);
   TGeoHMatrix valveVolBackMat = valveVolBackCom;
   TGeoHMatrix *valveVolBackMatrix = new TGeoHMatrix(valveVolBackMat);
   valveVolBackMatrix->SetName("ValveVolBackMatrix");
   valveVolBackMatrix->RegisterYourself();

   // -- BendEntrance - this is joined to the valve volume back  
   Box *bendEntranceShape = new Box("BendEntrance", bendEntranceHalfX, bendEntranceHalfY, bendEntranceHalfZ);
   TrackingVolume* bendEntrance = new TrackingVolume("BendEntrance", bendEntranceShape, heliumII);
   TGeoRotation bendEntranceRot("BendEntranceRot",bendEntrancePhi, bendEntranceTheta, bendEntrancePsi);
   TGeoTranslation bendEntranceTra("BendEntranceTra",bendEntranceXPos, bendEntranceYPos, bendEntranceZPos);
   TGeoCombiTrans bendEntranceCom(bendEntranceTra,bendEntranceRot);
   TGeoHMatrix bendEntranceMat = bendEntranceCom;
   TGeoHMatrix *bendEntranceMatrix = new TGeoHMatrix(bendEntranceMat);
   bendEntranceMatrix->SetName("BendEntranceMatrix");
   bendEntranceMatrix->RegisterYourself();

   // -- Create the composite Valve volume
   CompositeShape *valveVolShape = new CompositeShape("ValveVol", "(BendEntrance:BendEntranceMatrix + ValveVolBack:ValveVolBackMatrix)");
   TrackingVolume* valveVol = new TrackingVolume("ValveVol",valveVolShape,heliumII);
   valveVol->SetLineColor(kTeal-5);
   valveVol->SetLineWidth(1);
   valveVol->SetVisibility(kTRUE);
   valveVol->SetTransparency(20);
   TGeoRotation valveVolRot("ValveVolRot",valveVolPsi,valveVolTheta,valveVolPhi);
   TGeoTranslation valveVolTra("ValveVolTra",valveVolXPos,valveVolYPos,valveVolZPos);
   TGeoCombiTrans valveVolCom(valveVolTra,valveVolRot);
   TGeoHMatrix valveVolMat = valveVolCom;
   chamber->AddNode(valveVol, 1, new TGeoHMatrix(valveVolMat));
   Double_t valveVolCapacity = valveVolShape->Capacity();

/* // -- Define the valve in its open state 
   Double_t openValveRMin = 0., openValveRMax = 36.0*Units::mm, openValveHalfLength = 1.375*Units::mm;
   TGeoVolume *openValve = Builder::UCNInstance(geoManager)->MakeUCNTube("OpenValve", boundary, openValveRMin, openValveRMax, openValveHalfLength);
   Double_t openValveAngle = 90.0;
   Double_t openValveYPos = valveVolBackHalfLength - openValveHalfLength;
   TGeoRotation openValveRot("ValveVolBackRot",0,openValveAngle,0); // phi, theta, psi
   TGeoTranslation openValveTra("ValveVolBackTra",0.,openValveYPos,0.); // x, y, z
   TGeoCombiTrans openValveCom(openValveTra,openValveRot);
   TGeoHMatrix openValveMat = openValveCom;
   openValve->SetLineColor(kBlue+3);
   openValve->SetLineWidth(1);
   openValve->SetVisibility(kTRUE);
   openValve->SetTransparency(20);

   // -- Define the valve in its closed state 
   Double_t closedValveRMin = 0., closedValveRMax = 36.0*Units::mm, closedValveHalfLength = 1.375*Units::mm;
   TGeoVolume *closedValve = Builder::UCNInstance(geoManager)->MakeUCNTube("OpenValve", vacuum, closedValveRMin, closedValveRMax, closedValveHalfLength);
   Double_t closedValveAngle = 90.0;
   Double_t closedValveYPos = closedValveHalfLength - valveVolBackHalfLength;
   TGeoRotation closedValveRot("ValveVolBackRot",0,closedValveAngle,0); // phi, theta, psi
   TGeoTranslation closedValveTra("ValveVolBackTra",0.,closedValveYPos,0.); // x, y, z
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
   Tube *circleBendShape = new Tube("CircleBend", bendRMin, bendRMax, bendHalfLength);
   TrackingVolume* circleBend = new TrackingVolume("CircleBend", circleBendShape, heliumII);

   Box *bendBoxShape = new Box("BendBox", 2.0*bendHalfLength, bendRMax, bendRMax);
   Boundary* bendBox = new Boundary("BendBox", bendBoxShape, beryllium, surfaceRoughness);

   // -- Define the transformation of bendbox
   TGeoRotation bendBoxRot("BendBoxRot",bendBoxPhi,bendBoxTheta,bendBoxPsi);
   TGeoTranslation bendBoxTra("BendBoxTra",bendBoxXPos,bendBoxYPos,bendBoxZPos);
   TGeoCombiTrans bendBoxCom(bendBoxTra,bendBoxRot);
   TGeoHMatrix bendBoxMat = bendBoxCom;
   TGeoHMatrix *bendBoxMatrix = new TGeoHMatrix(bendBoxMat);
   bendBoxMatrix->SetName("BendBoxMatrix");
   bendBoxMatrix->RegisterYourself();
   // -- Create a composite bend
   CompositeShape* bendShape = new CompositeShape("BendShape","(CircleBend * BendBox:BendBoxMatrix)");
   TrackingVolume* bendVol = new TrackingVolume("BendVol",bendShape,heliumII);
   bendVol->SetLineColor(kYellow-3);
   bendVol->SetLineWidth(1);
   bendVol->SetVisibility(kTRUE);
   bendVol->SetTransparency(20);
   // -- Define the transformation of the bend
   TGeoRotation bendVolRot("BendVolRot",bendVolPhi,bendVolTheta,bendVolPsi);
   TGeoTranslation bendVolTra("BendVolTra", bendVolXPos, bendVolYPos, bendVolZPos);
   TGeoCombiTrans bendVolCom(bendVolTra,bendVolRot);
   TGeoHMatrix bendVolMat = bendVolCom;
   chamber->AddNode(bendVol, 1, new TGeoHMatrix(bendVolMat));
   Double_t bendCapacity = bendShape->Capacity();

   // -------------------------------------
   // -- DETECTOR VALVE
   // DetectorValveVol
   Box *detectorValveVolShape = new Box("DetectorValveVol", detectorValveVolHalfX, detectorValveVolHalfY, detectorValveVolHalfZ);
   TrackingVolume* detectorValveVol = new TrackingVolume("DetectorValveVol", detectorValveVolShape, heliumII);
   detectorValveVol->SetLineColor(kRed+3);
   detectorValveVol->SetLineWidth(1);
   detectorValveVol->SetVisibility(kTRUE);
   detectorValveVol->SetTransparency(0);
   // -- Define the Valve volume back
   TGeoRotation detectorValveVolRot("DetectorValveVolRot",detectorValveVolPhi, detectorValveVolTheta, detectorValveVolPsi);
   TGeoTranslation detectorValveVolTra("DetectorValveVolTra",detectorValveVolXPos, detectorValveVolYPos, detectorValveVolZPos);
   TGeoCombiTrans detectorValveVolCom(detectorValveVolTra,detectorValveVolRot);
   TGeoHMatrix detectorValveVolMat = detectorValveVolCom;
   chamber->AddNode(detectorValveVol, 1, new TGeoHMatrix(detectorValveVolMat));
   Double_t detectorValveVolCapacity = detectorValveVolShape->Capacity();

/*   // -------------------------------------
   // -- DetectorValve
   Box *detectorValveShape = new Box("DetectorValveShape", detectorValveHalfX, detectorValveHalfY, detectorValveHalfZ);
   Boundary* detectorValve = new Boundary("DetectorValve", detectorValveShape, beryllium, surfaceRoughness);
   detectorValve->SetLineColor(kBlue+3);
   detectorValve->SetLineWidth(1);
   detectorValve->SetVisibility(kTRUE);
   detectorValve->SetTransparency(0);
   // -- Define the Valve volume back
   TGeoRotation detectorValveRot("DetectorValveRot",detectorValvePhi,detectorValveTheta,detectorValvePsi);
   TGeoTranslation detectorValveTra("DetectorValveXPosTra",detectorValveXPos, detectorValveYPos, detectorValveZPos);
   TGeoCombiTrans detectorValveCom(detectorValveTra,detectorValveRot);
   TGeoHMatrix detectorValveMat = detectorValveCom;
   detectorValveVol->AddNode(detectorValve, 1, new TGeoHMatrix(detectorValveMat));
*/
   // -------------------------------------
   // -- DETECTOR TUBE
   // DetectorTubeTop - Entrance into the detector tube
   Tube *detectorTubeTopShape = new Tube("DetectorTubeTop", detectorTubeTopRMin, detectorTubeTopRMax, detectorTubeTopHalfLength);
   TrackingVolume* detectorTubeTop = new TrackingVolume("DetectorTubeTop", detectorTubeTopShape, heliumII);
   detectorTubeTop->SetLineColor(kOrange+1);
   detectorTubeTop->SetLineWidth(1);
   detectorTubeTop->SetVisibility(kTRUE);
   detectorTubeTop->SetTransparency(20);
   // -- Define the Valve volume back
   TGeoRotation detectorTubeTopRot("DetectorTubeTopRot",detectorTubeTopPhi, detectorTubeTopTheta, detectorTubeTopPsi);
   TGeoTranslation detectorTubeTopTra("DetectorTubeTopTra", detectorTubeTopXPos, detectorTubeTopYPos, detectorTubeTopZPos);
   TGeoCombiTrans detectorTubeTopCom(detectorTubeTopTra,detectorTubeTopRot);
   TGeoHMatrix detectorTubeTopMat = detectorTubeTopCom; 
   chamber->AddNode(detectorTubeTop, 1, new TGeoHMatrix(detectorTubeTopMat));
   Double_t detectorTubeTopCapacity = detectorTubeTopShape->Capacity();

   // -------------------------------------
   // -- DetectorTube
   Tube *detectorTubeShape = new Tube("DetectorTube", detectorTubeRMin, detectorTubeRMax, detectorTubeHalfLength);
   TrackingVolume* detectorTube = new TrackingVolume("DetectorTube", detectorTubeShape, heliumII);
   detectorTube->SetLineColor(kOrange+1);
   detectorTube->SetLineWidth(1);
   detectorTube->SetVisibility(kTRUE);
   detectorTube->SetTransparency(20);
   // -- Define the Valve volume back
   TGeoRotation detectorTubeRot("DetectorTubeRot",detectorTubePhi,detectorTubeTheta, detectorTubePsi);
   TGeoTranslation detectorTubeTra("DetectorTubeTra", detectorTubeXPos, detectorTubeYPos, detectorTubeZPos);
   TGeoCombiTrans detectorTubeCom(detectorTubeTra,detectorTubeRot);
   TGeoHMatrix detectorTubeMat = detectorTubeCom;
   chamber->AddNode(detectorTube, 1, new TGeoHMatrix(detectorTubeMat));
   Double_t detectorTubeCapacity = detectorTubeShape->Capacity();

   // -------------------------------------
   // -- Detector
   Double_t detectorEfficiency = 1.0;
   Tube *detectorShape = new Tube("Detector", detectorRMin, detectorRMax, detectorHalfLength);
   Detector* detector = new Detector("Detector",detectorShape,lithium6,detectorEfficiency);
   detector->SetLineColor(kGray+3);
   detector->SetLineWidth(1);
   detector->SetVisibility(kTRUE);
   detector->SetTransparency(0);
   // -- Define the Valve volume back
   TGeoRotation detectorRot("DetectorRot",detectorTubePhi,detectorTubeTheta,detectorTubePsi);
   TGeoTranslation detectorTra("DetectorTra", detectorXPos, detectorYPos, detectorZPos);
   TGeoCombiTrans detectorCom(detectorTra,detectorRot);
   TGeoHMatrix detectorMat = detectorCom;
   chamber->AddNode(detector, 1, new TGeoHMatrix(detectorMat));
   
   // -------------------------------------
   // -- GUIDE SECTION
   // Guide section has *probably* 4 segments. 
   Box *guideSegShape = new Box("GuideSeg", guideSegHalfX, guideSegHalfY, guideSegHalfZ);
   TrackingVolume* guideSeg = new TrackingVolume("GuideSeg", guideSegShape, heliumII);
   guideSeg->SetLineColor(kCyan-8);
   guideSeg->SetLineWidth(1);
   guideSeg->SetVisibility(kTRUE);
   guideSeg->SetTransparency(20);
   Double_t guideSegXPos = guideXPos;
   Double_t guideCapacity = 0.0;
   for (Int_t segNum = 1; segNum <= 5; segNum++) {
      // Define Guide Seg matrix
      TGeoRotation segmentRot("SegmentRot",guidePhi,guideTheta,guidePsi); // phi, theta, psi
      TGeoTranslation segmentTra("SegmentTra",guideSegXPos, guideYPos, guideZPos);
      TGeoCombiTrans segmentCom(segmentTra,segmentRot);
      TGeoHMatrix segmentMat = segmentCom;
      Char_t sourceMatrixName[20];
      sprintf(sourceMatrixName, "GuideSegMatrix%d", segNum);
      segmentMat.SetName(sourceMatrixName);
      chamber->AddNode(guideSeg, segNum, new TGeoHMatrix(segmentMat));
      // Shift next segment along by length of segment
      guideSegXPos = guideSegXPos - 2.0*guideSegHalfZ;
      // Calculate guide's volume
      guideCapacity += guideSegShape->Capacity();
   }
   
   Double_t totalVolume = sourceCapacity + valveVolEntraceCapacity + valveVolFrontCapacity + valveVolCapacity + bendCapacity + detectorValveVolCapacity + detectorTubeTopCapacity + guideCapacity;
   
   
   // -------------------------------------
   // -- RAMSEY CELL PRE-VOLUME SECTION
   // -- Define General Box shape dimensions
   Box *preVolumeBoxShape = new Box("PreVolumeBoxShape", preVolumeBoxHalfX, preVolumeBoxHalfY, preVolumeBoxHalfZ);
   TrackingVolume* preVolumeBox = new TrackingVolume("PreVolumeBox", preVolumeBoxShape, heliumII);
   preVolumeBox->SetLineColor(kMagenta-8);
   preVolumeBox->SetLineWidth(1);
   preVolumeBox->SetVisibility(kTRUE);
   preVolumeBox->SetTransparency(20);
   TGeoRotation preVolumeBoxRot("PreVolumeBoxRot", preVolumePhi, preVolumeTheta, 45);
   TGeoTranslation preVolumeBoxTra("PreVolumeBoxTra", preVolumeXPos, preVolumeYPos, preVolumeZPos);
   TGeoCombiTrans preVolumeBoxCom(preVolumeBoxTra,preVolumeBoxRot);
   TGeoHMatrix preVolumeBoxMat = preVolumeBoxCom;
   chamber->AddNode(preVolumeBox, 1, new TGeoHMatrix(preVolumeBoxMat));
   Double_t preVolumeBoxCapacity = preVolumeBoxShape->Capacity();
   
   // -------------------------------------
   // -- RAMSEY CELL
   // Define Pre-volume-end electrode 
   Tube *neutralElectrodeShape = new Tube("NeutralElectrodeShape", neutralElectrodeRMin, neutralElectrodeRMax, neutralElectrodeHalfZ);
   Boundary* neutralElectrode = new Boundary("NeutralElectrode", neutralElectrodeShape, beryllium, surfaceRoughness);
   neutralElectrode->SetLineColor(kRed-7);
   neutralElectrode->SetLineWidth(1);
   neutralElectrode->SetVisibility(kTRUE);
   neutralElectrode->SetTransparency(20);
   TGeoRotation neutralElectrodeRot("NeutralElectrodeRot", neutralElectrodePhi, neutralElectrodeTheta, neutralElectrodePsi);
   TGeoTranslation neutralElectrodeTra("NeutralElectrodeTra", neutralElectrodeXPos, neutralElectrodeYPos, neutralElectrodeZPos);
   TGeoCombiTrans neutralElectrodeCom(neutralElectrodeTra,neutralElectrodeRot);
   TGeoHMatrix neutralElectrodeMat = neutralElectrodeCom;
//   chamber->AddNode(neutralElectrode, 1, new TGeoHMatrix(neutralElectrodeMat));

   // Define valve holes in neutral electrode 
   Tube *neutralElectrodeHoleShape = new Tube("NeutralElectrodeHoleShape", neutralElectrodeHoleRMin, neutralElectrodeHoleRMax, neutralElectrodeHoleHalfZ);
   // 1
   TrackingVolume* neutralElectrodeHole1 = new TrackingVolume("NeutralElectrodeHole1", neutralElectrodeHoleShape, heliumII);
   neutralElectrodeHole1->SetLineColor(kMagenta-8);
   neutralElectrodeHole1->SetLineWidth(1);
   neutralElectrodeHole1->SetVisibility(kTRUE);
   neutralElectrodeHole1->SetTransparency(20);
   TGeoRotation neutralElectrodeHoleRot("NeutralElectrodeHoleRot", neutralElectrodeHolePhi, neutralElectrodeHoleTheta, neutralElectrodeHolePsi);
   TGeoTranslation neutralElectrodeHole1Tra("NeutralElectrodeHole1Tra", neutralElectrodeHole1XPos, neutralElectrodeHole1YPos, neutralElectrodeHole1ZPos);
   TGeoCombiTrans neutralElectrodeHole1Com(neutralElectrodeHole1Tra,neutralElectrodeHoleRot);
   TGeoHMatrix neutralElectrodeHole1Mat = neutralElectrodeHole1Com;
   chamber->AddNode(neutralElectrodeHole1, 1, new TGeoHMatrix(neutralElectrodeHole1Mat));
   Double_t neutralElectrodeHole1Capacity = neutralElectrodeHoleShape->Capacity();

   // 2
   Boundary* neutralElectrodeHole2 = new Boundary("NeutralElectrodeHole2", neutralElectrodeHoleShape, beryllium, surfaceRoughness);
   neutralElectrodeHole2->SetLineColor(kGray+3);
   neutralElectrodeHole2->SetLineWidth(1);
   neutralElectrodeHole2->SetVisibility(kTRUE);
   neutralElectrodeHole2->SetTransparency(20);
   TGeoTranslation neutralElectrodeHole2Tra("NeutralElectrodeHole2Tra", neutralElectrodeHole2XPos, neutralElectrodeHole2YPos, neutralElectrodeHole2ZPos);
   TGeoCombiTrans neutralElectrodeHole2Com(neutralElectrodeHole2Tra,neutralElectrodeHoleRot);
   TGeoHMatrix neutralElectrodeHole2Mat = neutralElectrodeHole2Com;
//   chamber->AddNode(neutralElectrodeHole2, 1, new TGeoHMatrix(neutralElectrodeHole2Mat));
   // 3
   TrackingVolume* neutralElectrodeHole3 = new TrackingVolume("NeutralElectrodeHole3", neutralElectrodeHoleShape, heliumII);
   neutralElectrodeHole3->SetLineColor(kMagenta-8);
   neutralElectrodeHole3->SetLineWidth(1);
   neutralElectrodeHole3->SetVisibility(kTRUE);
   neutralElectrodeHole3->SetTransparency(20);
   TGeoTranslation neutralElectrodeHole3Tra("NeutralElectrodeHole3Tra", neutralElectrodeHole3XPos, neutralElectrodeHole3YPos, neutralElectrodeHole3ZPos);
   TGeoCombiTrans neutralElectrodeHole3Com(neutralElectrodeHole3Tra,neutralElectrodeHoleRot);
   TGeoHMatrix neutralElectrodeHole3Mat = neutralElectrodeHole3Com;
   chamber->AddNode(neutralElectrodeHole3, 1, new TGeoHMatrix(neutralElectrodeHole3Mat));
   Double_t neutralElectrodeHole3Capacity = neutralElectrodeHoleShape->Capacity();

   // 4
   Boundary* neutralElectrodeHole4 = new Boundary("NeutralElectrodeHole4", neutralElectrodeHoleShape, beryllium, surfaceRoughness);
   neutralElectrodeHole4->SetLineColor(kGray+3);
   neutralElectrodeHole4->SetLineWidth(1);
   neutralElectrodeHole4->SetVisibility(kTRUE);
   neutralElectrodeHole4->SetTransparency(20);
   TGeoTranslation neutralElectrodeHole4Tra("NeutralElectrodeHole4Tra", neutralElectrodeHole4XPos, neutralElectrodeHole4YPos, neutralElectrodeHole4ZPos);
   TGeoCombiTrans neutralElectrodeHole4Com(neutralElectrodeHole4Tra,neutralElectrodeHoleRot);
   TGeoHMatrix neutralElectrodeHole4Mat = neutralElectrodeHole4Com;
//   chamber->AddNode(neutralElectrodeHole4, 1, new TGeoHMatrix(neutralElectrodeHole4Mat));

   // Neutral Cell
   Tube *neutralCellShape = new Tube("NeutralCellShape", neutralCellRMin, neutralCellRMax, neutralCellHalfZ);
   TrackingVolume* neutralCell = new TrackingVolume("NeutralCell", neutralCellShape, heliumII);
   neutralCell->SetLineColor(kYellow-8);
   neutralCell->SetLineWidth(1);
   neutralCell->SetVisibility(kTRUE);
   neutralCell->SetTransparency(20);
   TGeoRotation neutralCellRot("NeutralCellRot", neutralCellPhi, neutralCellTheta, neutralCellPsi);
   TGeoTranslation neutralCellTra("NeutralCellTra", neutralCellXPos, neutralCellYPos, neutralCellZPos);
   TGeoCombiTrans neutralCellCom(neutralCellTra,neutralCellRot);
   TGeoHMatrix neutralCellMat = neutralCellCom;
   chamber->AddNode(neutralCell, 1, new TGeoHMatrix(neutralCellMat));
   Double_t neutralCellCapacity = neutralCellShape->Capacity();

   // Cell Connector Tube
   Tube *cellConnectorShape = new Tube("CellConnectorShape", cellConnectorRMin, cellConnectorRMax, cellConnectorHalfZ);
   TrackingVolume* cellConnector = new TrackingVolume("CellConnector", cellConnectorShape, heliumII);
   cellConnector->SetLineColor(kMagenta-8);
   cellConnector->SetLineWidth(1);
   cellConnector->SetVisibility(kTRUE);
   cellConnector->SetTransparency(20);
   TGeoRotation cellConnectorRot("CellConnectorRot", cellConnectorPhi, cellConnectorTheta, cellConnectorPsi);
   TGeoTranslation cellConnectorTra("CellConnectorTra", cellConnectorXPos, cellConnectorYPos, cellConnectorZPos);
   TGeoCombiTrans cellConnectorCom(cellConnectorTra,cellConnectorRot);
   TGeoHMatrix cellConnectorMat = cellConnectorCom;
   neutralCell->AddNode(cellConnector, 1, new TGeoHMatrix(cellConnectorMat));

   // Cell Connector Tube Boundary
   Tube *cellConnectorBoundaryShape = new Tube("CellConnectorBoundaryShape", cellConnectorBoundaryRMin, cellConnectorBoundaryRMax, cellConnectorBoundaryHalfZ);
   Boundary* cellConnectorBoundary = new Boundary("CellConnectorBoundary", cellConnectorBoundaryShape, beryllium, surfaceRoughness);
   cellConnectorBoundary->SetLineColor(kGreen-1);
   cellConnectorBoundary->SetLineWidth(1);
   cellConnectorBoundary->SetVisibility(kFALSE);
   cellConnectorBoundary->SetTransparency(20);
   TGeoRotation cellConnectorBoundaryRot("CellConnectorBoundaryRot", cellConnectorBoundaryPhi, cellConnectorBoundaryTheta, cellConnectorBoundaryPsi);
   TGeoTranslation cellConnectorBoundaryTra("CellConnectorBoundaryTra", cellConnectorBoundaryXPos, cellConnectorBoundaryYPos, cellConnectorBoundaryZPos);
   TGeoCombiTrans cellConnectorBoundaryCom(cellConnectorBoundaryTra,cellConnectorBoundaryRot);
   TGeoHMatrix cellConnectorBoundaryMat = cellConnectorBoundaryCom;
   neutralCell->AddNode(cellConnectorBoundary, 1, new TGeoHMatrix(cellConnectorBoundaryMat));

   // Define Central electrode 
   Tube *centralElectrodeShape = new Tube("CentralElectrodeShape", centralElectrodeRMin, centralElectrodeRMax, centralElectrodeHalfZ);
   Boundary* centralElectrode = new Boundary("CentralElectrode", centralElectrodeShape, beryllium, surfaceRoughness);
   centralElectrode->SetLineColor(kRed-7);
   centralElectrode->SetLineWidth(1);
   centralElectrode->SetVisibility(kTRUE);
   centralElectrode->SetTransparency(20);
   TGeoRotation centralElectrodeRot("CentralElectrodeRot", centralElectrodePhi, centralElectrodeTheta, centralElectrodePsi);
   TGeoTranslation centralElectrodeTra("CentralElectrodeTra", centralElectrodeXPos, centralElectrodeYPos, centralElectrodeZPos);
   TGeoCombiTrans centralElectrodeCom(centralElectrodeTra,centralElectrodeRot);
   TGeoHMatrix centralElectrodeMat = centralElectrodeCom;
//   chamber->AddNode(centralElectrode, 1, new TGeoHMatrix(centralElectrodeMat));

   // Define hole through the Central electrode 
   Tube *centralElectrodeHoleShape = new Tube("CentralElectrodeHoleShape", centralElectrodeHoleRMin, centralElectrodeHoleRMax, centralElectrodeHoleHalfZ);
   TrackingVolume* centralElectrodeHole = new TrackingVolume("CentralElectrodeHole", centralElectrodeHoleShape, heliumII);
   centralElectrodeHole->SetLineColor(kMagenta-8);
   centralElectrodeHole->SetLineWidth(1);
   centralElectrodeHole->SetVisibility(kTRUE);
   centralElectrodeHole->SetTransparency(20);
   TGeoRotation centralElectrodeHoleRot("CentralElectrodeHoleRot", centralElectrodeHolePhi, centralElectrodeHoleTheta, centralElectrodeHolePsi);
   TGeoTranslation centralElectrodeHoleTra("CentralElectrodeHoleTra", centralElectrodeHoleXPos, centralElectrodeHoleYPos, centralElectrodeHoleZPos);
   TGeoCombiTrans centralElectrodeHoleCom(centralElectrodeHoleTra,centralElectrodeHoleRot);
   TGeoHMatrix centralElectrodeHoleMat = centralElectrodeHoleCom;
   chamber->AddNode(centralElectrodeHole, 1, new TGeoHMatrix(centralElectrodeHoleMat));
   Double_t centralElectrodeHoleCapacity = centralElectrodeHoleShape->Capacity(); 

   // HV Cell
   Tube *hvCellShape = new Tube("HVCellShape", hvCellRMin, hvCellRMax, hvCellHalfZ);
   TrackingVolume* hvCell = new TrackingVolume("HVCell", hvCellShape, heliumII);
   hvCell->SetLineColor(kYellow-8);
   hvCell->SetLineWidth(1);
   hvCell->SetVisibility(kTRUE);
   hvCell->SetTransparency(20);
   TGeoRotation hvCellRot("HVCellRot", hvCellPhi, hvCellTheta, hvCellPsi);
   TGeoTranslation hvCellTra("HVCellTra", hvCellXPos, hvCellYPos, hvCellZPos);
   TGeoCombiTrans hvCellCom(hvCellTra,hvCellRot);
   TGeoHMatrix hvCellMat = hvCellCom;
   chamber->AddNode(hvCell, 1, new TGeoHMatrix(hvCellMat));
   Double_t hvCellCapacity = hvCellShape->Capacity();

   // Define HV electrode 
   Tube *hvElectrodeShape = new Tube("HVElectrodeShape", hvElectrodeRMin, hvElectrodeRMax, hvElectrodeHalfZ);
   Boundary* hvElectrode = new Boundary("HVElectrode", hvElectrodeShape, beryllium, surfaceRoughness);
   hvElectrode->SetLineColor(kRed-8);
   hvElectrode->SetLineWidth(1);
   hvElectrode->SetVisibility(kTRUE);
   hvElectrode->SetTransparency(20);
   TGeoRotation hvElectrodeRot("HVElectrodeRot", hvElectrodePhi, hvElectrodeTheta, hvElectrodePsi);
   TGeoTranslation hvElectrodeTra("HVElectrodeTra", hvElectrodeXPos, hvElectrodeYPos, hvElectrodeZPos);
   TGeoCombiTrans hvElectrodeCom(hvElectrodeTra,hvElectrodeRot);
   TGeoHMatrix hvElectrodeMat = hvElectrodeCom;
//   chamber->AddNode(hvElectrode, 1, new TGeoHMatrix(hvElectrodeMat));
   
   // -------------------------------------
   // -- Close Geometry
   geoManager->CloseGeometry();
   
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "$(UCN_GEOM)/cryoedm_geom.root";
   cout << "Simulation Geometry Built... Writing to file: " << fileName << endl;
   geoManager->Export(fileName);
   
   // -------------------------------------
   // -- VISUALISATION GEOM
   // -- Here we swap out CompositeShape for TGeoCompositeShape due to explicit
   // -- TGeoShape downcasting problems in TGeoPainter. The geometry that is left becomes
   // --  the visualisation geometry used for drawing
   // First replace the composite shapes in the geometry
   TGeoCompositeShape *valveVolShapeVis = new TGeoCompositeShape("ValveVolVis","(BendEntrance:BendEntranceMatrix + ValveVolBack:ValveVolBackMatrix)");
   valveVol->SetShape(valveVolShapeVis);
   
   TGeoCompositeShape* bendShapeVis = new TGeoCompositeShape("BendShapeVis","(CircleBend * BendBox:BendBoxMatrix)");
   bendVol->SetShape(bendShapeVis);
   
   // -------------------------------------
   // -- Write out visualisation geometry to file
   const char *visFileName = "$(UCN_GEOM)/cryoedm_vis.root";
   cout << "Visualisation Geometry Built... Writing to file: " << visFileName << endl;
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
   Double_t cameraCentre[3] = {preVolumeXPos, preVolumeYPos, preVolumeZPos};
   glViewer->SetPerspectiveCamera(camera,4,100,&cameraCentre[0],0,0);
   // -- Draw Reference Point, Axes
   Double_t refPoint[3] = {0.,0.,0.};
   // Int_t axesType = 0(Off), 1(EDGE), 2(ORIGIN), Bool_t axesDepthTest, Bool_t referenceOn, const Double_t referencePos[3]
   glViewer->SetGuideState(0, kFALSE, kFALSE, refPoint);
   glViewer->UpdateScene();
   return kTRUE;
}
