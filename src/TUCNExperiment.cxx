// UCN Geometry class
// Author: Matthew Raso-Barnett  19/01/2009
#include <iostream>

#include "TMath.h"
#include "TGeoMedium.h"
#include "TGeoMatrix.h"

#include "TUCNGeoManager.h"
#include "TUCNGeoMaterial.h"
#include "Constants.h"

#include "TUCNExperiment.h"


using namespace std;

ClassImp(TUCNExperiment) 

//_____________________________________________________________________________
TUCNExperiment::TUCNExperiment()  
{
//	Default constructor
	Info("TUCNExperiment", "Default Constructor");
	fUCNGeometry = NULL;		
}

//_____________________________________________________________________________
TUCNExperiment::TUCNExperiment(TUCNGeoManager* geom)
{
	Info("TUCNExperiment", "Constructor");
	fUCNGeometry = geom;
	
	// Define materials and media to be used. The indexes are for retrieving the media from the GeoManager
	fIndexBlackhole = 1;
	fIndexTracking = 2;
	fIndexDetector = 3;
	fIndexWall = 4;
	
   TUCNGeoMaterial *matBlackhole = new TUCNGeoMaterial("Blackhole", 0,0,0);
	TUCNGeoMaterial *matTracking = new TUCNGeoMaterial("Tracking Material", 0,0,0);
   TUCNGeoMaterial *matDetector = new TUCNGeoMaterial("Detector Material", 0,0,0);
   TUCNGeoMaterial *matWalls = new TUCNGeoMaterial("Walls", 0,0,0);
	
	// Define the material properties
	matBlackhole->IsBlackhole(kTRUE);
	matTracking->IsTrackingMaterial(kTRUE);
	matDetector->IsDetectorMaterial(kTRUE);
	
  	new TGeoMedium("Blackhole", fIndexBlackhole, matBlackhole);
	new TGeoMedium("Tracking Medium", fIndexTracking, matTracking);
   new TGeoMedium("Detector", fIndexWall, matWalls);
	new TGeoMedium("Wall", fIndexDetector, matDetector);
	
	// Dimensions of Top Volume
	Double_t topX = 50.0*m;
	Double_t topY = 50.0*m;
	Double_t topZ = 50.0*m;
   
   // Make the top container volume
	TGeoVolume *top = geom->MakeBox("TOP", geom->GetMedium(fIndexBlackhole), topX, topY, topZ);
   geom->SetTopVolume(top);		
}

//_____________________________________________________________________________
TUCNExperiment::~TUCNExperiment()
{ 
// Destructor	
	Info("TUCNExperiment", "Destructor");	
}

//_____________________________________________________________________________
void TUCNExperiment::DrawGeometry()
{ 
   this->Geometry()->GetTopVolume()->Draw();
	this->Geometry()->SetVisLevel(5);
   this->Geometry()->SetVisOption(0);
}

//_____________________________________________________________________________
void TUCNExperiment::CloseGeometry()
{
	this->Geometry()->CloseGeometry();
}

//_____________________________________________________________________________
void TUCNExperiment::MakeBox(Double_t dx, Double_t dy, Double_t dz)
{
	TGeoVolume *outerchamber 	= this->Geometry()->MakeBox("OuterChamber", this->Geometry()->GetMedium(fIndexWall), dx/2, dy/2, dz/2);
	TGeoVolume *innerchamber 	= this->Geometry()->MakeBox("InnerChamber", this->Geometry()->GetMedium(fIndexTracking), (dx-0.2)/2, (dy-0.2)/2, (dz-0.2)/2);
	
	outerchamber->SetLineColor(6);
	outerchamber->SetLineWidth(1);
	outerchamber->SetVisibility(kTRUE);
	outerchamber->SetTransparency(50);
	
	innerchamber->SetLineColor(35);
	innerchamber->SetLineWidth(1);
	innerchamber->SetVisibility(kTRUE);
	innerchamber->SetTransparency(50);
	
	outerchamber->AddNode(innerchamber, 1);
	
	TGeoRotation *rotation = new TGeoRotation("rotation",15,75,30);
	this->Geometry()->GetTopVolume()->AddNode(outerchamber,1, rotation);	
}

