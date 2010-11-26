// UCNData.cpp
// Author: Simon JM Peeters
// Created July 2009

#include "TUCNData.h"

#include <iostream>
#include <sstream>
#include <locale>
#include <cassert>
#include <stdexcept>

#include "TKey.h"

#include "DataFileHierarchy.h"

using namespace std;

ClassImp(TUCNData)

//_____________________________________________________________________________
TUCNData::TUCNData() 
         :TNamed(),
          fObservers()
{
   // -- Default Constructor
   Info("TUCNData","Default Constructor");
   // -- Create the Trees
   fOutputFile = NULL;
   fInitialStatesFolder = NULL;
   fFinalStatesFolder = NULL;
}

//_____________________________________________________________________________
TUCNData::TUCNData(const TUCNData& other)
         :TNamed(other), 
          fOutputFile(other.fOutputFile),
          fInitialStatesFolder(other.fInitialStatesFolder),
          fFinalStatesFolder(other.fFinalStatesFolder),
          fObservers(other.fObservers)
{
   // Copy Constructor
   Info("TUCNData","Copy Constructor");
}

//_____________________________________________________________________________
TUCNData& TUCNData::operator=(const TUCNData& other)
{
// --assignment operator
   Info("TUCNData","Assignment");
   if(this!=&other) {
      TNamed::operator=(other);
      // Clean up File before assignment
      if (fOutputFile) {
         fOutputFile->Close();
         delete fOutputFile;
         fOutputFile = NULL;
      }
      fOutputFile = other.fOutputFile;
      
      fInitialStatesFolder = other.fInitialStatesFolder;
      fFinalStatesFolder = other.fFinalStatesFolder;
      
      // Clear list of observers before assignment
      PurgeObservers();
      fObservers = other.fObservers;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNData::~TUCNData()
{
   // -- Destructor
   Info("TUCNData","Destructor");
   if (fOutputFile) fOutputFile->Close(); delete fOutputFile;
   PurgeObservers();
}

//_____________________________________________________________________________
Bool_t TUCNData::Initialise(const TUCNInitialConfig& initialConfig)
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
Bool_t TUCNData::Initialise(const TUCNRunConfig& runConfig)
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
   fFinalStatesFolder->mkdir(Folders::bad.c_str());
   
   // Load 
   if (this->LoadParticles(runConfig) == kFALSE) {
      Error("Initialise","Cannot Load Particles");
      return kFALSE;
   }
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::LoadParticles(const TUCNRunConfig& runConfig)
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
   TDirectory* topDir = gDirectory;
   if (topDir->cd(Folders::particles.c_str()) == kFALSE) {
      Error("LoadParticles","No folder named: %s, found in input file",Folders::particles.c_str());
      return kFALSE;
   }
   TDirectory* partDir = gDirectory;
   ///////////////////////////////////////////////////////////////////////
   // -- Check Config for which particle Tree we wish to load as our initial particles here
   // Check runconfig option.
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
   TDirectory* sourceDir = gDirectory;
   TDirectory* outputDir = fInitialStatesFolder; 
   ///////////////////////////////////////////////////////////////////////
   // - Check Config for whether we will re-start the particles specified above from their initial
   // - states. This will only be done for particles that are not in their initial states already.
   Bool_t fromBeginning = runConfig.RestartFromBeginning();
   if (fromBeginning == kTRUE && sourceDir->GetName() != Folders::initialstates) {
      cout << "Resetting particles in the state: " << sourceDir->GetName();
      cout << ", to their initial state as the input to current Run " << endl;
   
   
   } else {
      // Otherwise we just propagate the particles from where they left off
      cout << "Loading particles in the state: " << sourceDir->GetName();
      cout << ", as the input to current Run, to continue their propagation" << endl;
      // Copy particles from the source directory into the initialstates directory of the run
      this->CopyDirectory(sourceDir, outputDir);
   }
   cout << "-------------------------------------------" << endl;
   cout << this->InitialParticles() << " particles have been loaded succesfully" << endl;
   cout << "-------------------------------------------" << endl;
   return kTRUE;
}

//_____________________________________________________________________________
void TUCNData::CopyDirectory(TDirectory * const sourceDir, TDirectory * const outputDir) {
   // Copy all objects and subdirs of directory source into the supplied output directory  
   outputDir->cd();
   // Loop on all entries of this directory
   TKey *key;
   TIter nextkey(sourceDir->GetListOfKeys());
   while ((key = (TKey*)nextkey())) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TDirectory")) {
         // Copy subdirectory to a new subdirectoy in outputDir
         sourceDir->cd(key->GetName());
         TDirectory *subdir = gDirectory;
         outputDir->cd();
         TDirectory* outputSubDir = outputDir->mkdir(subdir->GetName());
         CopyDirectory(subdir, outputSubDir);
         outputDir->cd();
      } else {
         // Copy Object
         sourceDir->cd();
         TObject *obj = key->ReadObj();
         outputDir->cd();
         obj->Write();
         delete obj;
     }
  }
  outputDir->SaveSelf(kTRUE);
  outputDir->cd();
}

