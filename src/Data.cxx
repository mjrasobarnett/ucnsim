// UCNData.cpp
// Author: Simon JM Peeters
// Created July 2009

#include "Data.h"

#include <iostream>
#include <sstream>
#include <locale>
#include <cassert>
#include <stdexcept>

#include "TKey.h"

#include "DataFileHierarchy.h"
#include "ProgressBar.h"

using namespace std;

ClassImp(Data)

//_____________________________________________________________________________
Data::Data() 
         :TNamed(),
          fObservers()
{
   // -- Default Constructor
   Info("Data","Default Constructor");
   // -- Create the Trees
   fOutputFile = NULL;
   fInitialStatesFolder = NULL;
   fFinalStatesFolder = NULL;
   fCurrentParticleDir = NULL;
}

//_____________________________________________________________________________
Data::Data(const Data& other)
         :TNamed(other), 
          fOutputFile(other.fOutputFile),
          fInitialStatesFolder(other.fInitialStatesFolder),
          fFinalStatesFolder(other.fFinalStatesFolder),
          fCurrentParticleDir(other.fCurrentParticleDir),
          fObservers(other.fObservers)
{
   // Copy Constructor
   Info("Data","Copy Constructor");
}

//_____________________________________________________________________________
Data& Data::operator=(const Data& other)
{
// --assignment operator
   Info("Data","Assignment");
   if(this!=&other) {
      TNamed::operator=(other);
      // Clean up File before assignment
      if (fOutputFile) {
         fOutputFile->Close();
         delete fOutputFile;
         fOutputFile = NULL;
         fInitialStatesFolder = NULL;
         fFinalStatesFolder = NULL;
         fCurrentParticleDir = NULL;
      }
      fOutputFile = other.fOutputFile;
      fInitialStatesFolder = other.fInitialStatesFolder;
      fFinalStatesFolder = other.fFinalStatesFolder;
      fCurrentParticleDir = other.fCurrentParticleDir;
      // Clear list of observers before assignment
      PurgeObservers();
      fObservers = other.fObservers;
   }
   return *this;
}

//_____________________________________________________________________________
Data::~Data()
{
   // -- Destructor
   Info("Data","Destructor");
   if (fOutputFile) fOutputFile->Close(); delete fOutputFile;
   fOutputFile = NULL;
   fInitialStatesFolder = NULL;
   fFinalStatesFolder = NULL;
   fCurrentParticleDir = NULL;
   PurgeObservers();
}

