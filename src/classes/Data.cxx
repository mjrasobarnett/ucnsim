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
#include "TTree.h"
#include "TBranch.h"

#include "RunConfig.h"
#include "Particle.h"
#include "Experiment.h"
#include "MagFieldArray.h"
#include "ElecFieldArray.h"
#include "ValidStates.h"
#include "Algorithms.h"
#include "DataAnalysis.h"

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
   fInputFile = NULL;
   fOutputFile = NULL;
   fInputTree = NULL;
   fOutputTree = NULL;
   fInputBranch = NULL;
   fCurrentParticle = NULL;
   fOutputManifest = NULL;
}

//_____________________________________________________________________________
Data::Data(const Data& other)
         :TNamed(other), 
          fInputFile(other.fInputFile),
          fOutputFile(other.fOutputFile),
          fInputTree(other.fInputTree),
          fOutputTree(other.fOutputTree),
          fInputBranch(other.fInputBranch),
          fCurrentParticle(other.fCurrentParticle),
          fOutputManifest(other.fOutputManifest),
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
   fInputBranch = NULL; // Dont delete the branch, let the Tree clean it up
   if (fInputTree) fInputTree->Delete(); fInputTree = NULL;
   if (fOutputTree) fOutputTree->Delete(); fOutputTree = NULL;
   if (fCurrentParticle) delete fCurrentParticle; fCurrentParticle = NULL;
   if (fOutputManifest) delete fOutputManifest; fOutputManifest = NULL;
   if (fInputFile) fInputFile->Close(); delete fInputFile; fInputFile = NULL;
   if (fOutputFile) fOutputFile->Close(); delete fOutputFile; fOutputFile = NULL;
   PurgeObservers();
}

//_____________________________________________________________________________
Bool_t Data::Initialise(const RunConfig& runConfig)
{
   // -- Open the output file, load the initial particles from the input file
   // -- and write the particle tree, as well as a copy of the runconfig to the
   // -- output file
   
   ///////////////////////////////////////////////////////////////////////
   // -- Open the file holding the initial particle tree
   const string inputFileName = runConfig.InputFileName();
   fInputFile = Analysis::DataFile::OpenRootFile(inputFileName, "READ");
   if (fInputFile == NULL) {return false;}
   ///////////////////////////////////////////////////////////////////////
   // -- Read into memory the input file's Particle Tree, 
   // -- and the input file's particle manifest
   fInputTree = this->ReadInParticleTree(fInputFile);
   // Check that we actually read both these objects into memory
   if (fInputTree == NULL) {
      Error("Initialise","Cannot find Particle Tree input file");
      return false;
   }
   // Open Output File
   const string outputFileName = runConfig.OutputFileName();
   fOutputFile = Analysis::DataFile::OpenRootFile(outputFileName, "RECREATE");
   if (fOutputFile == NULL) return false;
   // Create the Output Tree and Output Particle Manifest
   fOutputTree = new TTree("Particles","Tree of Particle Data");
   fOutputManifest = new ParticleManifest();
   // Store pointer to the current 'in-memory' Particle
   fCurrentParticle = new Particle();
   return true;
}

//_____________________________________________________________________________
vector<int> Data::GetListOfParticlesToLoad(const RunConfig& runConfig)
{
   // -- Locate particles designated as 'initial' in the run config. Copy all of these
   // -- particles into the output file ready for propagation.
   ///////////////////////////////////////////////////////////////////////
   // -- Check RunConfig for whether we are continuing to propagate particles from
   // -- their last recorded position, or whether we are restarting them from their
   // -- initial position
   Bool_t fromBeginning = runConfig.RestartFromBeginning();
   string selectedBranch;
   if (fromBeginning == false) {
      selectedBranch = States::final;
   } else {
      selectedBranch = States::initial;
   }
   ///////////////////////////////////////////////////////////////////////
   // -- Set the input branch from where we will read the particles
   fInputBranch = fInputTree->GetBranch(selectedBranch.c_str());
   if (fInputBranch == NULL) {
      Error("GetListOfParticlesToLoad","Could not find branch %s in input tree",selectedBranch.c_str());
      return vector<int>();
   }
   fInputTree->SetBranchAddress(fInputBranch->GetName(), &fCurrentParticle);
   // -- Read in the input manifest from file
   ParticleManifest* inputManifest = this->ReadInParticleManifest(fInputFile);
   if (inputManifest == NULL) {
      Error("Initialise","Cannot find Particle Manifest in input file");
      return vector<int>();
   }
   ///////////////////////////////////////////////////////////////////////
   // -- Copy all particles from the input branch into the current Run's tree
   vector<int> selectedIndexes = this->GetSelectedParticleIndexes(*inputManifest, runConfig);
   return selectedIndexes;
}