//_____________________________________________________________________________
void TUCNData::PurgeObservers()
{
   // -- Delete all observers held
   if (fObservers.empty() == kFALSE) {
      vector<TUCNObserver*>::iterator it;
      for(it = fObservers.begin(); it != fObservers.end(); ++it) {
         delete *it;
         *it = 0;
      }
   }
}

//_____________________________________________________________________________
void TUCNData::AddObserver(TUCNObserver* observer)
{
   // -- Add observer to internal list
   fObservers.push_back(observer);
}

//_____________________________________________________________________________
void TUCNData::RegisterObservers(TUCNParticle* particle)
{
   // -- Register all observers in list with the particle
   vector<TUCNObserver*>::iterator obsIter;
   for (obsIter = fObservers.begin(); obsIter != fObservers.end(); obsIter++) {
      (*obsIter)->RegisterInterest(*particle);
   }
}

//_____________________________________________________________________________
Bool_t TUCNData::SaveParticle(TUCNParticle* particle, const std::string& state)
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
TUCNParticle* const TUCNData::RetrieveParticle()
{
   return 0;
}

//_____________________________________________________________________________
Bool_t TUCNData::ChecksOut() const
{
// -- Perform a check out routine, to ensure data is consistent
/*
   // Sum initial and final states
   const Int_t initialParticles = this->InitialParticles();
   const Int_t finalParticles = this->FinalParticles();
   // Check that initial and final states match
   if (initialParticles != finalParticles) {
      Error("ChecksOut", "Initial particle states: %i, is not equal to final states: %i", initialParticles, finalParticles);
      return kFALSE;
   } 
   Info("ChecksOut","Initial particle states: %i. Number of Final States: %i.", initialParticles, finalParticles);
*/
   return kTRUE;
}


//_____________________________________________________________________________
Int_t TUCNData::InitialParticles() const
{
   return 0;
}

//_____________________________________________________________________________
Int_t TUCNData::PropagatingParticles() const
{
   return 0;
}

//_____________________________________________________________________________
Int_t TUCNData::DetectedParticles() const
{
   return 0;
}

//_____________________________________________________________________________
Int_t TUCNData::DecayedParticles() const
{
   return 0;
}

//_____________________________________________________________________________
Int_t TUCNData::AbsorbedParticles() const
{
   return 0;
}

//_____________________________________________________________________________
Int_t TUCNData::LostParticles() const
{
   return 0;
}

//_____________________________________________________________________________
Int_t TUCNData::BadParticles() const
{
   return 0;
}

//_____________________________________________________________________________
Int_t TUCNData::FinalParticles() const
{
   return 0;
}

