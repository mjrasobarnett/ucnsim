#include "Data.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <iterator>

#include "TList.h"
#include "TKey.h"
#include "TClass.h"
#include "TGeoManager.h"

#include "Experiment.h"
#include "MagFieldArray.h"
#include "ElecFieldArray.h"
#include "DataFileHierarchy.h"
#include "Algorithms.h"

#include <boost/algorithm/string.hpp>

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
   fParticleStatesFolder = NULL;
   fInitialStatesFolder = NULL;
   fCurrentParticleDir = NULL;
}

//_____________________________________________________________________________
Data::Data(const Data& other)
         :TNamed(other), 
          fOutputFile(other.fOutputFile),
          fParticleStatesFolder(other.fParticleStatesFolder),
          fInitialStatesFolder(other.fInitialStatesFolder),
          fCurrentParticleDir(other.fCurrentParticleDir),
          fObservers(other.fObservers)
{
   // Copy Constructor
   Info("Data","Copy Constructor");
}

//_____________________________________________________________________________
Data::~Data()
{
   // -- Destructor
   Info("Data","Destructor");
   if (fOutputFile) fOutputFile->Close(); delete fOutputFile;
   fOutputFile = NULL;
   fParticleStatesFolder = NULL;
   fInitialStatesFolder = NULL;
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
   fOutputFile->cd();
   TDirectory* topDir = gDirectory;
   // Need to create particles directory structure
   fParticleStatesFolder = topDir->mkdir(Folders::particles.c_str());
   // Create the initial states folder
   fInitialStatesFolder = fParticleStatesFolder->mkdir(Folders::initial.c_str());
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
   // Open and store pointer to output File
   TFile *outputfile = TFile::Open(outputFileName.c_str(), "recreate");
   if (!outputfile || outputfile->IsZombie()) {
      Error("Initialise","Unable to open output file");
      return kFALSE;
   }
   fOutputFile = outputfile;
   // Set up basic folder structure
   fOutputFile->cd();
   TDirectory* topDir = gDirectory;
   // Create a config directory and write copy of RunConfig to it
   TDirectory* configDir = topDir->mkdir(Folders::config.c_str());
   configDir->cd();
   runConfig.Write("RunConfig",TObject::kOverwrite);
   topDir->cd();
   // Create a particles directory structure
   fParticleStatesFolder = topDir->mkdir(Folders::particles.c_str());
   // Create the initial states folder
   fInitialStatesFolder = fParticleStatesFolder->mkdir(Folders::initial.c_str());
   // Create the typical subfolders of final states
   fParticleStatesFolder->mkdir(Folders::propagating.c_str());
   fParticleStatesFolder->mkdir(Folders::absorbed.c_str());
   fParticleStatesFolder->mkdir(Folders::detected.c_str());
   fParticleStatesFolder->mkdir(Folders::decayed.c_str());
   fParticleStatesFolder->mkdir(Folders::lost.c_str());
   fParticleStatesFolder->mkdir(Folders::anomalous.c_str());
   // Load initial particles into output file
   if (this->LoadParticles(runConfig) == kFALSE) {
      Error("Initialise","Cannot Load Particles");
      return kFALSE;
   }
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t Data::LoadParticles(const RunConfig& runConfig)
{
   // -- Locate particles designated as 'initial' in the run config. Copy all of these
   // -- particles into the initial states folder inside the output file.
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
   string which_particle_state = runConfig.ParticlesToLoad();
   // Convert to lowercase.
   boost::to_lower(which_particle_state);
   // Navigate to the folder containing the input particle states 
   Bool_t correctFolder = partDir->cd(which_particle_state.c_str());
   // check that we ended up in the correct folder
   if (correctFolder == kFALSE) {
      Error("LoadParticles","Cannot find folder: %s", which_particle_state.c_str());
      return kFALSE;
   }
   TDirectory * const sourceDir = gDirectory;
   TDirectory * const outputDir = fInitialStatesFolder; 
   ///////////////////////////////////////////////////////////////////////
   // -- Make a list of the IDs of which particle to load. 
   Bool_t loadAllParticles = runConfig.LoadAllParticles();
   TList* selected_particles = NULL;
   if (loadAllParticles == true) {
      // All particles are to be loaded
      selected_particles = sourceDir->GetListOfKeys();
   } else {
      // Only subset of particles are to be loaded
      vector<string> selected_IDs = runConfig.SelectedParticleIDs();
      cout << "Loading select particles: ";
      copy(selected_IDs.begin(), selected_IDs.end(), ostream_iterator<string>(cout,","));
      cout << endl;
      selected_particles = new TList();
      vector<string>::const_iterator idIter;
      for (idIter = selected_IDs.begin(); idIter != selected_IDs.end(); idIter++) {
         TKey* particleKey = sourceDir->FindKey(idIter->c_str());
         if (particleKey != NULL) {
            selected_particles->Add(particleKey);
         } else {
            cout << "Error: Cannot find particle " << *idIter << endl;
            return false;
         }
      }  
   }
   ///////////////////////////////////////////////////////////////////////
   // - Load Particles into the initial state folder of the output file 
   cout << "Loading particles in the state: " << sourceDir->GetName();
   cout << ", as the input to current Run, to continue their propagation" << endl;
   // Copy selected particles from the source directory into the initial states directory of the run
   Int_t counter = 0;
   TKey *key;
   TIter nextkey(selected_particles);
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TDirectory")) {
         ///////////////////////////////////////////////////////////////////////
         // - Check Config for whether we will re-start the particles specified above from their initial
         // - states. This will only be done for particles that are not in their initial states already.
         Bool_t fromBeginning = runConfig.RestartFromBeginning();
         if (fromBeginning == kTRUE && sourceDir->GetName() != Folders::initial) {
            // cd to the initial states folder of the input file. 
            partDir->cd(Folders::initial.c_str());
            // Look for the initial state folder of the current particle
            TKey* initial_key = gDirectory->FindKey(key->GetName());
            // If that folder exists, cd into it
            if (initial_key == NULL) {
               cout << "Cannot Find Initial state of particle: " << key->GetName() << endl;
               return false;
            } else {
               gDirectory->cd(initial_key->GetName());
            }
            // Copy this particles initial state to the output directory
            TDirectory * const initialDir = gDirectory;
            this->CopyDirectory(initialDir, outputDir);
         } else {
            // Copy particle's state into the output directory
            sourceDir->cd(key->GetName());
            TDirectory * const initialDir = gDirectory;
            this->CopyDirectory(initialDir, outputDir);
         }
      }
      ++counter;
      Algorithms::ProgressBar::PrintProgress(counter,sourceDir->GetNkeys(),1);
   }
   // Clean up
   inputfile->Close();
   if (inputfile) delete inputfile; inputfile = NULL;
   cout << "-------------------------------------------" << endl;
   cout << this->InitialParticles() << " particles have been loaded succesfully" << endl;
   cout << "-------------------------------------------" << endl;
   return true;
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
      Algorithms::ProgressBar::PrintProgress(counter,sourceDir->GetNkeys(),1);
   }
   outputDir->SaveSelf(kTRUE);
   outputDir->cd();
}