//_____________________________________________________________________________
vector<int> Data::GetSelectedParticleIndexes(const ParticleManifest& manifest, const RunConfig& runConfig) const
{
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Check RunConfig for which particle state we should take our initial particles from
   cout << "Determining which particles to load..." << endl;
   string which_particle_state = runConfig.ParticlesToLoad();
   vector<int> availableIndexes = manifest.GetListing(which_particle_state).GetTreeIndexes();
   if (availableIndexes.empty() == true) {
      Error("GetListOfSelectedParticles","Cannot find any Particles for state %s in input manifest",which_particle_state.c_str());
      return availableIndexes;
   }
   ///////////////////////////////////////////////////////////////////////
   // -- Check RunConfig for whether to load all the particles in this state, or 
   // -- only a subset of these particles, chosen by the User
   Bool_t loadAllParticles = runConfig.LoadAllParticles();
   if (loadAllParticles != true) {
      // Get the User-defined particle IDs they wish to propagte
      vector<int> selectedIndexes = runConfig.SelectedParticleIDs();
      CheckSelectedIndexList(selectedIndexes, availableIndexes);
      return selectedIndexes;
   }
   return availableIndexes;
}

//_____________________________________________________________________________
ParticleManifest* Data::ReadInParticleManifest(TFile* file) const
{
   // Loop on all entries of this directory searching for the first
   // ParticleManifest object
   ParticleManifest* manifest = NULL;
   TKey *key;
   TIter nextkey(file->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("ParticleManifest")) {
         manifest = dynamic_cast<ParticleManifest*>(key->ReadObj());
         break;
      } else {
         continue;
      }
   }
   return manifest;
}

//_____________________________________________________________________________
TTree* Data::ReadInParticleTree(TFile* file) const
{
   // Loop on all entries of this directory searching for the first Tree object
   TTree* inputTree = NULL;
   TKey *key;
   TIter nextkey(file->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TTree")) {
         inputTree = dynamic_cast<TTree*>(key->ReadObj());
      } else {
         continue;
      }
   }
   return inputTree;
}

//_____________________________________________________________________________
bool Data::CheckSelectedIndexList(vector<int>& selectedIndexes, vector<int>& availableIndexes) const
{
   // Take the intersection between ALL the particle IDs for the requested State
   // and those that have been selected by the User.
   sort(selectedIndexes.begin(), selectedIndexes.end());
   sort(availableIndexes.begin(), availableIndexes.end());
   vector<int>::iterator range;
   // Using the stl's set_intersection() algorithm for this. It requires both input vectors
   // to be SORTED. 
   range = set_intersection(availableIndexes.begin(), availableIndexes.end(), selectedIndexes.begin(), selectedIndexes.end(), selectedIndexes.begin());
   // Algorithm returns an iterator to the point in the output vector that is the end
   // of the elements that were in the interesection. We need to manually remove all 
   // the elements past the end of this point, to leave us with a vector of ONLY the
   // intersection elements 
   selectedIndexes.erase(range, selectedIndexes.end());
   if (selectedIndexes.empty()) {
      Error("CheckSelectedIndexList","None of selected Particle IDs are available for current state");
      return false;
   }
   // Printing these elements to stdout
   cout << "Loading specific particles, Numbers: ";
   copy(selectedIndexes.begin(), selectedIndexes.end(), ostream_iterator<int>(cout,","));
   cout << endl;
   return true;
}

