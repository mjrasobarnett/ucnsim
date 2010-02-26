#ifndef __CINT__

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "TCanvas.h"
#include "TBenchmark.h"
#include "TFile.h"
#include "TSystem.h"
#include "TRint.h"
#include "TRandom.h"
#include "TObjArray.h"

#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoTrack.h"
#include "TNtuple.h"
#include "TParticle.h"
#include "TParticlePDG.h"
#include "TGeoMatrix.h"
#include "TPolyMarker3D.h"

#include "TUCNData.h"
#include "TUCNGeoManager.h"
#include "TUCNGeoNavigator.h"
#include "TUCNGeoBuilder.h"
#include "TUCNGeoBBox.h"
#include "TUCNGeoMaterial.h"
#include "TUCNParticleCloud.h"
#include "TUCNParticle.h"
#include "TUCNExperiment.h"
#include "Constants.h"
#include "Units.h"


using std::cout;
using std::endl;

Int_t main(Int_t argc,Char_t **argv)
{
	TRint *theApp = new TRint("UCN App", &argc, argv);
#else 
	Int_t read_and_draw() {
	gSystem->Load("libPhysics");
	gSystem->Load("libGeom");
	gSystem->Load("libEG");
	gSystem->Load("libUCN"); // libUCN needs libGeom and libEG to be loaded first or it will complain. 
#endif
	// Read in the geometry
	TString ucngeom = gSystem->Getenv("UCN_GEOM");
	if ( ucngeom.Length() == 0 ) {
		cerr << "Could not find environment variable UCN_GEOM" << endl;
		return 1;
	}
	TString filename = ucngeom + "/geometry_simplebox.root";
	cerr << "Reading geometry from " << filename << endl;
	TFile * file = new TFile(filename,"READ");
	if ( !file->IsOpen() ) {
		cerr << "Could not open file " << filename << endl;
		cerr << "Exiting..." << endl;
		return 1;
	}
	TUCNGeoManager * myManager = (TUCNGeoManager*)file->Get("UCNGeom");
	if ( myManager == NULL ) {
		cerr << "Could not find UCNGeom in " << filename << endl;
		cerr << "Exiting..." << endl;
		return 1;
	}
	myManager->GetTopVolume()->Draw();
	myManager->SetVisLevel(4);
	myManager->SetVisOption(0);

	// Read in tracks
	TString ucnroot = gSystem->Getenv("UCN_ROOT");
	TString filename = ucnroot + "/tracks.root";
	TFile * file = new TFile(filename,"READ");
	if ( !file->IsOpen() ) {
		cerr << "Failed to open " << filename << endl;
		cerr << "Exiting ... " << endl;
		return 1;
	}
	TString dataname = "ucndata";
	TUCNData * data = file->Get(dataname);
	if ( data == NULL ) {
		cerr << "Failed to find " << dataname << " in " << filename << endl;
		cerr << "Exiting ... " << endl;
		return 1;
	}
	TTree    * tracks = data->GetTracks();
	TGeoTrack * track = NULL;
	tracks->SetBranchAddress("Tracks",&track);
	Int_t nentries = (Int_t)tracks->GetEntries();
	cerr << "Found " << nentries << " entries" << endl;
  for (Int_t i=0;i<nentries;i++) {
      tracks->GetEntry(i);
			myManager->AddTrack((TVirtualGeoTrack*)track);
   }
	// ----------------------------------------------					
	// -- Draw Tracks
	// This will only work if the option to store tracks in the method PropagateTracks is turned on. 
	myManager->DrawTracks();
				
#ifndef __CINT__
	//theApp->Run();
#endif		
	return 0;
}
