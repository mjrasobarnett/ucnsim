#include "TUCNGeoManager.h"
#include "TUCNGeoNavigator.h"
#include "TUCNGeoBuilder.h"
#include "TUCNGeoBBox.h"
#include "TUCNGeoMaterial.h"
#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoMatrix.h"
#include <iostream>

using namespace std;

TGeoNode* CreateGeometry_v1(TUCNGeoManager* myManager)
{
	// -- Making Materials
	// Every material made should be a TUCNMaterial. This class includes specific methods that identify the 
	// material as a one of three types: Tracking Material, Boundary Material, and BlackHole Material. All the
	// usual properties of materials can be set, but at the moment are not current required for navigation. In
	// the future expect to set the material element type, scattering length (or effective potential) and other
	// quantities. 
   	TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", 0,0,0);
   	TUCNGeoMaterial* matDetector  = new TUCNGeoMaterial("Detector Material", 0,0,0);

	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackHole(kTRUE);
	matDetector->IsDetectorMaterial(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
	TGeoMedium* detectorMedium = new TGeoMedium("DetectorBoundary",4, matDetector);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = myManager->MakeUCNBox("TOP",blackHole,20,20,20);
	myManager->SetTopVolume(chamber);
			
	// -- Make a GeoBBox object via the UCNGeoManager
	//	MakeBox has been overridden here. If you can tell me how this is possible without throwing a warning
	// or anything (seeing as this is a non-virtual method) I would be really interested to know. 
	// In any case, ALL make Volume methods have been overridden in the UCNManager to only build volumes from 
	// UCNShapes which contain the new tracking methods.
	Double_t boxX = 0.11, boxY = 0.11, boxZ = 1.01; 
	TGeoVolume* box   = myManager->MakeUCNBox("box",boundary, boxX, boxY, boxZ);
	TGeoVolume* innerBox  = myManager->MakeUCNBox("innerbox",vacuum, boxX-0.01, boxY-0.01, boxZ-0.01);
	TGeoVolume* sourceBox  = myManager->MakeUCNBox("sourcebox",vacuum, 0.01, 0.01, 0.01);
	TGeoVolume* detector  = myManager->MakeUCNBox("detector",detectorMedium, 0.1, 0.1, 0.01);
	
	
	box->SetLineColor(36);
	box->SetLineWidth(1);
	box->SetVisibility(kFALSE);
	box->SetTransparency(100);
	
	innerBox->SetLineColor(38);
	innerBox->SetLineWidth(1);
	innerBox->SetVisibility(kTRUE);
	innerBox->SetTransparency(80);
	
	sourceBox->SetLineColor(40);
	sourceBox->SetLineWidth(1);
	sourceBox->SetVisibility(kTRUE);
	sourceBox->SetTransparency(0);
	
	detector->SetLineColor(20);
	detector->SetLineWidth(1);
	detector->SetVisibility(kTRUE);
	detector->SetTransparency(0);
	
	// -- Define the transformation of the volume
	// Here is a general set-up of rotations and translations to be applied to a single volume. 
	TGeoRotation r1,r2; 
	r1.SetAngles(0,90,0);          	//	rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,0.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	matrix->Print();
	
	TGeoRotation r3; 
	r3.SetAngles(0,90,0);          	//	rotation defined by Euler angles 
	TGeoTranslation t3(0.,-0.1,0.9); 
	TGeoCombiTrans c3(t3,r3); 
	TGeoHMatrix hm2 = c3;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix2 = new TGeoHMatrix(hm2);
	
	TGeoRotation r4; 
	r4.SetAngles(0,0,0);          	//	rotation defined by Euler angles 
	TGeoTranslation t4(0.,0.,-0.9); 
	TGeoCombiTrans c4(t4,r4); 
	TGeoHMatrix hm3 = c4;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix3 = new TGeoHMatrix(hm3);
	
	
	// -- Create the nodes
	innerBox->AddNode(sourceBox,1, matrix3);	
	innerBox->AddNode(detector,2, matrix2);
	box->AddNode(innerBox,1);
	chamber->AddNode(box,1, matrix);
		
	cout << "Entries: " << box->GetNodes()->GetEntries() << endl;
	cout << "Detector: " << static_cast<TUCNGeoMaterial*>(detector->GetMedium()->GetMaterial())->IsDetectorMaterial() << endl;
	TGeoNode* sourceBoxNode = static_cast<TGeoNode*>(innerBox->GetNodes()->First());	
	// -- Arrange and close geometry
    myManager->CloseGeometry();

	return sourceBoxNode;
}

TGeoNode* CreateGeometry_v2(TUCNGeoManager* myManager) {
	// -- Making Materials
   TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", 0,0,0);
   
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackHole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = myManager->MakeBox("TOP",blackHole,20,20,20);
	myManager->SetTopVolume(chamber);
			
	// -- Make a UCNTube 
	Double_t rMin = 0.0, rMax = 0.11, length = 0.51; 
	TGeoVolume* tube   = myManager->MakeUCNTube("tube",boundary, rMin, rMax, length);
	TGeoVolume* innerTube  = myManager->MakeUCNTube("innerTube",vacuum, rMin, rMax-0.01, length-0.01);
	
	tube->SetLineColor(36);
	tube->SetLineWidth(1);
	tube->SetVisibility(kTRUE);
	tube->SetTransparency(50);
	
	innerTube->SetLineColor(8);
	innerTube->SetLineWidth(1);
	innerTube->SetVisibility(kTRUE);
	innerTube->SetTransparency(80);
	
	// -- Define the transformation of the volume
	// Here is a general set-up of rotations and translations to be applied to a single volume. 
	TGeoRotation r1,r2; 
	r1.SetAngles(0,0,0);          	//	rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,0.50); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	matrix->Print();
	
	// -- Create the nodes	
	tube->AddNode(innerTube,1);
	chamber->AddNode(tube,1, matrix);
	TGeoNode* innerTubeNode = static_cast<TGeoNode*>(tube->GetNodes()->First());	
		
	// -- Arrange and close geometry
   myManager->CloseGeometry();

	return innerTubeNode;
}

TGeoNode* CreateGeometry_v3(TUCNGeoManager* myManager) {
	// -- Making Materials
   TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", 0,0,0);
   
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackHole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = myManager->MakeBox("TOP",blackHole,20,20,20);
	myManager->SetTopVolume(chamber);
			
	// -- Make a UCNTube 
	Double_t rMin = 0.0, rMax = 1.0, length = 2.0; 
	TGeoVolume* tube   = myManager->MakeUCNTube("tube",boundary, rMin, rMax, length);
	TGeoVolume* innerTube  = myManager->MakeUCNTube("innerTube",vacuum, rMin, rMax-0.01, length-0.01);
	
	tube->SetLineColor(36);
	tube->SetLineWidth(1);
	tube->SetVisibility(kTRUE);
	tube->SetTransparency(50);
	
	innerTube->SetLineColor(8);
	innerTube->SetLineWidth(1);
	innerTube->SetVisibility(kTRUE);
	innerTube->SetTransparency(80);
	
	// -- Define the transformation of the volume
	// Here is a general set-up of rotations and translations to be applied to a single volume. 
	TGeoRotation r1,r2; 
	r1.SetAngles(0,37,0);          	//	rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,0.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	matrix->Print();
	
	// -- Create the nodes	
	tube->AddNode(innerTube,1);
	chamber->AddNode(tube,1, matrix);
	TGeoNode* innerTubeNode = static_cast<TGeoNode*>(tube->GetNodes()->First());	
		
	// -- Arrange and close geometry
   myManager->CloseGeometry();

	return innerTubeNode;
}

TGeoNode* CreateGeometry_v4(TUCNGeoManager* myManager)
{
	// -- Making Materials
	// Every material made should be a TUCNMaterial. This class includes specific methods that identify the 
	// material as a one of three types: Tracking Material, Boundary Material, and BlackHole Material. All the
	// usual properties of materials can be set, but at the moment are not current required for navigation. In
	// the future expect to set the material element type, scattering length (or effective potential) and other
	// quantities. 
	TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", 0,0,0);
//	TUCNGeoMaterial* matDetector  = new TUCNGeoMaterial("Detector Material", 0,0,0);

	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackHole(kTRUE);
//	matDetector->IsDetectorMaterial(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
//	TGeoMedium* detectorMedium = new TGeoMedium("DetectorBoundary",4, matDetector);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = myManager->MakeUCNBox("TOP",blackHole,20,20,20);
	myManager->SetTopVolume(chamber);
			
	// -- Make a GeoBBox object via the UCNGeoManager
	//	MakeBox has been overridden here. If you can tell me how this is possible without throwing a warning
	// or anything (seeing as this is a non-virtual method) I would be really interested to know. 
	// In any case, ALL make Volume methods have been overridden in the UCNManager to only build volumes from 
	// UCNShapes which contain the new tracking methods.
	Double_t boxX = 0.11, boxY = 0.11, boxZ = 1.01; 
	TGeoVolume* box   = myManager->MakeUCNBox("box",boundary, boxX, boxY, boxZ);
	TGeoVolume* innerBox  = myManager->MakeUCNBox("innerbox",vacuum, boxX-0.01, boxY-0.01, boxZ-0.01);	
	
	box->SetLineColor(36);
	box->SetLineWidth(1);
	box->SetVisibility(kFALSE);
	box->SetTransparency(100);
	
	innerBox->SetLineColor(38);
	innerBox->SetLineWidth(1);
	innerBox->SetVisibility(kTRUE);
	innerBox->SetTransparency(80);
	
	// -- Define the transformation of the volume
	// Here is a general set-up of rotations and translations to be applied to a single volume. 
	TGeoRotation r1; 
	r1.SetAngles(0,90,0);          	//	rotation defined by Euler angles 
	TGeoTranslation t1(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoHMatrix hm1 = c1;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix1 = new TGeoHMatrix(hm1);
	matrix1->Print();
	
/*	TGeoRotation r2; 
	r2.SetAngles(0,90,0);          	//	rotation defined by Euler angles 
	TGeoTranslation t2(0.,-0.1,0.9); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm2 = c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix2 = new TGeoHMatrix(hm2);
	
	TGeoRotation r3; 
	r3.SetAngles(0,0,0);          	//	rotation defined by Euler angles 
	TGeoTranslation t3(0.,0.,-0.9); 
	TGeoCombiTrans c3(t3,r3); 
	TGeoHMatrix hm3 = c3;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix3 = new TGeoHMatrix(hm3);
*/	
	
	// -- Create the nodes
	box->AddNode(innerBox,1);
	chamber->AddNode(box,1, matrix1);
		
	TGeoNode* sourceBoxNode = static_cast<TGeoNode*>(box->GetNodes()->First());	
	// -- Arrange and close geometry
    myManager->CloseGeometry();

	return sourceBoxNode;
}