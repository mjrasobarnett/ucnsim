// Run
// Author: Matthew Raso-Barnett  08/10/2009

#include <iostream>
#include <sstream>
#include <cassert>
#include <stdexcept>

#include "Run.h"
#include "ConfigFile.h"
#include "RunConfig.h"
#include "FieldManager.h"
#include "Particle.h"
#include "Data.h"
#include "Clock.h"

#include "TFile.h"
#include "TRandom.h"

#include "ProgressBar.h"
#include "Units.h"

using std::cout;
using std::endl;
using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(Run)

//_____________________________________________________________________________
Run::Run()
        :TNamed(),
         fRunConfig()
{
// -- Default constructor
   Info("Run", "Default Constructor");
   fData = NULL;
   fExperiment = NULL;
} 

//_____________________________________________________________________________
Run::Run(const RunConfig& runConfig)
        :TNamed(),
         fRunConfig(runConfig)
{
// -- constructor
   Info("Run", "Constructor");
   this->SetName(this->GetRunConfig().RunName().c_str());
   fData = new Data();
   fExperiment = new Experiment();
}

//_____________________________________________________________________________
Run::Run(const Run& other)
        :TNamed(other),
         fRunConfig(other.fRunConfig),
         fData(other.fData),
         fExperiment(other.fExperiment)
{
// -- Copy Constructor
   Info("RunManager", "Copy Constructor");
}

//_____________________________________________________________________________
Run& Run::operator=(const Run& other)
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
Run::~Run()
{
// -- Destructor
   Info("Run", "Destructor");
   if(fData) delete fData;
   if(fExperiment) delete fExperiment;
}

//_____________________________________________________________________________
Bool_t Run::Initialise()
{
   // -- Initialise the Run. Build the Experiment Object which loads the Geometry and fields
   // -- into memory. Build the Data Object which loads the data files and initial particles.
   // -- Finally check Run parameters in the RunConfig object for further instructions.
   cout << "-------------------------------------------" << endl;
   cout << "Initialising: " << this->GetName() << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Setup the Clock
   Clock::Instance()->SetEndOfRun(this->GetRunConfig().RunTime());
   Clock::Instance()->SetMaxStepInterval(this->GetRunConfig().MaxStepTime());
   Clock::Instance()->SetSpinMeasureFreq(this->GetRunConfig().SpinMeasurementFreq());
   Clock::Instance()->SetFieldMeasureFreq(this->GetRunConfig().FieldMeasurementFreq());
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
Bool_t Run::Start()
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
      Particle* particle = this->GetData()->RetrieveParticle();
      // Reset the clock
      Clock::Instance()->Reset();
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
         ProgressBar::PrintProgress(index, totalParticles, 2);
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
   cout << "Number With Anomalous Behaviour: " << this->GetData()->AnomalousParticles() << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t Run::Finish()
{
   // -- Clean up the Run before exporting it to file
   // Check that Data 'checks out'
   if (this->GetData()->ChecksOut() == kFALSE) {
      Error("Finish", "Number of initial states doesn't match final states.");
      return kFALSE;
   }
   // Write the geometry out to file
   this->GetExperiment()->ExportGeometry(*this);
   // Delete the experiment
   if (fExperiment) delete fExperiment; fExperiment = NULL;
   // Delete the Data
   if (fData) delete fData; fData = NULL;
   return kTRUE;
}

