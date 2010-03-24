// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <vector>
#include <stdexcept>
#include <sys/stat.h> // Required by Progress bar

#include "TUCNRun.h"
#include "TUCNConfigFile.h"
#include "TUCNGeoMaterial.h"
#include "TUCNFieldManager.h"
#include "TUCNMagField.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"
#include "TUCNData.h"
#include "TUCNGeoBBox.h"
#include "TUCNParabola.h"

#include "TGeoNode.h"
#include "TGeoPatternFinder.h"
#include "TGeoVoxelFinder.h"
#include "TParticle.h"
#include "TGeoNavigator.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TObjArray.h"
#include "TPolyMarker3D.h"
#include "TGeoManager.h"
#include "TRandom.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TVirtualGeoTrack.h"

#include "Units.h"

using std::cout;
using std::endl;
using std::cerr;
using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(TUCNRun)

//_____________________________________________________________________________
TUCNRun::TUCNRun()
        :TNamed("Run", "Default Run")
{
// -- Default constructor
   Info("TUCNRun", "Default Constructor");
   // Create the data object
   fParticles = 0;
   fData = 0;
   fRunTime = 0.0;
   fMaxStepTime = 0.0;
   fDiffuseCoeff = 0;
   fSampleMagField = kFALSE;
   fWallLosses = kFALSE;
   fAbsorbedCounter = 0;
   fDetectedCounter = 0;
   fDecayedCounter = 0;
   fLostCounter = 0;
} 

//_____________________________________________________________________________
TUCNRun::TUCNRun(const char *name, const char *title)
        :TNamed(name, title)
{
// -- Default constructor
   Info("TUCNRun", "Constructor");
   // Create the data object
   fParticles = new TObjArray(256);
   fData = new TUCNData("ucndata", "ucndata");
   fRunTime = 0.0;
   fMaxStepTime = 0.0;
   fDiffuseCoeff = 0;
   fSampleMagField = kFALSE;
   fWallLosses = kFALSE;
   fAbsorbedCounter = 0;
   fDetectedCounter = 0;
   fDecayedCounter = 0;
   fLostCounter = 0;
   // Set ownership over the particles
   fParticles->SetOwner(kTRUE);
}

//_____________________________________________________________________________
TUCNRun::TUCNRun(const TUCNRun& run)
		  :TNamed(run),
			fParticles(run.fParticles),
			fData(run.fData),
			fRunTime(run.fRunTime),
			fMaxStepTime(run.fMaxStepTime),
			fDiffuseCoeff(run.fDiffuseCoeff),
			fSampleMagField(run.fSampleMagField),
			fWallLosses(run.fWallLosses),
			fAbsorbedCounter(run.fAbsorbedCounter),
			fDetectedCounter(run.fDetectedCounter),
			fDecayedCounter(run.fDecayedCounter),
			fLostCounter(run.fLostCounter)
			
