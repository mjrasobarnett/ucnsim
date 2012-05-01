// This is a model of the 'Real' cryoEDM sourcetube - 25/01/2010
#include "Units.hpp"
#include "Constants.hpp"
#include "GeomParameters.hpp"

Bool_t BuildGeom(const TGeoManager* geoManager);
Bool_t BuildFields(const TGeoManager* geoManager);

using namespace GeomParameters;
using namespace FieldParameters;

//__________________________________________________________________________
Int_t hv_cell_geom()
{
   // Load project's shared library (see $ROOTALIAS for function definition)
   load_library("$UCN_DIR/lib/libUCN.so");
   // Create the geoManager and list of standard materials
   TGeoManager* geoManager = new TGeoManager("GeoManager","Geometry Manager");
   Materials::BuildMaterials(geoManager);
   // Build and write to file the simulation and visualisation geoms
   BuildGeom(geoManager);
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
Bool_t BuildGeom(const TGeoManager* geoManager) 
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
   Box* topShape = new Box("Top",topX,topY,topZ);
   BlackHole* top = new BlackHole("Top", topShape, blackhole);
   geoManager->SetTopVolume(top);
   top->SetVisibility(kFALSE);
   
   // -- Make the boundary volume in which all the others sit
   // -- This is what we will be reflecting off all the time
   Double_t surfaceRoughness = 0.05;
   Box* chamberShape = new Box("Chamber",chamberX,chamberY,chamberZ);
   Boundary* chamber = new Boundary("Chamber", chamberShape, beryllium, surfaceRoughness);
   chamber->SetLineColor(kOrange-7);
   chamber->SetLineWidth(1);
   chamber->SetVisibility(kFALSE);
   chamber->SetTransparency(80);
   // Add to geom
   top->AddNode(chamber,1);
   
   // HV Cell
   Tube *hvCellShape = new Tube("HVShape", hvCellRMin, hvCellRMax, hvCellHalfZ);
   TrackingVolume* hvCell = new TrackingVolume("HVCell", hvCellShape, heliumII);
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
   const char *fileName = "hv_cell_geom.root";
   cout << "--------------------------------" << endl;
   cout << "Simulation Geometry Built... Writing to file: " << fileName << endl;
   cout << "--------------------------------" << endl;
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
   // ---------------------------------
   // -- Magnetic Fields
//   MagFieldArray* magFieldArray = new MagFieldArray();
//   // -- Define solenoid field - uniform magnetic field
//   // Define shape of field
//   TGeoShape* fieldShape = new Tube("SolenoidFieldShape",hvCellRMin, hvCellRMax, hvCellHalfZ);
//   // Define transformation that locates field in geometry
//   TGeoMatrix* fieldMatrix = static_cast<TGeoMatrix*>(geoManager->GetListOfMatrices()->FindObject("HVCellMat"));
//   TGeoMatrix* fieldPosition = new TGeoHMatrix(*fieldMatrix);
//   // Define field vector in Coordinate system of Field
//   TVector3 fieldStrength(solenoidBx, solenoidBy, solenoidBz);
//   // Define field object
//   MagField* uniformField = new UniformMagField("SolenoidField", fieldStrength, fieldShape, fieldPosition);
//   // Add field to magfield manager
//   magFieldArray->AddField(uniformField);

//   // Create a Magnetic field map
//   TGeoShape* magFieldShape = new Tube("SolenoidFieldShape",hvCellRMin, hvCellRMax, hvCellHalfZ);
//   TGeoMatrix* magFieldPosition = new TGeoHMatrix(matrix);
//   MagFieldMap* fieldMap = new MagFieldMap("SolenoidField", magFieldShape, magFieldPosition);
//   if (fieldMap->BuildMap(filename) == kFALSE) {
//      Error("BuildFieldMap","Cannot open file: %s", filename);
//      return kFALSE;
//   }
//   // Add fieldmap to magfield manager
//   magFieldArray->AddField(fieldMap);
   
//   // Elec Field
//   TGeoShape* elecFieldShape = new Tube("SolenoidFieldShape",hvCellRMin, hvCellRMax, hvCellHalfZ);
//   TGeoMatrix* elecFieldPosition = new TGeoHMatrix(matrix);
//   TVector3 elecFieldStrength(hvCellEx, hvCellEy, hvCellEz);
//   ElecField* elecField = new UniformElecField("ElectricField", elecFieldStrength, elecFieldShape, elecFieldPosition);
//   // Add field to electric field manager
//   ElecFieldArray* elecFieldArray = new ElecFieldArray();
//   elecFieldArray->AddField(elecField);

   // -- Write fields to file
   const char *fieldsFileName = "hv_cell_fields.root";
   TFile *file = Analysis::DataFile::OpenRootFile(fieldsFileName, "recreate");
   gravField->Write(gravField->GetName());
//   magFieldArray->Write(magFieldArray->GetName());
   file->Close();
   // Cleanup
   delete gravField;
//   delete magFieldArray;
   return kTRUE;
}
