
Int_t geometry_simplebox() 
{
	TUCNGeoManager * myManager    = new TUCNGeoManager("UCNGeom", "Simple box geometry");
  TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", 0,0,0);
   
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackhole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum    = new TGeoMedium("Vacuum",   1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary  = new TGeoMedium("Boundary", 3, matBoundary);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = myManager->MakeBox("TOP",blackHole,20,20,20);
	myManager->SetTopVolume(chamber);

	TGeoVolume* box      = myManager->MakeBox("box",boundary, boxX, boxY, boxZ);
	TGeoVolume* innerBox = myManager->MakeBox("innerbox",vacuum, boxX-0.01, boxY-0.01, boxZ-0.01);
	
	box->SetLineColor(36);
	box->SetLineWidth(1);
	box->SetVisibility(kTRUE);
	box->SetTransparency(50);
	
	innerBox->SetLineColor(38);
	innerBox->SetLineWidth(1);
	innerBox->SetVisibility(kTRUE);
	innerBox->SetTransparency(80);
	
	// -- Define the transformation of the volume
	// Here is a general set-up of rotations and translations to be applied to a single volume. 
	TGeoRotation r1,r2; 
	r1.SetAngles(45,138,30);          	//	rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,0.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	
	// -- Create the nodes	
	box->AddNode(innerBox,1);
	box->Print();
	chamber->AddNode(box,1, matrix);
	chamber->Print();
		
	// -- Arrange and close geometry
  myManager->CloseGeometry();
	// -----------------------------------------------
	// Store geometry
	// -----------------------------------------------
	TString ucngeom = gSystem->Getenv("UCN_GEOM");
	if ( ucngeom.Length() == 0 ) {
		cerr << "Could not find environment variable UCN_GEOM" << endl;
		ucngeom = gSystem->WorkingDirectory();
		cerr << "Saving geometry in current directory:" << endl << ucngeom << endl;
	}
	TString filename = ucngeom + "/geometry_simplebox.root";
	cerr << "Saving geometry as " << filename << endl;
	gGeoManager->Export(filename);
#ifndef __CINT__
	//theApp->Run();
#endif		
	return 0;
}