//_____________________________________________________________________________
bool Data::CopyAllParticles(TBranch* inputBranch, TBranch* outputBranch)
{
   // Copy all particles from inputBranch 
   for (int id = 0; id < inputBranch->GetEntries(); id++) {
      int bytesCopied = inputBranch->GetEntry(id);
      if (bytesCopied <= 0) continue;
      outputBranch->Fill();
      int branchIndex = outputBranch->GetEntries() - 1;
      fOutputManifest->AddEntry(States::initial, fCurrentParticle->Id(), branchIndex);
      Algorithms::ProgressBar::PrintProgress(id, inputBranch->GetEntries(), 1);
   }
   return true;
}

//_____________________________________________________________________________
bool Data::CopySelectedParticles(const std::vector<int>& selectedIndexes, TBranch* inputBranch, TBranch* outputBranch)
{
   ///////////////////////////////////////////////////////////////////////
   // -- Iterate over the chosen particle IDs and copy these particles to the 'Initial'
   // -- branch of the Output file
   if (selectedIndexes.empty()) return false;
   vector<int>::const_iterator indexIter;
   for (indexIter = selectedIndexes.begin(); indexIter != selectedIndexes.end(); indexIter++) {
      int index = indexIter - selectedIndexes.begin(); 
      // Fetch the particle at the current index
      int bytesCopied = inputBranch->GetEntry(index);
      if (bytesCopied <= 0) {
         Error("CopySelectedParticles","Could not find particle %i in input branch",index);
         return false;
      }
      // If we found the particle, copy to the output branch
      outputBranch->Fill();
      int branchIndex = outputBranch->GetEntries() - 1;
      fOutputManifest->AddEntry(States::initial, fCurrentParticle->Id(), branchIndex);
      Algorithms::ProgressBar::PrintProgress(index, selectedIndexes.size(), 1);
   }
   return true;
}

//_____________________________________________________________________________
void Data::PurgeObservers()
{
   // -- Delete all observers held
   if (fObservers.empty() == false) {
      ObserverCategories::iterator categoryIter;
      for(categoryIter = fObservers.begin(); categoryIter != fObservers.end(); ++categoryIter) {
         ObserverList& observerList = categoryIter->second;
         ObserverList::iterator observerIter;
         for(observerIter = observerList.begin(); observerIter != observerList.end(); ++observerIter) {
            delete observerIter->second;
            observerIter->second = 0;
         }
      }
   }
}

//_____________________________________________________________________________
void Data::CreateObservers(const RunConfig& runConfig, Experiment& experiment)
{
   // -- Check Run configuration for which properties are to be monitored with Observers 
   cout << "-------------------------------------------" << endl;
   cout << "Creating Observers..." << endl;
   if (runConfig.ObserveBounces() == kTRUE) {
      // Create an observer to track UCN Bounces
      Observer* obs = new BounceObserver("BounceObserver");
      // Add observer to the list
      this->AddObserver(Categories::PerTrack, Subjects::Particles, obs);
   }
   if (runConfig.ObserveTracks() == kTRUE) {
      // Create an observer to track UCN path
      Observer* obs = new TrackObserver("TrackObserver", runConfig.TrackMeasureInterval());
      // Add observer to the list
      this->AddObserver(Categories::PerTrack, Subjects::Particles, obs);
   }
   if (runConfig.ObserveSpin() == kTRUE) {
      // Create an observer to track UCN Spin polarisation
      Observer* obs = new SpinObserver("SpinObserver", runConfig.SpinMeasureInterval());
      // Add observer to the list
      this->AddObserver(Categories::PerTrack, Subjects::Particles, obs);
   }
   // -- Attach observers to parts of the experiment
   if (runConfig.ObserveField() == kTRUE) {
      // Create an observer to record field seen by spin
      if (runConfig.MagFieldOn() == kTRUE) {
         Observer* obs = new FieldObserver("MagFieldObserver", runConfig.FieldMeasureInterval());
         // Add observer to the list
         this->AddObserver(Categories::PerTrack, Subjects::Fields, obs);
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
         this->AddObserver(Categories::PerTrack, Subjects::Fields, obs);
         // Define Observer's subject
         ElecFieldArray& elecFieldArray = experiment.GetFieldManager().GetElecFieldArray();
         obs->DefineSubject(&elecFieldArray);
         // Attach observer to its subject
         elecFieldArray.Attach(obs);
      }
   }
   // -- Attach observers to the Experiment
   if (runConfig.ObservePopulation() == true) {
      Observer* obs = new PopulationObserver("PopulationObserver", runConfig.PopulationMeasureInterval());
      // Add observer to the list - noting that its subject will be the particles
      this->AddObserver(Categories::PerRun, Subjects::Particles, obs);
   }
}

