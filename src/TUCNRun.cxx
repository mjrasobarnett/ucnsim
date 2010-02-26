// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#include <iostream>
#include <cassert>
#include <vector>
#include <stdexcept>

#include "TUCNRun.h"
#include "TUCNConfigFile.h"
#include "TUCNGeoMaterial.h"
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
using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(TUCNRun)

const Double_t 	TUCNRun::fgTolerance;


//_____________________________________________________________________________
TUCNRun::TUCNRun()
		  :TNamed("Run", "Default Run")
{
// -- Default constructor
   Info("TUCNRun", "Default Constructor");
	// Create the data object
	fData = new TUCNData("ucndata", "ucndata");
	fNeutrons = 0;
	fTotalEnergy = 0.0;
	fRunTime = 0.0;
	fMaxStepTime = 0.0;
	fDiffuseCoeff = 0;
	fSampleMagField = kFALSE;
	fWallLosses = kFALSE;
	fBoundaryLossCounter = 0;
	fDetectedCounter = 0;
	fDecayedCounter = 0;
	
	fUCNNextNode = 0;
	fStepTime = 1.0;
	fUCNIsStepEntering = kFALSE;
	fUCNIsStepExiting = kFALSE;
	fUCNIsOutside = kFALSE;
	fUCNIsOnBoundary = kFALSE;
	for (Int_t i=0; i<3; i++) {
		fUCNNormal[i] = 0.;
	}
} 

//_____________________________________________________________________________
TUCNRun::TUCNRun(const char *name, const char *title)
		  :TNamed(name, title)
{
// -- Default constructor
   Info("TUCNRun", "Constructor");
	// Create the data object
	fData = new TUCNData("ucndata", "ucndata");
	fNeutrons = 0;
	fTotalEnergy = 0.0;
	fRunTime = 0.0;
	fMaxStepTime = 0.0;
	fDiffuseCoeff = 0;
	fSampleMagField = kFALSE;
	fWallLosses = kFALSE;
	fBoundaryLossCounter = 0;
	fDetectedCounter = 0;
	fDecayedCounter = 0;
	
	fUCNNextNode = 0;
	fStepTime = 1.0;
	fUCNIsStepEntering = kFALSE;
	fUCNIsStepExiting = kFALSE;
	fUCNIsOutside = kFALSE;
	fUCNIsOnBoundary = kFALSE;
	for (Int_t i=0; i<3; i++) {
		fUCNNormal[i] = 0.;
	}
}

