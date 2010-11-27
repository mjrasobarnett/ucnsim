// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <vector>
#include <stdexcept>
#include <sys/stat.h> // Required by Progress bar
#include <list>

#include "TUCNRun.h"
#include "TUCNConfigFile.h"
#include "TUCNRunConfig.h"
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
using std::vector;
using std::runtime_error;
using std::list;

//#define VERBOSE_MODE

ClassImp(TUCNRun)

//_____________________________________________________________________________
TUCNRun::TUCNRun()
        :TNamed(),
         fRunConfig()
{
// -- Default constructor
   Info("TUCNRun", "Default Constructor");
   fData = NULL;
   fExperiment = NULL;
} 

//_____________________________________________________________________________
TUCNRun::TUCNRun(const TUCNRunConfig& runConfig)
        :TNamed(),
         fRunConfig(runConfig)
{
// -- constructor
   Info("TUCNRun", "Constructor");
   this->SetName(this->GetRunConfig().RunName().c_str());
   fData = new TUCNData();
   fExperiment = new TUCNExperiment();
}

//_____________________________________________________________________________
TUCNRun::TUCNRun(const TUCNRun& other)
        :TNamed(other),
         fRunConfig(other.fRunConfig),
         fData(other.fData),
         fExperiment(other.fExperiment)
{
// -- Copy Constructor
   Info("TUCNRunManager", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNRun& TUCNRun::operator=(const TUCNRun& other)
{
// --assignment operator
   if(this!=&other) {
      TNamed::operator=(other);
      fRunConfig = other.fRunConfig;
      // deallocate previous memory if necessary
      if (fData != NULL) delete fData; fData = NULL;
      fData = other.fData;
      // deallocate previous memory if necessary
      if (fExperiment != NULL) delete fExperiment; fExperiment = NULL;
      fExperiment = other.fExperiment;
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
Bool_t TUCNRun::Initialise()
{
   // -- Initialise the Run. Build the Experiment Object which loads the Geometry and fields
   // -- into memory. Build the Data Object which loads the data files and initial particles.
   // -- Finally check Run parameters in the RunConfig object for further instructions.
   cout << "-------------------------------------------" << endl;
   cout << "Initialising: " << this->GetName() << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Build Geometry
   ///////////////////////////////////////////////////////////////////////////////////////
   if (this->GetExperiment()->Initialise(this->GetRunConfig()) == kFALSE) {
      Error("Initialise","Failed to Build the Experiment");
      return kFALSE;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Load Particles
   ///////////////////////////////////////////////////////////////////////////////////////
   if (this->GetData()->Initialise(this->GetRunConfig()) == kFALSE) {
      Error("Initialise","Failed to Load the Initial Particle Distribution from File");
      return kFALSE;
   }     
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Check Run Parameters
   // Run Time
   if (this->GetRunConfig().RunTime() <= 0.0) { 
      Error("Initialise","No RunTime set"); 
      return kFALSE; 
   }
   // Max Step Time
   if (this->GetRunConfig().MaxStepTime() <= 0.0) { 
      Error("Initialise","No maxsteptime set!"); 
      return kFALSE; 
   }
   // Determine if we will have any wall losses
   if (this->GetRunConfig().WallLossesOn() == kFALSE) { 
      Error("Initialise","Wall Losses Off has not been Implemented yet."); 
      return kFALSE; 
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   cout << "-------------------------------------------" << endl;
   cout << "Run successfully initialised" << endl;
   cout << "Particles: " << this->GetData()->InitialParticles() << endl;
   cout << "RunTime(s): " << this->GetRunConfig().RunTime() << endl;
   cout << "MaxStepTime(s): " << this->GetRunConfig().MaxStepTime() << endl;
   cout << "WallLosses: " << this->GetRunConfig().WallLossesOn() << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::Start()
{
// -- Propagate the particles stored in the Run's Data, specified by configFile
   cout << "-------------------------------------------" << endl;
   cout << "Starting Simulation of " << this->GetRunConfig().RunName() << endl;
   cout << "Total Particles: " << this->GetData()->InitialParticles() << endl;
   cout << "-------------------------------------------" << endl;
   Int_t totalParticles = this->GetData()->InitialParticles();
   ///////////////////////////////////////////////////////////////////////
   // Loop over all particles stored in InitialParticles Tree
   for (Int_t index = 0; index < totalParticles; index++) {
      // Get Particle from list
      TUCNParticle* particle = this->GetData()->RetrieveParticle();
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
      ///////////////////////////////////////////////////////////////////////
      // Attempt to Propagate track
      try {
         Bool_t propagated = particle->Propagate(this);
         if (!propagated) {
            Error("Start", "Propagation Failed to Begin.");
            return kFALSE;
         }
      } catch (...) {
         // Serious tracking errors (eg: particle cannot be located correctly) will be thrown
         #ifdef VERBOSE_MODE
            cout << "-------------------------------------------" << endl;
            Error("Start","Exception thrown by particle %i. Propagation Failed.", index);
            cout << "-------------------------------------------" << endl << endl;
         #endif
         // Add this particle to special tree for errorneous particles
         particle->SaveState(this);
         delete particle;
         continue;
      }
      ///////////////////////////////////////////////////////////////////////
      // Add Final Particle State to data tree
      particle->SaveState(this);
      // Print Progress Bar to Screen
      #ifndef VERBOSE_MODE
         PrintProgress(index, totalParticles);
      #endif
      // Delete Particle
      delete particle;
   }
   ///////////////////////////////////////////////////////////////////////
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
Bool_t TUCNRun::Finish()
{
// -- Clean up the Run before exporting it to file
   TString outputFile = this->GetRunConfig().OutputFileName();
   if (outputFile.IsNull() || outputFile.IsWhitespace()) {
      Warning("Finish","No output file has been specified. Using default filename");
      outputFile = "outputdata.root";
   }
   // -- Check that Data 'checks out'
   if (this->GetData()->ChecksOut() == kFALSE) {
      Error("Finish", "Number of initial states doesn't match final states.");
      return kFALSE;
   }
   // -- Check we have a .root file
   if (!(outputFile.Contains(".root"))) {
      Warning("Export", "OutputFile is not a ROOT filename. Using default filename");
      outputFile = "outputdata.root";
   }
   cout << "-------------------------------------------" << endl;
   cout << "Writing " << this->GetName() << " out to File: " << outputFile << endl;
   // -- Delete the experiment
   if (fExperiment) delete fExperiment; fExperiment = NULL;
   // -- Delete the Data
   if (fData) delete fData; fData = NULL;
   // -- Write out run to the top level directory
   TFile *file = TFile::Open(outputFile,"UPDATE");
   if (!file || file->IsZombie()) {
      Error("Export","Cannot open file %s",outputFile.Data());
      return kFALSE;
   }
   this->Write(this->GetName());
   cout << this->GetName() << " was successfully written to file" << endl;
   file->ls();
   cout << "-------------------------------------------" << endl;
   // -- Clean up
   file->Close();
   delete file;
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
