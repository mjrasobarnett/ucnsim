// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#include <iostream>
#include <cassert>
#include <vector>

#include "TUCNRun.h"

#include "TUCNGeoManager.h"
#include "TUCNGeoNavigator.h"
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
	
	// Create the UCNNavigator and initialise in the UCNManager
	Info("TUCNRun", "Creating a new Navigator...");
	TUCNGeoNavigator* navigator = new TUCNGeoNavigator(static_cast<TUCNGeoManager*>(gGeoManager));
	fNavigatorIndex = gGeoManager->AddNavigator(navigator);
	gGeoManager->SetCurrentNavigator(fNavigatorIndex);
	
	// Create the data object
	fData = new TUCNData("ucndata", "ucndata");
} 

//_____________________________________________________________________________
TUCNRun::TUCNRun(const TUCNRun& run)
		  :TNamed(run),
			fData(run.fData),
			fNavigatorIndex(run.fNavigatorIndex)
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
		fNavigatorIndex = run.fNavigatorIndex;
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
void TUCNRun::Initialise(Int_t particles, Double_t totalEnergy)
{
	// SetCurrentNavigator
	gGeoManager->SetCurrentNavigator(fNavigatorIndex);
	// Turn on gravity
	this->TurnGravityOn();
	// Generating mono-energetic particles inside the source volume
	gGeoManager->GetCurrentNavigator()->Print();
	cout << gGeoManager->GetCurrentNode() << endl;
	cout << "Generating " << particles << " particles..."	<< endl;
	this->GenerateMonoEnergeticParticles(particles, totalEnergy);
	cout << "Particle's created. Preparing to Propagate..." << endl;
}

