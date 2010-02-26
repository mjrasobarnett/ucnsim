// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#include <iostream>
#include <cassert>
#include <vector>

#include "TUCNRun.h"

#include "TUCNGeoManager.h"
#include "TUCNGeoMaterial.h"
#include "TUCNGeoNavigator.h"
#include "TUCNFieldManager.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"

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
	fParticles = 0;
	fTotalEnergy = 0.0;
	fRunTime = 0.0;
	fMaxStepTime = 0.0;
	fBoundaryLossCounter = 0;
	fDetectedCounter = 0;
	fDecayedCounter = 0;
} 

//_____________________________________________________________________________
TUCNRun::TUCNRun(const char *name, const char *title)
		  :TNamed(name, title)
{
// -- Default constructor
   Info("TUCNRun", "Constructor");
	// Create the data object
	fData = new TUCNData("ucndata", "ucndata");
	fParticles = 0;
	fTotalEnergy = 0.0;
	fRunTime = 0.0;
	fMaxStepTime = 0.0;
	fBoundaryLossCounter = 0;
	fDetectedCounter = 0;
	fDecayedCounter = 0;
}

//_____________________________________________________________________________
TUCNRun::TUCNRun(const TUCNRun& run)
		  :TNamed(run),
			fData(run.fData),
			fParticles(run.fParticles),
			fTotalEnergy(run.fTotalEnergy),
			fRunTime(run.fRunTime),
			fMaxStepTime(run.fMaxStepTime),
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
		fParticles = run.fParticles;
		fTotalEnergy = run.fTotalEnergy;
		fRunTime = run.fRunTime;
		fMaxStepTime = run.fMaxStepTime;
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
void TUCNRun::Initialise(Int_t particles, Double_t totalEnergy, Double_t runTime, Double_t maxStepTime, TUCNGeoManager* geoManager, TUCNGravField* gravField)
{
	// Initialise the Run
	// Reset the Navigator
	gGeoManager->GetCurrentNavigator()->ResetAll();
	// Store the run parameters
	fParticles = particles;
	fTotalEnergy = totalEnergy;
	fRunTime = runTime;
	fMaxStepTime = maxStepTime;
	// Generating mono-energetic particles inside the source volume
	cout << "Generating " << fParticles << " particles..."	<< endl;
	this->GenerateMonoEnergeticParticles(geoManager, gravField);
	cout << "Particle's created. Ready to Propagate..." << endl;
}

