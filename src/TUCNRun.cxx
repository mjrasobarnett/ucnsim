// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#include <iostream>
#include <cassert>
#include <vector>

#include "TUCNRun.h"
#include "TUCNConfigFile.h"
#include "TUCNGeoManager.h"
#include "TUCNGeoMaterial.h"
#include "TUCNGeoNavigator.h"
#include "TUCNFieldManager.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"
#include "TUCNData.h"

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

//#define VERBOSE_MODE

ClassImp(TUCNRun)

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
	fBoundaryLossCounter = 0;
	fDetectedCounter = 0;
	fDecayedCounter = 0;
	fDiffuseCoeff = 0;
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
	fBoundaryLossCounter = 0;
	fDetectedCounter = 0;
	fDecayedCounter = 0;
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
			fBoundaryLossCounter(run.fBoundaryLossCounter),
			fDetectedCounter(run.fDetectedCounter),
			fDecayedCounter(run.fDecayedCounter)
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
		fBoundaryLossCounter = run.fBoundaryLossCounter;
		fDetectedCounter = run.fDetectedCounter;
		fDecayedCounter = run.fDecayedCounter;
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
	// - Number of particles
	fNeutrons = configFile->GetInt("Neutrons", this->GetName());
	if (fNeutrons == 0) { Warning("Initialise","No number of particles has been set"); return kFALSE; }
	///////////////////////////////////////////////////////////////////////////////////////
	// - Initial Energy
	// Get the initial energy in units of (neV)
	Double_t totalEnergyneV = configFile->GetFloat("InitialEnergy(neV)", this->GetName())*Units::neV;
	// Get the initial energy in units of the FermiPotential of the boundary
	Double_t V = static_cast<TUCNGeoMaterial*>(gGeoManager->GetMaterial("Boundary Material"))->FermiPotential(); // !!!! This is ugly and unsafe. Needs a work around.
	Double_t totalEnergyVFermi = configFile->GetFloat("InitialEnergy(VFermi)", this->GetName())*V;
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
	// - Run Time
	fRunTime = configFile->GetFloat("RunTime(s)", this->GetName())*Units::s;
	if (fRunTime == 0.0) { Warning("Initialise","No RunTime has been set"); return kFALSE; }
	///////////////////////////////////////////////////////////////////////////////////////
	// - Max Step Time
	fMaxStepTime = configFile->GetFloat("MaxStepTime(s)", this->GetName())*Units::s;
	if (fMaxStepTime == 0.0) { Warning("Initialise","No max step time has been set"); return kFALSE; }
	///////////////////////////////////////////////////////////////////////////////////////
	// - Diffuse Coefficient
	fDiffuseCoeff = configFile->GetFloat("DiffuseBounceCoefficient", this->GetName());
	// - Set material's roughness parameter to the Diffuse Coefficient
	static_cast<TUCNGeoMaterial*>(gGeoManager->GetMaterial("Boundary Material"))->RoughnessCoeff(fDiffuseCoeff); // !!!! This is ugly and unsafe. Needs a work around
	///////////////////////////////////////////////////////////////////////////////////////	
	// - Determine if we will sample the magnetic field
	fSampleMagField = configFile->GetBool("SampleMagField", this->GetName());
	///////////////////////////////////////////////////////////////////////////////////////
	cout << "Particles: " << fNeutrons << endl;
	cout << "Total Energy(neV): " << fTotalEnergy/Units::neV << endl;
	cout << "RunTime(s): " << fRunTime << endl;
	cout << "MaxStepTime(s): " << fMaxStepTime << endl;
	cout << "DiffuseCoeff: " << fDiffuseCoeff << endl;
	cout << "SampleMagField: " << fSampleMagField << endl;
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
	TUCNGeoNavigator* navigator = static_cast<TUCNGeoNavigator*>(geoManager->GetCurrentNavigator());
	assert(track != NULL && particle != NULL && navigator != NULL);
	
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
		navigator->DetermineNextStepTime(particle, fMaxStepTime, fRunTime);
		
		// -- Make a step
		if (!(navigator->MakeStep(track, gravField, magField))) {
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
		} else if (particle->WillDecay(navigator->GetStepTime()) == kTRUE) {
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