//_____________________________________________________________________________
Bool_t Data::Initialise(const InitialConfig& initialConfig)
{
   // -- Take the initialConfig, open the output file and necessary directories to store
   // -- particles that being generated.
   // Fetch the OutputFile name
   const string outputFileName = initialConfig.OutputFileName();
   if (outputFileName == "") { 
      Error("Initialise","No Output file path specified in run configuration");
      return kFALSE;
   }
   // Open and store pointer to File
   TFile *file = TFile::Open(outputFileName.c_str(), "recreate");
   if (!file || file->IsZombie()) {
      Error("Initialise","Unable to open output file");
      return kFALSE;
   }
   fOutputFile = file;
   // Set up basic folder structure
   // ___.root:/particles/initialstates  ___.root:/particles/finalstates
   fOutputFile->cd();
   TDirectory* topDir = gDirectory;
   // Need to create particles directory structure
   TDirectory* particlesDir = topDir->mkdir(Folders::particles.c_str());
   // Create the initial and final states folders
   fInitialStatesFolder = particlesDir->mkdir(Folders::initialstates.c_str());
   fFinalStatesFolder = particlesDir->mkdir(Folders::finalstates.c_str());
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t Data::Initialise(const RunConfig& runConfig)
{
   // -- Take the runConfig, open the output files and create directories to store
   // -- both initial and final states of the particles. Finally, load all particles specified
   // -- by the run config into the initial states folder, ready for propagation
   // Fetch the Output File name
   const string outputFileName = runConfig.OutputFileName();
   if (outputFileName == "") { 
      Error("Initialise","No Output file path specified in run configuration");
      return kFALSE;
   }
   // Open and store pointer tooutput File
   TFile *outputfile = TFile::Open(outputFileName.c_str(), "recreate");
   if (!outputfile || outputfile->IsZombie()) {
      Error("Initialise","Unable to open output file");
      return kFALSE;
   }
   fOutputFile = outputfile;
   // Set up basic folder structure
   // ___.root:/particles/initialstates  ___.root:/particles/finalstates
   fOutputFile->cd();
   TDirectory* topDir = gDirectory;
   // Need to create particles directory structure
   TDirectory* particlesDir = topDir->mkdir(Folders::particles.c_str(),"");
   // Create the initial and final states folders
   fInitialStatesFolder = particlesDir->mkdir(Folders::initialstates.c_str());
   fFinalStatesFolder = particlesDir->mkdir(Folders::finalstates.c_str());
   // Create the typical subfolders of final states
   fFinalStatesFolder->mkdir(Folders::propagating.c_str());
   fFinalStatesFolder->mkdir(Folders::absorbed.c_str());
   fFinalStatesFolder->mkdir(Folders::detected.c_str());
   fFinalStatesFolder->mkdir(Folders::decayed.c_str());
   fFinalStatesFolder->mkdir(Folders::lost.c_str());
   fFinalStatesFolder->mkdir(Folders::anomalous.c_str());
   // Load initial particles into output file
   if (this->LoadParticles(runConfig) == kFALSE) {
      Error("Initialise","Cannot Load Particles");
      return kFALSE;
   }
   // Initialise any observers selected by user
   this->InitialiseObservers(runConfig);
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t Data::LoadParticles(const RunConfig& runConfig)
{
   // -- Locate particles designated as 'initial' in the run config. Copy all of these
   // -- particles into the initialstates folder inside the output file.
   cout << "-------------------------------------------" << endl;
   cout << "Loading the Particles" << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////
   // -- Get name of file holding the data from runconfig 
   const string inputFileName = runConfig.InputFileName();
   if (inputFileName == "") { 
      Error("LoadParticles","No Input file path specified in run configuration");
      return kFALSE;
   }
   TFile *inputfile = TFile::Open(inputFileName.c_str(), "read");
   if (!inputfile || inputfile->IsZombie()) {
      Error("LoadParticles","Unable to open input file");
      return kFALSE;
   }
   // Determine which particle states to take as the initial states for current run
   cout << "Determining which particles to load..." << endl;
   inputfile->cd();
   TDirectory* const topDir = gDirectory;
   if (topDir->cd(Folders::particles.c_str()) == kFALSE) {
      Error("LoadParticles","No folder named: %s, found in input file",Folders::particles.c_str());
      return kFALSE;
   }
   TDirectory * const partDir = gDirectory;
   ///////////////////////////////////////////////////////////////////////
   // -- Check Config for which particle states we wish to load as our input particles
   string whichParticles = runConfig.ParticlesToLoad();
   // Convert to lowercase.
   locale loc;
   for (size_t i=0; i < whichParticles.length(); ++i) {
      whichParticles[i] = tolower(whichParticles[i],loc);
   }
   // Navigate to the folder containing the input particle states 
   if (whichParticles != Folders::initialstates) {
      partDir->cd(Folders::finalstates.c_str());
      TDirectory* finalStatesDir = gDirectory;
      Bool_t correctFolder = finalStatesDir->cd(whichParticles.c_str());
      // check that we ended up in the correct folder
      if (correctFolder == kFALSE) {
         Error("LoadParticles","Cannot find folder: ", whichParticles.c_str());
         return kFALSE;
      }
   } else {
      Bool_t correctFolder = partDir->cd(Folders::initialstates.c_str());
      // check that we ended up in the correct folder
      if (correctFolder == kFALSE) {
         Error("LoadParticles","Cannot find folder: ", Folders::initialstates.c_str());
         return kFALSE;
      }
   }
   TDirectory * const sourceDir = gDirectory;
   TDirectory * const outputDir = fInitialStatesFolder; 
   ///////////////////////////////////////////////////////////////////////
   // - Check Config for whether we will re-start the particles specified above from their initial
   // - states. This will only be done for particles that are not in their initial states already.
   Bool_t fromBeginning = runConfig.RestartFromBeginning();
   if (fromBeginning == kTRUE && sourceDir->GetName() != Folders::initialstates) {
      cout << "Resetting particles in the state: " << sourceDir->GetName();
      cout << ", to their initial state as the input to current Run " << endl;
      // Loop on all entries of source directory, which will be a set of folders containing
      // the particles in this state
      TKey *key;
      TIter nextkey(sourceDir->GetListOfKeys());
      while ((key = static_cast<TKey*>(nextkey.Next()))) {
         const char *classname = key->GetClassName();
         TClass *cl = gROOT->GetClass(classname);
         if (!cl) continue;
         if (cl->InheritsFrom("TDirectory")) {
            // For each particle-folder in the current directory, find its corresponding
            // initialstates folder in the input file, and make a copy of it in the output file
            partDir->cd(Folders::initialstates.c_str());
            TDirectory * const initialDir = gDirectory;
            this->CopyDirectory(initialDir, outputDir);
         }
      }
   } else {
      // Otherwise we just propagate the particles from where they left off
      cout << "Loading particles in the state: " << sourceDir->GetName();
      cout << ", as the input to current Run, to continue their propagation" << endl;
      // Copy particles from the source directory into the initialstates directory of the run
      this->CopyDirectoryContents(sourceDir, outputDir);
   }
   // Clean up
   inputfile->Close();
   if (inputfile) delete inputfile; inputfile = NULL;
   cout << "-------------------------------------------" << endl;
   cout << this->InitialParticles() << " particles have been loaded succesfully" << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
}

//_____________________________________________________________________________
void Data::CopyDirectory(TDirectory * const sourceDir, TDirectory * const outputDir) {
   // Copy the source directory and all its subdirectories into the supplied output directory
   // as a new subdirectory  
   TDirectory* copiedDir = outputDir->mkdir(sourceDir->GetName());
   copiedDir->cd();
   // Loop on all entries of this directory
   TKey *key;
   TIter nextkey(sourceDir->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TDirectory")) {
         // Copy subdirectory to a new subdirectoy in copiedDir
         sourceDir->cd(key->GetName());
         TDirectory *subdir = gDirectory;
         copiedDir->cd();
         CopyDirectory(subdir, copiedDir);
         copiedDir->cd();
      } else {
         // Copy Object
         sourceDir->cd();
         TObject *obj = key->ReadObj();
         copiedDir->cd();
         obj->Write();
         delete obj;
     }
  }
  copiedDir->SaveSelf(kTRUE);
  outputDir->cd();
}

//_____________________________________________________________________________
void Data::CopyDirectoryContents(TDirectory * const sourceDir, TDirectory * const outputDir) {
   // -- Similar to CopyDirectory but here we only copy all the objects and subdirs of the source
   // -- directory into the supplied output directory, not the source directory itself
   outputDir->cd();
   // Loop on all entries of this directory
   TKey *key;
   TIter nextkey(sourceDir->GetListOfKeys());
   Int_t counter = 0;
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TDirectory")) {
         // Copy subdirectory to a new subdirectoy in outputDir
         sourceDir->cd(key->GetName());
         TDirectory *subdir = gDirectory;
         outputDir->cd();
         TDirectory* outputSubDir = outputDir->mkdir(subdir->GetName());
         CopyDirectoryContents(subdir, outputSubDir);
         outputDir->cd();
      } else {
         // Copy Object
         sourceDir->cd();
         TObject *obj = key->ReadObj();
         outputDir->cd();
         obj->Write();
         delete obj;
     }
     ++counter;
     ProgressBar::PrintProgress(counter,sourceDir->GetNkeys(),1);
  }
  outputDir->SaveSelf(kTRUE);
  outputDir->cd();
}