//______________________________________________________________________________
Bool_t TUCNRun::GenerateMonoEnergeticParticles(TUCNGeoManager* geoManager, TUCNGravField* gravField)
{
	// Generates a uniform distribution of particles with random directions all with the same total energy (kinetic plus potential).
	// defined at z = 0.	
	
	Int_t totalParticles = fParticles;
	Double_t totalEnergy = fTotalEnergy;
	
	// -- 0. Get the source volume and matrix that places it in the geometry
	TGeoVolume* sourceVolume = geoManager->GetSourceVolume();
	TGeoMatrix* sourceMatrix = geoManager->GetSourceMatrix();
	assert(sourceVolume != NULL && sourceMatrix != NULL);
	
	// -- 1. Check for presence of field
	// If there is a gravitational field we will need to later determine the kinetic energy from the TotalEnergy provided. 
	Bool_t fieldPresent = kFALSE;
	Double_t gravAcceleration = 0.0;
	Double_t fieldDir[3] = {0.,0.,0.};
	
	if (gravField) {
		fieldPresent = kTRUE;
		gravAcceleration = (gravField->GravAcceleration());
		fieldDir[0] = gravField->Nx(); 
		fieldDir[1] = gravField->Ny();
		fieldDir[2] = gravField->Nz();
	}

	// -- 2. Get the Source volume and matrix transformation of volume	
	// Determine the dimensions of the source volume's bounding box.
	TGeoShape* shape = sourceVolume->GetShape();
	TGeoBBox* boundingBox = static_cast<TGeoBBox*>(shape);
	const Double_t boxWallX = boundingBox->GetDX();
	const Double_t boxWallY = boundingBox->GetDY();
	const Double_t boxWallZ = boundingBox->GetDZ();
	const Double_t boxOriginX = (boundingBox->GetOrigin())[0];
	const Double_t boxOriginY = (boundingBox->GetOrigin())[1];
	const Double_t boxOriginZ = (boundingBox->GetOrigin())[2];

	#ifdef VERBOSE_MODE
		cout << "Generating random MonoEnergetic particle inside source volume: " << sourceVolume->GetName() << endl;
		cout << "Walls of Bounding Box - X: " << boxWallX << "\t" << "Y: " << boxWallY << "\t" << "Z: " << boxWallZ << endl; 
		cout << "Origin of Bounding Box - X: " << boxOriginX << "\t" << "Y: " << boxOriginY << "\t" << "Z: " << boxOriginZ << endl;
		cout << "Finding random point inside source volume..." << endl;
	#endif
	
	// -- Loop over the total number of particles to be created. 
	for (Int_t i = 0; i < totalParticles; i++) {
	
		// -- 3. Find a random point inside the Volume
		TGeoNode* currentNode = NULL;
		Double_t point[3] = {0.,0.,0.}, lpoint[3] = {0.,0.,0.}, dir[3] = {0.,0.,0.}, mom[3] = {0.,0.,0.};
		Double_t kineticEnergy = 0.0, momentum = 0.0, height = 0.0, startTime = 0.0;
	
		// Create a particle
		TUCNParticle* particle = new TUCNParticle();
	
		// Find a random point inside the volume provided
		while (currentNode == NULL) {
			// First generate random point inside bounding box, in the local coordinate frame of the box
			lpoint[0] = boxOriginX + boxWallX*gRandom->Uniform(-1.0, 1.0);
			lpoint[1] = boxOriginY + boxWallY*gRandom->Uniform(-1.0, 1.0);
			lpoint[2] = boxOriginZ + boxWallZ*gRandom->Uniform(-1.0, 1.0);
			// Next transform point to the global coordinate frame
			sourceMatrix->LocalToMaster(lpoint, point);
			// Then test to see if this point is inside the volume
			gGeoManager->SetCurrentPoint(point);
			currentNode = gGeoManager->FindNode();
			
			if (currentNode == NULL) {
				cerr << "Point - X: " << point[0] << "\t" << "Y: " << point[1] << "\t" << "Z:" << point[2] << "\t" << "not found in geometry!" << endl;
				return kFALSE;
			} else if (currentNode->GetVolume() != sourceVolume) {
				currentNode = NULL;
				continue;
			}
			
			// -- 5. Calculate the particle's kinetic energy

			// If there is a grav field we need to calculate the height of our particle in the gravitational potential 
			// in order to get the particle's kinetic energy from the defined total energy. 
			if (fieldPresent == kTRUE) {
				// Determine the height of our particle in the global coordinate system of TOP.
				// Take the dot product of the point vector with the field direction unit vector to get the height
				// of this point in the gravitational field. This assumes a convention that 'height' is measured along
				// an axis that INCREASES in the opposite direction to the field direction vector (which is usually 'downwards')
				height = -1.0*(point[0]*fieldDir[0] + point[1]*fieldDir[1] + point[2]*fieldDir[2]);
				#ifdef VERBOSE_MODE
					cout << "Height from origin of TOP volume: " <<  height << endl; 
				#endif
			}

			// Kinetic Energy
			kineticEnergy = totalEnergy - particle->Mass_GeV_c2()*gravAcceleration*height;
			if (kineticEnergy <= 0.0) {
				// Energy is negative. Find another point.
				currentNode = NULL;
				kineticEnergy = 0.0;
				height = 0.0;
				continue;
			}
			#ifdef VERBOSE_MODE
				cout << "Total Energy (neV): " << totalEnergy/Units::neV << "\t" <<  "KE (neV): " << kineticEnergy/Units::neV << endl;; 
			#endif
			
			
		}
		#ifdef VERBOSE_MODE
			cout << "Final Point Chosen - X: " << point[0] << "\t" << "Y: " << point[1] << "\t" << "Z:" << point[2] << endl;
		#endif

		// -- 4. Determine a random direction vector on the unit sphere weighted by an additional cos(theta).
		// Phi ranges from 0 to 2*Pi, Theta from 0 to Pi.
		Double_t phi = gRandom->Uniform(0.0, 1.0)*2.0*TMath::Pi();
		Double_t u   = gRandom->Uniform(0.0, 0.5);
		Double_t theta = 0.0; 
		if (gRandom->Uniform(0.0,1.0) > 0.5) {
			theta = TMath::ACos(TMath::Sqrt(1.0 - 2*u));
		} else {
			theta = TMath::ACos(-TMath::Sqrt(1.0 - 2*u));
		}
		dir[0] = TMath::Cos(phi)*TMath::Sin(theta);
		dir[1] = TMath::Sin(phi)*TMath::Sin(theta);
		dir[2] = TMath::Cos(theta);
		// Check that it is a normalised direction vector
		assert(TMath::Abs(TMath::Sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]) - 1.0) < 1.E-10);
	
		// -- 6. Initialise Particle
		// Momentum (eV)
		momentum = TMath::Sqrt(2.0*particle->Mass_GeV()*kineticEnergy);
		mom[0] = momentum*dir[0];
		mom[1] = momentum*dir[1];
		mom[2] = momentum*dir[2];
		
		#ifdef VERBOSE_MODE
			cout << "Particle Momentum (eV) - P: " << momentum/Units::eV << "\t" << "Px:" << mom[0]/Units::eV << "\t" << "Py: " << mom[1]/Units::eV << "\t" << "Pz: " << mom[2]/Units::eV << endl; 
		#endif
		
		// Initialise particle 
		particle->SetProductionVertex(point[0], point[1], point[2], startTime);
		particle->SetMomentum(mom[0], mom[1], mom[2], kineticEnergy);
