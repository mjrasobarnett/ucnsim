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
   // -- Ramsey Cell
   const Double_t ramseyCellRMin = 0.; 
   const Double_t ramseyCellRMax = 235.0*Units::mm;
   const Double_t ramseyCellHalfLength = 120.*Units::mm;
   const Double_t ramseyCellAngle = 90.0;

   const Double_t bFieldX = 0.;
   const Double_t bFieldY = 0.;
   const Double_t bFieldZ = 5.0*Units::uT;
   
}

using namespace ModelParameters;

//__________________________________________________________________________
Int_t ramseycell_geom()
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
   // -- Build Ramsey-Cell Volume
   TUCNGeoTube *ramseyCellShape = new TUCNGeoTube("RamseyCell", ramseyCellRMin, ramseyCellRMax, ramseyCellHalfLength);
   TUCNTrackingVolume* ramseyCell = new TUCNTrackingVolume("RamseyCell", ramseyCellShape, heliumII);
   ramseyCell->SetLineColor(kAzure-4);
   ramseyCell->SetLineWidth(1);
   ramseyCell->SetVisibility(kTRUE);
   ramseyCell->SetTransparency(20);
   
   // -- Define SourceTube matrix
   // Rotation seems to be applied before translation so bear that in mind when choosing which
   // coordinate to transform in translation
   TGeoRotation ramseyCellRot("ramseyCellRot",0,ramseyCellAngle,0); // phi, theta, psi
   TGeoTranslation ramseyCellTra("ramseyCellTra",0.,0.,0.); // x, y, z
   TGeoCombiTrans ramseyCellCom(ramseyCellTra,ramseyCellRot);
   TGeoHMatrix ramseyCellMat = ramseyCellCom;
   Char_t ramseyCellName[14];
   sprintf(ramseyCellName, "RamseyCellMat"); 
   ramseyCellMat.SetName(ramseyCellName);
   
   // -------------------------------------
   // -- Add Volume to Geometry
   chamber->AddNode(ramseyCell, 1, new TGeoHMatrix(ramseyCellMat));
   
   // -------------------------------------
   // -- Create and Attach Magnetic Field defined in local coordinate system
   TUCNMagField* magField = new TUCNUniformMagField("SolenoidField", bFieldX, bFieldY, bFieldZ);
   ramseyCell->AttachLocalMagField(magField);
   
   // -------------------------------------
   // -- Close Geometry
   geoManager->CloseGeometry();
   
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "$(UCNSIM)/geom/ramseycell_geom.root";
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

