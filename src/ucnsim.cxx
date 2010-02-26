/////////////////////////////////////////////////////////////////////////////////
// ROOT tutorial
// example of standalone exectutable
//
// November 2008, S.J.M.Peeters@sussex.ac.uk
////////////////////////////////////////////////////////////////////////////////

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

/*
	TBenchmark benchmark;
	benchmark.Start("UCN Simulation");
	// --------------------------------------------------------------------------------------

	// -- Build Geometry
	TUCNGeometry *Geometry = new TUCNGeometry();
	
	Double_t boxX = 0.2, boxY = 0.2, boxZ = 1.;
	Geometry->MakeCuboid(boxX, boxY, boxZ);
	// -- Add Gravitational Field  -- Right now need to define direction in positive direction (even if its actually downwards!)
	Double_t grav_field_dir[3] = {0,0,-1};
	Geometry->AddField(&grav_field_dir[0], grav_acceleration);
	
	Geometry->CloseGeometry();

	// --------------------------------------------------------------------------------------
	// -- Program Parameters	
	Int_t neutrons = 100000;
	Double_t total_energy = 150*neV;	
	Double_t steptime = 60.0 *s;
	Double_t runtime = 60.0 *s;
//	Int_t maxstepnumber = 400;
	Double_t probDiffuseBounce = 0.1;

	// -- Initialise counters	
	Int_t detected = 0;
	Int_t remaining = 0;
	Int_t thrownaway = 0;
	
	// --------------------------------------------------------------------------------------
	// Write Data out to File
	ofstream out1("final-run2-4.txt");
	ofstream out2("initial-run2-4.txt");
	ofstream out3("initial-run2-4y-angles.txt");
	ofstream out4("trackerrors.txt");
	// --------------------------------------------------------------------------------------
	out1 << "Number" << "\t" << "Neutron Time" << "\t" << "Neutron Energy" << "\t" << "Neutron Path Length" << "\t";
	out1 << "X Position" << "\t" << "Y Position" << "\t" << "Z Position" << "\t" << "PX" << "\t" << "PY" << "\t" << "PZ" << endl;
	// --------------------------------------------------------------------------------------
	out2 << "Number" << "\t" << "Neutron Time" << "\t" << "Neutron Energy" << "\t" << "Neutron Path Length" << "\t";
	out2 << "X Position" << "\t" << "Y Position" << "\t" << "Z Position" << "\t" << "PX" << "\t" << "PY" << "\t" << "PZ" << endl;
	// --------------------------------------------------------------------------------------
	out3 << "counter1" << "\t" << "counter2" << "\t" << "dx"<< "\t" << "dy" << "\t" << "dz" << "\t" << "theta" << "\t" << "phi" << "\t" << endl;
	Int_t counter1 = 0, counter2 = 0;
	// --------------------------------------------------------------------------------------
	out4 << "Track number" << endl;
	
	// Store Final Points
	TPolyMarker3D FinalPoints(neutrons, 1);
		
	for (Int_t tracknumber = 0; tracknumber < neutrons; ++tracknumber) {
		
		// -- Define random positions and start time inside Cuboid
		Double_t x = gRandom->Uniform(-0.99, 0.99)*(boxX/2);
		Double_t y = gRandom->Uniform(-0.99, 0.99)*(boxY/2);
		Double_t z = gRandom->Uniform(0.01, 0.99)*boxZ;
		Double_t t = gRandom->Uniform(0.0,10.0); 
		
		// -- Define random directions by picking random points in unit sphere - see notes for algorithm 7/5/09
	//	Double_t phi = gRandom->Uniform(0.0, 1.0)*2*pi;
	//	Double_t u = gRandom->Uniform(-1.0, 1.0);
	//	
	//	Double_t dx = TMath::Cos(phi)*TMath::Sqrt(1.0-u*u);  // Need to add plus and minus TMath::Sqrt(1.0-u*u) to get the full theta between 0 and Pi.
	//	Double_t dy = TMath::Sin(phi)*TMath::Sqrt(1.0-u*u);  // Need to add plus and minus TMath::Sqrt(1.0-u*u) to get the full theta between 0 and Pi.
	//	Double_t dz = u;
		
		// -- Define random directions in unit sphere weighted by extra cos(theta) term, ie: biased towards the poles. See notes for details of algorithm
		Double_t phi = gRandom->Uniform(0.0, 1.0)*2*pi;
		Double_t u   = gRandom->Uniform(0.0, 0.5);
		Double_t theta; 
		if (gRandom->Uniform(0.0,1.0) > 0.5) {
			theta = TMath::ACos(TMath::Sqrt(1.0 - 2*u));
			counter1++;
		} else {
			theta = TMath::ACos(-TMath::Sqrt(1.0 - 2*u));
			counter2++;
		}
		Double_t dx = TMath::Cos(phi)*TMath::Sin(theta);
		Double_t dy = TMath::Sin(phi)*TMath::Sin(theta);
		Double_t dz = TMath::Cos(theta);
		out3 << counter1 << "\t" << counter2 << "\t" << dx << "\t" << dy << "\t" << dz << "\t" << theta << "\t" << phi << "\t" << endl;
		
		//  Define Particles in global reference frame				
		TVector3 position(x,y,z); 
		TVector3 direction(dx,dy,dz);
		TVector3 unitdirection = direction.Unit();

		TUCNParticle neutron(total_energy);
		neutron.InitialiseParticle(position.X(), position.Y(), position.Z(), unitdirection.X(), unitdirection.Y(), unitdirection.Z(), t);
		out2 << tracknumber << "\t" << neutron.T() /s << "\t" << neutron.Energy() /neV << "\t" << neutron.GetPathLength() /m ;
		out2 << "\t" << neutron.Vx() /m << "\t" << neutron.Vy() /m ;
		out2 << "\t" << neutron.Vz() /m << "\t" << neutron.Px() /eV << "\t" << neutron.Py() /eV << "\t" << neutron.Pz() /eV << endl;
						
		// -- Create Track
		TUCNTrack track(tracknumber, steptime);
		track.InitialiseTrack(&neutron);
		track.SetDiffuseProbDecimal(probDiffuseBounce);
		
		// -- Propagate Track
		if (tracknumber == 64450)  {
			track.PropagateForSetTime(runtime);  		
//		track.Propagate(maxstepnumber);
		}

		// --------------------------------------------------------------------------------------
		// -- Collect Data
		if (track.Detected()) {
			detected += 1;
		} else if (track.ThrownAway()) {
			thrownaway +=1;
			out4 << tracknumber << endl;
		} else {
			remaining += 1;
		}	
		
		FinalPoints.SetPoint(tracknumber, neutron.Vx(), neutron.Vy(), neutron.Vz());
		out1 << tracknumber << "\t" << neutron.T() /s << "\t" << neutron.Energy() /neV << "\t" << neutron.GetPathLength() /m ;
		out1 << "\t" << neutron.Vx() /m << "\t" << neutron.Vy() /m ;
		out1 << "\t" << neutron.Vz() /m << "\t" << neutron.Px() /eV << "\t" << neutron.Py() /eV << "\t" << neutron.Pz() /eV << endl;
	
	
//		gGeoManager->GetCurrentTrack()->Delete();
	
	}
	
	benchmark.Stop("UCN Simulation");
	out1.close();
	out2.close();
	
	// --------------------------------------------------------------------------------------
	// -- Finish
	benchmark.Show("UCN Simulation");
	cout << endl << "________________________________________________________" << endl << endl;	
	cout << "Total Number of Neutrons: " << neutrons << endl;
	cout << "Number Detected: " << detected << endl;
	cout << "Number Remaining: " << remaining << endl;
	cout << "Number Thrown Away: " << thrownaway << endl;
	cout << endl << "________________________________________________________" << endl << endl;
	
	// -- Draw Tube
	TCanvas * ucncanvas = new TCanvas("UCNCanvas","UCNs in Guide Tube under Gravity",0,0,800,800);
	ucncanvas->cd();
	Geometry->DrawGeometry();
	
	// -- Draw Tracks
//	gGeoManager->DrawTracks();
	
	// -- Draw Individual Track
//	((TGeoTrack*)gGeoManager->GetTrack(72))->Draw();
	
	// -- Draw Final Points
	FinalPoints.SetMarkerColor(2);
	FinalPoints.SetMarkerStyle(6);
//	FinalPoints.Draw();
	
	// --------------------------------------------------------------------------------------
		cout << "Done!" << endl;

*/	
#ifndef __CINT__
	theApp->Run();
#endif
	return 0;
}