//_____________________________________________________________________________
TUCNRun::TUCNRun(const TUCNRun& run)
		  :TNamed(run),
			fData(run.fData),
			fNeutrons(run.fNeutrons),
			fTotalEnergy(run.fTotalEnergy),
			fRunTime(run.fRunTime),
			fMaxStepTime(run.fMaxStepTime),
			fDiffuseCoeff(run.fDiffuseCoeff),
			fSampleMagField(run.fSampleMagField),
			fWallLosses(run.fWallLosses),
			fBoundaryLossCounter(run.fBoundaryLossCounter),
			fDetectedCounter(run.fDetectedCounter),
			fDecayedCounter(run.fDecayedCounter),
			fUCNNextNode(run.fUCNNextNode),
			fStepTime(run.fStepTime),
			fUCNIsStepEntering(run.fUCNIsStepEntering),
         fUCNIsStepExiting(run.fUCNIsStepExiting),
			fUCNIsOutside(run.fUCNIsOutside),
			fUCNIsOnBoundary(run.fUCNIsOnBoundary)
{
// -- Copy Constructor
	Info("TUCNRunManager", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNRun& TUCNRun::operator=(const TUCNRun& run)
{
// --assignment operator
	if(this!=&run) {
		TNamed::operator=(run);
      fData = run.fData;
		fNeutrons = run.fNeutrons;
		fTotalEnergy = run.fTotalEnergy;
		fRunTime = run.fRunTime;
		fMaxStepTime = run.fMaxStepTime;
		fDiffuseCoeff = run.fDiffuseCoeff;
		fSampleMagField = run.fSampleMagField;
		fWallLosses = run.fWallLosses;
		fBoundaryLossCounter = run.fBoundaryLossCounter;
		fDetectedCounter = run.fDetectedCounter;
		fDecayedCounter = run.fDecayedCounter;
		fUCNNextNode = run.fUCNNextNode;
		fStepTime = run.fStepTime;
		fUCNIsStepEntering = run.fUCNIsStepEntering;
		fUCNIsStepExiting = run.fUCNIsStepExiting;
		fUCNIsOutside = run.fUCNIsOutside;
		fUCNIsOnBoundary = run.fUCNIsOnBoundary;
		for (Int_t i=0; i<3; i++) {
			fUCNNormal[i] = run.fUCNNormal[i];
		}
	}
   return *this;
}

//______________________________________________________________________________
TUCNRun::~TUCNRun()
{
// -- Destructor
	Info("TUCNRun", "Destructor");
	fData->Delete();
}

//_____________________________________________________________________________
Bool_t TUCNRun::Initialise(TUCNConfigFile* configFile)
{
	// Initialise the Run
	cout << "-------------------------------------------" << endl;
	cout << "Initialising: " << this->GetName() << endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// Store the run parameters
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Number of particles
	fNeutrons = TMath::Abs(configFile->GetInt("Neutrons", this->GetName()));
	if (fNeutrons == 0) { Warning("Initialise","No number of particles has been set"); return kFALSE; }
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Get the FermiPotential of the Wall 
	// !!!! This is ugly and unsafe. Needs a work around. Perhaps get the name of the boundary material from configFile
	TUCNGeoMaterial* boundaryMaterial = static_cast<TUCNGeoMaterial*>(gGeoManager->GetMaterial("BoundaryMaterial"));
	if (boundaryMaterial == NULL) { Error("Initialise","BoundaryMaterial cannot be found"); return kFALSE; }
	Double_t fermiV = boundaryMaterial->FermiPotential(); 
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Define Neutrons' Initial Energy
	// Get the initial energy in units of (neV)
	Double_t totalEnergyneV = TMath::Abs(configFile->GetFloat("InitialEnergy(neV)", this->GetName()))*Units::neV;
	// Get the initial energy in units of the FermiPotential of the boundary
	Double_t totalEnergyVFermi = TMath::Abs(configFile->GetFloat("InitialEnergy(VFermi)", this->GetName()))*fermiV;
	// Determine what to do based on which value was set in the config file
	if (totalEnergyneV != 0.0) {
		fTotalEnergy = totalEnergyneV;
	} else if (totalEnergyVFermi != 0.0) {
		fTotalEnergy = totalEnergyVFermi;
	} else {
		Warning("Initialise","No initial energy has been set for this Run.");
		return kFALSE;
	}
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Run Time
	fRunTime = TMath::Abs(configFile->GetFloat("RunTime(s)", this->GetName()))*Units::s;
	if (fRunTime == 0.0) { Warning("Initialise","No RunTime has been set"); return kFALSE; }
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Max Step Time
	fMaxStepTime = TMath::Abs(configFile->GetFloat("MaxStepTime(s)", this->GetName()))*Units::s;
	if (fMaxStepTime == 0.0) { Warning("Initialise","No max step time has been set"); return kFALSE; }
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Diffuse Coefficient
	fDiffuseCoeff = TMath::Abs(configFile->GetFloat("DiffuseBounceCoefficient", this->GetName()));
	// -- Set material's roughness parameter to the Diffuse Coefficient
	boundaryMaterial->RoughnessCoeff(fDiffuseCoeff); 
	///////////////////////////////////////////////////////////////////////////////////////	
	// -- Determine if we will sample the magnetic field
	fSampleMagField = configFile->GetBool("SampleMagField", this->GetName());
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Determine if we will sample the magnetic field
	fWallLosses = configFile->GetBool("WallLosses", this->GetName());
	if (fWallLosses == kFALSE) {
		boundaryMaterial->WPotential(0.0); 
	} else {
		Double_t W = TMath::Abs(configFile->GetFloat("WPotential(neV)", "Geometry"))*Units::neV;
		boundaryMaterial->WPotential(W);
	}
	///////////////////////////////////////////////////////////////////////////////////////
	cout << "Particles: " << fNeutrons << endl;
	cout << "Total Energy(neV): " << fTotalEnergy/Units::neV << endl;
	cout << "RunTime(s): " << fRunTime << endl;
	cout << "MaxStepTime(s): " << fMaxStepTime << endl;
	cout << "DiffuseCoeff: " << fDiffuseCoeff << endl;
	cout << "SampleMagField: " << fSampleMagField << endl;
	cout << "WallLosses: " << fWallLosses << endl;
	cout << "-------------------------------------------" << endl;
	cout << "Run successfully initialised" << endl;
	cout << "-------------------------------------------" << endl;
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::Propagate(TGeoManager* geoManager, TUCNFieldManager* fieldManager)
{
// -- Propagate all tracks stored in the GeoManager for a set period of time
	Int_t numberOfTracks = geoManager->GetNtracks();
	// Container to store Ids of lost tracks;
	vector<Int_t> lostTracks;

	for (Int_t trackid = 0; trackid < numberOfTracks; trackid++) {
		// Get Track from list
		TVirtualGeoTrack* track = geoManager->GetTrack(trackid);
		// Set Current Track
		geoManager->SetCurrentTrack(track);
		// Store the random number seed in particle
		static_cast<TUCNParticle*>(track->GetParticle())->SetInitialSeed(gRandom->GetSeed());
		// Add Initial Particle State to data tree
		this->AddInitialParticle(static_cast<TUCNParticle*>(track->GetParticle()));
		// Propagate track
		Bool_t propagated = this->PropagateTrack(geoManager, fieldManager);
		if (!propagated) {
			lostTracks.push_back(trackid);
		}
		// Add Final Particle State to data tree
		this->AddParticle(static_cast<TUCNParticle*>(track->GetParticle()));
		// Reset Track to release memory
		track->ResetTrack();
	}
	
	if (lostTracks.size() != 0) {
		cout << "-------------------------------------------" << endl;
		cout << "Erroneously Lost Tracks: " << lostTracks.size() << endl;
		for (UInt_t i = 0; i < lostTracks.size(); i++) {
			cout << "Id: " << lostTracks[i] << endl;
		}
		cout << "-------------------------------------------" << endl;
	}
	
	cout << "-------------------------------------------" << endl;
	cout << "Propagation Results: " << endl;
	cout << "Total Particles: " << this->Neutrons() << endl;
	cout << "Number Detected: " << this->Detected() << endl;
	cout << "Number Lost To Boundary: " << this->LostToBoundary() << endl;
	cout << "Number Decayed: " << this->Decayed() << endl;
	cout << "-------------------------------------------" << endl;
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::PropagateTrack(TGeoManager* geoManager, TUCNFieldManager* fieldManager)
{
	// Propagate track through geometry until it is either stopped or the runTime has been reached
	// Track passed MUST REFERENCE A TUCNPARTICLE as its particle type. UNITS:: runTime, stepTime in Seconds
	
	// -- Get the 
	TVirtualGeoTrack* track = geoManager->GetCurrentTrack();
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	TGeoNavigator* navigator = geoManager->GetCurrentNavigator();
	assert(track != NULL && particle != NULL );
	
	///////////////////////////////////////////////////////////////////////////////////////	
	// -- Get the Fields
	TUCNGravField* gravField = 0;
	TUCNMagField* magField = 0;
	gravField = fieldManager->GravField();
	// -- Check if we are to sample the mag field
	if (fSampleMagField == kTRUE) {
		magField = fieldManager->MagField();
		if (magField == NULL) {
			Error("PropagateTrack","Configuration asks to sample mag field, but no field can be found!");
			return kFALSE;
		}
	}
	
	///////////////////////////////////////////////////////////////////////////////////////	
	// -- 1. Initialise Track
	// Initialise track - Sets navigator's current point/direction/node to that of the particle
	geoManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
		particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
	
		#ifdef VERBOSE_MODE				
		cout << "PropagateForSetTime - Starting Run - Max time (seconds): " <<  fRunTime << endl;
	#endif
	
	// -- Check that Particle has not been initialised inside a boundary or detector		
	if (static_cast<TUCNGeoMaterial*>(navigator->GetCurrentNode()->GetMedium()->GetMaterial())->IsTrackingMaterial() == kFALSE) {
		cerr << "Track number " << track->GetId() << " initialised inside boundary of " << navigator->GetCurrentVolume()->GetName() << endl;
		return kFALSE;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////	
	// -- 2. Propagation Loop
	Int_t stepNumber;
	for (stepNumber = 1 ; ; stepNumber++) {
		
		#ifdef VERBOSE_MODE		
			cout << "STEP " << stepNumber << "\t" << particle->T() << " s" << "\t" << navigator->GetCurrentNode()->GetName() << endl;	
		#endif
			
		// -- Calculate the Next StepTime
		this->DetermineNextStepTime(particle, fMaxStepTime, fRunTime);
		
		// -- Make a step
		if (!(this->MakeStep(track, gravField, magField))) {
			Error("PropagateTrack","Error in Step. Step Failed to complete.");
			return kFALSE;	
		} 
		// -- Determine Particle destination
		// Has lost flag been set?
		if (particle->LostToBoundary() == kTRUE) {
			fBoundaryLossCounter++;
			break; // -- End Propagtion Loop
		// Has detected flag been set?
		} else if (particle->Detected() == kTRUE) {
			fDetectedCounter++;
			break; // -- End Propagation Loop
		// Has particle decayed within steptime?
		} else if (particle->WillDecay(this->GetStepTime()) == kTRUE) {
			fDecayedCounter++;
			particle->Decayed(kTRUE); // Set Decay Flag
			break; // -- End Propagation Loop
		// -- Have we reached the maximum runtime?
		} else if (particle->T() >= fRunTime) {
			break; // -- End Propagation Loop
		}
	}	
	// -- END OF PROPAGATION LOOP
//	cout << "FINAL STATUS: " << "Track: " << track->GetId() << "\t" << "Steps taken: " << stepNumber << "\t";
//	cout << "Time: " << particle->T() << "s" << "\t" << "Final Medium: " << navigator->GetCurrentNode()->GetMedium()->GetName() << "\t";
//	cout << "Bounces: " << particle->Bounces() << "\t" << "Diffuse: " << particle->DiffuseBounces() << "\t" << "Specular: " << particle->SpecularBounces() << endl;

	return kTRUE;
}

//_____________________________________________________________________________
void TUCNRun::DrawParticles(TCanvas* canvas, TPolyMarker3D* points)
{
	// -- Draw Volumes
	canvas->Draw();
	gGeoManager->GetTopVolume()->Draw();
	gGeoManager->SetVisLevel(4);
	gGeoManager->SetVisOption(0);
	
	// -- Number of particles
	Int_t particles = this->Neutrons();
	
	// -- Draw Particles
	for (Int_t i = 0; i < particles; i++) {
		TUCNParticle* particle = this->GetParticle(i);
		assert(particle != NULL);
		points->SetPoint(i, particle->Vx(), particle->Vy(), particle->Vz());
	}
	
	points->SetMarkerColor(2);
	points->SetMarkerStyle(6);
	points->Draw();
}

//_____________________________________________________________________________
void	TUCNRun::DrawTrack(TCanvas* canvas, Int_t trackID)
{
	canvas->Draw();
	// -- Draw Volumes
	gGeoManager->GetTopVolume()->Draw();
	gGeoManager->SetVisLevel(4);
	gGeoManager->SetVisOption(0);
	// -- Draw Track
	TVirtualGeoTrack* track = this->GetTrack(trackID);
	assert(track != NULL);
	track->Draw();
}

//_____________________________________________________________________________
Bool_t TUCNRun::Export(TString& outputFile)
{
// -- Export this run to file
	cout << "-------------------------------------------" << endl;
	cout << "Writing " << this->GetName() << " out to File: " << outputFile << endl;
	// -- Check that Data 'checks out'
	if (this->GetData()->ChecksOut() == kFALSE) {
		Error("Export", "TUCNData doesn't check out. Cannot Export Run to file.");
		return kFALSE;
	}
	// -- Check we have a .root file
	if (!(outputFile.Contains(".root"))) {
		Error("Export", "OutputFile is not a ROOT filename");
		return kFALSE;
	} 
	// -- Write out
	TFile *f = TFile::Open(outputFile,"update");
	if (!f || f->IsZombie()) {
	   Error("Export","Cannot open file");
	   return kFALSE;
	}
	char keyname[256];
	strcpy(keyname,this->GetName());
	this->Write(keyname);
	cout << "Run: " << keyname << " was successfully written to file" << endl;
	cout << "-------------------------------------------" << endl;
	// -- Clean up
	delete f;
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::AddTrack(TGeoTrack* track)
{
	return this->GetData()->AddTrack(track);
}

//_____________________________________________________________________________
Bool_t TUCNRun::AddInitialParticle(TUCNParticle* particle) 
{
	return this->GetData()->AddInitialParticleState(particle);
}

//_____________________________________________________________________________
Bool_t TUCNRun::AddParticle(TUCNParticle* particle)
{
	return this->GetData()->AddFinalParticleState(particle);
}

//_____________________________________________________________________________
TGeoTrack* TUCNRun::GetTrack(Int_t trackID)
{
	// -- Retrieve the requested track from the Data
	return this->GetData()->GetTrack(trackID);
}

//_____________________________________________________________________________
TUCNParticle* TUCNRun::GetParticle(Int_t particleID)
{
	// -- Retrieve the requested particle from the Data
	return this->GetData()->GetFinalParticleState(particleID);
}

//_____________________________________________________________________________
TUCNParticle* TUCNRun::GetInitialParticle(Int_t particleID)
{
	// -- Retrieve the requested initial particle state from the Data
	return this->GetData()->GetInitialParticleState(particleID);
}








//_____________________________________________________________________________
Double_t* TUCNRun::FindUCNNormal()
{
// Computes fast normal to next crossed boundary, assuming that the current point
// is close enough to the boundary. Works only after calling FindNextBoundaryAndStepAlongParabola.
// This method must be used exclusively instead of TGeoNavigator::FindNormal when gravity is present.
	if (fUCNNextNode == NULL) return 0;
   Double_t local[3];
   Double_t ldir[3];
   Double_t lnorm[3];
   gGeoManager->GetCurrentNavigator()->GetHMatrix()->MasterToLocal(gGeoManager->GetCurrentNavigator()->GetCurrentPoint(), local);
   gGeoManager->GetCurrentNavigator()->GetHMatrix()->MasterToLocalVect(gGeoManager->GetCurrentNavigator()->GetCurrentDirection(), ldir);
   fUCNNextNode->GetVolume()->GetShape()->ComputeNormal(local, ldir, lnorm);
   gGeoManager->GetCurrentNavigator()->GetHMatrix()->LocalToMasterVect(lnorm, fUCNNormal);
	return fUCNNormal;
}

//_____________________________________________________________________________
TGeoNode* TUCNRun::FindNextBoundaryAlongParabola(TVirtualGeoTrack* track, TUCNGravField* field, Double_t stepTime, Bool_t onBoundary)
{
// Find the time it takes a particle to reach the next nearest boundary and store it fTimeStep.
// Return the node to which this boundary belongs. If the time to the boundary is found to be
// larger than the stepTime specified, then return the current node and set the fTimeStep to
// the stepTime. 

	// -- Get the track's particle
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	// -- Get the current Navigator
	TGeoNavigator* navigator = gGeoManager->GetCurrentNavigator();
	
	// -- Get the global field	
	Double_t globalField[3] 	 = {field->Gx(), field->Gy(), field->Gz()}; 
	Double_t globalPoint[3] 	 = {particle->Vx(), particle->Vy(), particle->Vz()};
	Double_t globalVelocity[3]  = {particle->VelocityX(), particle->VelocityY(), particle->VelocityZ()};
	
	// Compute maximum stepsize
	const Double_t stepMax = TUCNParabola::Instance()->ArcLength(globalVelocity, globalField, stepTime);
	
	#ifdef VERBOSE_MODE
		cout << "FNBAP - Starting StepTime: " << this->GetStepTime() << "\t" << "Starting StepSize: " << navigator->GetStep() << endl;
		cout << "FNBAP - Velocity: " << particle->Velocity() << endl;
		cout << "FNBAP - Global Field: X: " << globalField[0] << "\t" << "Y: " << globalField[1] << "\t" << "Z: " << globalField[2] << endl;	
		cout << "FNBAP - Global Point: X: " << globalPoint[0] << "\t" << "Y: " << globalPoint[1] << "\t" << "Z: " << globalPoint[2] << endl;	
		cout << "FNBAP - Global Velocity: X: " << globalVelocity[0] << "\t" << "Y: " << globalVelocity[1] << "\t" << "Z: " << globalVelocity[2] << endl;	
	#endif
	
	// Set initial step to be large
	navigator->SetStep(TGeoShape::Big());

   // Initialise ROOT flags and initial nodes
   Int_t iact = 3;
   Int_t nextDaughterIndex = -2;
	
	fUCNIsStepEntering = kFALSE; // flag for whether we are about to enter a new volume
   fUCNIsStepExiting = kFALSE; // flag for whether we are about to exit the current volume
   
	Bool_t computeGlobal = kFALSE;
   
	fUCNIsOnBoundary = onBoundary; // flag for whether we are sitting on a boundary
   
	Double_t safety = 0.;
  
	TGeoNode *top_node = gGeoManager->GetTopNode();
   TGeoVolume *top_volume = top_node->GetVolume();
   
	// -- Work out the safety distance and decide whether we need to calculate the full
	// -- distances to boundaries, or whether we will definitely remain within the current vol
	if (stepMax<1E29) {
      if (stepMax <= 0) {
			Error("FindNextBoundaryAlongParabola","StepSize proposed is negative.");
			return 0;
      }
		// Calculate the safety distance
      safety = navigator->Safety();
      safety = TMath::Abs(safety);
      // Check if we are on a boundary
		if (safety < TGeoShape::Tolerance()) fUCNIsOnBoundary = kTRUE;
      else fUCNIsOnBoundary = kFALSE;
      // Update stepTime / stepSize
		navigator->SetStep(stepMax);
		this->SetStepTime(stepTime);
		#ifdef VERBOSE_MODE		
			cout << "FNBAP - fTimeStep: " << this->GetStepTime() << "\t" << "fStep: " << navigator->GetStep() << endl;
		#endif
		// If we are within the safety distance, return the current node
      if (stepMax + TGeoShape::Tolerance() < safety) {
         fUCNNextNode = navigator->GetCurrentNode();
			return navigator->GetCurrentNode();
      }   
   }
	
	// -- Initialise the storage for local coordinates and for the distance and times to boundary
   Double_t snext = TGeoShape::Big(); // snext is the distance to the nearest boundary
	Double_t tnext = TGeoShape::Big(); // tnext is the time to the nearest boundary
   Double_t localPoint[3] = {0.,0.,0.};
   Double_t localVelocity[3] = {0.,0.,0.};
	Double_t localField[3] = {0.,0.,0.};
	
   // -- Compute distance and time to the closest boundary of the current node
   // -- If point is outside (outside geometry), just check the top node
   if (navigator->IsOutside()) {
		Error("FindNextBoundaryAlongParabola","Point is Outside");
		TUCNGeoBBox* topShape = static_cast<TUCNGeoBBox*>(top_volume->GetShape());
		tnext = topShape->TimeFromInsideAlongParabola(globalPoint, globalVelocity, globalField, navigator->GetStep());
		topShape = 0; 
		snext = TUCNParabola::Instance()->ArcLength(globalVelocity, globalField, tnext);
		fUCNNextNode = top_node;
      if (snext < (navigator->GetStep() - TGeoShape::Tolerance())) {
         fUCNIsStepEntering = kTRUE;
         navigator->SetStep(snext);
			this->SetStepTime(tnext);
         #ifdef VERBOSE_MODE		
				cout << "FNBAP - fTimeStep: " << this->GetStepTime() << "\t" << "fStep: " << navigator->GetStep() << endl;
			#endif
			Int_t indnext = fUCNNextNode->GetVolume()->GetNextNodeIndex();
         nextDaughterIndex = indnext;
         while (indnext>=0) {
            fUCNNextNode = fUCNNextNode->GetDaughter(indnext);
            indnext = fUCNNextNode->GetVolume()->GetNextNodeIndex();
         }
         return fUCNNextNode;
      }
      return 0;
   }

	// -- Get the local coordinates
   navigator->GetCurrentMatrix()->MasterToLocal(&globalPoint[0], &localPoint[0]);
   navigator->GetCurrentMatrix()->MasterToLocalVect(&globalVelocity[0], &localVelocity[0]);
   navigator->GetCurrentMatrix()->MasterToLocalVect(&globalField[0], &localField[0]);
   TGeoVolume *currentVol = navigator->GetCurrentNode()->GetVolume();
   
   // -- Find time and distance to exiting current node
	#ifdef VERBOSE_MODE		
		cout << "FNBAP - Find distance to exit current Volume: " << currentVol->GetName() << endl;
	#endif
	
	TUCNGeoBBox* currentShape = static_cast<TUCNGeoBBox*>(currentVol->GetShape());
   tnext = currentShape->TimeFromInsideAlongParabola(localPoint, localVelocity, localField, navigator->GetStep()); 
	currentShape = 0;
	snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
   // -- Check if we are within the safety region
	if (snext < (navigator->GetStep() - TGeoShape::Tolerance())) {
      fUCNNextNode = navigator->GetCurrentNode();
      nextDaughterIndex = -1;
      fUCNIsStepExiting  = kTRUE;
      fUCNIsStepEntering = kFALSE;
      navigator->SetStep(snext);
		this->SetStepTime(tnext);
		#ifdef VERBOSE_MODE		
			cout << "FNBAP - fTimeStep: " << this->GetStepTime() << "\t" << "fStep: " << navigator->GetStep() << endl;
		#endif
		// I believe we are saying that if the step size is very small ( < 1E-6) then return the current node
		// as we must be hitting this boundary. I am not sure about this...
      if (navigator->GetStep() < 1E-6) return navigator->GetCurrentNode();
   }
	// If the current step size is reasonable, then set set next node to current node
	fUCNNextNode = (navigator->GetStep() < 1E20) ? navigator->GetCurrentNode():0;
	
   // -- Find next daughter boundary for the current volume
   Int_t idaughter = -1;
   this->FindNextDaughterBoundaryAlongParabola(localPoint,localVelocity,localField,idaughter,computeGlobal);
   if (idaughter >= 0) nextDaughterIndex = idaughter;
   
	TGeoNode *currentNode = 0;
   TGeoNode *daughterNode = 0;
   TGeoVolume *motherVolume = 0;
   // if we are in an overlapping node, check also the mother(s)
   if (navigator->GetNmany()) {
      #ifdef VERBOSE_MODE		
			cout << "FNBAP - In overlapping node" << endl;
		#endif
		Double_t motherPoint[3], motherVelocity[3], motherField[3];
      Double_t daughterPoint[3], daughterVelocity[3], daughterField[3];
      Int_t novlps;
      Int_t idovlp = -1;
      Int_t safelevel = navigator->GetSafeLevel();
      navigator->PushPath(safelevel+1);
      while (navigator->IsCurrentOverlapping() == kTRUE) {
         Int_t *ovlps = navigator->GetCurrentNode()->GetOverlaps(novlps);
         navigator->CdUp();
         motherVolume = navigator->GetCurrentNode()->GetVolume();
         navigator->GetCurrentMatrix()->MasterToLocal(&globalPoint[0], &motherPoint[0]);
         navigator->GetCurrentMatrix()->MasterToLocalVect(&globalVelocity[0], &motherVelocity[0]);
         navigator->GetCurrentMatrix()->MasterToLocalVect(&globalField[0], &motherField[0]);
         // check distance to out
         snext = TGeoShape::Big();
         if (!motherVolume->IsAssembly()) {
				TUCNGeoBBox* motherShape = static_cast<TUCNGeoBBox*>(motherVolume->GetShape());
				tnext = motherShape->TimeFromInsideAlongParabola(motherPoint, motherVelocity, motherField, navigator->GetStep()); 
				motherShape = 0;
				snext = TUCNParabola::Instance()->ArcLength(motherVelocity, motherField, tnext);
         }
			// check if distance to exit is less than the proposed stepsize
			if (snext < navigator->GetStep() - TGeoShape::Tolerance()) {
            fUCNIsStepExiting  = kTRUE;
            fUCNIsStepEntering = kFALSE;
            navigator->SetStep(snext);
				this->SetStepTime(tnext);
				#ifdef VERBOSE_MODE		
					cout << "FNBAP - fTimeStep: " << this->GetStepTime() << "\t" << "fStep: " << navigator->GetStep() << endl;
				#endif
				fUCNNextNode = navigator->GetCurrentNode();
            nextDaughterIndex = -3;
            navigator->DoBackupState();
         }
         // check overlapping nodes
         for (Int_t i=0; i<novlps; i++) {
            currentNode = motherVolume->GetNode(ovlps[i]);
            if (!currentNode->IsOverlapping()) {
               currentNode->cd();
               currentNode->MasterToLocal(&motherPoint[0], &daughterPoint[0]);
               currentNode->MasterToLocalVect(&motherVelocity[0], &daughterVelocity[0]);
               currentNode->MasterToLocalVect(&motherField[0], &daughterField[0]);
               // Current point may be inside the other node - geometry error that we ignore
               snext = TGeoShape::Big();
               if (!currentNode->GetVolume()->Contains(daughterPoint)) {
                  TUCNGeoBBox* shape = static_cast<TUCNGeoBBox*>(currentNode->GetVolume()->GetShape());
						tnext = shape->TimeFromInsideAlongParabola(daughterPoint, daughterVelocity, daughterField, navigator->GetStep()); 
						shape = 0;
						snext = TUCNParabola::Instance()->ArcLength(daughterVelocity, daughterField, tnext);
					}
               // Determine if distance to node is within current stepsize
					if (snext < navigator->GetStep() - TGeoShape::Tolerance()) {
                  fUCNIsStepExiting  = kTRUE;
                  fUCNIsStepEntering = kFALSE;
                  navigator->SetStep(snext);
						this->SetStepTime(tnext);
                  #ifdef VERBOSE_MODE		
							cout << "FNBAP - fTimeStep: " << this->GetStepTime() << "\t" << "fStep: " << navigator->GetStep() << endl;
						#endif
						fUCNNextNode = currentNode;
                  nextDaughterIndex = -3;
                  navigator->CdDown(ovlps[i]);
                  navigator->DoBackupState();
                  navigator->CdUp();
               }
            } else {
               // another many - check if point is in or out
               currentNode->cd();
               currentNode->MasterToLocal(&motherPoint[0], &daughterPoint[0]);
               currentNode->MasterToLocalVect(&motherVelocity[0], &daughterVelocity[0]);
               currentNode->MasterToLocalVect(&motherField[0], &daughterField[0]);
               // Check if point is inside
					if (currentNode->GetVolume()->Contains(daughterPoint)) {
                  if (currentNode->GetVolume()->GetNdaughters()) {
                     navigator->CdDown(ovlps[i]);
                     fUCNIsStepEntering  = kFALSE;
                     fUCNIsStepExiting  = kTRUE;
                     daughterNode = this->FindNextDaughterBoundaryAlongParabola(daughterPoint,daughterVelocity,daughterField,idaughter,computeGlobal);
                     if (daughterNode) {
                        fUCNNextNode = daughterNode;
                        nextDaughterIndex = -3;
                        navigator->CdDown(idovlp);
                        Int_t indnext = navigator->GetCurrentNode()->GetVolume()->GetNextNodeIndex();
                        Int_t iup=0;
                        while (indnext>=0) {
                           navigator->CdDown(indnext);
                           iup++;
                           indnext = navigator->GetCurrentNode()->GetVolume()->GetNextNodeIndex();
                        }
                        navigator->DoBackupState();
                        while (iup>0) {
                           navigator->CdUp();
                           iup--;
                        }   
                        navigator->CdUp();
                     }
                     navigator->CdUp();
                  }   
               } else {
               // Point is outside   
						TUCNGeoBBox* shape = static_cast<TUCNGeoBBox*>(currentNode->GetVolume()->GetShape());
						tnext = shape->TimeFromInsideAlongParabola(daughterPoint, daughterVelocity, daughterField, navigator->GetStep()); 
						shape = 0;
						snext = TUCNParabola::Instance()->ArcLength(daughterVelocity, daughterField, tnext);
                  // Determine if distance to node is within current stepsize
						if (snext < navigator->GetStep() - TGeoShape::Tolerance()) {
                     fUCNIsStepExiting  = kTRUE;
                     fUCNIsStepEntering = kFALSE;
							navigator->SetStep(snext);
							this->SetStepTime(tnext);
                     #ifdef VERBOSE_MODE		
								cout << "FNBAP - fTimeStep: " << this->GetStepTime() << "\t" << "fStep: " << navigator->GetStep() << endl;
							#endif
							fUCNNextNode = currentNode;
                     nextDaughterIndex = -3;
                     navigator->CdDown(ovlps[i]);
                     navigator->DoBackupState();
                     navigator->CdUp();
                  }               
               }  
            }
         }
      }
      
		// Now we are in a non-overlapping node
      if (navigator->GetNmany()) {
      // We have overlaps up in the branch, check distance to exit
         Int_t up = 1;
         Int_t imother;
         Int_t nmany = navigator->GetNmany();
         Bool_t ovlp = kFALSE;
         Bool_t nextovlp = kFALSE;
         Bool_t offset = kFALSE;
         currentNode = navigator->GetCurrentNode();
         TGeoNode *motherNode, *mup;
         TGeoHMatrix *matrix;
         while (nmany) {
            motherNode = navigator->GetMother(up);
            mup = motherNode;
            imother = up+1;
            offset = kFALSE;
            while (mup->IsOffset()) {
               mup = navigator->GetMother(imother++);
               offset = kTRUE;
            }   
            nextovlp = mup->IsOverlapping();
            if (offset) {
               motherNode = mup;
               if (nextovlp) nmany -= imother-up;
               up = imother-1;
            } else {    
               if (ovlp) nmany--;
            }   
            if (ovlp || nextovlp) {
               matrix = navigator->GetMotherMatrix(up);
               matrix->MasterToLocal(&globalPoint[0], &daughterPoint[0]);
               matrix->MasterToLocalVect(&globalVelocity[0], &daughterVelocity[0]);
               matrix->MasterToLocalVect(&globalField[0], &daughterField[0]);
					snext = TGeoShape::Big();
               if (!motherNode->GetVolume()->IsAssembly()) {
               	TUCNGeoBBox* motherShape = static_cast<TUCNGeoBBox*>(motherNode->GetVolume()->GetShape());
						tnext = motherShape->TimeFromInsideAlongParabola(daughterPoint, daughterVelocity, daughterField, navigator->GetStep()); 
						motherShape = 0;
						snext = TUCNParabola::Instance()->ArcLength(daughterVelocity, daughterField, tnext);
					}
					// Determine if distance to node is within current stepsize
					if (snext < navigator->GetStep() - TGeoShape::Tolerance()) {
                  fUCNIsStepEntering  = kFALSE;
                  fUCNIsStepExiting  = kTRUE;
                  navigator->SetStep(snext);
						this->SetStepTime(tnext);
                  #ifdef VERBOSE_MODE		
							cout << "FNBAP - fTimeStep: " << this->GetStepTime() << "\t" << "fStep: " << navigator->GetStep() << endl;
						#endif
						fUCNNextNode = motherNode;
                  nextDaughterIndex = -3;
                  while (up--) navigator->CdUp();
                  navigator->DoBackupState();
                  up = 1;
                  currentNode = navigator->GetCurrentNode();
                  ovlp = currentNode->IsOverlapping();
                  continue;
               }   
            }   
            currentNode = motherNode;
            ovlp = nextovlp;
            up++;            
         }
      }      
      navigator->PopPath();
   }
   return fUCNNextNode;
}

//_____________________________________________________________________________
TGeoNode* TUCNRun::FindNextDaughterBoundaryAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, Int_t &idaughter, Bool_t compmatrix)
{
// Computes as fStep the distance to next daughter of the current volume. 
// The point and direction must be converted in the coordinate system of the current volume.
// The proposed step limit is fStep.
	
	// -- First Get the current local and global fields
	Double_t motherField[3] = {field[0], field[1], field[2]}; 
	Double_t motherPoint[3] = {point[0], point[1], point[2]};
	Double_t motherVelocity[3] = {velocity[0], velocity[1], velocity[2]};
	
	#ifdef VERBOSE_MODE		
		cout << "FNDBAP - Mother Local Field: X: " << motherField[0] << "\t" << "Y: " << motherField[1] << "\t" << "Z: " << motherField[2] << endl;
	#endif
   
	// -- Initialising some important parameters
	Double_t tnext = TGeoShape::Big();
	Double_t snext = TGeoShape::Big();
   idaughter = -1; // nothing crossed
   TGeoNode *nodefound = 0;
	TGeoHMatrix* globalMatrix = gGeoManager->GetCurrentNavigator()->GetCurrentMatrix(); // This has been added because we do not have access to fGlobalMatrix in TGeoNavigator
	
   // Get number of daughters. If no daughters we are done.
   TGeoVolume *vol = gGeoManager->GetCurrentNavigator()->GetCurrentNode()->GetVolume();
   Int_t nd = vol->GetNdaughters();
   
	#ifdef VERBOSE_MODE		
		cout << "FNDBAP - Volume: " << vol->GetName() << "\t" << "Checking number of Daughters: " << nd << endl;
	#endif
	
	if (!nd) return 0;  // No daughter 
   if (gGeoManager->IsActivityEnabled() && !vol->IsActiveDaughters()) return 0;
   Double_t localPoint[3], localVelocity[3], localField[3];
   TGeoNode *current = 0;
   Int_t i=0;
   
	// ************************************************************************************
	// -- BRANCH 1
	// -- if current volume is divided, we are in the non-divided region. We
   // -- check first if we are inside a cell in which case compute distance to next cell
   // ************************************************************************************
	
   TGeoPatternFinder *finder = vol->GetFinder();
   if (finder) {
      Int_t ifirst = finder->GetDivIndex();
      Int_t ilast = ifirst+finder->GetNdiv()-1;
      current = finder->FindNode(motherPoint);
      if (current) {
         // Point inside a cell: find distance to next cell
         Int_t index = current->GetIndex();
         if ((index-1) >= ifirst) ifirst = index-1;
         else                     ifirst = -1;
         if ((index+1) <= ilast)  ilast  = index+1;
         else                     ilast  = -1;
      }
      if (ifirst>=0) {   
         current = vol->GetNode(ifirst);
         current->cd();
         current->MasterToLocal(&motherPoint[0], localPoint);
         current->MasterToLocalVect(&motherVelocity[0], localVelocity);
			current->MasterToLocalVect(&motherField[0], &localField[0]);
         // ***************************************************************************************************
			#ifdef VERBOSE_MODE		
				cout << "FNDBAP - Divided Volume. Calculate distance from outside to first cell." << endl;
				cout << "FNDBAP - Volume: " << current->GetName() << endl;
				cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t" << "Z: " << localField[2] << endl;
			#endif
			tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, gGeoManager->GetCurrentNavigator()->GetStep());  
			snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         if (snext < (gGeoManager->GetCurrentNavigator()->GetStep() - fgTolerance)) {
            if (compmatrix) {
               gGeoManager->GetCurrentNavigator()->GetHMatrix()->CopyFrom(globalMatrix);
               gGeoManager->GetCurrentNavigator()->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            gGeoManager->GetCurrentNavigator()->SetStep(snext);
				this->SetStepTime(tnext);
				fUCNNextNode = current;
            nodefound = current;
            idaughter = ifirst;
         }
      }   
      if (ilast==ifirst) return nodefound;
      if (ilast>=0) { 
         current = vol->GetNode(ilast);
         current->cd();
         current->MasterToLocal(&motherPoint[0], localPoint);
         current->MasterToLocalVect(&motherVelocity[0], localVelocity);
			current->MasterToLocalVect(&motherField[0], localField);
			// ***************************************************************************************************
			#ifdef VERBOSE_MODE		
				cout << "FNDBAP - Divided Volume. Calculate distance from outside to last cell." << endl;
				cout << "FNDBAP - Volume: " << current->GetName() << endl;
				cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t" << "Z: " << localField[2] << endl;
			#endif
			tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, gGeoManager->GetCurrentNavigator()->GetStep());
			snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         if (snext < (gGeoManager->GetCurrentNavigator()->GetStep() - fgTolerance)) {
            if (compmatrix) {
               gGeoManager->GetCurrentNavigator()->GetHMatrix()->CopyFrom(globalMatrix);
               gGeoManager->GetCurrentNavigator()->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            gGeoManager->GetCurrentNavigator()->SetStep(snext);
				this->SetStepTime(tnext);
				fUCNNextNode = current;
            nodefound = current;
            idaughter = ilast;
         }
      }   
      return nodefound;
   }
   // if only few daughters, check all and exit
   TGeoVoxelFinder *voxels = vol->GetVoxels();
   Int_t indnext;
   if (nd<5 || !voxels) {
      for (i=0; i<nd; i++) {
         current = vol->GetNode(i);
         if (gGeoManager->IsActivityEnabled() && !current->GetVolume()->IsActive()) continue;
         current->cd();
         if (voxels && voxels->IsSafeVoxel(motherPoint, i, gGeoManager->GetCurrentNavigator()->GetStep())) continue;
         current->MasterToLocal(motherPoint, localPoint);
         current->MasterToLocalVect(motherVelocity, localVelocity);
			current->MasterToLocalVect(motherField, localField);
         if (current->IsOverlapping() && current->GetVolume()->Contains(localPoint)) continue;
			// ***************************************************************************************************
         #ifdef VERBOSE_MODE		
				cout << "FNDBAP - Only Few daughters. Calculate distance from outside to all." << endl;
				cout << "FNDBAP - Volume: " << current->GetName() << endl;
				cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t" << "Z: " << localField[2] << endl;
			#endif
			tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, gGeoManager->GetCurrentNavigator()->GetStep());
			snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         if (snext < (gGeoManager->GetCurrentNavigator()->GetStep() - fgTolerance)) {
				#ifdef VERBOSE_MODE		
					cout << "FNDBAP - Distance to daughter is less than distance to exit current volume." << endl;
            #endif
				indnext = current->GetVolume()->GetNextNodeIndex();
				if (compmatrix) {
               gGeoManager->GetCurrentNavigator()->GetHMatrix()->CopyFrom(globalMatrix);
               gGeoManager->GetCurrentNavigator()->GetHMatrix()->Multiply(current->GetMatrix());
            }    
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            gGeoManager->GetCurrentNavigator()->SetStep(snext);
				this->SetStepTime(tnext);
				fUCNNextNode = current;
            nodefound = fUCNNextNode;   
            idaughter = i;   
            while (indnext>=0) {
               current = current->GetDaughter(indnext);
               if (compmatrix) gGeoManager->GetCurrentNavigator()->GetHMatrix()->Multiply(current->GetMatrix());
               fUCNNextNode = current;
               nodefound = current;
               indnext = current->GetVolume()->GetNextNodeIndex();
            }
         }
      }
		#ifdef VERBOSE_MODE		
			cout << "FNDBAP - Return nearest Node found if any." << endl;
      #endif
		return nodefound;
   }
   // if current volume is voxelized, first get current voxel
   Int_t ncheck = 0;
   Int_t sumchecked = 0;
   Int_t *vlist = 0;
   voxels->SortCrossedVoxels(motherPoint, motherVelocity);
   while ((sumchecked<nd) && (vlist=voxels->GetNextVoxel(motherPoint, motherVelocity, ncheck))) {
      for (i=0; i<ncheck; i++) {
         current = vol->GetNode(vlist[i]);
         if (gGeoManager->IsActivityEnabled() && !current->GetVolume()->IsActive()) continue;
         current->cd();
         current->MasterToLocal(motherPoint, localPoint);
         current->MasterToLocalVect(motherVelocity, localVelocity);
			current->MasterToLocalVect(&motherField[0], &localField[0]);
         if (current->IsOverlapping() && current->GetVolume()->Contains(localPoint)) continue;
			#ifdef VERBOSE_MODE		
				cout << "FNDBAP - Volume is voxelized. Calculate distance from outside to voxel." << endl;
				cout << "FNDBAP - Volume: " << current->GetName() << endl;
				cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t" << "Z: " << localField[2] << endl;
			#endif
			tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, gGeoManager->GetCurrentNavigator()->GetStep());
			snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         sumchecked++;
//         printf("checked %d from %d : snext=%g\n", sumchecked, nd, snext);
         if (snext < (gGeoManager->GetCurrentNavigator()->GetStep() - fgTolerance)) {
            indnext = current->GetVolume()->GetNextNodeIndex();
				if (compmatrix) {
               gGeoManager->GetCurrentNavigator()->GetHMatrix()->CopyFrom(globalMatrix);
               gGeoManager->GetCurrentNavigator()->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            gGeoManager->GetCurrentNavigator()->SetStep(snext);
				this->SetStepTime(tnext);
				fUCNNextNode = current;
            nodefound = fUCNNextNode;
            idaughter = vlist[i];
            while (indnext>=0) {
               current = current->GetDaughter(indnext);
               if (compmatrix) gGeoManager->GetCurrentNavigator()->GetHMatrix()->Multiply(current->GetMatrix());
               fUCNNextNode = current;
               nodefound = current;
               indnext = current->GetVolume()->GetNextNodeIndex();
            }
         }
      }
   }
   return nodefound;
}

//_____________________________________________________________________________
TGeoNode* TUCNRun::FindNextBoundaryAndStepAlongParabola(TVirtualGeoTrack* track, TUCNGravField* field, Double_t stepTime, Bool_t compsafe)
{
// Compute distance to next boundary within STEPMAX. If no boundary is found,
// propagate current point along current direction with fStep=STEPMAX. Otherwise
// propagate with fStep=SNEXT (distance to boundary) and locate/return the next 
// node.
	
	// -- Get the track's particle
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	
	// -- Get the global field	
	Double_t globalField[3] 	 = {field->Gx(), field->Gy(), field->Gz()}; 
	Double_t globalPoint[3] 	 = {particle->Vx(), particle->Vy(), particle->Vz()};
	Double_t globalDir[3]	 	 = {particle->DirX(), particle->DirY(), particle->DirZ()};
	Double_t globalVelocity[3]  = {particle->VelocityX(), particle->VelocityY(), particle->VelocityZ()};
	
	Double_t currentField[3]	 = {globalField[0], globalField[1], globalField[2]};
	Double_t currentPoint[3]    = {globalPoint[0], globalPoint[1], globalPoint[2]};
	Double_t currentDir[3]      = {globalDir[0], globalDir[1], globalDir[2]};
	Double_t currentVelocity[3] = {globalVelocity[0], globalVelocity[1], globalVelocity[2]};
		
	// Compute maximum stepsize
	const Double_t stepMax = TUCNParabola::Instance()->ArcLength(globalVelocity, globalField, stepTime);
	
	// -- Some initialisations
	static Int_t icount = 0;
   icount++;
   Int_t iact = 3;
   Int_t nextindex;
   Bool_t is_assembly;
   fUCNIsStepExiting  = kFALSE;
   fUCNIsStepEntering = kFALSE;
	fUCNIsOutside = gGeoManager->GetCurrentNavigator()->IsOutside();
	fUCNIsOnBoundary = gGeoManager->GetCurrentNavigator()->IsOnBoundary();
   TGeoNode *skip;
	
	// -- Store time step and step distance -- these two should always be convertible
	this->SetStepTime(stepTime);
	gGeoManager->GetCurrentNavigator()->SetStep(stepMax);
   Double_t snext = TGeoShape::Big();
	Double_t tnext = TGeoShape::Big();
	
	#ifdef VERBOSE_MODE
		cout << "FNBASAP - Starting StepTime: " << this->GetStepTime() << "\t" << "Starting StepSize: " << gGeoManager->GetCurrentNavigator()->GetStep() << endl;
		cout << "FNBASAP - Velocity: " << particle->Velocity() << endl;
		cout << "FNBASAP - Global Field: X: " << globalField[0] << "\t" << "Y: " << globalField[1] << "\t" << "Z: " << globalField[2] << endl;	
		cout << "FNBASAP - Global Point: X: " << globalPoint[0] << "\t" << "Y: " << globalPoint[1] << "\t" << "Z: " << globalPoint[2] << endl;	
		cout << "FNBASAP - Global Dir: X: "   << globalDir[0] << "\t" << "Y: " << globalDir[1] << "\t" << "Z: " << globalDir[2] << endl;	
		cout << "FNBASAP - Global Velocity: X: " << globalVelocity[0] << "\t" << "Y: " << globalVelocity[1] << "\t" << "Z: " << globalVelocity[2] << endl;	
	#endif
		
	// *********************************************************************
	// -- BRANCH 0
	// -- Compute safety
   // *********************************************************************
	if (compsafe) {
      #ifdef VERBOSE_MODE		
			Warning("FindNextBoundaryAndStepAlongParabola","Branch 0. Step will not be computed if Safety > stepMax.");
		#endif
		fUCNIsOnBoundary = kFALSE;
      gGeoManager->GetCurrentNavigator()->Safety();
      // -- If proposed step less than safety, nothing to check
      if (gGeoManager->GetCurrentNavigator()->GetSafeDistance() > gGeoManager->GetCurrentNavigator()->GetStep() + fgTolerance) return gGeoManager->GetCurrentNavigator()->GetCurrentNode();
   }   
   
	// -- If we are on the boundary step off it by tolerance
	Double_t extra = (fUCNIsOnBoundary)?fgTolerance:0.0;
   
/*	#ifdef VERBOSE_MODE
		cout << "FNBASAP - Is On Boundary?: " << gGeoManager->GetCurrentNavigator()->IsUCNOnBoundary() << endl;
		cout << "FNBASAP - Current Point: X: " << currentPoint[0] << "\t" << "Y: " << currentPoint[1] << "\t" << "Z: " << currentPoint[2] << endl;
		cout << "FNBASAP - Make microstep off boundary by amount: " << extra << endl;
	#endif
	
	fUCNIsOnBoundary = kFALSE;
   // -- Update fPoint
	currentPoint[0] += extra*currentDir[0];
   currentPoint[1] += extra*currentDir[1];
   currentPoint[2] += extra*currentDir[2];
	gGeoManager->GetCurrentNavigator()->SetCurrentPoint(currentPoint);
	
	#ifdef VERBOSE_MODE
		cout << "FNBASAP - Is On Boundary?: " << gGeoManager->GetCurrentNavigator()->IsUCNOnBoundary() << endl;
		cout << "FNBASAP - New Current Point: X: " << currentPoint[0] << "\t" << "Y: " << currentPoint[1] << "\t" << "Z: " << currentPoint[2] << endl;
	#endif
*/	
	gGeoManager->GetCurrentNavigator()->GetHMatrix()->CopyFrom(gGeoManager->GetCurrentNavigator()->GetCurrentMatrix());
   	
	// *********************************************************************
	// -- BRANCH 1
	// -- fIsOutside means outside TOP volume - ie: outside geometry
   // *********************************************************************	
	if (fUCNIsOutside) {
      // -- Find Distance And Time to reach TOP volume -- Time is used to make the step. 
      // ***************************************************************************************************
		#ifdef VERBOSE_MODE		
			cout << "FNBASAP - Branch 1. Outside TOP volume (geometry). Calculate distance to TOP." << endl;
			cout << "FNBASAP - Global Field: X: " << globalField[0] << "\t" << "Y: " << globalField[1] << "\t" << "Z: " << globalField[2] << endl;	
		#endif
		tnext = static_cast<TUCNGeoBBox*>(gGeoManager->GetTopVolume()->GetShape())->TimeFromOutsideAlongParabola(currentPoint, currentVelocity, currentField, gGeoManager->GetCurrentNavigator()->GetStep()); 
		snext = TUCNParabola::Instance()->ArcLength(currentVelocity, currentField, tnext);;
		// -- Check if distance to Top is less than the current Stepsize
		if (snext < gGeoManager->GetCurrentNavigator()->GetStep() - fgTolerance) {
			if (snext<=0) {
            // -- Distance to top <= 0. Set = 0, then make a step by extra
				snext = 0.0;
				tnext = 0.0;
				gGeoManager->GetCurrentNavigator()->SetStep(snext);
				this->SetStepTime(tnext);
            // -- Update fPoint
				currentPoint[0] -= extra*currentDir[0];
			   currentPoint[1] -= extra*currentDir[1];
			   currentPoint[2] -= extra*currentDir[2];
				gGeoManager->GetCurrentNavigator()->SetCurrentPoint(currentPoint);
         } else {
				// -- Distance to top is greater than 0. Set stored stepsize fStep to be gGeoManager->GetCurrentNavigator() distance + extra. 
            gGeoManager->GetCurrentNavigator()->SetStep(snext+extra);
				this->SetStepTime(tnext+extra);
         }
   		// -- Top is reachable - and distance to it is less than proposed step - hence we are Entering
         fUCNIsStepEntering = kTRUE;
         fUCNNextNode = gGeoManager->GetTopNode();
         nextindex = fUCNNextNode->GetVolume()->GetNextNodeIndex();
			while (nextindex>=0) {
            gGeoManager->GetCurrentNavigator()->CdDown(nextindex);
            fUCNNextNode = gGeoManager->GetCurrentNavigator()->GetCurrentNode();
            nextindex = fUCNNextNode->GetVolume()->GetNextNodeIndex();
            if (nextindex < 0) gGeoManager->GetCurrentNavigator()->GetHMatrix()->CopyFrom(gGeoManager->GetCurrentNavigator()->GetCurrentMatrix());
         }
   
         // -- Move global point by snext onto boundary
         // ***************************************************************************************************
			#ifdef VERBOSE_MODE		
				cout << "FNBASAP - Branch 1. Entering TOP. Updating Global Point with grav correction." << endl;
				cout << "FNBASAP - StepTime: " << tnext << endl;
			#endif
			const Double_t timestep = tnext;
			
			currentPoint[0] += currentVelocity[0]*timestep + 0.5*currentField[0]*timestep*timestep; 
		   currentPoint[1] += currentVelocity[1]*timestep + 0.5*currentField[1]*timestep*timestep; 
		   currentPoint[2] += currentVelocity[2]*timestep + 0.5*currentField[2]*timestep*timestep;	
			gGeoManager->GetCurrentNavigator()->SetCurrentPoint(currentPoint);
			
			// -- Update Direction to reflect new position
			currentVelocity[0] = currentVelocity[0] + currentField[0]*timestep;
			currentVelocity[1] = currentVelocity[1] + currentField[1]*timestep;
			currentVelocity[2] = currentVelocity[2] + currentField[2]*timestep;
			Double_t velocityMag = TMath::Sqrt((currentVelocity[0]*currentVelocity[0]) + (currentVelocity[1]*currentVelocity[1]) + (currentVelocity[2]*currentVelocity[2])); 
			if (velocityMag == 0.) throw runtime_error("Velocity is zero!");
			
			currentDir[0] = currentVelocity[0]/velocityMag;
			currentDir[1] = currentVelocity[1]/velocityMag;
			currentDir[2] = currentVelocity[2]/velocityMag;
			gGeoManager->GetCurrentNavigator()->SetCurrentDirection(currentDir);
			
			fUCNIsOnBoundary = kTRUE;
         fUCNIsOutside = kFALSE;
         return gGeoManager->GetCurrentNavigator()->CrossBoundaryAndLocate(kTRUE, gGeoManager->GetCurrentNavigator()->GetCurrentNode());
      }
      // -- New point still outside, and the top node is not within snext, but is reachable.
		if (snext<TGeoShape::Big()) {
        	// -- Move global point by proposed stepsize fStep
			// ***************************************************************************************************
         #ifdef VERBOSE_MODE		
				cout << "FNBASAP - Branch 1. New Point outside but TOP is reachable. Updating Global Point." << endl;
				cout << "FNBASAP - StepTime: " << tnext << endl;
			#endif
			fUCNNextNode = gGeoManager->GetTopNode();
			const Double_t timestep = (this->GetStepTime())-extra;
			
			currentPoint[0] += currentVelocity[0]*timestep + 0.5*currentField[0]*timestep*timestep; 
		   currentPoint[1] += currentVelocity[1]*timestep + 0.5*currentField[1]*timestep*timestep; 
		   currentPoint[2] += currentVelocity[2]*timestep + 0.5*currentField[2]*timestep*timestep;	
			gGeoManager->GetCurrentNavigator()->SetCurrentPoint(currentPoint);	
			
			// -- Update Direction to reflect new position
			currentVelocity[0] = currentVelocity[0] + currentField[0]*timestep;
			currentVelocity[1] = currentVelocity[1] + currentField[1]*timestep;
			currentVelocity[2] = currentVelocity[2] + currentField[2]*timestep;
			Double_t velocityMag = TMath::Sqrt((currentVelocity[0]*currentVelocity[0]) + (currentVelocity[1]*currentVelocity[1]) + (currentVelocity[2]*currentVelocity[2])); 
			if (velocityMag == 0.) throw runtime_error("Velocity is zero!");
			
			currentDir[0] = currentVelocity[0]/velocityMag;
			currentDir[1] = currentVelocity[1]/velocityMag;
			currentDir[2] = currentVelocity[2]/velocityMag;
			gGeoManager->GetCurrentNavigator()->SetCurrentDirection(currentDir);
		
			return fUCNNextNode;
      }      
      // top node not reachable from current point/direction
      #ifdef VERBOSE_MODE		
			Warning("FindNextBoundaryAndStepAlongParabola","Branch 1. TOP is NOT reachable.");
		#endif
		fUCNNextNode = 0;
      fUCNIsOnBoundary = kFALSE;
      return 0;
   }
   
	// *********************************************************************
	// -- BRANCH 2
	// -- So we are not outside the geometry. Get point and direction in local coordinate frame and calculate distance to boundary of current node. 
   // *********************************************************************
	Double_t localPoint[3], localVelocity[3], localField[3]; // Containers for the local point and dir
   Int_t icrossed = -2;
   gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->MasterToLocal(currentPoint, &localPoint[0]);
   gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->MasterToLocalVect(currentVelocity, &localVelocity[0]);
	gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->MasterToLocalVect(currentField, &localField[0]);
   TGeoVolume *vol = gGeoManager->GetCurrentNavigator()->GetCurrentNode()->GetVolume();
   
	// -- Find distance to exiting current node
   // ***************************************************************************************************
	tnext = static_cast<TUCNGeoBBox*>(vol->GetShape())->TimeFromInsideAlongParabola(localPoint, localVelocity, localField, gGeoManager->GetCurrentNavigator()->GetStep()); 
	snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
	fUCNNextNode = gGeoManager->GetCurrentNavigator()->GetCurrentNode();
	#ifdef VERBOSE_MODE		
		cout << "FNBASAP - Branch 2. Find distance to exit current Volume: " << vol->GetName() << endl;
		cout << "FNBASAP - Local Field: X: " << localField[0] << "\t" << "Y: " << localField[1] << "\t" << "Z: " << localField[2] << endl;
		cout << "FNBASAP - Time to boundary: " << tnext << "\t" << "Distance to Boundary: " << snext << endl;
		cout << "FNBASAP - snext - fStep: " << snext - gGeoManager->GetCurrentNavigator()->GetStep() << endl;
	#endif
	// -- If distance to exiting current node is <= Tolerance value (1e-10) make a small step by gGeoManager->GetCurrentNavigator() tolerance value
	if (snext <= fgTolerance) {
      snext = fgTolerance;
		tnext = fgTolerance;
		gGeoManager->GetCurrentNavigator()->SetStep(snext);
		this->SetStepTime(tnext);
      fUCNIsOnBoundary = kTRUE;
      fUCNIsStepEntering = kFALSE;
      fUCNIsStepExiting = kTRUE;
      skip = gGeoManager->GetCurrentNavigator()->GetCurrentNode();
		
		// -- Move global point by Tolerance value
		// ***************************************************************************************************
	   currentPoint[0] += gGeoManager->GetCurrentNavigator()->GetStep()*currentDir[0]; 
	   currentPoint[1] += gGeoManager->GetCurrentNavigator()->GetStep()*currentDir[1]; 
	   currentPoint[2] += gGeoManager->GetCurrentNavigator()->GetStep()*currentDir[2];	
		gGeoManager->GetCurrentNavigator()->SetCurrentPoint(currentPoint);
		
      is_assembly = gGeoManager->GetCurrentNavigator()->GetCurrentNode()->GetVolume()->IsAssembly();
      if (!(gGeoManager->GetCurrentNavigator()->GetLevel()) && !is_assembly) {
         fUCNIsOutside = kTRUE ;
         return 0;
      }
      if (gGeoManager->GetCurrentNavigator()->GetCurrentNode()->IsOffset()) {
			#ifdef VERBOSE_MODE		
				Warning("FindNextBoundaryAndStepAlongParabola","Branch 2. fCurrentNode->IsOffset(). Entering CrossDivisionCell().");
			#endif
			throw runtime_error("In TUCNRun FNBASAP Need to call CrossDivisionCell but can't!"); //return gGeoManager->GetCurrentNavigator()->CrossDivisionCell();   
      }
		// -- Cross Boundary and return new volume
		if (gGeoManager->GetCurrentNavigator()->GetLevel()) {
			gGeoManager->GetCurrentNavigator()->CdUp(); 
      } else {
      	skip = 0;
      }
		return gGeoManager->GetCurrentNavigator()->CrossBoundaryAndLocate(kFALSE, skip);
   }   
	
	// -- If distance to exiting current node is smaller than proposed Stepsize - then set our stepsize fStep to gGeoManager->GetCurrentNavigator() distance (snext)
   if (snext < gGeoManager->GetCurrentNavigator()->GetStep() - fgTolerance) {
      icrossed = -1;
      gGeoManager->GetCurrentNavigator()->SetStep(snext);
		this->SetStepTime(tnext);
      fUCNIsStepEntering = kFALSE;
      fUCNIsStepExiting = kTRUE;
   }
   
	// Find next daughter boundary for the current volume
   // ***************************************************************************************************
	Int_t idaughter = -1;
	TGeoNode *crossed = this->FindNextDaughterBoundaryAlongParabola(localPoint, localVelocity, localField, idaughter, kTRUE);
   if (crossed) {
      fUCNIsStepExiting = kFALSE;
      icrossed = idaughter;
      fUCNIsStepEntering = kTRUE;
   }   
   TGeoNode *current = 0;
   TGeoNode *dnode = 0;
   TGeoVolume *mother = 0;
   
	// *********************************************************************
	// -- BRANCH 3
	// -- if we are in an overlapping node, check also the mother(s) - fNmany is number of overlapping nodes on current branch
   // *********************************************************************
   if (gGeoManager->GetCurrentNavigator()->GetNmany()) {
      Double_t motherPoint[3], motherVelocity[3], motherField[3];
		Double_t daughterPoint[3], daughterVelocity[3], daughterField[3];
      Int_t novlps;
      Int_t safelevel = gGeoManager->GetCurrentNavigator()->GetSafeLevel();
      gGeoManager->GetCurrentNavigator()->PushPath(safelevel+1);
      while (gGeoManager->GetCurrentNavigator()->IsCurrentOverlapping()) {
         Int_t *ovlps = gGeoManager->GetCurrentNavigator()->GetCurrentNode()->GetOverlaps(novlps);
         gGeoManager->GetCurrentNavigator()->CdUp();
         mother = gGeoManager->GetCurrentNavigator()->GetCurrentNode()->GetVolume();
         gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->MasterToLocal(currentPoint, &motherPoint[0]);
         gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->MasterToLocalVect(currentVelocity, &motherVelocity[0]);
			gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->MasterToLocalVect(currentField, &motherField[0]);
         // -- Check distance to out
         snext = TGeoShape::Big();
			tnext = TGeoShape::Big();
         if (!mother->IsAssembly()) {
         	// ***************************************************************************************************
				#ifdef VERBOSE_MODE		
					cout << "FNBASAP - Branch 3. In overlapping Node - check distance to mother" << endl;
					cout << "FNBASAP - Moth Local Field: X: " << motherField[0] << "\t" << "Y: " << motherField[1] << "\t" << "Z: " << motherField[2] << endl;
				#endif
				tnext = static_cast<TUCNGeoBBox*>(mother->GetShape())->TimeFromInsideAlongParabola(motherPoint, motherVelocity, motherField, gGeoManager->GetCurrentNavigator()->GetStep()); 
				snext = TUCNParabola::Instance()->ArcLength(motherVelocity, motherField, tnext);
         }
			if (snext < gGeoManager->GetCurrentNavigator()->GetStep() - fgTolerance) {
            // exiting mother first (extrusion)
            icrossed = -1;
            gGeoManager->GetCurrentNavigator()->PopDummy();
            gGeoManager->GetCurrentNavigator()->PushPath(safelevel+1);
            fUCNIsStepEntering = kFALSE;
            fUCNIsStepExiting = kTRUE;
            gGeoManager->GetCurrentNavigator()->SetStep(snext);
				this->SetStepTime(tnext);
            gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->CopyFrom(gGeoManager->GetCurrentNavigator()->GetHMatrix());
            fUCNNextNode = gGeoManager->GetCurrentNavigator()->GetCurrentNode();
         }
         // check overlapping nodes
         for (Int_t i=0; i<novlps; i++) {
            current = mother->GetNode(ovlps[i]);
            if (!current->IsOverlapping()) {
               current->cd();
               current->MasterToLocal(&motherPoint[0], &daughterPoint[0]);
               current->MasterToLocalVect(&motherVelocity[0], &daughterVelocity[0]);
               current->MasterToLocalVect(&motherField[0], &daughterField[0]);
					// Current point may be inside the other node - geometry error that we ignore
               snext = TGeoShape::Big();
					tnext = TGeoShape::Big();
					if (!current->GetVolume()->Contains(daughterPoint))
						// ***************************************************************************************************
			         #ifdef VERBOSE_MODE		
							cout << "FNBASAP - Branch 3. In overlapping Node - check distance to enter other nodes" << endl;
							cout << "FNBASAP - Local Field: X: " << daughterField[0] << "\t" << "Y: " << daughterField[1] << "\t" << "Z: " << daughterField[2] << endl;
						#endif
						tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(daughterPoint, daughterVelocity, daughterField, gGeoManager->GetCurrentNavigator()->GetStep()); 
						snext = TUCNParabola::Instance()->ArcLength(daughterVelocity, daughterField, tnext);
               if (snext < gGeoManager->GetCurrentNavigator()->GetStep() - fgTolerance) {
                  gGeoManager->GetCurrentNavigator()->PopDummy();
                  gGeoManager->GetCurrentNavigator()->PushPath(safelevel+1);
                  gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->CopyFrom(gGeoManager->GetCurrentNavigator()->GetHMatrix());
                  gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->Multiply(current->GetMatrix());
                  fUCNIsStepEntering = kFALSE;
                  fUCNIsStepExiting = kTRUE;
                  icrossed = ovlps[i];
                  gGeoManager->GetCurrentNavigator()->SetStep(snext);
                  this->SetStepTime(tnext);
						fUCNNextNode = current;
               }
            } else {
               // another many - check if point is in or out
               current->cd();
               current->MasterToLocal(&motherPoint[0], &daughterPoint[0]);
               current->MasterToLocalVect(&motherVelocity[0], &daughterVelocity[0]);
               current->MasterToLocalVect(&motherField[0], &daughterField[0]);
					if (current->GetVolume()->Contains(daughterPoint)) {
                  if (current->GetVolume()->GetNdaughters()) {
                     gGeoManager->GetCurrentNavigator()->CdDown(ovlps[i]);
                     // ***************************************************************************************************
							dnode = FindNextDaughterBoundaryAlongParabola(daughterPoint, daughterVelocity, daughterField, idaughter, kFALSE);
                     if (dnode) {
                        gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->CopyFrom(gGeoManager->GetCurrentNavigator()->GetHMatrix());
                        gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->Multiply(dnode->GetMatrix());
                        icrossed = idaughter;
                        gGeoManager->GetCurrentNavigator()->PopDummy();
                        gGeoManager->GetCurrentNavigator()->PushPath(safelevel+1);
                        fUCNIsStepEntering = kFALSE;
                        fUCNIsStepExiting = kTRUE;
                        fUCNNextNode = dnode;
                     }   
                     gGeoManager->GetCurrentNavigator()->CdUp();
                  }   
               } else {
						// ***************************************************************************************************
						#ifdef VERBOSE_MODE		
							cout << "FNBASAP - Branch 3. In overlapping Node - check distance to exit" << endl;
							cout << "FNBASAP - Local Field: X: " << daughterField[0] << "\t" << "Y: " << daughterField[1] << "\t" << "Z: " << daughterField[2] << endl;
						#endif
						tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(daughterPoint, daughterVelocity, daughterField, gGeoManager->GetCurrentNavigator()->GetStep()); 
						snext = TUCNParabola::Instance()->ArcLength(daughterVelocity, daughterField, tnext);
                  if (snext < gGeoManager->GetCurrentNavigator()->GetStep() - fgTolerance) {
                     gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->CopyFrom(gGeoManager->GetCurrentNavigator()->GetHMatrix());
                     gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->Multiply(current->GetMatrix());
                     fUCNIsStepEntering = kFALSE;
                     fUCNIsStepExiting = kTRUE;
                     gGeoManager->GetCurrentNavigator()->SetStep(snext);
                     this->SetStepTime(tnext);
							fUCNNextNode = current;
                     icrossed = ovlps[i];
                     gGeoManager->GetCurrentNavigator()->PopDummy();
                     gGeoManager->GetCurrentNavigator()->PushPath(safelevel+1);
                  }               
               }  
            }
         }
      }
      // Now we are in a non-overlapping node
      if (gGeoManager->GetCurrentNavigator()->GetNmany()) {
      // We have overlaps up in the branch, check distance to exit
         Int_t up = 1;
         Int_t imother;
         Int_t nmany = gGeoManager->GetCurrentNavigator()->GetNmany();
         Bool_t ovlp = kFALSE;
         Bool_t nextovlp = kFALSE;
         Bool_t offset = kFALSE;
         TGeoNode *currentnode = gGeoManager->GetCurrentNavigator()->GetCurrentNode();
         TGeoNode *mothernode, *mup;
         TGeoHMatrix *matrix;
         while (nmany) {
            mothernode = gGeoManager->GetCurrentNavigator()->GetMother(up);
            mup = mothernode;
            imother = up+1;
            offset = kFALSE;
            while (mup->IsOffset()) {
               mup = gGeoManager->GetCurrentNavigator()->GetMother(imother++);
               offset = kTRUE;
            }   
            nextovlp = mup->IsOverlapping();
            if (offset) {
               mothernode = mup;
               if (nextovlp) nmany -= imother-up;
               up = imother-1;
            } else {    
               if (ovlp) nmany--;
            }
            if (ovlp || nextovlp) {
               matrix = gGeoManager->GetCurrentNavigator()->GetMotherMatrix(up);
               matrix->MasterToLocal(currentPoint, daughterPoint);
               matrix->MasterToLocalVect(currentVelocity, daughterVelocity);
               matrix->MasterToLocalVect(currentField, daughterField);
					snext = TGeoShape::Big();
					tnext = TGeoShape::Big();
					if (!mothernode->GetVolume()->IsAssembly()) {
						// ***************************************************************************************************
						#ifdef VERBOSE_MODE		
							cout << "FNBASAP - Branch 3. Non-overlapping Node - checking distance to exit" << endl;	
							cout << "FNBASAP - Local Field: X: " << daughterField[0] << "\t" << "Y: " << daughterField[1] << "\t" << "Z: " << daughterField[2] << endl;
						#endif
						tnext = static_cast<TUCNGeoBBox*>(mothernode->GetVolume()->GetShape())->TimeFromInsideAlongParabola(daughterPoint, daughterVelocity, daughterField, gGeoManager->GetCurrentNavigator()->GetStep()); 
						snext = TUCNParabola::Instance()->ArcLength(daughterVelocity, daughterField, tnext);
					}
               fUCNIsStepEntering = kFALSE;
               fUCNIsStepExiting  = kTRUE;
               if (snext < gGeoManager->GetCurrentNavigator()->GetStep() - fgTolerance) {
                  fUCNNextNode = mothernode;
                  gGeoManager->GetCurrentNavigator()->GetCurrentMatrix()->CopyFrom(matrix);
                  gGeoManager->GetCurrentNavigator()->SetStep(snext);
                  this->SetStepTime(tnext);
						while (up--) gGeoManager->GetCurrentNavigator()->CdUp();
                  gGeoManager->GetCurrentNavigator()->PopDummy();
                  gGeoManager->GetCurrentNavigator()->PushPath();
                  icrossed = -1;
                  up = 1;
                  currentnode = gGeoManager->GetCurrentNavigator()->GetCurrentNode();
                  ovlp = currentnode->IsOverlapping();
                  continue;
               }   
            }   
            currentnode = mothernode;
            ovlp = nextovlp;
            up++;            
         }
      }      
      gGeoManager->GetCurrentNavigator()->PopPath();
   }
   #ifdef VERBOSE_MODE		
		cout << "FNBASAP - Branch 3. Updating Global Point. fTimeStep: " << this->GetStepTime() << "\t" << "fStep: " << gGeoManager->GetCurrentNavigator()->GetStep() << endl;
	#endif
	
	const Double_t timestep = this->GetStepTime();
	currentPoint[0] += currentVelocity[0]*timestep + 0.5*currentField[0]*timestep*timestep; 
   currentPoint[1] += currentVelocity[1]*timestep + 0.5*currentField[1]*timestep*timestep; 
   currentPoint[2] += currentVelocity[2]*timestep + 0.5*currentField[2]*timestep*timestep;	
	gGeoManager->GetCurrentNavigator()->SetCurrentPoint(currentPoint);
	
	// -- Update Direction to reflect new position
	currentVelocity[0] = currentVelocity[0] + currentField[0]*timestep;
	currentVelocity[1] = currentVelocity[1] + currentField[1]*timestep;
	currentVelocity[2] = currentVelocity[2] + currentField[2]*timestep;
	Double_t velocityMag = TMath::Sqrt((currentVelocity[0]*currentVelocity[0]) + (currentVelocity[1]*currentVelocity[1]) + (currentVelocity[2]*currentVelocity[2])); 
	if (velocityMag == 0.) throw runtime_error("Velocity is zero!");
	
	currentDir[0] = currentVelocity[0]/velocityMag;
	currentDir[1] = currentVelocity[1]/velocityMag;
	currentDir[2] = currentVelocity[2]/velocityMag;
	gGeoManager->GetCurrentNavigator()->SetCurrentDirection(currentDir);
	
	gGeoManager->GetCurrentNavigator()->SetStep(gGeoManager->GetCurrentNavigator()->GetStep() + extra);
	this->SetStepTime(this->GetStepTime() + extra);
	
	// *********************************************************************
	// -- BRANCH 4
	// -- Final check on results of above
   // *********************************************************************
	if (icrossed == -2) {
      // Nothing crossed within stepMax -> propagate and return same location   
      #ifdef VERBOSE_MODE		
			cout << "FNBASAP - Branch 4. Nothing crossed within stepMax. Propagating point and returning same location." << endl;
		#endif
		fUCNIsOnBoundary = kFALSE;
      return gGeoManager->GetCurrentNavigator()->GetCurrentNode();
   }
   fUCNIsOnBoundary = kTRUE;
   if (icrossed == -1) {
      #ifdef VERBOSE_MODE		
			cout << "FNBASAP - Branch 4. On Boundary. Crossing boundary and locating." << endl;
		#endif
		skip = gGeoManager->GetCurrentNavigator()->GetCurrentNode();
      is_assembly = gGeoManager->GetCurrentNavigator()->GetCurrentNode()->GetVolume()->IsAssembly();
      if (!(gGeoManager->GetCurrentNavigator()->GetLevel()) && !is_assembly) {
         fUCNIsOutside = kTRUE;
         return 0;
      }   
      if (gGeoManager->GetCurrentNavigator()->GetCurrentNode()->IsOffset()) {
			throw runtime_error("In TUCNRun FNBASAP Need to call CrossDivisionCell but can't!"); //return gGeoManager->GetCurrentNavigator()->CrossDivisionCell();
		}   
      if (gGeoManager->GetCurrentNavigator()->GetLevel()) gGeoManager->GetCurrentNavigator()->CdUp();
      else        skip = 0;
//      is_assembly = fCurrentNode->GetVolume()->IsAssembly();
//      while (fLevel && is_assembly) {
//         gGeoManager->GetCurrentNavigator()->CdUp();
//         is_assembly = fCurrentNode->GetVolume()->IsAssembly();
//         skip = fCurrentNode;
//      }   
      return gGeoManager->GetCurrentNavigator()->CrossBoundaryAndLocate(kFALSE, skip);
   }   
   current = gGeoManager->GetCurrentNavigator()->GetCurrentNode();   
   gGeoManager->GetCurrentNavigator()->CdDown(icrossed);
   nextindex = gGeoManager->GetCurrentNavigator()->GetCurrentNode()->GetVolume()->GetNextNodeIndex();
	while (nextindex>=0) {
      current = gGeoManager->GetCurrentNavigator()->GetCurrentNode();
      gGeoManager->GetCurrentNavigator()->CdDown(nextindex);
      nextindex = gGeoManager->GetCurrentNavigator()->GetCurrentNode()->GetVolume()->GetNextNodeIndex();
   }   
	#ifdef VERBOSE_MODE		
		cout << "FindNextBoundaryAndStepAlongParabola - Branch 4. Crossing boundary and locating." << endl;
   #endif
	return gGeoManager->GetCurrentNavigator()->CrossBoundaryAndLocate(kTRUE, current);	
}

//_____________________________________________________________________________
Double_t TUCNRun::DetermineNextStepTime(TUCNParticle* particle, const Double_t maxStepTime, const Double_t runTime)			 
{
// Placeholder for method to calculate the next step time depending on electric/magnetic field environment
	// Start with the maximum stepTime 
	this->SetStepTime(maxStepTime);
	
	// -- Check if we will reach the maximum runtime of the track. If so propagate only until this time.
	if (runTime == 0.0) {
		return this->GetStepTime();
	} else if (particle->T() > (runTime - this->GetStepTime())) {
		this->SetStepTime((runTime - particle->T()) + fgTolerance);
	}
	return this->GetStepTime();
}

//_____________________________________________________________________________
void TUCNRun::SetStepTime(Double_t stepTime) 
{
	assert(stepTime > 0.0);
	fStepTime = stepTime;
}

//_____________________________________________________________________________
Bool_t TUCNRun::MakeStep(TVirtualGeoTrack* track, TUCNGravField* gravField, TUCNMagField* magField)
{
	// -- Find time to reach next boundary and step along parabola
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	
	// -- Save Path to current node - we will want to return to this in the event we make a bounce
	const char* path = gGeoManager->GetCurrentNavigator()->GetPath();
	#ifdef VERBOSE_MODE	
		cout << "Current PATH: " << path << endl;
	#endif
	
	#ifdef VERBOSE_MODE	
		cout << "------------------- START OF STEP ----------------------" << endl;
		cout << "Initial Steptime (s): " << this->GetStepTime() << endl;
		cout << "Initial Vertex (m): " << "X:" << particle->Vx() << "\t" << "Y:" << particle->Vy()  << "\t" << "Z:" << particle->Vz() << "\t" << "T:" << particle->T() << endl;
		cout << "Initial Dir: " << "X:" << particle->DirX()  << "\t" << "Y:" << particle->DirY()  << "\t" << "Z:" << particle->DirZ() << "\t" << "Mag:" << particle->Dir() << endl;
		cout << "Initial Vel (m/s): " << "X:" << particle->VelocityX()  << "\t" << "Y:" << particle->VelocityY()  << "\t" << "Z:" << particle->VelocityZ() << "\t" << "V:" << particle->Velocity() << endl;
		cout << "Initial Mom (eV): " << "X:" << particle->Px()  << "\t" << "Y:" << particle->Py()  << "\t" << "Z:" << particle->Pz() << "\t" << "P:" << particle->P() << endl;
		cout << "Initial Volume: "    << gGeoManager->GetCurrentNavigator()->GetCurrentVolume()->GetName() << endl;
		cout << "Initial Energy (neV): "    << particle->Energy() /Units::neV << endl;
		cout << "-----------------------------" << endl;
		cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
		cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
		cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
		cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
		cout << "-----------------------------" << endl << endl;
	#endif
	
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Find Next Boundary
	if (gravField == NULL) {
		if (!(gGeoManager->GetCurrentNavigator()->FindNextBoundaryAndStep(this->GetStepTime()))) {
			Error("MakeStep", "FindNextBoundaryAndStep has failed to find the next node");
			return kFALSE;
		}
		// TODO: UPDATE CLASS DATA MEMBERS NOW LIKE fUCNNextNode
		// - Calculate the time travelled
		assert(particle->Velocity() > 0.0);
		Double_t timeTravelled = gGeoManager->GetCurrentNavigator()->GetStep()/particle->Velocity(); 
		this->SetStepTime(timeTravelled);
	} else {
		
		// -- Check whether the routine FindNextBoundaryAlongParabola returns a steptime different from
		// -- FindNextBoundaryAndStepAlongParabola
		const Double_t initialStepTime = this->GetStepTime();
		const Double_t initialStep = gGeoManager->GetStep();
		
		if (!(this->FindNextBoundaryAlongParabola(track, gravField, this->GetStepTime(), this->IsUCNOnBoundary()))) {
			Error("MakeStep", "FindNextBoundaryAlongParabola has failed to find the next node");
			return kFALSE;
		}
		const Double_t firstStepTime = this->GetStepTime();
		const Double_t firstStep = gGeoManager->GetStep();
		
		gGeoManager->SetStep(initialStep);
		this->SetStepTime(initialStepTime);
		
		if (!(this->FindNextBoundaryAndStepAlongParabola(track, gravField, this->GetStepTime()))) {
			Error("MakeStep", "FindNextBoundaryAndStepAlongParabola has failed to find the next node");
			return kFALSE;	
		}
		
		const Double_t finalStepTime = this->GetStepTime();
		const Double_t finalStep = gGeoManager->GetStep();
		
		if (TMath::Abs(finalStep - firstStep) > 2.*TGeoShape::Tolerance()) {
			cout << TMath::Abs(finalStep - firstStep) << endl;
			cout << "Test Step: " << firstStep << "\t" << "Real Step: " << finalStep << endl;
			cout << "Test StepTime: " << firstStepTime << "\t" << "Real StepTime: " << finalStepTime << endl;
		}	
	}
	
	// -- Sample Magnetic Field if there is one	
	if (magField != NULL) {
		const Double_t integratedField = magField->IntegratedField(this->GetStepTime(), particle, gravField);
		particle->SampleMagField(integratedField, this->GetStepTime());	
	}
	
	// -- Update track/particle properties
	this->UpdateTrack(track, this->GetStepTime(), gravField);
	///////////////////////////////////////////////////////////////////////////////////////
	
	#ifdef VERBOSE_MODE	
		cout << "------------------- END OF STEP ----------------------" << endl;
		cout << "Final Steptime (s): " << this->GetStepTime() << endl;
		cout << "Final Vertex (m): " << "X:" << particle->Vx() << "\t" << "Y:" << particle->Vy()  << "\t" << "Z:" << particle->Vz() << "\t" << "T:" << particle->T() << endl;
		cout << "Final Dir: " << "X:" << particle->DirX()  << "\t" << "Y:" << particle->DirY()  << "\t" << "Z:" << particle->DirZ() << "\t" << "Mag:" << particle->Dir() << endl;
		cout << "Final Vel (m/s): " << "X:" << particle->VelocityX()  << "\t" << "Y:" << particle->VelocityY()  << "\t" << "Z:" << particle->VelocityZ() << "\t" << "V:" << particle->Velocity() << endl;
		cout << "Final Mom (eV): " << "X:" << particle->Px()  << "\t" << "Y:" << particle->Py()  << "\t" << "Z:" << particle->Pz() << "\t" << "P:" << particle->P() << endl;
		cout << "Final Volume: "    << gGeoManager->GetCurrentNavigator()->GetCurrentVolume()->GetName() << endl;
		cout << "Final Energy (neV): "    << particle->Energy() /Units::neV << endl;
		cout << "-----------------------------" << endl;
		cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
		cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
		cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
		cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
		cout << "-----------------------------" << endl << endl;
	#endif	
	
	// -- Get the current material we are in to determine what to do next
	TUCNGeoMaterial* currentMaterial = static_cast<TUCNGeoMaterial*>(gGeoManager->GetCurrentNavigator()->GetCurrentVolume()->GetMedium()->GetMaterial());
	// -- Get the normal vector to the boundary
	Double_t* normal = 0;
	if (gravField) {
		normal = this->FindUCNNormal();
	} else {
		normal = gGeoManager->GetCurrentNavigator()->FindNormal();
	}
	
	// -- Determine what to do if we are on a boundary
	// -- Is Track on the surface of a boundary?
	if (currentMaterial->IsTrackingMaterial() == kFALSE) {
		#ifdef VERBOSE_MODE	
			cout << "On Boundary - Deciding what action to take..." << endl;
		#endif
		// -- Are we on the surface of a detector?
		if (currentMaterial->IsDetectorMaterial() == kTRUE) {
			// -- Was particle detected?
			Double_t prob = gRandom->Uniform(0.0,1.0);
			if (prob <= currentMaterial->DetectionEfficiency()) {	
				// -- PARTICLE DETECTED
				particle->Detected(kTRUE);  // Set detected flag
			} else {	
				// -- PARTICLE NOT-DETECTED.
				// -- Eventually we will change this to allow particle to track inside the detector
				cerr << "Not implemented yet" << endl;
				return kFALSE;
			}
		// -- Was particle lost to boundary (absorbed/upscattered) ?
		} else if (particle->IsLostToWall(currentMaterial, normal)) {	
			particle->LostToBoundary(kTRUE); // Set lost flag
		// -- Are we outside the geometry heirarchy we have built - ie: in TOP
		} else if (currentMaterial->IsBlackHole() == kTRUE) {
			Error("MakeStep","Particle is in `BlackHole' material region. Particle has been lost through the wall!");
			return kFALSE;
		} else {
			// -- PARTICLE ON SURFACE OF BOUNDARY
			TGeoNode* boundaryNode = gGeoManager->GetCurrentNavigator()->GetCurrentNode();
			#ifdef VERBOSE_MODE	
				cout << "------------------- BOUNCE ----------------------" << endl;
				cout << "Current Node: " << boundaryNode->GetName() << endl;
				cout << "On Boundary? " << this->IsUCNOnBoundary() << endl;
			#endif	
			// -- Make a Bounce
			this->Bounce(track, normal, currentMaterial);

			// -- cd back to the saved node before we made the step -- stored in 'path'. 
			gGeoManager->GetCurrentNavigator()->cd(path);
			this->UpdateTrack(track); // Passing default arguments here as we are only changing the direction from bouncing

			TGeoNode* finalNode = gGeoManager->GetCurrentNavigator()->GetCurrentNode();
			#ifdef VERBOSE_MODE	
				cout << "-------------------------------------------------" << endl;
				cout << "Final Node: " << finalNode->GetName() << endl;
				cout << "On Boundary? " << this->IsUCNOnBoundary() << endl;
				cout << "-------------------------------------------------" << endl << endl;
			#endif

			// -- Check if the particle is still registered as being on the boundary
			if (finalNode == boundaryNode) {
				cout << "Initial Node: " << boundaryNode->GetName() << "\t" << "Final Node: " << finalNode->GetName() << endl;
				return kFALSE;
			}
		}
	}
	
	return kTRUE;
}

//_____________________________________________________________________________
void TUCNRun::UpdateTrack(TVirtualGeoTrack* track, const Double_t timeInterval, TUCNGravField* gravField)
{
// Take the particle and update its position, momentum, time and energy with the current properties stored in the Navigator
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	
	#ifdef VERBOSE_MODE
		cout << "UpdateTrack -- Initial X: " << particle->Vx() << "\t" << "Y: " << particle->Vy() << "\t" <<  "Z: " << particle->Vz() << "\t" <<  "T: " << particle->T() << endl;
		cout << "UpdateTrack -- Initial PX: " << particle->Px() << "\t" << "PY: " << particle->Py() << "\t" <<  "PZ: " << particle->Pz() << "\t" <<  "E: " << particle->Energy()/Units::neV << endl;
	#endif
	
	const Double_t* pos = gGeoManager->GetCurrentNavigator()->GetCurrentPoint();
	const Double_t* dir = gGeoManager->GetCurrentNavigator()->GetCurrentDirection(); 
	
	Double_t heightClimbed = 0.0;
	Double_t gravPotentialEnergy = 0.0;
	
	if (gravField) {
		// Determine the height of our particle in the global coordinate system of TOP.
		// Take the dot product of the point vector with the field direction unit vector to get the height of this point in the gravitational field.
		// This assumes a convention that 'height' is measured along an axis that INCREASES in the opposite direction to the field direction vector
		// (which is usually 'downwards')
		heightClimbed = -1.0*((pos[0] - particle->Vx())*gravField->Nx() + (pos[1] - particle->Vy())*gravField->Ny() + (pos[2] - particle->Vz())*gravField->Nz());
		gravPotentialEnergy = particle->Mass_GeV_c2()*gravField->GravAcceleration()*heightClimbed;
	}
	
	// Determine current Kinetic energy of particle given the height climbed in graviational field
	Double_t kineticEnergy = particle->Energy() - gravPotentialEnergy;
	
	// Detemine current momentum
	Double_t momentum = TMath::Sqrt(2.0*particle->Mass_GeV()*kineticEnergy);
	Double_t mom[3] = {momentum*dir[0], momentum*dir[1], momentum*dir[2]};
	
	// Update particle
	particle->SetProductionVertex(pos[0], pos[1], pos[2], particle->T() + timeInterval);
	particle->SetMomentum(mom[0], mom[1], mom[2], kineticEnergy);
	particle->IncreaseDistance(gGeoManager->GetCurrentNavigator()->GetStep()); // Increase the distance travelled stored in the particle
	
	// Update track
	track->AddPoint(particle->Vx(), particle->Vy(), particle->Vz(), particle->T());
		
	#ifdef VERBOSE_MODE
		cout << "UpdateTrack -- Height climbed: " << heightClimbed << "\t" << "Kinetic Energy Gained(Lost): " << -gravPotentialEnergy/Units::neV << endl;
		cout << "UpdateTrack -- Final X: " << particle->Vx() << "\t" << "Y: " << particle->Vy() << "\t" <<  "Z: " << particle->Vz() << "\t" <<  "T: " << particle->T() << endl;
		cout << "UpdateTrack -- Final PX: " << particle->Px() << "\t" << "PY: " << particle->Py() << "\t" <<  "PZ: " << particle->Pz() << "\t" <<  "E: " << particle->Energy()/Units::neV << endl;
	#endif
}


//_____________________________________________________________________________
Bool_t TUCNRun::Bounce(TVirtualGeoTrack* track, const Double_t* normal, TUCNGeoMaterial* wallMaterial)
{
	// -- Get particle
	TUCNParticle* ucnparticle = static_cast<TUCNParticle*>(track->GetParticle());
	
	// -- Direction Vector
	Double_t dir[3] = {(gGeoManager->GetCurrentNavigator()->GetCurrentDirection())[0], (gGeoManager->GetCurrentNavigator()->GetCurrentDirection())[1], (gGeoManager->GetCurrentNavigator()->GetCurrentDirection())[2]};
	
	// -- Normal Vector
	Double_t norm[3] = {normal[0], normal[1], normal[2]};
	
	// Check if the normal vector is actually pointing in the wrong direction 
	// (wrong means pointing along the direction of the track, rather than in the opposite direction)
	Double_t dotProduct = dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2];
	if (dotProduct > 0.) {
		// If so, reflect the normal to get the correct direction
		norm[0] = -norm[0];
		norm[1] = -norm[1];
		norm[2] = -norm[2];
	}
		
	// -- Calculate Probability of diffuse reflection
	Double_t fermiPotential = wallMaterial->FermiPotential();
	Double_t diffuseCoefficient = wallMaterial->RoughnessCoeff();
	Double_t diffuseProbability = ucnparticle->DiffuseProbability(diffuseCoefficient, normal, fermiPotential);
	
	// Determine Reflection Type 
	Double_t prob = gRandom->Uniform(0.0,1.0);
	if (prob <= diffuseProbability) {
		// -- Diffuse Bounce
		this->DiffuseBounce(dir, norm);
		ucnparticle->MadeDiffuseBounce(); // Update counter
	} else {
		// -- Specular Bounce
		this->SpecularBounce(dir, norm);
		ucnparticle->MadeSpecularBounce(); // Update counter
	}

	// -- Set New Direction
	gGeoManager->GetCurrentNavigator()->SetCurrentDirection(dir);
	
	// -- Update Track
	this->UpdateTrack(track);	

	// -- Update Bounce Counter
	ucnparticle->MadeBounce();
	
	return kTRUE;
}	

//_____________________________________________________________________________
Bool_t TUCNRun::SpecularBounce(Double_t* dir, const Double_t* norm)
{
	#ifdef VERBOSE_MODE	
		cout << "----------------------------" << endl;
		cout << "Specular Bounce" << endl;
		cout << "BEFORE - nx: " << dir[0] << "\t" << "ny: " << dir[1] << "\t" << "ny: " << dir[2] << endl;
		cout << "normx: " << norm[0] << "\t" << "normy: " << norm[1] << "\t" << "normz: " << norm[2] << endl;
	#endif
	
	Double_t dotProduct = dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2];	
	// Reflection Law for Specular Reflection
	dir[0] = dir[0] - 2.0*dotProduct*norm[0];
	dir[1] = dir[1] - 2.0*dotProduct*norm[1];  
	dir[2] = dir[2] - 2.0*dotProduct*norm[2];  
	
	#ifdef VERBOSE_MODE	
		cout << "AFTER - nx: " << dir[0] << "\t" << "ny: " << dir[1] << "\t" << "nz: " << dir[2] << endl;
	#endif	  

	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::DiffuseBounce(Double_t* dir, const Double_t* norm)
{
	#ifdef VERBOSE_MODE	
		cout << "----------------------------" << endl;
		cout << "Diffuse Bounce" << endl;
		cout << "BEFORE - nx: " << dir[0] << "\t" << "ny: " << dir[1] << "\t" << "nz: " << dir[2] << endl;
		cout << "normx: " << norm[0] << "\t" << "normy: " << norm[1] << "\t" << "normz: " << norm[2] << endl;
	#endif	
	
	// First we need to pick random angles to choose the orientation of our diffuse direction vector. 
	// Correct method for UCN physics though is to weight these angles towards the poles by adding an extra cos(theta)
	// derivation of how to pick these angles is in notes
	// Overall solid angle used is dOmega = cos(theta)sin(theta) dtheta dphi
	Double_t phi = gRandom->Uniform(0.0, 1.0)*2*TMath::Pi();
	// We do not want the full range of theta from 0 to pi/2 however. An angle of pi/2 would imply moving iff the boundary exactly parallel to
	// the current surface.Therefore we should restrict theta to a slightly smaller proportion of angles - letting u be between
	// 0 and 0.499, ensures theta between 0 and ~89 degrees. 
	Double_t u = gRandom->Uniform(0.0, 0.499);
	// We ignore the negative sqrt term when selecting theta, since we are only interested in theta between 0 and pi/2 
	// (negative root provides the pi/2 to pi branch) 
	Double_t theta = TMath::ACos(TMath::Sqrt(1.0 - 2*u)); 
	
	// Calculate local normal vector	
	Double_t lnorm[3] = {0.,0.,0.};
	gGeoManager->GetCurrentNavigator()->MasterToLocalVect(norm, lnorm);
	TVector3 localNorm(lnorm[0], lnorm[1], lnorm[2]);
	assert(TMath::Abs(localNorm.Mag() - 1.0) < fgTolerance);
	
	// ------------------------------------------------------------------
	// -- FIND A SINGLE ARBITRARY VECTOR PERPENDICULAR TO THE LOCAL NORMAL
	// Define a preferred direction in our coordinate system - usually the z-direction - that we want to be perpendicular to the normal vector of our surface
	TVector3 upAxis(0.,0.,1.);
		
	// Here we check to make sure that the upAxis we chose is not parallel to the normal vector. If it is, we try another one, x. 
	if (TMath::Abs(upAxis.Dot(localNorm)) > fgTolerance) {
		upAxis.SetXYZ(1.,0.,0.);
		if (TMath::Abs(upAxis.Dot(localNorm)) > fgTolerance) {
			// In the (hopefully) unlikely case that localNorm has components in the x and z directions. This has been observed when including the
			// bend geometry. This suggests that in the local coordinate system in this instance, the z-axis is not along the vertical as we usually assume.
			// Thus including this check covers our backs. 
			upAxis.SetXYZ(0.,1.,0.);
			if (TMath::Abs(upAxis.Dot(localNorm)) > fgTolerance) {
				cout << "Axis X: " << upAxis.X() << "\t" <<  "Y: " << upAxis.Y() << "\t" <<   "Z: " << upAxis.Z() << endl;
				cout << "LocalNorm X: " << localNorm.X() << "\t" <<  "Y: " << localNorm.Y() << "\t" <<   "Z: " << localNorm.Z() << endl;
				throw runtime_error("In TUCNRun::DiffuseBounce - Could not find an axis perpendicular to normal. Normal is parallel to z and x axes!!!");
			}
		}
	}
	// Calculate the cross product of the 'up' vector with our normal vector to get a vector guaranteed to be perpendicular to the normal. 
	// This is the vector we will want to rotate around initially. 
	TVector3 perpAxis = localNorm.Cross(upAxis); 
	assert(TMath::Abs(perpAxis.Mag() - 1.0) < fgTolerance);
	// ------------------------------------------------------------------
	
	// Rotate normal vector about perpendicular axis by angle theta. Using Rodrigues' formula derived in notes. 
	TVector3 rotatedNorm(0.,0.,0.);
	{
		// name term2 and term3 just refer to the terms as they appear in the Rodrigues' formula
		TVector3 term2 = localNorm - (localNorm.Dot(perpAxis))*perpAxis; 
		TVector3 term3 = perpAxis.Cross(localNorm);
		rotatedNorm = localNorm + (TMath::Cos(theta) - 1.0)*term2 + TMath::Sin(theta)*term3;
	}
	assert(TMath::Abs(rotatedNorm.Mag() - 1.0) < fgTolerance);
	
	// Rotate the newly rotated Normal vector, rotatedNorm, by angle phi, this time rotating about the original normal vector, norm.
	TVector3 direction(0.,0.,0.);
	{
		TVector3 term3 = localNorm.Cross(rotatedNorm);
		TVector3 term2 = rotatedNorm - (rotatedNorm.Dot(localNorm))*localNorm;
		direction = rotatedNorm + (TMath::Cos(phi) - 1.0)*term2 + TMath::Sin(phi)*term3;
	}
	assert(TMath::Abs(direction.Mag() - 1.0) < fgTolerance); 
	
	// Convert the direction vector back to the global frame
	Double_t ldir[3] = {direction.X(), direction.Y(), direction.Z()};
	gGeoManager->GetCurrentNavigator()->LocalToMasterVect(ldir, dir);
	
	assert(TMath::Abs(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2] - 1.0) < fgTolerance);
	// Assert that our final direction is not perpendicular to the normal. This could result in escaping the boundary
	assert(TMath::Abs(dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2]) > fgTolerance); 
	
	#ifdef VERBOSE_MODE	
		cout << "AFTER - nx: " << dir[0] << "\t" << "nx: " << dir[1] << "\t" << "nx: " << dir[2] << endl;
	#endif
	
	return kTRUE;
}