//_____________________________________________________________________________
void Data::PurgeObservers()
{
   // -- Delete all observers held
   if (fObservers.empty() == kFALSE) {
      vector<Observer*>::iterator it;
      for(it = fObservers.begin(); it != fObservers.end(); ++it) {
         delete *it;
         *it = 0;
      }
   }
}

//_____________________________________________________________________________
void Data::InitialiseObservers(const RunConfig& runConfig)
{
   // -- Check Run configuration for which properties are to be monitored with Observers 
   cout << "-------------------------------------------" << endl;
   cout << "Setting up Observers" << endl;
   cout << "ObservePolarisation: " << runConfig.ObservePolarisation() << endl;
   cout << "ObserveBounces: " << runConfig.ObserveBounces() << endl;
   cout << "ObserveTracks: " << runConfig.ObserveTracks() << endl;
   if (runConfig.ObservePolarisation() == kTRUE) {
      // Create an observer to track UCN Spin polarisation
      Observer* obs = new SpinObserver(runConfig);
      // Add observer to the list
      this->AddObserver(obs);
   }
   if (runConfig.ObserveBounces() == kTRUE) {
      // Create an observer to track UCN Bounces
      Observer* obs = new BounceObserver(runConfig);
      // Add observer to the list
      this->AddObserver(obs);
   }
   if (runConfig.ObserveTracks() == kTRUE) {
      // Create an observer to track UCN path
      Observer* obs = new TrackObserver(runConfig);
      // Add observer to the list
      this->AddObserver(obs);
   }
}