//_____________________________________________________________________________
void Data::PurgeObservers()
{
   // -- Delete all observers held
   if (fObservers.empty() == kFALSE) {
      multimap<string, Observer*>::iterator it;
      for(it = fObservers.begin(); it != fObservers.end(); ++it) {
         delete it->second;
         it->second = 0;
      }
   }
}

//_____________________________________________________________________________
void Data::CreateObservers(const RunConfig& runConfig, const Experiment& experiment)
{
   // -- Check Run configuration for which properties are to be monitored with Observers 
   cout << "-------------------------------------------" << endl;
   cout << "Creating Observers" << endl;
   cout << "ObserveSpin: " << runConfig.ObserveSpin() << endl;
   cout << "Spin Measurement Interval (s): " << runConfig.SpinMeasureInterval() << endl;
   cout << "ObserveField: " << runConfig.ObserveField() << endl;
   cout << "Field Measurement Interval (s): " << runConfig.FieldMeasureInterval() << endl;
   cout << "ObserveBounces: " << runConfig.ObserveBounces() << endl;
   cout << "ObserveTracks: " << runConfig.ObserveTracks() << endl;
   cout << "Track Measurement Interval (s): " << runConfig.TrackMeasureInterval() << endl;
   if (runConfig.ObserveBounces() == kTRUE) {
      // Create an observer to track UCN Bounces
      Observer* obs = new BounceObserver("BounceObserver");
      // Add observer to the list
      this->AddObserver("Particles", obs);
   }
   if (runConfig.ObserveTracks() == kTRUE) {
      // Create an observer to track UCN path
      Observer* obs = new TrackObserver("TrackObserver", runConfig.TrackMeasureInterval());
      // Add observer to the list
      this->AddObserver("Particles", obs);
   }
   if (runConfig.ObserveSpin() == kTRUE) {
      // Create an observer to track UCN Spin polarisation
      Observer* obs = new SpinObserver("SpinObserver", runConfig.SpinMeasureInterval());
      // Add observer to the list
      this->AddObserver("Particles", obs);
   }
   // -- Attach observers to parts of the experiment
   if (runConfig.ObserveField() == kTRUE) {
      // Create an observer to record field seen by spin
      if (runConfig.MagFieldOn() == kTRUE) {
         Observer* obs = new FieldObserver("MagFieldObserver", runConfig.FieldMeasureInterval());
         // Add observer to the list
         this->AddObserver("Fields", obs);
         // Define Observer's subject
         MagFieldArray& magFieldArray = experiment.GetFieldManager().GetMagFieldArray();
         obs->DefineSubject(&magFieldArray);
         // Attach observer to its subject
         magFieldArray.Attach(obs);
      }
      // Create an observer to record field seen by spin
      if (runConfig.ElecFieldOn() == kTRUE) {
         Observer* obs = new FieldObserver("ElecFieldObserver", runConfig.FieldMeasureInterval());
         // Add observer to the list
         this->AddObserver("Fields", obs);
         // Define Observer's subject
         ElecFieldArray& elecFieldArray = experiment.GetFieldManager().GetElecFieldArray();
         obs->DefineSubject(&elecFieldArray);
         // Attach observer to its subject
         elecFieldArray.Attach(obs);
      }
   }
}

