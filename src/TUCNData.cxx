// UCNData.cpp
// Author: Simon JM Peeters
// Created July 2009

#include "TUCNData.h"

#include "TTree.h"
#include "TUCNParticle.h"

#include <iostream>
#include <cassert>

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
   fInputFile = NULL;
   fOutputFile = NULL;
   fCurrentDir = NULL;
   fNextKey = NULL;
}

//_____________________________________________________________________________
TUCNData::TUCNData(const TUCNInitialConfig& initialConfig)
         :TNamed("","")
{
   // -- Constructor
   Info("TUCNData","Constructor");
   this->SetName(initialConfig.RunName().c_str());
   fInputFile = NULL;
   fCurrentDir = NULL;
   fNextKey = NULL;
   
   // -- Fetch the OutputFile name
   const string outputFileName = initialConfig.OutputFileName();
   if (outputFileName == "") { 
      throw runtime_error("Error: No Output file path specified in: %s", outputFileName.c_str());
   }
   // -- Open and store pointer to File
   TFile *file = TFile::Open(outputFileName, "recreate");
   if (!file || file->IsZombie()) {
      throw runtime_error("Error: Unable to open file: %s", outputFileName.c_str());
   }
   fOutputFile = file;
   // -- Set up basic folder structure
   // -- ___.root:/particles/initialstates  ___.root:/particles/finalstates
   TDirectory* topDir = fOutputFile->cd();
   if (topDir->cd(Folders::particles.c_str()) == kFALSE) {
      // Need to create particles directory structure
      TDirectory* particlesDir = topDir->mkdir(Folders::particles.c_str());
      // Create the initial and final states folders
      fInitialStatesFolder = particlesDir->mkdir(Folders::initialstates.c_str());
      fFinalStatesFolder = particlesDir->mkdir(Folders::finalstates.c_str());
      // Create the typical subfolders of final states
      fFinalStatesFolder->mkdir(Folders::propagating);
      fFinalStatesFolder->mkdir(Folders::absorbed);
      fFinalStatesFolder->mkdir(Folders::detected);
      fFinalStatesFolder->mkdir(Folders::decayed);
      fFinalStatesFolder->mkdir(Folders::lost);
      fFinalStatesFolder->mkdir(Folders::bad);
   }
}

//_____________________________________________________________________________
TUCNData::TUCNData(const TUCNRunConfig& runConfig)
         :TNamed("","")
{
   // -- Constructor
   Info("TUCNData","Constructor");
   this->SetName(runConfig.RunName().c_str());
   fCurrentDir = NULL;
   fNextKey = NULL;
   // -- Fetch the Input and Output File name
   const string inputFileName = runConfig.InputFileName();
   if (inputFileName == "") { 
      throw runtime_error("Error: No Input file path specified in: %s", inputFileName.c_str());
   }
   const string outputFileName = runConfig.OutputFileName();
   if (outputFileName == "") { 
      throw runtime_error("Error: No Output file path specified in: %s", outputFileName.c_str());
   }
   // -- Open and store pointer to input and output File
   TFile *inputfile = TFile::Open(inputFileName, "read");
   if (!inputfile || inputfile->IsZombie()) {
      throw runtime_error("Error: Unable to open file: %s", inputFileName.c_str());
   }
   fInputFile = inputfile;
   TFile *outputfile = TFile::Open(outputFileName, "recreate");
   if (!outputfile || outputfile->IsZombie()) {
      throw runtime_error("Error: Unable to open file: %s", outputFileName.c_str());
   }
   fOutputFile = outputfile;
   // -- Set up basic folder structure
   // -- ___.root:/particles/initialstates  ___.root:/particles/finalstates
   TDirectory* topDir = fOutputFile->cd();
   if (topDir->cd(Folders::particles.c_str()) == kFALSE) {
      // Need to create particles directory structure
      TDirectory* particlesDir = topDir->mkdir(Folders::particles.c_str());
      // Create the initial and final states folders
      fInitialStatesFolder = particlesDir->mkdir(Folders::initialstates.c_str());
      fFinalStatesFolder = particlesDir->mkdir(Folders::finalstates.c_str());
      // Create the typical subfolders of final states
      fFinalStatesFolder->mkdir(Folders::propagating);
      fFinalStatesFolder->mkdir(Folders::absorbed);
      fFinalStatesFolder->mkdir(Folders::detected);
      fFinalStatesFolder->mkdir(Folders::decayed);
      fFinalStatesFolder->mkdir(Folders::lost);
      fFinalStatesFolder->mkdir(Folders::bad);
   }
}

//_____________________________________________________________________________
TUCNData::TUCNData(const TUCNData& other)
         :TNamed(other), 
          fInputFile(other.fInputFile),
          fOutputFile(other.fOutputFile),
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
      if (fInputFile) {
         fInputFile->Close();
         delete fInputFile;
         fInputFile = NULL;
      }
      fInputFile = other.fInputFile;
      // Clean up File before assignment
      if (fOutputFile) {
         fOutputFile->Close();
         delete fOutputFile;
         fOutputFile = NULL;
      }
      fOutputFile = other.fOutputFile;
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
   if (fInputFile) fInputFile->Close(); delete fInputFile;
   if (fOutputFile) fOutputFile->Close(); delete fOutputFile;
   PurgeObservers();
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
Bool_t TUCNData::SaveParticle(const string& state, TUCNParticle* particle)
{
   return kTRUE;
}

//_____________________________________________________________________________
TUCNParticle* const TUCNData::RetrieveParticle(const string& state, const Int_t index)
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