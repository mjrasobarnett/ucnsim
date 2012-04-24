#include "Units.hpp"
#include "Constants.hpp"
#include "GeomParameters.hpp"

Bool_t Build_Geom(const TGeoManager* geoManager);
Bool_t BuildFields(const TGeoManager* geoManager);

using namespace GeomParameters;
//__________________________________________________________________________
Int_t guide_section_geom()
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
   double camera[3] = {detectorValveVolXPos, detectorValveVolYPos, detectorValveVolZPos};
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
   detectorValveVolMat.SetName("DetectorValveVolMat");
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
   Double_t guideSegZPos = guideZPos;
   Double_t guideCapacity = 0.0;
   for (Int_t segNum = 1; segNum <= 5; segNum++) {
      // Define Guide Seg matrix
      TGeoRotation segmentRot("SegmentRot",guidePhi,guideTheta,guidePsi); // phi, theta, psi
      TGeoTranslation segmentTra("SegmentTra",guideXPos, guideYPos, guideSegZPos);
      TGeoCombiTrans segmentCom(segmentTra,segmentRot);
      TGeoHMatrix segmentMat = segmentCom;
      Char_t sourceMatrixName[20];
      sprintf(sourceMatrixName, "GuideSegMatrix%d", segNum);
      segmentMat.SetName(sourceMatrixName);
      chamber->AddNode(guideSeg, segNum, new TGeoHMatrix(segmentMat));
      // Shift next segment along by length of segment
      guideSegZPos = guideSegZPos + 2.0*guideSegHalfZ;
      // Calculate guide's volume
      guideCapacity += guideSegShape->Capacity();
   }
   
   // -------------------------------------
   // -- Close Geometry
   geoManager->CloseGeometry();
   
   // -------------------------------------
   // -- Write out geometry to file
   const char *fileName = "guide_section_geom.root";
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

   // ---------------------------------
   // -- Create a Magnetic field and write it to file
   string magfilename = "runs/polarisation/guide_tube/C8=9 A +SQUIDs Coil =26_4 mA +D1=2 A + 6WS TC guide tube -100 z +80 cm.txt";
   // Define shape of field
   TGeoShape* magFieldShape = new Box("FieldShape",0.031, 0.031, 3.0);
   // Define transformation that locates field in geometry
   TGeoMatrix* fieldMatrix = static_cast<TGeoMatrix*>(geoManager->GetListOfMatrices()->FindObject("DetectorValveVolMat"));
   TGeoMatrix* magFieldPosition = new TGeoHMatrix(*fieldMatrix);
   MagFieldMap* field = new MagFieldMap("Field", magFieldShape, magFieldPosition);
   if (field->BuildMap(magfilename) == kFALSE) {
      Error("BuildFieldMap","Cannot open file: %s", magfilename);
      return kFALSE;
   }
   // Add field to magfield manager
   MagFieldArray* magFieldArray = new MagFieldArray();
   magFieldArray->AddField(field);
  
   // ---------------------------------
   // -- Write magfieldmanager to geometry file
   const char *outputFileName = "guide_section_fields.root";
   TFile *file = Analysis::DataFile::OpenRootFile(fieldsFileName, "recreate");
   gravField->Write(gravField->GetName());
   magFieldArray->Write(magFieldArray->GetName());
   f->Close();s
   delete gravField;
   delete magFieldArray;
   return kTRUE;
}