//______________________________________________________________________________
Bool_t TUCNRun::GenerateMonoEnergeticParticles(Int_t totalParticles, Double_t totalEnergy)
{
	// Generates a uniform distribution of particles with random directions all with the same total energy (kinetic plus potential).
	// defined at z = 0.	
	
	// -- 0. Get the source volume and matrix that places it in the geometry
	TGeoVolume* sourceVolume = static_cast<TUCNGeoManager*>(gGeoManager)->GetSourceVolume();
	TGeoMatrix* sourceMatrix = static_cast<TUCNGeoManager*>(gGeoManager)->GetSourceMatrix();
	assert(sourceVolume != NULL && sourceMatrix != NULL);
	
	// -- 1. Check for presence of field
	// If there is a gravitational field we will need to later determine the kinetic energy from the TotalEnergy provided. 
	Bool_t fieldPresent = kFALSE;
	Double_t gravAcceleration = 0.0;
	Double_t fieldDir[3] = {0.,0.,0.};
	
	if (static_cast<TUCNGeoManager*>(gGeoManager)->CheckForGravity() == kTRUE) {
		fieldPresent = kTRUE;
		gravAcceleration = (TUCNGravField::Instance()->GravAcceleration());
		fieldDir[0] = TUCNGravField::Instance()->Nx(); 
		fieldDir[1] = TUCNGravField::Instance()->Ny();
		fieldDir[2] = TUCNGravField::Instance()->Nz();
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
				// Take the dot product of the point vector with the field direction unit vector to get the height of this point in the gravitational field.
				// This assumes a convention that 'height' is measured along an axis that INCREASES in the opposite direction to the field direction vector
				// (which is usually 'downwards')
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

		// -- 4. Determine a random direction vector on the unit sphere weighted by an additional cos(theta). Phi ranges from 0 to 2*Pi, Theta from 0 to Pi.
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
Bool_t TUCNRun::PropagateAllTracks(Double_t runTime, Double_t maxStepTime)
{
// -- Propagate all tracks stored in the GeoManager for a set period of time
	Int_t numberOfTracks = gGeoManager->GetNtracks();
	// Container to store Ids of lost tracks;
	vector<Int_t> lostTracks;

	for (Int_t trackid = 0; trackid < numberOfTracks; trackid++) {
		// Get Track from list
		TVirtualGeoTrack* track = gGeoManager->GetTrack(trackid);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
		// Set Current Track -- needed by fData
		gGeoManager->SetCurrentTrack(track);
		// Initialise track - Sets navigator's current point/direction/node to that of the particle
		gGeoManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
			particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
		// Propagate track
		Bool_t propagated = static_cast<TUCNGeoNavigator*>(gGeoManager->GetCurrentNavigator())->PropagateTrack(track, runTime, maxStepTime);
		if (!propagated) lostTracks.push_back(trackid);
		// Add Track to data tree
		fData->AddParticle(particle);
//		fData->AddTrack(track);
		// Reset Track to release memory
		track->ResetTrack();
	}
	
	if (lostTracks.size() != 0) {
		cout << "Lost Tracks:" << endl;
		for (UInt_t i = 0; i < lostTracks.size(); i++) {
			cout << lostTracks[i] << endl;
		}
	}
	
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::PropagateAllTracks(Int_t steps, Double_t maxStepTime)
{
// -- Propagate all tracks stored in the GeoManager for a set number of steps
	Int_t numberOfTracks = gGeoManager->GetNtracks();
	// Container to store Ids of lost tracks;
	vector<Int_t> lostTracks;

	for (Int_t trackid = 0; trackid < numberOfTracks; trackid++) {
		// Get Track from list
		TVirtualGeoTrack* track = gGeoManager->GetTrack(trackid);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
		// Set Current Track -- needed by fData
		gGeoManager->SetCurrentTrack(track);
		// Initialise track - Sets navigator's current point/direction/node to that of the particle
		gGeoManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
			particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
		// Propagate track
		Bool_t propagated = static_cast<TUCNGeoNavigator*>(gGeoManager->GetCurrentNavigator())->PropagateTrack(track, steps, maxStepTime);
		if (!propagated) lostTracks.push_back(trackid);
		// Add Track to data tree
		fData->AddParticle(particle);
		// Reset Track to release memory
		track->ResetTrack();
	}
	
	if (lostTracks.size() != 0) {
		cout << "Lost Tracks:" << endl;
		for (UInt_t i = 0; i < lostTracks.size(); i++) {
			cout << lostTracks[i] << endl;
		}
	}
	
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::PropagateTrack(Double_t runTime, Double_t maxStepTime, Int_t trackIndex)
{
// -- Propagate single track stored in the GeoManager for a set period of time
	Int_t numberOfTracks = gGeoManager->GetNtracks();
	assert(trackIndex >= 0 && trackIndex < numberOfTracks);
	
	// Get Track from list
	TVirtualGeoTrack* track = gGeoManager->GetTrack(trackIndex);
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	// Set Current Track -- needed by fData
	gGeoManager->SetCurrentTrack(track);
	// Initialise track - Sets navigator's current point/direction/node to that of the particle
	gGeoManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
		particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
	// Propagate track
	static_cast<TUCNGeoNavigator*>(gGeoManager->GetCurrentNavigator())->PropagateTrack(track, runTime, maxStepTime);
	// Add Track to data tree
	fData->AddParticle(particle);
	// Reset Track to free memory
	track->ResetTrack();
	
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRun::PropagateTrack(Int_t steps, Double_t maxStepTime, Int_t trackIndex)
{
// -- Propagate single track stored in the GeoManager for a set number of steps
	Int_t numberOfTracks = gGeoManager->GetNtracks();
	assert(trackIndex >= 0 && trackIndex < numberOfTracks);
	
	// Get Track from list
	TVirtualGeoTrack* track = gGeoManager->GetTrack(trackIndex);
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	// Set Current Track -- needed by fData
	gGeoManager->SetCurrentTrack(track);
	// Initialise track - Sets navigator's current point/direction/node to that of the particle
	gGeoManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
		particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
	// Propagate track
	static_cast<TUCNGeoNavigator*>(gGeoManager->GetCurrentNavigator())->PropagateTrack(track, steps, maxStepTime);
	// Add Track to data tree
	fData->AddParticle(particle);
	// Reset Track to free memory
	track->ResetTrack();
	
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