//_____________________________________________________________________________
void TUCNExperiment::MakeTube(Double_t length, Double_t innerRadius, Double_t outerRadius)
{
	TGeoVolume *tube 			= new TGeoVolumeAssembly("Tube");
   TGeoVolume *outertube 	= this->Geometry()->MakeTube("Outertube", this->Geometry()->GetMedium(fIndexWall), 0, outerRadius, length);
	TGeoVolume *innertube 	= this->Geometry()->MakeTube("Innertube", this->Geometry()->GetMedium(fIndexTracking), 0., innerRadius, length);
	TGeoVolume *tubedoor 	= this->Geometry()->MakeTube("Tubedoor", this->Geometry()->GetMedium(fIndexWall), 0, outerRadius, 0.5); 
	TGeoVolume *detector 	= this->Geometry()->MakeTube("Detector", this->Geometry()->GetMedium(fIndexDetector), 0, outerRadius, 0.5);
	
	TGeoTranslation *detectorposition = new TGeoTranslation("detectorposition",0.,0.,length+0.5);
	TGeoTranslation *doorposition = new TGeoTranslation("doorposition",0.,0., -length-0.5);
	
	outertube->AddNode(innertube,1);
	
	tube->AddNode(outertube,1);
	tube->AddNode(detector,1, detectorposition);
	tube->AddNode(tubedoor,1, doorposition);
		
	outertube->SetLineColor(36); // See TAttLine for Line Colours
	outertube->SetLineWidth(1);
	outertube->SetVisibility(kTRUE);
	
	detector->SetLineColor(12);
	detector->SetLineWidth(1);
	detector->SetVisibility(kTRUE);
	
	tubedoor->SetLineColor(35);
	tubedoor->SetLineWidth(1);
	tubedoor->SetVisibility(kTRUE);

	innertube->SetVisibility(kFALSE);
	
	this->Geometry()->GetTopVolume()->AddNode(tube,1);	
}

//_____________________________________________________________________________
void TUCNExperiment::MakeVerticalTube(Double_t length, Double_t innerRadius, Double_t outerRadius)
{
	// -- Here we make a vertical tube where the zero of our gravitational potential is at the bottom of the tube
	
	//TGeoVolume *tube 			= new TGeoVolumeAssembly("Tube");
   TGeoVolume *outertube 	= this->Geometry()->MakeTube("Outertube", this->Geometry()->GetMedium(fIndexWall), 0, outerRadius, length);
	TGeoVolume *innertube 	= this->Geometry()->MakeTube("Innertube", this->Geometry()->GetMedium(fIndexTracking), 0., innerRadius, length);
	TGeoVolume *tubedoor 	= this->Geometry()->MakeTube("Tubedoor", this->Geometry()->GetMedium(fIndexWall), 0, innerRadius, 0.1); 
	TGeoVolume *detector 	= this->Geometry()->MakeTube("Detector", this->Geometry()->GetMedium(fIndexDetector), 0, innerRadius, 0.1);
	
	TGeoTranslation *detectorposition = new TGeoTranslation("detectorposition",0.,0.,length-0.1);
	TGeoTranslation *doorposition = new TGeoTranslation("doorposition",0.,0., -length+0.1);
	
	outertube->AddNode(innertube,1);
	
	innertube->AddNode(tubedoor,1, doorposition);
	innertube->AddNode(detector,1, detectorposition);
	//tube->AddNode(outertube,1);
	//tube->AddNode(detector,1, detectorposition);
	//tube->AddNode(tubedoor,1, doorposition);
	
	TGeoRotation *rotation = new TGeoRotation("rotation",0,90,0);
	TGeoCombiTrans *tubetransformation = new TGeoCombiTrans("tubetransfortmation", 0.0, length, 0.0, rotation);
	
	
	outertube->SetLineColor(36); // See TAttLine for Line Colours
	outertube->SetLineWidth(1);
	outertube->SetVisibility(kTRUE);
	
	detector->SetLineColor(12);
	detector->SetLineWidth(1);
	detector->SetVisibility(kTRUE);
	
	tubedoor->SetLineColor(35);
	tubedoor->SetLineWidth(1);
	tubedoor->SetVisibility(kTRUE);

	innertube->SetLineColor(5);
	innertube->SetLineWidth(1);
	innertube->SetVisibility(kTRUE);
	
	this->Geometry()->GetTopVolume()->AddNode(outertube,1, tubetransformation);
	
}


