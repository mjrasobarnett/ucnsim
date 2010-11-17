// This is a model of the 'Real' cryoEDM sourcetube - 25/01/2010
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
using namespace FieldParameters;

//__________________________________________________________________________
Int_t ramseycell_geom()
{
   // Create the geoManager and list of standard materials
   TGeoManager* geoManager = new TGeoManager("GeoManager","Geometry Manager");
   Materials::BuildMaterials(geoManager);
   // Build and write to file the simulation and visualisation geoms
   Build_Geom(geoManager);
   // Draw geometry
   Draw_Geom(geoManager);
   
   return 0;
}

//__________________________________________________________________________
Bool_t Build_Geom(const TGeoManager* geoManager) 
{
   // -------------------------------------
   // *** BUILDING GEOMETRY
   // -------------------------------------
   cout << "--------------------------------" << endl;
   cout << "Building Geometry" << endl;
   cout << "--------------------------------" << endl;
   // Materials - Define the materials used. 
   // Leave the neutron properties to be defined on a run-by-run basis
   TGeoMedium* beryllium = geoManager->GetMedium("Beryllium");
   TGeoMedium* blackhole = geoManager->GetMedium("BlackHole");
   TGeoMedium* heliumII = geoManager->GetMedium("HeliumII");
   
   // -------------------------------------
   // -- Making Top Volume
   TUCNGeoBBox* topShape = new TUCNGeoBBox("Top",topX,topY,topZ);
   TUCNBlackHole* top = new TUCNBlackHole("Top", topShape, blackhole);
   geoManager->SetTopVolume(top);
   top->SetVisibility(kFALSE);
   
   // -- Make the boundary volume in which all the others sit
   // -- This is what we will be reflecting off all the time
   Double_t surfaceRoughness = 0.05;
   TUCNGeoBBox* chamberShape = new TUCNGeoBBox("Chamber",chamberX,chamberY,chamberZ);
   TUCNBoundary* chamber = new TUCNBoundary("Chamber", chamberShape, beryllium, surfaceRoughness);
   chamber->SetLineColor(kOrange-7);
   chamber->SetLineWidth(1);
   chamber->SetVisibility(kFALSE);
   chamber->SetTransparency(80);
   // Add to geom
   top->AddNode(chamber,1);
   
   // HV Cell
   TUCNGeoTube *hvCellShape = new TUCNGeoTube("HVShape", hvCellRMin, hvCellRMax, hvCellHalfZ);
   TUCNTrackingVolume* hvCell = new TUCNTrackingVolume("HVCell", hvCellShape, heliumII);
   hvCell->SetLineColor(kYellow-8);
   hvCell->SetLineWidth(1);
   hvCell->SetVisibility(kTRUE);
   hvCell->SetTransparency(20);
   TGeoRotation hvCellRot("HVCellRot", hvCellPhi, hvCellTheta, hvCellPsi);
   TGeoTranslation hvCellTra("HVCellTra", 0., 0., 0.);
   TGeoCombiTrans hvCellCom(hvCellTra,hvCellRot);
   TGeoHMatrix hvCellMat = hvCellCom;   
   hvCellMat.SetName("HVCellMat");
   chamber->AddNode(hvCell, 1, new TGeoHMatrix(hvCellMat));
   
   // -------------------------------------
   // -- Close Geometry
   geoManager->CloseGeometry();
   
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "$(UCN_GEOM)/ramseycell_geom.root";
   cerr << "Simulation Geometry Built... Writing to file: " << fileName << endl;
   geoManager->Export(fileName);
   
   // -------------------------------------
   // *** BUILDING MAGNETIC FIELDS
   // -------------------------------------
   cout << "--------------------------------" << endl;
   cout << "Initialising Magnetic fields" << endl;
   cout << "--------------------------------" << endl;
   TUCNMagFieldManager* magFieldManager = new TUCNMagFieldManager();
   
   // -- Define solenoid field - uniform magnetic field
   // Define shape of field
   TGeoShape* fieldShape = new TUCNGeoTube("SolenoidFieldShape",hvCellRMin, hvCellRMax, hvCellHalfZ);
   // Define transformation that locates field in geometry
   TGeoMatrix* fieldPosition = new TGeoHMatrix(hvCellMat);
   // Define field vector
   TVector3 fieldStrength(solenoidBx, solenoidBy, solenoidBz);
   // Define field object
   TUCNMagField* uniformField = new TUCNUniformMagField("SolenoidField", fieldStrength, fieldShape, fieldPosition);
   
   // Add field to magfield manager
   magFieldManager->AddField(uniformField);
   
   // -- Write magfieldmanager to geometry file
   const char *magFileName = "$(UCN_DIR)/geom/ramseycell_fields.root";
   TFile *f = TFile::Open(magFileName,"recreate");
   if (!f || f->IsZombie()) {
     Error("Export","Cannot open file: %s", magFileName);
     return kFALSE;
   }
   magFieldManager->Write(magFieldManager->GetName());
   f->ls();
   f->Close();
   if (magFieldManager) delete magFieldManager;
   magFieldManager = 0;
   cout << "--------------------------------" << endl;   
   // -------------------------------------
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