/*

//_____________________________________________________________________________
Bool_t TUCNData::AddParticleState(TString treeName, TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   TTree* tree = this->FetchTree(treeName);
   if (tree == NULL) {
      Error("AddParticleState","Failed to fetch tree with name %s",treeName.Data());
      return 0;
   }
   return this->AddParticleState(tree, particle);
}

//_____________________________________________________________________________
Bool_t TUCNData::AddParticleState(TTree* tree, TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (tree->GetBranch(tree->GetName()) == NULL) {
      tree->Branch(tree->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the InitialParticleState Tree
   tree->SetBranchAddress(tree->GetName(),&particle);
   tree->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddInitialParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fInitialParticles->GetBranch(fInitialParticles->GetName()) == NULL) {
      fInitialParticles->Branch(fInitialParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the InitialParticleState Tree
   fInitialParticles->SetBranchAddress(fInitialParticles->GetName(),&particle);
   fInitialParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddPropagatingParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fPropagatingParticles->GetBranch(fPropagatingParticles->GetName()) == NULL) {
      fPropagatingParticles->Branch(fPropagatingParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the PropagatingParticleState Tree
   fPropagatingParticles->SetBranchAddress(fPropagatingParticles->GetName(),&particle);
   fPropagatingParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddDetectedParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fDetectedParticles->GetBranch(fDetectedParticles->GetName()) == NULL) {
      fDetectedParticles->Branch(fDetectedParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the Detected ParticleState Tree
   fDetectedParticles->SetBranchAddress(fDetectedParticles->GetName(),&particle);
   fDetectedParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddDecayedParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fDecayedParticles->GetBranch(fDecayedParticles->GetName()) == NULL) {
      fDecayedParticles->Branch(fDecayedParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fDecayedParticles->SetBranchAddress(fDecayedParticles->GetName(),&particle);
   fDecayedParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddAbsorbedParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fAbsorbedParticles->GetBranch(fAbsorbedParticles->GetName()) == NULL) {
      fAbsorbedParticles->Branch(fAbsorbedParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fAbsorbedParticles->SetBranchAddress(fAbsorbedParticles->GetName(),&particle);
   fAbsorbedParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddLostParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fLostParticles->GetBranch(fLostParticles->GetName()) == NULL) {
      fLostParticles->Branch(fLostParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fLostParticles->SetBranchAddress(fLostParticles->GetName(),&particle);
   fLostParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNData::AddBadParticleState(TUCNParticle* particle)
{
   // -- Create the Branches in the Trees where we will store the objects
   if (fBadParticles->GetBranch(fBadParticles->GetName()) == NULL) {
      fBadParticles->Branch(fBadParticles->GetName(),"TUCNParticle",&particle);
   }
   // Add the particle to the Tree
   fBadParticles->SetBranchAddress(fBadParticles->GetName(),&particle);
   fBadParticles->Fill();
   return kTRUE;
}

//_____________________________________________________________________________
TUCNParticle*  TUCNData::GetParticleState(TString treeName, Int_t index)
{
// -- Retrieve the particle from the TTree	
   TTree* tree = this->FetchTree(treeName);
   if (tree == NULL) {
      Error("GetParticleState","Failed to fetch tree with name %s",treeName.Data());
      return 0;
   }
   return this->GetParticleState(tree, index);
}

//_____________________________________________________________________________
TUCNParticle*  TUCNData::GetParticleState(TTree* tree, Int_t index)
{
// -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > tree->GetEntries() || index < 0) {
      Error("GetParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   tree->SetBranchAddress(tree->GetName(), &particle);
   tree->GetEntry(index);
   if (particle == NULL) {
      Error("GetParticleState","Failed to Get Particle: %i from Tree: %s",index,tree->GetName());
      return 0;
   }
   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetInitialParticleState(Int_t index)
{
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fInitialParticles->GetEntries() || index < 0) {
      Error("GetInitialParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fInitialParticles->SetBranchAddress(fInitialParticles->GetName(), &particle);
   fInitialParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetInitialParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }
   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetPropagatingParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fPropagatingParticles->GetEntries() || index < 0) {
      Error("GetPropagatingParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fPropagatingParticles->SetBranchAddress(fPropagatingParticles->GetName(), &particle);
   fPropagatingParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetPropagatingParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetDetectedParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fDetectedParticles->GetEntries() || index < 0) {
      Error("GetDetectedParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fDetectedParticles->SetBranchAddress(fDetectedParticles->GetName(), &particle);
   fDetectedParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetDetectedParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetDecayedParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fDecayedParticles->GetEntries() || index < 0) {
      Error("GetDecayedParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fDecayedParticles->SetBranchAddress(fDecayedParticles->GetName(), &particle);
   fDecayedParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetDecayedParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetAbsorbedParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fAbsorbedParticles->GetEntries() || index < 0) {
      Error("GetAbsorbedParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fAbsorbedParticles->SetBranchAddress(fAbsorbedParticles->GetName(), &particle);
   fAbsorbedParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetAbsorbedParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetLostParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fLostParticles->GetEntries() || index < 0) {
      Error("GetLostParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fLostParticles->SetBranchAddress(fLostParticles->GetName(), &particle);
   fLostParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetLostParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TUCNParticle* TUCNData::GetBadParticleState(Int_t index) {
   // -- Retrieve the particle from the TTree	
   TUCNParticle* particle = 0;
   if (index > fBadParticles->GetEntries() || index < 0) {
      Error("GetBadParticleState","No Entry exists for Particle: %i", index);
      return 0;
   }
   fBadParticles->SetBranchAddress(fBadParticles->GetName(), &particle);
   fBadParticles->GetEntry(index);
   if (particle == NULL) {
      Error("GetBadParticleState","Failed to Get Particle: %i from Tree",index);
      return 0;
   }   return particle;
}

//_____________________________________________________________________________
TTree* TUCNData::FetchTree(TString treeName) {
// -- Return pointer to a particular Tree based on the name given
// -- This could be converted to a switch statement if this is felt to be more efficient,
// -- however the number of calls to this function would suggest this is unimportant
   // Convert to Lower Case
   treeName.ToLower();
   // Determine which Tree is trying to be selected
   if (treeName == "initial" || treeName == "initialparticles" || treeName == "initialstates") {
      Info("FetchTree","Returning the InitialParticles Tree");
      return fInitialParticles;
   } else if (treeName == "propagating" || treeName == "propagatingparticles" || treeName == "propagatingstates") {
      Info("FetchTree","Returning the PropagatingParticles Tree");
      return fPropagatingParticles;
   } else if (treeName == "decayed" || treeName == "decayedparticles" || treeName == "decayedstates") {
      Info("FetchTree","Returning the DecayedParticles Tree");
      return fDecayedParticles;
   } else if (treeName == "detected" || treeName == "detectedparticles" || treeName == "detectedstates") {
      Info("FetchTree","Returning the DetectedParticles Tree");
      return fDetectedParticles;
   } else if (treeName == "absorbed" || treeName == "absorbedparticles" || treeName == "absorbedstates") {
      Info("FetchTree","Returning the AbsorbedParticles Tree");
      return fAbsorbedParticles;
   } else if (treeName == "lost" || treeName == "lostparticles" || treeName == "loststates") {
      Info("FetchTree","Returning the LostParticles Tree");
      return fLostParticles;
   } else if (treeName == "bad" || treeName == "badparticles" || treeName == "badstates") {
      Info("FetchTree","Returning the BadParticles Tree");
      return fBadParticles;
   } else {
      Error("FetchTree","Input TreeName: %s does not match any listed in Data",treeName.Data());
      return 0;
   }
}

//_____________________________________________________________________________
void TUCNData::PlotObservers(TTree* tree)
{
   // Pass Tree of particles to each observer so that they can make a plot of their results
   vector<TUCNObserver*>::iterator obsIter;
   for (obsIter = fObservers.begin(); obsIter != fObservers.end(); obsIter++) {
      (*obsIter)->Plot(this, tree);
   }
}

*/