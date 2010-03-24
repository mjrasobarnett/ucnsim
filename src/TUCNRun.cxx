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
using std::string;
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
   fData = 0;
   fExperiment = 0;
   fRunTime = 0.0;
   fMaxStepTime = 0.0;
} 

//_____________________________________________________________________________
TUCNRun::TUCNRun(const char *name, const char *title)
        :TNamed(name, title)
{
// -- Default constructor
   Info("TUCNRun", "Constructor");
   // Create the data object
   fData = new TUCNData("UCNData", "Data for UCNSIM Run");
   fExperiment = new TUCNExperiment();
   fRunTime = 0.0;
   fMaxStepTime = 0.0;
}

//_____________________________________________________________________________
TUCNRun::TUCNRun(const TUCNRun& run)
        :TNamed(run),
         fData(run.fData),
         fExperiment(run.fExperiment),
         fRunTime(run.fRunTime),
         fMaxStepTime(run.fMaxStepTime)
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
      fData = run.fData;
      fExperiment = run.fExperiment;
      fRunTime = run.fRunTime;
      fMaxStepTime = run.fMaxStepTime;
   }
   return *this;
}

//______________________________________________________________________________
TUCNRun::~TUCNRun()
{
// -- Destructor
   Info("TUCNRun", "Destructor");
   if(fData) delete fData;
   if(fExperiment) delete fExperiment;
}