//_____________________________________________________________________________
void TUCNExperiment::MakeTorus(Double_t R,Double_t Rmin,Double_t Rmax,Double_t Phi1,Double_t Dphi)
{
	TGeoVolume *torus 			= new TGeoVolumeAssembly("Torus");
   TGeoVolume *outertorus 	= this->Geometry()->MakeTorus("Outertorus", this->Geometry()->GetMedium(fIndexWall), R, 0, Rmax, Phi1, Dphi);
	TGeoVolume *innertorus 	= this->Geometry()->MakeTorus("Innertorus", this->Geometry()->GetMedium(fIndexTracking), R, 0., Rmin, Phi1, Dphi);
//	TGeoVolume *tubedoor 	= this->Geometry()->MakeTube("Tubedoor", this->Geometry()->GetMedium(fIndexWall), 0, Rmax, 0.5); 
//	TGeoVolume *detector 	= this->Geometry()->MakeTube("Detector", this->Geometry()->GetMedium(fIndexDetector), 0, Rmax, 0.5);
		
	outertorus->AddNode(innertorus,1);
	
	torus->AddNode(outertorus,1);
//	torus->AddNode(detector,1, detectorposition);
//	torus->AddNode(tubedoor,1, doorposition);
		
	outertorus->SetLineColor(36); // See TAttLine for Line Colours
	outertorus->SetLineWidth(1);
	outertorus->SetVisibility(kTRUE);
	
//	detector->SetLineColor(12);
//	detector->SetLineWidth(1);
//	detector->SetVisibility(kTRUE);
	
//	tubedoor->SetLineColor(35);
//	tubedoor->SetLineWidth(1);
//	tubedoor->SetVisibility(kTRUE);

	innertorus->SetVisibility(kFALSE);
	
	this->Geometry()->GetTopVolume()->AddNode(torus,1);
	
}

//_____________________________________________________________________________
void TUCNExperiment::MakeTestVolume()
{
	Double_t outerchamberX = 5, outerchamberY = 5, outerchamberZ = 5;
	Double_t innerchamber1X = 4, innerchamber1Y = 4, innerchamber1Z = 4;
	Double_t innerchamber2X = 2, innerchamber2Y = 2, innerchamber2Z = 2;
	Double_t innerchamber3X = 1, innerchamber3Y = 1, innerchamber3Z = 1;
	
	
	TGeoVolume *outerchamber 	= this->Geometry()->MakeBox("OuterChamber", this->Geometry()->GetMedium(fIndexWall), outerchamberX, outerchamberY, outerchamberZ);
	TGeoVolume *innerchamber1 	= this->Geometry()->MakeBox("InnerChamber1", this->Geometry()->GetMedium(fIndexWall), innerchamber1X, innerchamber1Y, innerchamber1Z);
	TGeoVolume *innerchamber2  = this->Geometry()->MakeBox("InnerChamber2", this->Geometry()->GetMedium(fIndexWall), innerchamber2X, innerchamber2Y, innerchamber2Z);
	TGeoVolume *innerchamber3  = this->Geometry()->MakeBox("InnerChamber3", this->Geometry()->GetMedium(fIndexDetector), innerchamber3X, innerchamber3Y, innerchamber3Z);
	
	
	outerchamber->SetLineColor(6);
	outerchamber->SetLineWidth(1);
	outerchamber->SetVisibility(kTRUE);
	outerchamber->SetTransparency(50);
	
	innerchamber1->SetLineColor(35);
	innerchamber1->SetLineWidth(1);
	innerchamber1->SetVisibility(kTRUE);
	innerchamber1->SetTransparency(50);
	
	innerchamber2->SetLineColor(60);
	innerchamber2->SetLineWidth(1);
	innerchamber2->SetVisibility(kTRUE);
	innerchamber2->SetTransparency(50);
	
	innerchamber3->SetLineColor(85);
	innerchamber3->SetLineWidth(1);
	innerchamber3->SetVisibility(kTRUE);
	innerchamber3->SetTransparency(70);
	
	TGeoRotation *rotation1 = new TGeoRotation("rotation1",170,76,9);
	TGeoRotation *rotation2 = new TGeoRotation("rotation2",12,42.5,140);
	TGeoRotation *rotation3 = new TGeoRotation("rotation3",45,45,15);
	
	innerchamber2->AddNode(innerchamber3,1, rotation3);
	innerchamber1->AddNode(innerchamber2,1, rotation2);
	outerchamber->AddNode(innerchamber1, 1, rotation1);
	
	TGeoRotation *rotation = new TGeoRotation("rotation",15,75,30);
	this->Geometry()->GetTopVolume()->AddNode(outerchamber,1, rotation);	
}

