// Run
// Author: Matthew Raso-Barnett  08/10/2009

#include <iostream>
#include <sstream>
#include <cassert>
#include <stdexcept>

#include "Run.h"
#include "ConfigFile.h"
#include "FieldManager.h"
#include "Particle.h"
#include "Data.h"
#include "Clock.h"

#include "TFile.h"
#include "TRandom.h"

#include "Algorithms.h"
#include "Units.h"

using namespace std;

//#define VERBOSE_MODE

ClassImp(Run)

//_____________________________________________________________________________
Run::Run()
        :TNamed(),
         fRunConfig(),
         fData()
{
// -- Default constructor
   Info("Run", "Default Constructor");
   fExperiment = NULL;
} 

//_____________________________________________________________________________
Run::Run(const RunConfig& runConfig)
        :TNamed(runConfig.RunName().c_str(), "The Run"),
         fRunConfig(runConfig),
         fData()
{
// -- constructor
   Info("Run", "Constructor");
   this->SetName(this->GetRunConfig().RunName().c_str());
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
   if (Clock::Instance()->Initialise(this->GetRunConfig()) == kFALSE) {
      Error("Initialise","Failed to initialise the Clock");
      return kFALSE;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Build Geometry
   ///////////////////////////////////////////////////////////////////////////////////////
   if (fExperiment->Initialise(this->GetRunConfig()) == kFALSE) {
      Error("Initialise","Failed to Build the Experiment");
      return kFALSE;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Initialise the DataFile and load initial particles
   ///////////////////////////////////////////////////////////////////////////////////////
   if (this->GetData().Initialise(this->GetRunConfig()) == kFALSE) {
      Error("Initialise","Failed to Load the Initial Particle Distribution from File");
      return kFALSE;
   }
   // -- Create any observers selected by user
   this->GetData().CreateObservers(this->GetRunConfig(), *fExperiment);
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
   cout << "-------------------------------------------" << endl;
   return kTRUE;
}


//_____________________________________________________________________________
Bool_t Run::Start()
{
   // -- Initialise the Random number generator to a TRandom3a
   gRandom = new TRandom3a();
   TRandom3a* rndGenerator = dynamic_cast<TRandom3a*>(gRandom);
   // -- Propagate the particles stored in the Run's Data, specified by configFile
   vector<int> selectedParticles = fData.GetListOfParticlesToLoad(fRunConfig);
   size_t totalParticles = selectedParticles.size();
   cout << "-------------------------------------------" << endl;
   cout << "Starting Simulation of " << this->GetRunConfig().RunName() << endl;
   cout << "Particles to propagate: " << totalParticles << endl;
   cout << "RunTime(s): " << this->GetRunConfig().RunTime() << endl;
   cout << "MaxStepTime(s): " << this->GetRunConfig().MaxStepTime() << endl;
   cout << "WallLosses: " << this->GetRunConfig().WallLossesOn() << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////
   // Loop over all particles stored in InitialParticles Tree
   vector<int>::const_iterator indexIter;
   for (indexIter = selectedParticles.begin(); indexIter != selectedParticles.end(); indexIter++) {
      const int particleNumber = indexIter - selectedParticles.begin();
      // Get Particle from list
      Particle* particle = fData.RetrieveParticle(*indexIter);
      if (particle == NULL) {
         Error("Start","Failed to retrieve particle from Data");
         return false;
      }
      // If the particle has stored a previous random generator state, load it
      const TRandom3State* previousRndState = particle->GetRandomGeneratorState();
      if (previousRndState != NULL) {rndGenerator->SetState(*previousRndState);}
      // Hold a copy of the Random Generator's state before particle's propagation
      TRandom3State initialRndState = rndGenerator->GetState();
      // Hold a copy of Particle's initial state before propagating
      Particle initialParticle(*particle);
      // Register Observers with Particle
      fData.RegisterObservers(particle);
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
         Error("Start","Particle %i has failed to propagate properly.", particleNumber);
         // Store the initial random generator's state for this particle
         initialParticle.SaveRandomGeneratorState(initialRndState);
      }
      ///////////////////////////////////////////////////////////////////////
      // Add Initial Particle State to data tree
      fData.SaveInitialParticle(&initialParticle);
      ///////////////////////////////////////////////////////////////////////
      // Add Final Particle State to data tree
      particle->SaveState(fData);
      ///////////////////////////////////////////////////////////////////////
      // Reset the clock
      Clock::Instance()->Reset();
      // Reset the observers
      this->GetData().ResetObservers();
      ///////////////////////////////////////////////////////////////////////
      // Print Progress Bar to Screen
      #ifndef VERBOSE_MODE
         Algorithms::ProgressBar::PrintProgress(particleNumber, totalParticles, 2);
      #endif
   }
   ///////////////////////////////////////////////////////////////////////
   cout << "-------------------------------------------" << endl;
   cout << "Propagation Results: " << endl;
   cout << "Total Particles: " << this->GetData().FinalParticles() << endl;
   cout << "Number Still Propagating: " << this->GetData().PropagatingParticles() << endl;
   cout << "Number Detected: " << this->GetData().DetectedParticles() << endl;
   cout << "Number Absorbed by Boundary: " << this->GetData().AbsorbedParticles() << endl;
   cout << "Number Decayed: " << this->GetData().DecayedParticles() << endl;
   cout << "Number Lost To Outer Geometry: " << this->GetData().LostParticles() << endl;
   cout << "Number With Anomalous Behaviour: " << this->GetData().AnomalousParticles() << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t Run::Finish()
{
   // -- Clean up the Run before exporting it to file
   // Check that Data 'checks out'
   if (this->GetData().ChecksOut() == kFALSE) {
      Error("Finish", "Number of initial states doesn't match final states.");
      return kFALSE;
   }
   // Write the Particle Tree to file
   fData.WriteObjectToFile(&fRunConfig);
   // Write the geometry out to file
   fExperiment->Export(*this);
   // Write the Particle Tree to file
   fData.Export();
   return kTRUE;
}

