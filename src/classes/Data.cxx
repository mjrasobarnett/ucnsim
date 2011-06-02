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
   fOutputFile = NULL;
   fOutputTree = NULL;
   fCurrentParticle = NULL;
   fOutputManifest = NULL;
}

//_____________________________________________________________________________
Data::Data(const Data& other)
         :TNamed(other), 
          fOutputFile(other.fOutputFile),
          fOutputTree(other.fOutputTree),
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
   if (fOutputTree) fOutputTree->Delete(); fOutputTree = NULL;
   if (fCurrentParticle) delete fCurrentParticle; fCurrentParticle = NULL;
   if (fOutputManifest) delete fOutputManifest; fOutputManifest = NULL;
   if (fOutputFile) fOutputFile->Close(); delete fOutputFile; fOutputFile = NULL;
   PurgeObservers();
}

//_____________________________________________________________________________
Bool_t Data::Initialise(const RunConfig& runConfig)
{
   // -- Open the output file, load the initial particles from the input file
   // -- and write the particle tree, as well as a copy of the runconfig to the
   // -- output file
   // Open Output File
   const string outputFileName = runConfig.OutputFileName();
   fOutputFile = Analysis::DataFile::OpenRootFile(outputFileName, "RECREATE");
   if (fOutputFile == NULL) return false;
   // Create the Output Tree and Output Particle Manifest
   fOutputTree = new TTree("Particles","Tree of Particle Data");
   fOutputManifest = new ParticleManifest();
   // Store pointer to the current 'in-memory' Particle
   fCurrentParticle = new Particle();
   // Write a copy of the RunConfig to File
   runConfig.Write("RunConfig",TObject::kOverwrite);
   // Load initial particles into output file
   if (this->LoadParticles(runConfig) == false) return false;
   return true;
}

//_____________________________________________________________________________
Bool_t Data::LoadParticles(const RunConfig& runConfig)
{
   // -- Locate particles designated as 'initial' in the run config. Copy all of these
   // -- particles into the output file ready for propagation.
   cout << "-------------------------------------------" << endl;
   cout << "Loading the Particles" << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////
   // -- Open the file holding the initial particle tree
   const string inputFileName = runConfig.InputFileName();
   TFile* inputFile = Analysis::DataFile::OpenRootFile(inputFileName, "READ");
   if (inputFile == NULL) {return false;}
   inputFile->cd();
   ///////////////////////////////////////////////////////////////////////
   // -- Read into memory the input file's Particle Tree, 
   // -- and the input file's particle manifest
   TTree* inputTree = ReadInParticleTree(inputFile);
   ParticleManifest* manifest = ReadInParticleManifest(inputFile);
   // Check that we actually read both these objects into memory
   if (inputTree == NULL || manifest == NULL) {
      Error("LoadParticles","Cannot find Particle Tree or Particle Manifest in input file");
      return false;
   }
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
   TBranch* inputBranch = inputTree->GetBranch(selectedBranch.c_str());
   if (inputBranch == NULL) {
      Error("LoadParticles","Could not find branch %s in input tree",selectedBranch.c_str());
      return false;
   }
   inputTree->SetBranchAddress(inputBranch->GetName(), &fCurrentParticle);
   ///////////////////////////////////////////////////////////////////////
   // -- Set the output branch where we will write the initial particles to
   TBranch* outputBranch = fOutputTree->Branch(States::initial.c_str(), fCurrentParticle->ClassName(), &fCurrentParticle,32000,0);
   ///////////////////////////////////////////////////////////////////////
   // -- Copy all particles from the input branch into the current Run's tree
   vector<int> selectedIndexes = this->GetSelectedParticleIndexes(*manifest, runConfig);
   if (CopySelectedParticles(selectedIndexes, inputBranch, outputBranch) == false) {return false;}
   // -- Write Output Tree to file
   fOutputFile->cd();
   fOutputTree->Write(fOutputTree->GetName(),TObject::kOverwrite);
   // -- Clean up
   inputFile->Close();
   cout << "-------------------------------------------" << endl;
   cout << this->InitialParticles() << " particles have been loaded succesfully" << endl;
   cout << "-------------------------------------------" << endl;
   return true;
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
      Error("CheckSelectedParticleIDs","None of selected Particle IDs are available for current state");
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
         Error("LoadParticles","Could not find particle %i in input branch",index);
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
   // Write observers to Tree and then delete them
   particle->WriteObserversToTree(fOutputTree);
   particle->DetachAll();
   // Write Particle to output branch
   TBranch* particleBranch = fOutputTree->GetBranch(States::final.c_str());
   if (particleBranch == NULL) {
      Info("WriteToTree","Creating Branch %s in output tree", States::final.c_str());
      particleBranch = fOutputTree->Branch(States::final.c_str(), particle->ClassName(), &particle, 32000,0);
   }
   fOutputTree->SetBranchAddress(particleBranch->GetName(), &particle);
   particleBranch->Fill();
   // Update Manifest
   int branchIndex = particleBranch->GetEntries() - 1;
   fOutputManifest->AddEntry(state, particle->Id(), branchIndex);
   return true;
}

//_____________________________________________________________________________
Particle* const Data::RetrieveParticle(unsigned int index)
{
   TBranch* inputBranch = fOutputTree->GetBranch(States::initial.c_str());
   if (inputBranch == NULL) {
      Error("RetrieveParticle","Could not find branch %s in input tree",States::initial.c_str());
      return NULL;
   }
   fOutputTree->SetBranchAddress(inputBranch->GetName(), &fCurrentParticle);
   if (index >= inputBranch->GetEntries()) {
      Error("RetrieveParticle","Requested index, %i, is larger than number of particles",index);
      return NULL;
   }
   inputBranch->GetEntry(index);
   // Register Observers with Particle
   this->RegisterObservers(fCurrentParticle);
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
void Data::SaveGeometry(TGeoManager* const geoManager)
{
   // -- Write copy of Geometry to data file
   fOutputFile->cd();
   geoManager->Write("Geometry",TObject::kOverwrite);
   return;
}

//_____________________________________________________________________________
void Data::ExportData()
{
   fOutputFile->cd();
   fOutputTree->Write(fOutputTree->GetName(), TObject::kOverwrite);
   fOutputManifest->Write(fOutputManifest->GetName(), TObject::kOverwrite);
}