//_____________________________________________________________________________
void TUCNExperiment::MakeCuboid(Double_t dx, Double_t dy, Double_t dz)
{
	Double_t outerchamberX = (dx/2.)+0.025, outerchamberY = (dy/2.)+0.025, outerchamberZ = (dz/2.)+0.0001;
	Double_t innerchamber1X = (dx/2.), innerchamber1Y = (dy/2.), innerchamber1Z = (dz/2.);
//	Double_t innerchamber2X = 2, innerchamber2Y = 2, innerchamber2Z = 2;
//	Double_t innerchamber3X = 1, innerchamber3Y = 1, innerchamber3Z = 1;
	
	
	TGeoVolume *outerchamber 	= this->Geometry()->MakeBox("OuterChamber", this->Geometry()->GetMedium(fIndexWall), outerchamberX, outerchamberY, outerchamberZ);
	TGeoVolume *innerchamber1 	= this->Geometry()->MakeBox("InnerChamber1", this->Geometry()->GetMedium(fIndexTracking), innerchamber1X, innerchamber1Y, innerchamber1Z);
//	TGeoVolume *innerchamber2  = this->Geometry()->MakeBox("InnerChamber2", this->Geometry()->GetMedium(fIndexWall), innerchamber2X, innerchamber2Y, innerchamber2Z);
//	TGeoVolume *innerchamber3  = this->Geometry()->MakeBox("InnerChamber3", this->Geometry()->GetMedium(fIndexDetector), innerchamber3X, innerchamber3Y, innerchamber3Z);
	
	
	outerchamber->SetLineColor(1);
	outerchamber->SetLineWidth(1);
	outerchamber->SetVisibility(kTRUE);
	outerchamber->SetTransparency(50);
	
	innerchamber1->SetLineColor(35);
	innerchamber1->SetLineWidth(1);
	innerchamber1->SetVisibility(kTRUE);
	innerchamber1->SetTransparency(50);
	
/*	innerchamber2->SetLineColor(60);
	innerchamber2->SetLineWidth(1);
	innerchamber2->SetVisibility(kTRUE);
	innerchamber2->SetTransparency(50);
	
	innerchamber3->SetLineColor(85);
	innerchamber3->SetLineWidth(1);
	innerchamber3->SetVisibility(kTRUE);
	innerchamber3->SetTransparency(70);
*/	
//	TGeoRotation *rotation1 = new TGeoRotation("rotation1",170,76,9);
//	TGeoRotation *rotation2 = new TGeoRotation("rotation2",12,42.5,140);
//	TGeoRotation *rotation3 = new TGeoRotation("rotation3",45,45,15);
	
//	innerchamber2->AddNode(innerchamber3,1, rotation3);
//	innerchamber1->AddNode(innerchamber2,1, rotation2);
	outerchamber->AddNode(innerchamber1, 1);
	
	TGeoRotation *rotation = new TGeoRotation("rotation",0.,0.,0.);
	TGeoCombiTrans *transformation = new TGeoCombiTrans("transfortmation", 0.0, 0.0, outerchamberZ, rotation);
	
	this->Geometry()->GetTopVolume()->AddNode(outerchamber,1, transformation);	
}