//		particle->Print();
		
		// -- 7. Create the track
		// Make track and add to list
		#ifdef VERBOSE_MODE
			cout << "Creating particle track..." << endl; 
		#endif
		TVirtualGeoTrack* track = gGeoManager->MakeTrack(i, particle->GetPdgCode(), particle);
		gGeoManager->AddTrack(track);
		// Add initial point to track
		track->AddPoint(particle->Vx(), particle->Vy(), particle->Vz(), particle->T());
	}		
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::PropagateTracks(TUCNFieldManager* fieldManager)
{
// -- Propagate all tracks stored in the GeoManager for a set period of time
	Int_t numberOfTracks = gGeoManager->GetNtracks();
	// Container to store Ids of lost tracks;
	vector<Int_t> lostTracks;

	for (Int_t trackid = 0; trackid < numberOfTracks; trackid++) {
		// Get Track from list
		TVirtualGeoTrack* track = gGeoManager->GetTrack(trackid);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
		// Propagate track
		Bool_t propagated = this->Propagate(track, fieldManager);
		if (!propagated) lostTracks.push_back(trackid);
		// Add Track to data tree
		fData->AddParticle(particle);
//		fData->AddTrack(track);
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
	
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::Propagate(TVirtualGeoTrack* track, TUCNFieldManager* fieldManager)
{
	// Propagate track through geometry until it is either stopped or the runTime has been reached
	// Track passed MUST REFERENCE A TUCNPARTICLE as its particle type. UNITS:: runTime, stepTime in Seconds
	
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	TUCNGeoNavigator* navigator = static_cast<TUCNGeoNavigator*>(gGeoManager->GetCurrentNavigator());
	
	// -- 1. Initialise Track
	// Set Current Track -- needed by fData
	gGeoManager->SetCurrentTrack(track);
	// Initialise track - Sets navigator's current point/direction/node to that of the particle
	gGeoManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
		particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
	
		#ifdef VERBOSE_MODE				
		cout << "PropagateForSetTime - Starting Run - Max time (seconds): " <<  fRunTime << endl;
	#endif
	
	// -- Check that Particle has not been initialised inside a boundary or detector		
	if (static_cast<TUCNGeoMaterial*>(navigator->GetCurrentNode()->GetMedium()->GetMaterial())->IsTrackingMaterial() == kFALSE) {
		cerr << "Track number " << track->GetId() << " initialised inside boundary of " << navigator->GetCurrentVolume()->GetName() << endl;
		return kFALSE;
	}
	
	///////////////////////////////////	
	// -- 2. Propagation Loop
	///////////////////////////////////
	Int_t stepNumber;
	for (stepNumber = 1 ; ; stepNumber++) {
		
		#ifdef VERBOSE_MODE		
			cout << "STEP " << stepNumber << "\t" << particle->T() << " s" << "\t" << navigator->GetCurrentNode()->GetName() << endl;	
		#endif
			
		// -- Calculate the Next StepTime
		navigator->DetermineNextStepTime(particle, fMaxStepTime, fRunTime);
		
		// -- Make a step
		Bool_t stepSuccess = navigator->MakeStep(track, fieldManager);
		assert(stepSuccess == kTRUE);
		
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
	Double_t avgField = particle->AvgMagField();
	Double_t avgFieldError = particle->AvgMagFieldError(avgField);
	
//	cout << "AvgField: " << avgField << "\t" << "Error: " << avgFieldError << endl; 
	
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
	Int_t particles = fData->GetTracks()->GetEntries();
	cout << "Particles: " << particles << endl;
	
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
	TGeoTrack* track = this->GetTrack(trackID);
	assert(track != NULL);
	track->Draw();
}

//_____________________________________________________________________________
void	TUCNRun::WriteOutData(TFile* file)
{
	assert(file->IsOpen());
	cout << "Writing data to file: " << file->GetName() << endl;
	fData->Write();
	cout << "WriteOutData completed" << endl;		
}

//_____________________________________________________________________________
TGeoTrack* TUCNRun::GetTrack(Int_t trackID)
{
	TGeoTrack* track = 0;
	assert(trackID < fData->GetTracks()->GetEntries() && trackID >= 0);
	fData->GetTracks()->SetBranchAddress("Tracks", &track);
	fData->GetTracks()->GetEntry(trackID);
	return track;
}

//_____________________________________________________________________________
TUCNParticle* TUCNRun::GetParticle(Int_t particleID)
{
	TUCNParticle* particle = 0;
	assert(particleID < fData->GetTracks()->GetEntries() && particleID >= 0);
	fData->GetTracks()->SetBranchAddress("Particles", &particle);
	fData->GetTracks()->GetEntry(particleID);
	return particle;
}

