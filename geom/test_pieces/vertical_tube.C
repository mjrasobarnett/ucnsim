#include "Units.hpp"
#include "Constants.hpp"
#include "GeomParameters.hpp"

Bool_t Build_Geom(const TGeoManager* geoManager);
Bool_t BuildFields(const TGeoManager* geoManager);

using namespace GeomParameters;

//__________________________________________________________________________
Int_t vertical_tube()
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
   TGeoMedium* copper = geoManager->GetMedium("Copper");
   TGeoMedium* aluminium = geoManager->GetMedium("Aluminium");
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
   // -- Vertical Tube   
   // -- Make a SourceTube Segment
   Tube *tubeShape = new Tube("Tube", 0.0, 0.235, 0.06);
   TrackingVolume* tube = new TrackingVolume("Tube", tubeShape, heliumII);
   tube->SetLineColor(kAzure-4);
   tube->SetLineWidth(1);
   tube->SetVisibility(kTRUE);
   tube->SetTransparency(20);
   // -- Define matrix
   TGeoRotation rot("Rot",0,0,0); // phi, theta, psi
   TGeoTranslation tra("Tra",0.,0.,0.); // x, y, z
   TGeoCombiTrans com(tra,rot);
   TGeoHMatrix mat = com;
   Char_t matrixName[20];
   sprintf(matrixName, "TubeMatrix"); 
   mat.SetName(matrixName);
   chamber->AddNode(tube, 1, new TGeoHMatrix(mat));
    
   // -------------------------------------
   // -- Close Geometry
   geoManager->CloseGeometry();
   
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "vertical_tube_geom.root";
   cerr << "Simulation Geometry Built... Writing to file: " << fileName << endl;
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
   GravField* gravField = new GravField();
   printf("Gravitational Field - nx: %.4f \t ny: %.4f \t nz: %.4f\n",gravField->Nx(),gravField->Ny(),gravField->Nz());
   // -- Write fields to file
   const char *fieldsFileName = "vertical_tube_fields.root";
   TFile *file = Analysis::DataFile::OpenRootFile(fieldsFileName, "recreate");
   cout << "Fields Created... Writing to file: " << fieldsFileName << endl;
   gravField->Write(gravField->GetName());
   file->Close();
   // Cleanup
   delete gravField;
   return kTRUE;
}
