#include "Units.hpp"
#include "Constants.hpp"
#include "GeomParameters.hpp"

Bool_t Build_Geom(const TGeoManager* geoManager);
Bool_t BuildFields(const TGeoManager* geoManager);

using namespace GeomParameters;

//__________________________________________________________________________
Int_t detector_tube()
{
   // Load project's shared library (see $ROOTALIAS for function definition)
   load_library("$UCN_DIR/lib/libUCN.so");
   // Create the geoManager
   TGeoManager* geoManager = new TGeoManager("GeoManager","Geometry Manager");
   // Build and write to file the simulation and visualisation geoms
   Build_Geom(geoManager);
   // Draw Geom
   TCanvas* geomCanvas = new TCanvas("GeomCanvas","Canvas for visualisation of EDM Geom",60,40,200,200);
   geomCanvas->cd();
   double camera[3] = {0., 0., 0.};
   Analysis::Geometry::DrawGeometry(*geomCanvas, *geoManager, camera);
   // Build Fields
   BuildFields(geoManager);
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
   TGeoMedium* quartz = geoManager->GetMedium("Quartz");
   TGeoMedium* ptfe = geoManager->GetMedium("PTFE");
   
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
   detectorTubeTop->SetLineColor(kOrange+3);
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
   cout << detectorTubeTopXPos << " " <<  detectorTubeTopYPos << " " <<  detectorTubeTopZPos << endl;
   // -------------------------------------
   // -- DetectorTube
   Tube *detectorTubeShape = new Tube("DetectorTube", detectorTubeRMin, detectorTubeRMax, detectorTubeHalfLength);
   TrackingVolume* detectorTube = new TrackingVolume("DetectorTube", detectorTubeShape, heliumII);
   detectorTube->SetLineColor(kOrange-4);
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
   // -- DetectorTubeBottom
   Tube *detectorTubeBottomShape = new Tube("DetectorTubeBottom", detectorTubeBottomRMin, detectorTubeBottomRMax, detectorTubeBottomHalfLength);
   Boundary* detectorTubeBottom = new Boundary("DetectorTubeBottom", detectorTubeBottomShape, ptfe, surfaceRoughness);
   detectorTubeBottom->SetLineColor(kOrange+1);
   detectorTubeBottom->SetLineWidth(1);
   detectorTubeBottom->SetVisibility(kTRUE);
   detectorTubeBottom->SetTransparency(20);
   // -- Define the Valve volume back
   TGeoRotation detectorTubeBottomRot("DetectorTubeBottomRot",detectorTubeBottomPhi,detectorTubeBottomTheta, detectorTubeBottomPsi);
   TGeoTranslation detectorTubeBottomTra("DetectorTubeBottomTra", detectorTubeBottomXPos, detectorTubeBottomYPos, detectorTubeBottomZPos);
   TGeoCombiTrans detectorTubeBottomCom(detectorTubeBottomTra,detectorTubeBottomRot);
   TGeoHMatrix detectorTubeBottomMat = detectorTubeBottomCom;
   chamber->AddNode(detectorTubeBottom, 1, new TGeoHMatrix(detectorTubeBottomMat));
   Double_t detectorTubeBottomArea = TMath::Pi()*TMath::Power(detectorTubeBottomRMax,2.0);
   
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
   Double_t detectorArea = TMath::Pi()*TMath::Power(detectorRMax,2.0);
   
   cout << "--------------------------------" << endl;
   cout << "Ratio of Detector Area to Tube Bottom: " << detectorArea/detectorTubeBottomArea << endl;
   cout << "Detector (m^2): " << detectorArea << "\t Tube Bottom (m^2): " << detectorTubeBottomArea << endl;
   cout << "--------------------------------" << endl;
   
   // -------------------------------------
   // -- Close Geometry
   geoManager->CloseGeometry();
 
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "detector_tube_geom.root";
   cout << "Simulation Geometry Built... Writing to file: " << fileName << endl;
   geoManager->Export(fileName);
   
   return kTRUE;
}

//__________________________________________________________________________
Bool_t BuildFields(const TGeoManager* geoManager)
{
   cout << "--------------------------------" << endl;
   cout << "Building Fields" << endl;
   cout << "--------------------------------" << endl;
   // ---------------------------------
   // -- Gravitational Field
   GravField* gravField = new GravField(-1.0,0.0,0.0);
   printf("Gravitational Field - nx: %.4f \t ny: %.4f \t nz: %.4f\n",gravField->Nx(),gravField->Ny(),gravField->Nz());
   // -- Write fields to file
   const char *fieldsFileName = "detector_tube_fields.root";
   TFile *file = Analysis::DataFile::OpenRootFile(fieldsFileName, "recreate");
   cout << "Fields Created... Writing to file: " << fieldsFileName << endl;
   gravField->Write(gravField->GetName());
   file->Close();
   // Cleanup
   delete gravField;
   return kTRUE;
}
