/*
#ifndef __CINT__

#include "TBenchmark.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TParticle.h"


#include <iostream>
#include <fstream>
#include <stdexcept>

#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TStyle.h"
#include "TRandom.h"
#include "TBenchmark.h"
#include "TSystem.h"
#include "TGeoManager.h"
#include "TGeoTrack.h"
#include "TRint.h"
#include "TF1.h"
#include "TPolyMarker3D.h"


#include "TUCNExperiment.h"
#include "TUCNParticle.h"
#include "Constants.h"


using namespace std;

Int_t main(Int_t argc,Char_t **argv)
{
	TRint *theApp = new TRint("UCN App", &argc, argv);
#else 
Int_t ucnstandalone() {	
	gSystem->Load("libPhysics");
	gSystem->Load("libGeom");
	gSystem->Load("libEG");
	gSystem->Load("libUCN");
#endif


	// --------------------------------------------------------------------------------------

	// -- Build Geometry
	TUCNGeometry *Geometry = new TUCNGeometry();
	
	Double_t boxX = 0.2, boxY = 0.2, boxZ = 1.;
	Geometry->MakeCuboid(boxX, boxY, boxZ);
	// -- Add Gravitational Field  -- Right now need to define direction in positive direction (even if its actually downwards!)
	Double_t grav_field_dir[3] = {0,0,-1};
	Geometry->AddField(&grav_field_dir[0], grav_acceleration);
	
	Geometry->CloseGeometry();
	Geometry->DrawGeometry();
	// --------------------------------------------------------------------------------------
	
	gGeoManager->FindNode(0.,0.,0.);	
	TVector3 norm(0.0, -1.0/TMath::Sqrt(2), +1.0/TMath::Sqrt(2));
	TVector3 finalDir(0.,0.,0.);
	
	cout << "----------------------------" << endl;
	cout << "Diffuse Bounce" << endl;
	cout << "Current Volume: " << gGeoManager->GetCurrentVolume()->GetName() << endl;
	cout << "Normal Vector: " << norm.X() << "\t" << norm.Y() << "\t" << norm.Z() << endl;	
	
	// Calculate local normal vector	
	Double_t normal[3] = {norm.X(), norm.Y(), norm.Z()};
	Double_t localnormal[3];
	gGeoManager->MasterToLocalVect(normal, localnormal);
	TVector3 localNorm(localnormal[0], localnormal[1], localnormal[2]);
	cout << "Local Normal: " << localNorm.X() << "\t" << localNorm.Y() << "\t" << localNorm.Z() << endl;

	
	Int_t nbins = 100;
	
	TH1F * Histogram1a = new TH1F("Histogram1a","Theta", nbins, 0.0, TMath::Pi());
	TH1F * Histogram1b = new TH1F("Histogram1b","Phi", nbins, 0.0, TMath::Pi()*2.0);
	TH3F * Histogram1c = new TH3F("Histogram1c","Directions", nbins/2, -1.0, 1.0, nbins/2, -1.0, 1.0, nbins/2, -1.0, 1.0);
	
	for (Int_t i = 0; i < 1000000; i++) {
		// Define a random phi and theta on the unit sphere (or half unit sphere as we want theta only between 0 and Pi/2)
		Double_t phi = gRandom->Uniform(0.0, 1.0)*2*TMath::Pi();
		Double_t theta = 0.0;
		
//		Double_t u = gRandom->Uniform(0.0, 1.0);
//		theta = TMath::ACos(u);
		
		Double_t u = gRandom->Uniform(0.0, 0.5);
//		Double_t dice = gRandom->Uniform(0.0,1.0);
//		if (dice > 0.5)	{
		theta = TMath::ACos(TMath::Sqrt(1.0 - 2*u));
//		} else {
//			theta = TMath::ACos(-TMath::Sqrt(1.0 - 2*u));
//		}
		
		TVector3 localNorm(0.,-1.,0.);
		localNorm = localNorm.Unit();
		// Define a preferred direction in our coordinate system - usually the z-direction - that we want to be perpendicular to the normal vector of our surface
		TVector3 upAxis(0.,0.,1.);
		// Calculate the cross product of the 'up' vector with our normal vector to get a vector guaranteed to be perpendicular to the normal. This is the vector we will want to rotate around initially. 
		TVector3 perpAxis = localNorm.Cross(upAxis);
		// Here we check to make sure that the upAxis we chose is not parallel to the normal vector. If it is, we try another one, x. 
		if (TMath::Abs(perpAxis.Dot(perpAxis) - 1.0) > 1.e-10) {
			upAxis.SetXYZ(1.,0.,0.);
			perpAxis = localNorm.Cross(upAxis);
			if (TMath::Abs(perpAxis.Dot(perpAxis) - 1.0) > 1.e-10) {
				throw runtime_error("Error: in Diffuse bounce routine. Could not find an axis perpendicular to normal. Normal is apparanetly parallel to z and x axes!!!");
			}
		} 
		// Rotate normal vector about perpendicular axis by angle theta. Using Rodrigues' formula derived in notes. 
		TVector3 rotatedNorm;
		{
			TVector3 term3 = perpAxis.Cross(localNorm);
			TVector3 term2 = localNorm - (localNorm.Dot(perpAxis))*perpAxis;
			rotatedNorm = localNorm + (TMath::Cos(theta) - 1.0)*term2 + TMath::Sin(theta)*term3;
		}
		// Rotate the newly rotated Normal vector, rotatedNorm, by angle phi, this time rotating about the original normal vector, norm.
		TVector3 direction;
		{
			TVector3 term3 = localNorm.Cross(rotatedNorm);
			TVector3 term2 = rotatedNorm - (rotatedNorm.Dot(localNorm))*localNorm;
			direction = rotatedNorm + (TMath::Cos(phi) - 1.0)*term2 + TMath::Sin(phi)*term3;
		}
		
		assert(TMath::Abs(direction.Dot(localNorm)) > 1.e-10);
		
		
//		cout << "Local Normal: " << localNorm.X() << "\t" << localNorm.Y() << "\t" << localNorm.Z() << endl;
//		cout << "'Up' Axis: " << upAxis.X() << "\t" << upAxis.Y() << "\t" << upAxis.Z() << endl;
//		cout << "Perpendicular vector to normal: " << perpAxis.X() << "\t" << perpAxis.Y() << "\t" << perpAxis.Z() << endl;
//		cout << "Normal vector after rotating by theta: " << rotatedNorm.X() << "\t" << rotatedNorm.Y() << "\t" << rotatedNorm.Z() << endl;
//		cout << "Final direction vector after rotating by phi: " << direction.X() << "\t" << direction.Y() << "\t" << direction.Z() << endl;
	
		Histogram1a->Fill(theta);
		Histogram1b->Fill(phi);
		Histogram1c->Fill(direction.X(), direction.Y(), direction.Z());	
	}
	
	TCanvas * canvas1 = new TCanvas("Canvas1","Canvas",0,0,800,800);
	canvas1->Divide(2,1);
	canvas1->SetGrid();
	
	canvas1->cd(1);
	Histogram1a->SetXTitle("Theta");
	Histogram1a->Draw();
	
	canvas1->cd(2);
	Histogram1b->SetXTitle("Phi");
	Histogram1b->Draw();
	
	gStyle->SetPalette(1); // As recommended by the ROOT manual
	
	TCanvas * canvas2 = new TCanvas("Canvas2","Canvas",0,0,800,800);
	canvas2->cd(1);
	Histogram1c->SetXTitle("X");
	Histogram1c->SetYTitle("Y");
	Histogram1c->SetZTitle("Z");
	Histogram1c->SetTitle("Norm is unit vector of (1,1,0)");
	Histogram1c->Draw("");
	
	// Convert the direction vector back to the global frame
//	Double_t dir[3] = {direction.X(), direction.Y(), direction.Z()};
//	Double_t globalDir[3];
//	gGeoManager->LocalToMasterVect(dir, globalDir);
//	
//	finalDir.SetXYZ(globalDir[0], globalDir[1], globalDir[2]);
//	cout << "Global Final Direction: " << finalDir.X() << "\t" << finalDir.Y() << "\t" << finalDir.Z() << endl;

	
#ifndef __CINT__
	theApp->Run();
#endif
	return 0;
}
*/