//_____________________________________________________________________________
void Data::AddObserver(Observer* observer)
{
   // -- Add observer to internal list
   fObservers.push_back(observer);
}

//_____________________________________________________________________________
void Data::RegisterObservers(Particle* particle)
{
   // -- Register all observers in list with the particle
   vector<Observer*>::iterator obsIter;
   for (obsIter = fObservers.begin(); obsIter != fObservers.end(); obsIter++) {
      (*obsIter)->RegisterInterest(*particle);
   }
}

//_____________________________________________________________________________
Bool_t Data::SaveParticle(Particle* particle, const std::string& state)
{
   // -- Navigate to folder "state" in the particles/finalstates directory of the output data
   // -- file. If folder "state" doesn't yet exist, then create it and set as the current directory
   if (state == Folders::initialstates) {
      Bool_t foundFolder = fInitialStatesFolder->cd();
      if (foundFolder == kFALSE) {
         Error("SaveParticle","Unable to open folder: %s", state.c_str());
         return kFALSE;
      }
   } else {
      Bool_t foundFolder = fFinalStatesFolder->cd(state.c_str());
      if (foundFolder == kFALSE) {
         // If we failed to find a folder in the final states directory with name 'state'
         // create a new folder for this type of particle
         fFinalStatesFolder->mkdir(state.c_str());
      }
   }
   // We should now be in the correct folder for our particle
   TDirectory* stateDir = gDirectory;
   // Make a new folder for particle named by its ID.
   ostringstream s_ID;
   s_ID << particle->Id();
   TDirectory* particleDir = stateDir->mkdir(s_ID.str().c_str());
   // Write particle to folder
   particle->WriteToFile(particleDir);
   return kTRUE;
}

