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
   // Don't allocate memory here as per ROOT guidelines for reading objects from ROOT files
   fData = NULL;
   fExperiment = NULL;
} 

//_____________________________________________________________________________
TUCNRun::TUCNRun(const TUCNRunConfig& runConfig)
        :TNamed(),
         fRunConfig(runConfig)
{
// -- Default constructor
   Info("TUCNRun", "Constructor");
   // Don't allocate memory here as per ROOT guidelines for reading objects from ROOT files
   fData = NULL;
   fExperiment = NULL;
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
   this->SetName(this->GetRunConfig().RunName().c_str());
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Allocate Memory
   ///////////////////////////////////////////////////////////////////////////////////////
   if (fData != NULL) delete fData; fData = NULL;
   fData = new TUCNData("UCNData");
   if (fExperiment != NULL) delete fExperiment; fExperiment = NULL;
   fExperiment = new TUCNExperiment();
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
   if (this->LoadParticles(this->GetRunConfig()) == kFALSE) {
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
   if (this->GetRunConfig().MaxStepTime() == 0.0) { 
      Error("Initialise","No maxsteptime set!"); 
      return kFALSE; 
   }
   // Determine if we will have any wall losses
   if (this->GetRunConfig().WallLossesOn() == kFALSE) { 
      Error("Initialise","Wall Losses Off has not been Implemented yet."); 
      return kFALSE; 
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Initialise Observers
   if (this->InitialiseObservers(this->GetRunConfig()) == kFALSE) {
      Error("Initialise","Failed to Initialise Observers");
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
   cout << "Number Propagating: " << this->GetData()->PropagatingParticles() << endl;
   cout << "Number Detected: " << this->GetData()->DetectedParticles() << endl;
   cout << "Number Absorbed by Boundary: " << this->GetData()->AbsorbedParticles() << endl;
   cout << "Number Decayed: " << this->GetData()->DecayedParticles() << endl;
   cout << "Number Lost To Outer Geometry: " << this->GetData()->LostParticles() << endl;
   cout << "Number With Anomalous Behaviour: " << this->GetData()->BadParticles() << endl;
   cout << "-------------------------------------------" << endl;
   Int_t totalParticles = this->GetData()->InitialParticles();
   ///////////////////////////////////////////////////////////////////////
   // Loop over all particles stored in InitialParticles Tree
   for (Int_t index = 0; index < totalParticles; index++) {
      // Get Particle from list
      TUCNParticle* particle = dynamic_cast<TUCNParticle*>(this->GetInitialParticle(index));
      // Register Observers with particle
      this->GetData()->RegisterObservers(particle);
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
         this->GetData()->AddBadParticleState(particle);
         delete particle;
         continue;
      }
      ///////////////////////////////////////////////////////////////////////
      // Add Final Particle State to data tree
      if (!(particle->SaveState(this))) {
         Error("Start","Particle has failed to save its final state correctly");
         return kFALSE;
      }
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
// -- Export this run to file
   TString outputFile = this->GetRunConfig().OutputFileName();
   if (outputFile.IsNull() || outputFile.IsWhitespace()) {
      Warning("Finish","No output file has been specified. Using default filename");
      outputFile = "outputdata.root";
   }
   // -- Check that Data 'checks out'
   if (this->GetData()->ChecksOut() == kFALSE) {
      Error("Export", "TUCNData doesn't check out. Cannot Export Run to file.");
      return kFALSE;
   }
   // -- Check we have a .root file
   if (!(outputFile.Contains(".root"))) {
      Warning("Export", "OutputFile is not a ROOT filename. Using default filename");
      outputFile = "outputdata.root";
   }
   cout << "-------------------------------------------" << endl;
   cout << "Writing " << this->GetName() << " out to File: " << outputFile << endl;
   // -- Clear the experiment
   this->GetExperiment()->ClearManager();
   // -- Write out
   TFile *f = TFile::Open(outputFile,"recreate");
   if (!f || f->IsZombie()) {
      Error("Export","Cannot open file %s",outputFile.Data());
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
TUCNParticle* TUCNRun::GetInitialParticle(Int_t index)
{
   // -- Retrieve the requested initial particle state from the Data
   return this->GetData()->GetInitialParticleState(index);
}

//_____________________________________________________________________________
Bool_t TUCNRun::LoadParticles(const TUCNRunConfig& runConfig)
{
// Fetch initial particles tree from file specified in Config File
   ///////////////////////////////////////////////////////////////////////
   // Get name of file holding the data from ConfigFile 
   TString particlesFile = runConfig.InputFileName();
   if (particlesFile == "") { 
      Error("LoadParticles","No File holding the initial particle data has been specified");
      return kFALSE;
   }
   ///////////////////////////////////////////////////////////////////////
   // Fetch Data object holding Initial Particles
   TString inputDataName = runConfig.InputRunName();
   if (inputDataName == "") {
      Error("LoadParticles","No Run holding initial particle data has been specified");
      return kFALSE;
   }
   cout << "-------------------------------------------" << endl;
   cout << "Loading Particles from File: " << particlesFile << endl;
   TFile *f = TFile::Open(particlesFile,"read");
   if (!f || f->IsZombie()) {
      Error("Initialise","Cannot open file");
      return kFALSE;
   }
   TUCNRun* importedRun = 0;
   f->ls();
   f->GetObject(inputDataName,importedRun);
   delete f;
   if (!importedRun) {
      Error("LoadParticles","Could not find Run object named: %s in %s", inputDataName.Data(), particlesFile.Data());
      return kFALSE;
   }
   TUCNData* importedData = importedRun->GetData();
   cout << "Located Data: " << importedData->GetName();
   cout << " inside " << importedRun->GetName() << endl;
   cout << "Determining which particles to load..." << endl;
   
   ///////////////////////////////////////////////////////////////////////
   // Check Config for which particle Tree we wish to load as our initial particles here
   TString whichParticles = runConfig.ParticlesToLoad();
   if (particlesFile == "") { 
      Warning("LoadParticles","No Particle Tree specified in ConfigFile, 'WhichParticles'. Taking 'Initial' Tree by default.");
      whichParticles = "Initial";
   }
   // Fetch the specified Tree
   TTree* inputTree = importedData->FetchTree(whichParticles);
   if (inputTree == NULL) {
      Error("LoadParticles","Could not find Tree by searching for %s", whichParticles.Data());
      return kFALSE;
   }
   cout << "Loading particles stored in Tree: " << inputTree->GetName() << endl;
   // Define where we will store the particleID's from this Tree
   vector<Int_t> particleIDs;
   ///////////////////////////////////////////////////////////////////////
   // - Check Config for whether we will take all of the particles in the Tree specified
   Bool_t allParticles = runConfig.LoadAllParticles();
   if (allParticles == kFALSE) {
      // Not using all particles -> We need to read in the particleIDs stated
      Error("LoadParticles","Not using All Particles. Not Implemented Yet");
      return kFALSE;
   } else {
      // Using all particles in Tree.
      for (Int_t i=0;i<inputTree->GetEntriesFast();i++) {
         // Fetch all particles
         TUCNParticle* particle = importedData->GetParticleState(inputTree, i);
         // Add Particle ID to the list
         particleIDs.push_back(particle->Id());
      }
   }
   
   ///////////////////////////////////////////////////////////////////////
   // - Check Config for whether we will re-start the particles specified above from their initial
   // - state or not. This will always be done, except for those particles in the Propagating Tree,
   // - where we can either re-start, or continue to propagate from their final state
   Bool_t fromBeginning = runConfig.RestartFromBeginning();
   if (fromBeginning == kFALSE && TString(inputTree->GetName()) == "Propagating") {
      // - In this case, we are continuing to propagate the particles from their states in the
      // - 'propagating' tree, so we will store this tree. 
      // Loop over all entries in the Propagating Tree
      cout << "Loading still Propagating particles into this Run to continue their motion" << endl;
      cout << "Initial particles will be initialised to that of the previous final states" << endl;
      for (Int_t i = 0; i < inputTree->GetEntriesFast(); i++) {
         // Get the particle for each entry in Tree
         TUCNParticle* particle = importedData->GetPropagatingParticleState(i);
         // Check whether this particle's ID matches any in our particleID vector
         for (UInt_t j = 0; j < particleIDs.size(); j++) {
            if (particle->Id() == particleIDs[j]) {
               // If so, save that particle
               this->SaveInitialParticle(particle);
            }
         }
      }
   } else {
      // - Otherwise, we want to fetch the initialStates of the particles specified in particleIDs
      // For all entries specified in our particleID vector, fetch and save
      // the particle corresponding to that
      cout << "Storing Loaded particles as the Initial Particles for this Run..." << endl;
      for (UInt_t i = 0; i < particleIDs.size(); i++) {
         TUCNParticle* particle = importedData->GetInitialParticleState(particleIDs[i]);
         this->SaveInitialParticle(particle);
      }
   }
   // - We should now have a Tree stored in Run, that has its InitialParticles Tree filled
   // - with whatever particles were specified in the configuration file.
   cout << fData->InitialParticles() << " particles have been loaded succesfully" << endl;
   cout << "-------------------------------------------" << endl;
   delete importedRun;
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::InitialiseObservers(const TUCNRunConfig& runConfig)
{
   // Check Run configuration for which properties are to be monitored with Observers 
   cout << "-------------------------------------------" << endl;
   cout << "Setting up Observers" << endl;
   if (runConfig.ObservePolarisation() == kTRUE) {
      // Create an observer to track UCN Spin polarisation
      TUCNObserver* obs = new TUCNSpinObserver(runConfig);
      // Add observer to the list
      this->GetData()->AddObserver(obs);
      obs = NULL;
   }
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
