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
Bool_t BuildFieldMap(const TGeoHMatrix& matrix);

using namespace GeomParameters;

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
   TGeoRotation detectorValveVolRot("DetectorValveVolXPosRot",detectorValveVolPhi,detectorValveVolTheta,detectorValveVolPsi); // phi, theta, psi
   TGeoTranslation detectorValveVolTra("DetectorValveVolXPosTra", detectorValveVolXPos, detectorValveVolYPos, detectorValveVolZPos);
   TGeoCombiTrans detectorValveVolCom(detectorValveVolTra,detectorValveVolRot);
   TGeoHMatrix detectorValveVolMat = detectorValveVolCom;
   chamber->AddNode(detectorValveVol, 1, new TGeoHMatrix(detectorValveVolMat));
   Double_t detectorValveVolCapacity = detectorValveVolShape->Capacity();
   
   
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
   for (Int_t segNum = 1; segNum <= 3; segNum++) {
      // Define Guide Seg matrix
      TGeoRotation segmentRot("SegmentRot",guidePhi,guideTheta,0); // phi, theta, psi
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
   
   // -------------------------------------
   // -- Close Geometry
   geoManager->CloseGeometry();
   
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "$(UCN_GEOM)/guide_section_geom.root";
   cout << "Simulation Geometry Built... Writing to file: " << fileName << endl;
   geoManager->Export(fileName);
   
   double* origin = detectorValveVolMat->GetTranslation();
   double start_boundary[3] = {detectorValveVolShape->GetDX(),detectorValveVolShape->GetDY(),detectorValveVolShape->GetDZ()};
   cout << "Valve Vol Origin: " << origin[0] << "\t" << origin[1] << "\t" << origin[2] << endl;
   cout << "Valve Vol Boundary +/-: " << start_boundary[0] << "\t" << start_boundary[1] << "\t" << start_boundary[2] << endl;
   
   TGeoHMatrix* end_matrix = dynamic_cast<TGeoHMatrix*>(geoManager->GetListOfMatrices()->FindObject("GuideSegMatrix3"));
   double* end_origin = end_matrix->GetTranslation();
   double end_boundary[3] = {guideSegShape->GetDX(),guideSegShape->GetDY(),guideSegShape->GetDZ()};
   cout << "Pre-Volume Origin: " << end_origin[0] << "\t" << end_origin[1] << "\t" << end_origin[2] << endl;
   cout << "Pre-Volume Boundary +/-: " << end_boundary[0] << "\t" << end_boundary[1] << "\t" << end_boundary[2] << endl;
   
   BuildFieldMap(detectorValveVolMat);
   
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
   Double_t cameraCentre[3] = {detectorValveVolXPos, detectorValveVolYPos, detectorValveVolZPos};
   glViewer->SetPerspectiveCamera(camera,4,100,&cameraCentre[0],0,0);
   // -- Draw Reference Point, Axes
   Double_t refPoint[3] = {0.,0.,0.};
   // Int_t axesType = 0(Off), 1(EDGE), 2(ORIGIN), Bool_t axesDepthTest, Bool_t referenceOn, const Double_t referencePos[3]
   glViewer->SetGuideState(0, kFALSE, kFALSE, refPoint);
   glViewer->UpdateScene();
   return kTRUE;
}

//__________________________________________________________________________
Bool_t BuildFieldMap(const TGeoHMatrix& matrix)
{
   // Create a Uniform Magnetic field and write it to file
   string filename = "runs/polarisation/guide_tube/C8=9 A +SQUIDs Coil =26_4 mA +D1=2 A + 6WS TC guide tube -100 z +80 cm.txt";
   // Define shape of field
   TGeoShape* magFieldShape = new Box("FieldShape",0.031, 0.031, 3.0);
   // Define transformation that locates field in geometry
   TGeoMatrix* magFieldPosition = new TGeoHMatrix(matrix);
   magFieldPosition->Print();
   MagFieldMap* field = new MagFieldMap("Field", magFieldShape, magFieldPosition);
   if (field->BuildMap(filename) == kFALSE) {
      Error("BuildFieldMap","Cannot open file: %s", filename);
      return kFALSE;
   }
   // Add field to magfield manager
   MagFieldArray* magFieldArray = new MagFieldArray();
   magFieldArray->AddField(field);
   
/*   // Elec Field
   // Define shape of field
   TGeoShape* elecFieldShape = new Tube("SolenoidFieldShape",hvCellRMin, hvCellRMax, hvCellHalfZ);
   // Define transformation that locates field in geometry
   TGeoMatrix* elecFieldPosition = new TGeoHMatrix(matrix);
   TVector3 elecFieldStrength(hvCellEx, hvCellEy, hvCellEz);
   ElecField* elecField = new UniformElecField("ElectricField", elecFieldStrength, elecFieldShape, elecFieldPosition);
   // Add field to electric field manager
   ElecFieldArray* elecFieldArray = new ElecFieldArray();
   elecFieldArray->AddField(elecField);
*/   
   // -- Write magfieldmanager to geometry file
   const char *magFileName = "geom/fields.root";
   TFile *f = TFile::Open(magFileName,"recreate");
   if (!f || f->IsZombie()) {
     Error("BuildFieldMap","Cannot open file: %s", magFileName);
     return kFALSE;
   }
   magFieldArray->Write(magFieldArray->GetName());
//   elecFieldArray->Write(elecFieldArray->GetName());
   f->ls();
   f->Close();
   delete magFieldArray;
//   delete elecFieldArray;
   magFieldArray = 0;
//   elecFieldArray = 0;
   return kTRUE;
}