//_____________________________________________________________________________
void Data::AddObserver(const string category, const string subject, Observer* observer)
{
   // -- Add observer to internal list
   ObserverCategories::iterator categoryIter = fObservers.find(category);
   if (categoryIter == fObservers.end()) {
      ObserverList newList;
      newList.insert(pair<string, Observer*>(subject, observer));
      fObservers.insert(pair<string, ObserverList>(category, newList));
   } else {
      ObserverList& observerList = categoryIter->second;
      observerList.insert(pair<string, Observer*>(subject, observer));
   }
}

//______________________________________________________________________________
void Data::RegisterObservers(Particle* particle)
{
   // -- Attach to particle the relevant observers
   ObserverCategories::iterator categoryIter;
   for(categoryIter = fObservers.begin(); categoryIter != fObservers.end(); ++categoryIter) {
      const string category = categoryIter->first;
      ObserverList& observerList = categoryIter->second;
      if (category == Categories::PerTrack) {
         AttachObservers(observerList, particle);
      } else if (category == Categories::PerRun) {
         AttachObservers(observerList, particle);
      } else {
         cerr << "Error: An observer category, " << category << " has been added that is not recognised." << endl;
         throw runtime_error("Unsure how to handle this observer");
      }
   }
}

//______________________________________________________________________________
void Data::ResetObservers()
{
   // -- After each particle has finished its propagation, Reset all the observers
   // -- before the next propagation. For those observers recording data on a track-by-track
   // -- basis, Write their data to file.
   ObserverCategories::iterator categoryIter;
   for(categoryIter = fObservers.begin(); categoryIter != fObservers.end(); ++categoryIter) {
      const string category = categoryIter->first;
      ObserverList& observerList = categoryIter->second;
      ObserverList::iterator observerIter;
      for(observerIter = observerList.begin(); observerIter != observerList.end(); ++observerIter) {
         string subject = observerIter->first;
         Observer* observer = observerIter->second;
         if (category == Categories::PerTrack) {
            // If the observer is recording data on a 'PerTrack' basis, reset
            // both its clock, and its internal data
            observer->WriteToFile(*this);
            observer->ResetInternalClock();
            observer->ResetData();
         } else if (category == Categories::PerRun) {
            // If the observer is recording data on a 'PerRun' basis, *just* reset
            // its clock
            observer->ResetInternalClock();
         } else {
            cerr << "Error: An observer category, " << category;
            cerr << " has been added that is not recognised." << endl;
            throw runtime_error("Unsure how to handle this observer");
         }
      }
   }
}

//______________________________________________________________________________
void Data::AttachObservers(ObserverList& observerList, Particle* particle)
{
   ObserverList::iterator observerIter;
   for(observerIter = observerList.begin(); observerIter != observerList.end(); ++observerIter) {
      string subject = observerIter->first;
      Observer* observer = observerIter->second;
      if (subject == Subjects::Particles) {
         // If observer's ONLY subject is the particle, then reset previous particle's
         // data and define Observer's subject to be current particle
         observer->DefineSubject(particle);
         particle->Attach(observer);
      } else if (subject == Subjects::Fields) {
         // If observer's subject is primarily the particle's observed fields, 
         // then the data is still relevant only to a single particle, therefore it isn't
         // a shared observer, however the observer's subject isn't the particle itself
         // but rather the field it sees. Thus, reset the previous particles data, 
         // but don't change the observer's subject.
         particle->Attach(observer);
      } else {
         cerr << "Error: An observer subject, " << subject << " has been added that is not recognised." << endl;
         throw runtime_error("Unsure how to handle this observer");
      }
      // Get observers to make a measurement of the initial state of their subjects
      observer->RecordEvent(particle->GetPoint(), particle->GetVelocity(), Context::Creation);
   }
}