{
// -- Copy Constructor
	Info("TUCNRunManager", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNRun& TUCNRun::operator=(const TUCNRun& run)
{
// --assignment operator
	if(this!=&run) {
		TNamed::operator=(run);
      fParticles = run.fParticles;
		fData = run.fData;
		fRunTime = run.fRunTime;
		fMaxStepTime = run.fMaxStepTime;
		fDiffuseCoeff = run.fDiffuseCoeff;
		fSampleMagField = run.fSampleMagField;
		fWallLosses = run.fWallLosses;
		fAbsorbedCounter = run.fAbsorbedCounter;
		fDetectedCounter = run.fDetectedCounter;
		fDecayedCounter = run.fDecayedCounter;
      fLostCounter = run.fLostCounter;
	}
   return *this;
}

//______________________________________________________________________________
TUCNRun::~TUCNRun()
{
// -- Destructor
	Info("TUCNRun", "Destructor");
   fParticles->Delete();
	fData->Delete();
}

//_____________________________________________________________________________
Bool_t TUCNRun::Initialise(TUCNConfigFile* configFile)
{
   // Initialise the Run
   cout << "-------------------------------------------" << endl;
   cout << "Initialising: " << this->GetName() << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // Store the run parameters
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Load Particles
   TString particlesFile = configFile->GetString("ParticleFile","I/O");
   if (particlesFile == "") { 
      Error("Initialise","No Particles File has been specified");
      return kFALSE;
   }
   cout << "-------------------------------------------" << endl;
   cout << "Loading Particles from File: " << particlesFile << endl;
   TFile *f = TFile::Open(particlesFile,"read");
   if (!f || f->IsZombie()) {
      Error("Export","Cannot open file");
      return kFALSE;
   }
   TUCNData* initialParticles = 0;
   f->GetObject("InitialParticleData;1",initialParticles);
   delete f;
   // copy particles into Run's particle list
   for(Int_t i = 0;i < initialParticles->InitialParticles(); i++) {
      this->AddParticle(initialParticles->GetInitialParticleState(i));
   }
   cout << this->GetParticles()->GetEntriesFast() << " Particles loaded succesfully" << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Get the FermiPotential of the Wall 
   // !!!! This is ugly and unsafe. Needs a work around. 
   // Perhaps get the name of the boundary material from configFile
   TUCNGeoMaterial* boundaryMaterial = static_cast<TUCNGeoMaterial*>(gGeoManager->GetMaterial("BoundaryMaterial"));
   if (boundaryMaterial == NULL) { 
      Error("Initialise","BoundaryMaterial cannot be found");
      return kFALSE;
   }
   Double_t fermiV = boundaryMaterial->FermiPotential();    
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Run Time
   fRunTime = TMath::Abs(configFile->GetFloat("RunTime(s)", this->GetName()))*Units::s;
   if (fRunTime == 0.0) { Warning("Initialise","No RunTime has been set"); return kFALSE; }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Max Step Time
   fMaxStepTime = TMath::Abs(configFile->GetFloat("MaxStepTime(s)", this->GetName()))*Units::s;
   if (fMaxStepTime == 0.0) { 
      Warning("Initialise","No max step time has been set");
      return kFALSE;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Diffuse Coefficient
   fDiffuseCoeff = TMath::Abs(configFile->GetFloat("DiffuseBounceCoefficient", this->GetName()));
   // -- Set material's roughness parameter to the Diffuse Coefficient
   boundaryMaterial->RoughnessCoeff(fDiffuseCoeff); 
   ///////////////////////////////////////////////////////////////////////////////////////	
   // -- Determine if we will sample the magnetic field
   fSampleMagField = configFile->GetBool("SampleMagField", this->GetName());
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Determine if we will sample the magnetic field
   fWallLosses = configFile->GetBool("WallLosses", this->GetName());
   if (fWallLosses == kFALSE) {
      boundaryMaterial->WPotential(0.0); 
   } else {
      Double_t W = TMath::Abs(configFile->GetFloat("WPotential(neV)", "Geometry"))*Units::neV;
      boundaryMaterial->WPotential(W);
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   cout << "Particles: " << this->GetParticles()->GetEntriesFast() << endl;
   cout << "RunTime(s): " << fRunTime << endl;
   cout << "MaxStepTime(s): " << fMaxStepTime << endl;
   cout << "DiffuseCoeff: " << fDiffuseCoeff << endl;
   cout << "SampleMagField: " << fSampleMagField << endl;
   cout << "WallLosses: " << fWallLosses << endl;
   cout << "-------------------------------------------" << endl;
   cout << "Run successfully initialised" << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::Propagate(TGeoManager* geoManager, TUCNFieldManager* fieldManager)
{
   // -- Propagate all tracks stored in the GeoManager for a set period of time
   Int_t numberOfTracks = this->GetParticles()->GetEntriesFast();
   // Container to store Ids of lost tracks;
   std::vector<Int_t> lostTracks;
   
   for (Int_t trackid = 0; trackid < numberOfTracks; trackid++) {
      // Print Progress Bar to Screen
      #ifndef VERBOSE_MODE
         PrintProgress(trackid, numberOfTracks);
      #endif
      // Get Particle from list
      TUCNParticle* particle = static_cast<TUCNParticle*>(this->GetParticles()->At(trackid));
      // Store the random number seed in particle
      particle->SetRandomSeed(gRandom->GetSeed());
      // Add Initial Particle State to data tree
      this->SaveInitialParticle(particle);
      // Attempt to Propagate track
      try {
         particle->Propagate(this, geoManager->GetCurrentNavigator(), fieldManager);
      } catch (...) {
         // Serious tracking errors (eg: particle cannot be located correctly) will be thrown
         cout << "-------------------------------------------" << endl;
         Error("Propagate","Exception thrown by particle %i. Propagation Failed.", trackid);
         cout << "-------------------------------------------" << endl << endl;
         // Save Errorneous Particle Initial and Final State in special bad track Tree
         // ...
         // Can then reload these particles at later stage with debugging code on
         // continue;
      }
      // Add Final Particle State to data tree
      this->SaveParticle(particle);
   }
   
   cout << "-------------------------------------------" << endl;
   cout << "Propagation Results: " << endl;
   cout << "Total Particles: " << this->GetParticles()->GetEntriesFast() << endl;
   cout << "Number Detected: " << this->Detected() << endl;
   cout << "Number Absorbed by Boundary: " << this->Absorbed() << endl;
   cout << "Number Decayed: " << this->Decayed() << endl;
   cout << "Number Lost To Outer Geometry: " << this->Lost() << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
}

//_____________________________________________________________________________
void TUCNRun::DrawParticles(TCanvas* canvas, TPolyMarker3D* points)
{
	// -- Draw Volumes
	canvas->Draw();
	gGeoManager->GetTopVolume()->Draw();
	gGeoManager->SetVisLevel(4);
	gGeoManager->SetVisOption(0);
	
	// -- Number of particles
	Int_t particles = this->GetParticles()->GetEntriesFast();
	
	// -- Draw Particles
	for (Int_t i = 0; i < particles; i++) {
		TUCNParticle* particle = this->GetParticle(i);
		assert(particle != NULL);
		points->SetPoint(i, particle->Vx(), particle->Vy(), particle->Vz());
	}
	
	points->SetMarkerColor(2);
	points->SetMarkerStyle(6);
	points->Draw();
}

//_____________________________________________________________________________
Bool_t TUCNRun::Export(TString& outputFile)
{
// -- Export this run to file
	cout << "-------------------------------------------" << endl;
	cout << "Writing " << this->GetName() << " out to File: " << outputFile << endl;
	// -- Check that Data 'checks out'
	if (this->GetData()->ChecksOut() == kFALSE) {
		Error("Export", "TUCNData doesn't check out. Cannot Export Run to file.");
		return kFALSE;
	}
	// -- Check we have a .root file
	if (!(outputFile.Contains(".root"))) {
		Error("Export", "OutputFile is not a ROOT filename");
		return kFALSE;
	} 
	// -- Write out
	TFile *f = TFile::Open(outputFile,"update");
	if (!f || f->IsZombie()) {
	   Error("Export","Cannot open file");
	   return kFALSE;
	}
	char keyname[256];
	strcpy(keyname,this->GetName());
	this->Write(keyname);
	cout << "Run: " << keyname << " was successfully written to file" << endl;
	cout << "-------------------------------------------" << endl;
	// -- Clean up
	delete f;
	return kTRUE;
}

/*//_____________________________________________________________________________
Bool_t TUCNRun::AddTrack(TVirtualGeoTrack* track)
{
	return this->GetData()->AddTrack(static_cast<TGeoTrack*>(track));
}
*/
//_____________________________________________________________________________
Bool_t TUCNRun::AddParticle(TUCNParticle* particle)
{
   // Add particle to the particle array
   Int_t index = particle->Id();
   this->GetParticles()->AddAtAndExpand(particle, index);
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::SaveInitialParticle(TUCNParticle* particle) 
{
	return this->GetData()->AddInitialParticleState(particle);
}

//_____________________________________________________________________________
Bool_t TUCNRun::SaveParticle(TUCNParticle* particle)
{
	return this->GetData()->AddFinalParticleState(particle);
}

/*//_____________________________________________________________________________
TGeoTrack* TUCNRun::GetTrack(Int_t trackID)
{
	// -- Retrieve the requested track from the Data
	return this->GetData()->GetTrack(trackID);
}
*/
//_____________________________________________________________________________
TUCNParticle* TUCNRun::GetParticle(Int_t particleID)
{
	// -- Retrieve the requested particle from the Data
	return this->GetData()->GetFinalParticleState(particleID);
}

//_____________________________________________________________________________
TUCNParticle* TUCNRun::GetInitialParticle(Int_t particleID)
{
	// -- Retrieve the requested initial particle state from the Data
	return this->GetData()->GetInitialParticleState(particleID);
}

//_____________________________________________________________________________
void TUCNRun::PrintProgress(Int_t entry, Float_t nEntriesF, Int_t mintime)
{
 // Written by Nicholas Devenish - 01/2010
  Int_t nEntries = (Int_t)nEntriesF;

  // Are we streaming to a file? If so, show the old style
  struct stat buf;
  fstat(fileno(stdout), &buf);
  // Check if we are a file, or a pipe (i.e. in case the user tees)
  const bool isFile = buf.st_mode & (S_IFREG | S_IFIFO) ;
  if (isFile) {
    Float_t fract = ceil(nEntries/20.);  
    if (ceil(((Float_t)entry)/fract)==((Float_t)entry)/fract)
    {
        cout 
          <<"Fraction of loop complete: "<<entry 
          <<"/"<<nEntries<<"  ("
          <<(Int_t)(100.*entry/nEntries)<<"%)"<<endl;
    }
    return;
  }


  // How wide should we make the progress bar?
  const Int_t width = 70;
  // How long is the string for entries?
  static Int_t countlen = -1;
  // How long is our progress bar?
  static Int_t barlen = -1;
  // The entry number of the next bar entry
  static Int_t nextbar = -1;
  // When did we start?
  static time_t starttime = 0;
  // when do we next update?
  static time_t nextupdate = 0;
  // If we are processing the first entry, reset everything
  if (entry <= 1)
  {
    // Get the new length of the entry string
    countlen = (Int_t)TMath::Ceil(TMath::Log10(nEntries)) + 1;
    nextbar = -1;
    starttime = time(NULL);

    barlen = width - 14 - countlen*2 - 1;

    // Don't update until we get to the minimum time
    nextupdate = starttime + mintime;
  }

  // Check here to see if we should update; otherwise, return
  // Check to see if the bar would update
  // or, alternatively, it is time to refresh.
  if ((time(NULL) < nextupdate) && (entry < nextbar || (nextbar == -1)))return;
  nextupdate = time(NULL) + 10;

  // Because this is used in several places, make it here
  Float_t frac = (Float_t)entry / (Float_t)nEntries;

  // Prepare the progress bar string
  TString bar;
  if (entry <= nEntries)
  {
    // Work out how many characters we are in
    Int_t numeq = TMath::FloorNint(frac*barlen);

    // Work out when the next bar will occur
    nextbar = (Int_t)((Float_t)(numeq+1)/(Float_t)barlen*nEntries);
    //cout << "Next bar at: " << nextbar << "        " << endl;
    bar = TString('=', numeq);
    bar += TString(' ', barlen - numeq);
  } else if (entry > nEntries) {
    // We have gone over. Oh no!
    bar = TString('+', barlen);
  } else if (entry < 0) {
    // Somehow, we are below zero. Handle it nonetheless
    bar = TString('-', barlen);
  }


  // Prepare the ETA
  Float_t elapsed_time = (Float_t)(time(NULL) - starttime);
  Float_t time_left = -60;
  if (frac > 1e-6) {
    time_left = (elapsed_time / frac) - elapsed_time;
  }
  Int_t mins, seconds;
  mins    = (Int_t)TMath::Floor(time_left / 60.0f);
  seconds = (Int_t)TMath::Floor(time_left - (Float_t)(mins*60.0f));
  // TString ETA;
  std::ostringstream ETA;

  ETA << "ETA ";
  if ((mins < 0 || seconds < 0) || (mins == 0 && seconds == 0)) {
    ETA << "--:--";
  } else {
    ETA << std::setfill('0') << std::setw(2) << mins << ":" << std::setw(2) << seconds;
  }

  cout << " Progress: [" << bar << "] "
       << std::setw(countlen) << entry << "/" << nEntries
       << " " << ETA.str()
       << '\r'
       << std::flush;
  // Move to the next line, if this is the final entry!
  if (entry == nEntries) {
    cout << endl;
  }
}

//_____________________________________________________________________________
Int_t TUCNRun::Neutrons() const
{
   return fParticles->GetEntriesFast();
}