//_____________________________________________________________________________
Bool_t TUCNRun::Initialise(TUCNConfigFile& configFile)
{
   // Initialise the Run
   cout << "-------------------------------------------" << endl;
   cout << "Initialising: " << this->GetName() << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Build Geometry
   ///////////////////////////////////////////////////////////////////////////////////////
   if (!(this->GetExperiment()->Initialise(configFile, *this))) {
      Error("Initialise","Failed to Build the Experiment");
      return kFALSE;
   }
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Load Particles
   ///////////////////////////////////////////////////////////////////////////////////////
   if (!(this->LoadParticles(configFile))) {
      Error("Initialise","Failed to Load the Initial Particle Distribution from File");
      return kFALSE;
   }  
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Run Parameters
   // Run Time
   fRunTime = TMath::Abs(configFile.GetFloat("RunTime(s)", this->GetName()))*Units::s;
   if (fRunTime <= 0.0) { Error("Initialise","No RunTime set!"); return kFALSE; }
   // Max Step Time
   fMaxStepTime = TMath::Abs(configFile.GetFloat("MaxStepTime(s)", this->GetName()))*Units::s;
   if (fMaxStepTime == 0.0) { Error("Initialise","No maxsteptime set!"); return kFALSE; }
   // Determine if we will have any wall losses
   Bool_t wallLosses = configFile.GetBool("WallLosses", this->GetName());
   cout << "Wall Losses needs to be re-implemented" << endl;
   /*
      Need a new way to do this
   */
   ///////////////////////////////////////////////////////////////////////////////////////
   cout << "Particles: " << this->GetData()->InitialParticles() << endl;
   cout << "RunTime(s): " << fRunTime << endl;
   cout << "MaxStepTime(s): " << fMaxStepTime << endl;
   cout << "WallLosses: " << wallLosses << endl;
   cout << "-------------------------------------------" << endl;
   cout << "Run successfully initialised" << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::Start()
{
// -- Propagate the particles stored in the Run's Data, specified by configFile
   cout << "-------------------------------------------" << endl;
   cout << "Starting Simulation of " << this->GetName() << endl;
   cout << "-------------------------------------------" << endl;
   Int_t totalParticles = this->GetData()->InitialParticles();
   ///////////////////////////////////////////////////////////////////////////////////////
   // Loop over all particles stored in InitialParticles Tree
   for (Int_t particleID = 0; particleID < totalParticles; particleID++) {
      // Get Particle from list
      TUCNParticle* particle = dynamic_cast<TUCNParticle*>(this->GetInitialParticle(particleID));
      // Determine whether we are restoring to the start of a previously propagated track.
      // If so we want to set the Random Generator's seed back to what it was at the start of this
      // particle's propagation. This seed is stored (currently) in the particle itself. Otherwise
      // just store the seed in the new particle.
      if (particle->GetRandomSeed() == 0) {
         // Particle is 'new', store current random seed in particle
         particle->SetRandomSeed(gRandom->GetSeed());
      } else {
         // Particle is 'restored' -> seed must be restored too
         gRandom->SetSeed(particle->GetRandomSeed());
      }
      ///////////////////////////////////////////////////////////////////////////////////////
      // Attempt to Propagate track
      try {
         Bool_t propagated = particle->Propagate(this);
         if (!propagated) {
            Error("Start", "Propagation Failed to Begin.");
            return kFALSE;
         }
      } catch (...) {
         // Serious tracking errors (eg: particle cannot be located correctly) will be thrown
         cout << "-------------------------------------------" << endl;
         Error("Start","Exception thrown by particle %i. Propagation Failed.", particleID);
         cout << "-------------------------------------------" << endl << endl;
         // Add this particle to special tree for errorneous particles
         this->GetData()->AddBadParticleState(particle);
         delete particle;
         continue;
      }
      ///////////////////////////////////////////////////////////////////////////////////////
      // Add Final Particle State to data tree
      if (!(particle->SaveState(this))) {
         Error("Start","Particle has failed to save its final state correctly");
         return kFALSE;
      }
      // Print Progress Bar to Screen
      #ifndef VERBOSE_MODE
         PrintProgress(particleID, totalParticles);
      #endif
      // Delete Particle
      delete particle;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   cout << "-------------------------------------------" << endl;
   cout << "Propagation Results: " << endl;
   cout << "Total Particles: " << this->GetData()->InitialParticles() << endl;
   cout << "Number Still Propagating: " << this->GetData()->PropagatingParticles() << endl;
   cout << "Number Detected: " << this->GetData()->DetectedParticles() << endl;
   cout << "Number Absorbed by Boundary: " << this->GetData()->AbsorbedParticles() << endl;
   cout << "Number Decayed: " << this->GetData()->DecayedParticles() << endl;
   cout << "Number Lost To Outer Geometry: " << this->GetData()->LostParticles() << endl;
   cout << "Number With Anomalous Behaviour: " << this->GetData()->BadParticles() << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::Finish(TUCNConfigFile& configFile)
{
// -- Export this run to file
   TString outputFile = configFile.GetString("OutputDataFile",this->GetName());
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
   cout << keyname << " was successfully written to file" << endl;
   f->ls();
   cout << "-------------------------------------------" << endl;
   // -- Clean up
   delete f;
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::SaveInitialParticle(TUCNParticle* particle) 
{
   return this->GetData()->AddInitialParticleState(particle);
}

//_____________________________________________________________________________
TUCNParticle* TUCNRun::GetInitialParticle(Int_t particleID)
{
   // -- Retrieve the requested initial particle state from the Data
   return this->GetData()->GetInitialParticleState(particleID);
}

//_____________________________________________________________________________
Bool_t TUCNRun::LoadParticles(TUCNConfigFile& configFile)
{
// Fetch initial particles tree from file specified in Config File
   // Get name of file holding the data from ConfigFile 
   TString particlesFile = configFile.GetString("InputDataFile",this->GetName());
   if (particlesFile == "") { 
      Error("LoadParticles","No File holding the Initial Particle Distribution has been specified");
      return kFALSE;
   }
   // Fetch Data object holding Initial Particles
   cout << "-------------------------------------------" << endl;
   cout << "Loading Particles from File: " << particlesFile << endl;
   TFile *f = TFile::Open(particlesFile,"read");
   if (!f || f->IsZombie()) {
      Error("Initialise","Cannot open file");
      return kFALSE;
   }
   TUCNData* importedData = 0;
   f->ls();
   f->GetObject("RunData;1",importedData);
   delete f;
   if (!importedData) {
      Error("LoadParticles","Could not find Data object named 'RunData;1' in %s", particlesFile.Data());
      return kFALSE;
   }
   // Check Configuration for whether we will simulate all the particles in the Tree
   if (!(configFile.GetBool("AllParticles",this->GetName()))) {
      // If NOT, then we need to extract the list of which particles are to be simulated
      /*
         ... Needs implementing
         Produce an array of integers representing the particleIDs we wish to include
      */
      this->SaveInitialParticle(importedData->GetInitialParticleState(0));
      delete importedData;
   } else {
      // If we want all particles, just set imported DataFile as the Run's data
      if (fData) delete fData;
      fData = importedData;
   }
   cout << fData->InitialParticles() << " particles have been loaded succesfully" << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
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