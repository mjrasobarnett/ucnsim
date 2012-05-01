#include "GeomParameters.hpp"

Bool_t Build_Geom(const TGeoManager* geoManager);
Bool_t BuildFields(const TGeoManager* geoManager);

using namespace GeomParameters;

//__________________________________________________________________________
Int_t ramseycell_geom()
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
   double camera[3] = {preVolumeXPos, preVolumeYPos, preVolumeZPos};
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
   neutralCell->SetLineColor(kGreen-9);
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
   centralElectrodeHole->SetLineColor(kGray+3);
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
   hvCell->SetLineColor(kCyan-10);
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
   const char *fileName = "ramseycell_geom.root";
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
   GravField* gravField = new GravField(-1.0,0.0,0.0); // Pointing down the X-Axis for this geom
   printf("Gravitational Field - nx: %.4f \t ny: %.4f \t nz: %.4f\n",gravField->Nx(),gravField->Ny(),gravField->Nz());
   // -- Write fields to file
   const char *fieldsFileName = "ramseycell_fields.root";
   TFile *file = Analysis::DataFile::OpenRootFile(fieldsFileName, "recreate");
   cout << "Fields Created... Writing to file: " << fieldsFileName << endl;
   gravField->Write(gravField->GetName());
   file->Close();
   // Cleanup
   delete gravField;
   return kTRUE;
}