//_____________________________________________________________________________
Particle* const Data::RetrieveParticle()
{
   // -- Retrieve the next particle from fInitialStatesFolder. The current particle
   // -- is referenced by the the TIter fCurrentParticleDir.
   // Cd into the folder of initial particle states
   fInitialStatesFolder->cd();
   // If there is no iterator set on the particles already, do so.
   if (fCurrentParticleDir == NULL) {
      fCurrentParticleDir = new TIter(fInitialStatesFolder->GetListOfKeys());
   }
   // Get the next particle folder
   TKey* nextDir = static_cast<TKey*>(fCurrentParticleDir->Next());
   if (nextDir == NULL) {
      // Reached end of the directory. No more particles to load
      return NULL;
   }
   const char *dir_classname = nextDir->GetClassName();
   TClass *dir_cl = gROOT->GetClass(dir_classname);
   assert(dir_cl->InheritsFrom("TDirectory"));
   // Cd into the next particle's folder
   fInitialStatesFolder->cd(nextDir->GetName());
   TDirectory *nextParticleDir = gDirectory;
   // Search Directory for the particle
   Particle* nextParticle = this->LocateParticle(nextParticleDir);
   // Search Directory for any observables previously recorded to be continued
   vector<Observer*>::iterator obsIter;
   for (obsIter = fObservers.begin(); obsIter != fObservers.end(); obsIter++) {
      (*obsIter)->RegisterInterest(*nextParticle);
      (*obsIter)->LoadExistingObservables(nextParticleDir);      
   }
   return nextParticle;
}

//_____________________________________________________________________________
Particle* Data::LocateParticle(TDirectory * const particleDir)
{
   // -- Loop on all entries of this directory looking for a particle
   Particle* particle = NULL;
   TKey *key;
   TIter nextkey(particleDir->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("Particle")) {
         particle = dynamic_cast<Particle*>(key->ReadObj());
         break;
      }
   }
   return particle;
}

//_____________________________________________________________________________
Bool_t Data::ChecksOut() const
{
// -- Perform a check out routine, to ensure data is consistent
   // Sum initial and final states
   const Int_t initialParticles = this->InitialParticles();
   const Int_t finalParticles = this->FinalParticles();
   // Check that initial and final states match
   if (initialParticles != finalParticles) {
      Error("ChecksOut", "Initial particle states: %i, is not equal to final states: %i", initialParticles, finalParticles);
      return kFALSE;
   } 
   Info("ChecksOut","Initial particle states: %i. Number of Final States: %i.", initialParticles, finalParticles);
   return kTRUE;
}


//_____________________________________________________________________________
Int_t Data::InitialParticles() const
{
   // -- Count the number of particle states in the Initial Particles folder
   return fInitialStatesFolder->GetNkeys();
}

//_____________________________________________________________________________
Int_t Data::PropagatingParticles() const
{
   if (fFinalStatesFolder->cd(Folders::propagating.c_str()) == kFALSE) {
      return 0;
   } else {
      return gDirectory->GetNkeys();
   }
}

//_____________________________________________________________________________
Int_t Data::DetectedParticles() const
{
   if (fFinalStatesFolder->cd(Folders::detected.c_str()) == kFALSE) {
      return 0;
   } else {
      return gDirectory->GetNkeys();
   }
}

//_____________________________________________________________________________
Int_t Data::DecayedParticles() const
{
   if (fFinalStatesFolder->cd(Folders::decayed.c_str()) == kFALSE) {
      return 0;
   } else {
      return gDirectory->GetNkeys();
   }
}

//_____________________________________________________________________________
Int_t Data::AbsorbedParticles() const
{
   if (fFinalStatesFolder->cd(Folders::absorbed.c_str()) == kFALSE) {
      return 0;
   } else {
      return gDirectory->GetNkeys();
   }
}

//_____________________________________________________________________________
Int_t Data::LostParticles() const
{
   if (fFinalStatesFolder->cd(Folders::lost.c_str()) == kFALSE) {
      return 0;
   } else {
      return gDirectory->GetNkeys();
   }
}

//_____________________________________________________________________________
Int_t Data::AnomalousParticles() const
{
   if (fFinalStatesFolder->cd(Folders::anomalous.c_str()) == kFALSE) {
      return 0;
   } else {
      return gDirectory->GetNkeys();
   }
}

//_____________________________________________________________________________
Int_t Data::FinalParticles() const
{
   Int_t final = PropagatingParticles() + DetectedParticles() + AbsorbedParticles() + DecayedParticles() + LostParticles() + AnomalousParticles();
   return final;
}