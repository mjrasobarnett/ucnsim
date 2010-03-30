// -- Geometry for Mike P, of a simple vertically aligned cylinder of dimensions approximately those of the ramsey cell (RT?)
#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"

#include "../include/Units.h"
#include "../include/Constants.h"

Int_t simulation_geom();
Int_t visualisation_geom();

//__________________________________________________________________________
Int_t test_verticaltube_geom()
{
   // Build and write to file the simulation geom. Since no composites used, no need for
   // seperate visualisation geometry
   simulation_geom();
   return 0;
}

//__________________________________________________________________________
Int_t simulation_geom() {
   // Create the geoManager
   TGeoManager* geoManager = new TGeoManager("GeoManager", "Geometry Manager");

   // -------------------------------------
   // BUILDING GEOMETRY

   // Materials - Define the materials used. Leave the neutron properties to be defined on a run-by-run basis
   TUCNGeoMaterial *matTracking  = new TUCNGeoMaterial("TrackingMaterial",0,0);
   TUCNGeoMaterial *matBlackHole = new TUCNGeoMaterial("BlackHole",0,0);
   TUCNGeoMaterial *matBoundary  = new TUCNGeoMaterial("BoundaryMaterial",0,0);

   matTracking->IsTrackingMaterial(kTRUE);
   matBlackHole->IsBlackHole(kTRUE);

   // -- Making Mediums
   TGeoMedium *vacuum = new TGeoMedium("Vacuum",1, matTracking);
   TGeoMedium *blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
   TGeoMedium *boundary = new TGeoMedium("Boundary",3, matBoundary);

   // -- Making Top Volume
   TGeoVolume *chamber = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNBox("Top",blackHole,20,20,20);
   geoManager->SetTopVolume(chamber);

   // -- Make a GeoTube object via the UCNGeoManager
   Double_t rMin = 0.0, rMax = 0.236, length = 0.121;
   Double_t innerR = rMax - 0.001;
   Double_t innerLength = length - 0.001; 
   TGeoVolume *tube = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("Tube",boundary,rMin,rMax,length/2.);
   TGeoVolume *innerTube = TUCNGeoBuilder::UCNInstance(geoManager)->MakeUCNTube("InnerTube",vacuum,rMin,innerR,innerLength/2.);


   // -- Define the general transformation of the volume
   TGeoRotation r1,r2;
   r1.SetAngles(0,0,0); // rotation defined by Euler angles 
   r2.SetAngles(0,0,0);
   TGeoTranslation t1(0.,0.,innerLength/2.);
   TGeoTranslation t2(0.,0.,0.);
   TGeoCombiTrans c1(t1,r1);
   TGeoCombiTrans c2(t2,r2);
   TGeoHMatrix hm = c1 * c2; // composition is done via TGeoHMatrix class 
   TGeoHMatrix *matrix = new TGeoHMatrix(hm);
   matrix->SetName("SourceMatrix");

   TGeoVolume *volume = tube;
   TGeoVolume *innerVolume = innerTube;

   // -- Create the nodes	
   tube->AddNode(innerTube, 1);
   chamber->AddNode(tube, 1, matrix);

   // -- Arrange and close geometry
   geoManager->CloseGeometry();
   
   // -------------------------------------
   // -- Draw the geometry in OpenGLViewer
   TCanvas* canvas = new TCanvas("GeomCanvas","Canvas for visualisation of EDM Geom",60,40,600,600);
   canvas->cd();
   geoManager->SetVisLevel(3); // Default draws 4 levels down volume heirarchy
   geoManager->SetVisOption(0); // Default is 1, but 0 draws all the intermediate volumes not just the final bottom layer
   geoManager->GetTopVolume()->Draw("ogl");

   // -- Get the GLViewer so we can manipulate the camera
   TGLViewer * glViewer = dynamic_cast<TGLViewer*>(gPad->GetViewer3D());

   // -- Select Draw style 
   glViewer->SetStyle(TGLRnrCtx::kFill); // TGLRnrCtx::kFill, TGLRnrCtx::kOutline, TGLRnrCtx::kWireFrame

   // -- Set Background colour
   glViewer->SetClearColor(TColor::kBlack);

   // -- Set Lights - turn some off if you wish
   //	TGLLightSet* lightSet = glViewer->GetLightSet();
   //	lightSet->SetLight(TGLLightSet::kLightLeft, kFALSE);

   // -- Set Camera type
   // kCameraPerspXOZ, kCameraPerspYOZ, kCameraPerspXOY, kCameraOrthoXOY
   // kCameraOrthoXOZ, kCameraOrthoZOY, kCameraOrthoXnOY, kCameraOrthoXnOZ, kCameraOrthoZnOY
   TGLViewer::ECameraType camera = 2;
   glViewer->SetCurrentCamera(camera);
   // glViewer->CurrentCamera().SetExternalCenter(kTRUE);
   Double_t cameraCentre[3] = {0., 0., 0.};
   glViewer->SetPerspectiveCamera(camera,4,100,&cameraCentre[0],0,0);

   // -- Draw Reference Point, Axes
   Double_t refPoint[3] = {0.,0.,0.};
   // Int_t axesType = 0(Off), 1(EDGE), 2(ORIGIN), Bool_t axesDepthTest, Bool_t referenceOn, const Double_t referencePos[3]
   glViewer->SetGuideState(0, kFALSE, kFALSE, refPoint);

   glViewer->UpdateScene();
   
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "geom/test_verticaltube_geom.root";
   cerr << "Geometry Built... Writing to file: " << fileName << endl;
   geoManager->Export(fileName);

   fileName = "geom/test_verticaltube_vis.root";
   cerr << "Visualisation Geometry Built... Writing to file: " << fileName << endl;
   geoManager->Export(fileName);
   return 0;
}