//_____________________________________________________________________________
Bool_t Data::SaveInitialParticle(Particle* particle)
{
   // Detach observers
   particle->DetachAll();
   // Write Particle to output branch
   this->WriteObjectToTree(particle, States::initial.c_str());
   // Update Manifest
   int branchIndex = fOutputTree->GetBranch(States::initial.c_str())->GetEntries() - 1;
   fOutputManifest->AddEntry(States::initial, particle->Id(), branchIndex);
   return true;
}

//_____________________________________________________________________________
Bool_t Data::SaveFinalParticle(Particle* particle, const std::string& state)
{
   // Detach observers
   particle->DetachAll();
   // Write Particle to output branch
   this->WriteObjectToTree(particle, States::final.c_str());
   // Update Manifest
   int branchIndex = fOutputTree->GetBranch(States::final.c_str())->GetEntries() - 1;
   fOutputManifest->AddEntry(state, particle->Id(), branchIndex);
   return true;
}

//_____________________________________________________________________________
Particle* const Data::RetrieveParticle(unsigned int index)
{
   if (fInputBranch == NULL) {
      Error("RetrieveParticle","No input branch set");
      return NULL;
   }
   fInputTree->SetBranchAddress(fInputBranch->GetName(), &fCurrentParticle);
   if (index >= fInputBranch->GetEntries()) {
      Error("RetrieveParticle","Requested index, %i, is larger than number of particles",index);
      return NULL;
   }
   fInputBranch->GetEntry(index);
   return fCurrentParticle;
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
   return fOutputManifest->GetListing(States::initial).Entries();
}

//_____________________________________________________________________________
Int_t Data::PropagatingParticles() const
{
   return fOutputManifest->GetListing(States::propagating).Entries();
}

//_____________________________________________________________________________
Int_t Data::DetectedParticles() const
{
   return fOutputManifest->GetListing(States::detected).Entries();
}

//_____________________________________________________________________________
Int_t Data::DecayedParticles() const
{
   return fOutputManifest->GetListing(States::decayed).Entries();
}

//_____________________________________________________________________________
Int_t Data::AbsorbedParticles() const
{
   return fOutputManifest->GetListing(States::absorbed).Entries();
}

//_____________________________________________________________________________
Int_t Data::LostParticles() const
{
   return fOutputManifest->GetListing(States::lost).Entries();
}

//_____________________________________________________________________________
Int_t Data::AnomalousParticles() const
{
   return fOutputManifest->GetListing(States::anomalous).Entries();
}

//_____________________________________________________________________________
Int_t Data::FinalParticles() const
{
   Int_t final = PropagatingParticles() + DetectedParticles() + AbsorbedParticles() + DecayedParticles() + LostParticles() + AnomalousParticles();
   return final;
}

//_____________________________________________________________________________
void Data::Export()
{
   this->WriteObjectToFile(fOutputTree);
   this->WriteObjectToFile(fOutputManifest);
   // Write out any 'PerRun' observers
   ObserverCategories::iterator categoryIter = fObservers.find(Categories::PerRun);
   if (categoryIter != fObservers.end()) {
      ObserverList& observerList = categoryIter->second;
      ObserverList::iterator observerIter;
      for(observerIter = observerList.begin(); observerIter != observerList.end(); ++observerIter) {
         Observer* observer = observerIter->second;
         observer->WriteToFile(*this);
      }
   }
}

//_____________________________________________________________________________
int Data::WriteObjectToFile(TObject* object)
{
   // -- Write a copy of the object to the top level directory of the File
   fOutputFile->cd();
   int bytesCopied = object->Write(object->GetName(),TObject::kOverwrite);
   return bytesCopied;
}

//_____________________________________________________________________________
int Data::WriteObjectToTree(TObject* object, const char* branchName)
{
   // -- Write out the provided data to a branch on the provided tree, named after
   // -- the classname of the data
   TBranch* branch = fOutputTree->GetBranch(branchName);
   if (branch == NULL) {
      // -- If there is no branch yet, create one
      Info("WriteObjectToTree","Creating Branch %s in output tree", branchName);
      branch = fOutputTree->Branch(branchName, object->ClassName(), &object, 32000,0);
   }
   fOutputTree->SetBranchAddress(branchName, &object);
   int bytesCopied = branch->Fill();
   return bytesCopied;
}