//_____________________________________________________________________________
void Data::AddObserver(string subject, Observer* observer)
{
   // -- Add observer to internal list
   fObservers.insert(pair<string, Observer*>(subject, observer));
}

//_____________________________________________________________________________
void Data::RegisterObservers(Particle* particle)
{
   // -- Attach to particle only those observers in map with their subject being particles
   multimap<string, Observer*>::iterator obsIter;
   for (obsIter = fObservers.begin(); obsIter != fObservers.end(); obsIter++) {
      string subject = obsIter->first;
      Observer* observer = obsIter->second;
      observer->ResetData();
      if (subject == "Particles") {
         // If observer's subject is particle, then attach it to the particle
         observer->DefineSubject(particle);
         particle->Attach(observer);
      } else if (subject == "Fields") {
         // If observer's subject is fields, also attach it to the particle but don't change its
         // subject, as the particle is not subject, but it can issue notifications
         particle->Attach(observer);
      }
      // Get observers to make a measurement of the initial state of their subjects
      observer->RecordEvent(particle->GetPoint(), particle->GetVelocity(), Context::Creation);
   }
}

//_____________________________________________________________________________
Bool_t Data::SaveParticle(Particle* particle, const std::string& state)
{
   // -- Navigate to folder "state" in the particles/finalstates directory of the output data
   // -- file. If folder "state" doesn't yet exist, then create it and set as the current directory
   Bool_t foundFolder = fParticleStatesFolder->cd(state.c_str());
   if (foundFolder == kFALSE) {
      Error("SaveParticle","Unable to open folder: %s", state.c_str());
      return kFALSE;
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
   // Register Observers with particle
   this->RegisterObservers(nextParticle);
   // Search Directory for any observables previously recorded to be continued
   multimap<string, Observer*>::iterator obsIter;
   for (obsIter = fObservers.begin(); obsIter != fObservers.end(); obsIter++) {
      obsIter->second->LoadExistingData(nextParticleDir);
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
   Info("ChecksOut","Output Data written to: %s",fOutputFile->GetName());
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
   if (fParticleStatesFolder->cd(Folders::propagating.c_str()) == kFALSE) {
      return 0;
   } else {
      return gDirectory->GetNkeys();
   }
}

//_____________________________________________________________________________
Int_t Data::DetectedParticles() const
{
   if (fParticleStatesFolder->cd(Folders::detected.c_str()) == kFALSE) {
      return 0;
   } else {
      return gDirectory->GetNkeys();
   }
}

//_____________________________________________________________________________
Int_t Data::DecayedParticles() const
{
   if (fParticleStatesFolder->cd(Folders::decayed.c_str()) == kFALSE) {
      return 0;
   } else {
      return gDirectory->GetNkeys();
   }
}

//_____________________________________________________________________________
Int_t Data::AbsorbedParticles() const
{
   if (fParticleStatesFolder->cd(Folders::absorbed.c_str()) == kFALSE) {
      return 0;
   } else {
      return gDirectory->GetNkeys();
   }
}

//_____________________________________________________________________________
Int_t Data::LostParticles() const
{
   if (fParticleStatesFolder->cd(Folders::lost.c_str()) == kFALSE) {
      return 0;
   } else {
      return gDirectory->GetNkeys();
   }
}

//_____________________________________________________________________________
Int_t Data::AnomalousParticles() const
{
   if (fParticleStatesFolder->cd(Folders::anomalous.c_str()) == kFALSE) {
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

//_____________________________________________________________________________
void Data::SaveGeometry(TGeoManager* const geoManager)
{
   // -- Write copy of Geometry to data file
   fOutputFile->cd();
   TDirectory* topDir = gDirectory;
   TDirectory* geomDir = topDir->mkdir(Folders::geometry.c_str());
   if (geomDir == NULL) {
      topDir->cd(Folders::geometry.c_str());
      geomDir = gDirectory;
   }
   geomDir->cd();
   geoManager->Write("GeoManager",TObject::kOverwrite);
   topDir->cd();
   return;
}

