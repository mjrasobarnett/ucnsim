// TUCNRunManager
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>

#include "TUCNRunManager.h"

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

#include "../geom/geometries.h"
#include "Units.h"

using std::cout;
using std::endl;
using std::cerr;

//#define VERBOSE_MODE

ClassImp(TUCNRunManager)

//_____________________________________________________________________________
TUCNRunManager::TUCNRunManager()
					:TNamed("RunManager", "Default Run Manager")
{
// -- Default constructor
   Info("TUCNRunManager", "Constructor");
	fManager = new TUCNGeoManager("GeoManager", "Geometry Manager");
	
	// Create the UCNNavigator and initialise in the UCNManager
	Info("TUCNRunManager", "Creating Navigator...");
	TUCNGeoNavigator* navigator = new TUCNGeoNavigator( fManager );
	Int_t navIndex = fManager->AddNavigator(navigator);
	fManager->SetCurrentNavigator(navIndex);
	navigator = NULL;
	
	// Create the data object
	fData = new TUCNData("ucndata", "ucndata");
	
} 

//_____________________________________________________________________________
TUCNRunManager::TUCNRunManager(const TUCNRunManager& runm)
					:TNamed(runm),
					 fManager(runm.fManager),
					 fData(runm.fData)
{
// -- Copy Constructor
	Info("TUCNRunManager", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNRunManager& TUCNRunManager::operator=(const TUCNRunManager& runm)
{
// --assignment operator
	if(this!=&runm) {
      TNamed::operator=(runm);
		fManager = runm.fManager;
		fData = runm.fData;
		
	}
   return *this;
}

//______________________________________________________________________________
TUCNRunManager::~TUCNRunManager()
{ 
// -- Destructor
	Info("TUCNRunManager", "Destructor");
//	fManager->Delete();
//	fData->Delete();
}

// -- METHODS --

//______________________________________________________________________________
TGeoNode* TUCNRunManager::CreateGeometry()
{ 
// -- 
	Info("TUCNRunManager", "CreateGeometry");
	// -- Create Geometry using function specified in geometries.C macro
	return CreateGeometry_v1(fManager);
}

//______________________________________________________________________________
Bool_t TUCNRunManager::GenerateMonoEnergeticParticles(TGeoVolume* sourceVolume, TGeoMatrix* matrix, Int_t totalParticles, Double_t totalEnergy)
{

	// Warning: It does not at this time (04/06/2009) seem possible to extract the TGeoMatrix used to position a volume in the overall geometry heirarchy
	// directly from the TGeoVolume object itself, and thus it must be given as an arguement.

	// -- 1. Check for presence of field
	// If there is a gravitational field we will need to later determine the kinetic energy from the TotalEnergy provided. 
	Bool_t fieldPresent = kFALSE;
	Double_t gravAcceleration = 0.0;
	Double_t fieldDir[3] = {0.,0.,0.};
	
	if (fManager->CheckForGravity() == kTRUE) {
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
			if (matrix != NULL) {
				matrix->LocalToMaster(lpoint, point);
			}
			// Then test to see if this point is inside the volume
			fManager->SetCurrentPoint(point);
			currentNode = fManager->FindNode();

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
			cout << "Particle Momentum (eV) - P: " momentum/Units::eV << "\t" << "Px:" << mom[0]/Units::eV << "\t" << "Py: " << mom[1]/Units::eV << "\t" << "Pz: " << mom[2]/Units::eV << endl; 
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
		TVirtualGeoTrack* track = fManager->MakeTrack(i, particle->GetPdgCode(), particle);
		fManager->AddTrack(track);
		// Add initial point to track
		track->AddPoint(particle->Vx(), particle->Vy(), particle->Vz(), particle->T());
	}		
	return kTRUE;
}


//_____________________________________________________________________________
Bool_t TUCNRunManager::PropagateAllTracks(Double_t runTime, Double_t maxStepTime)
{
// -- Propagate all tracks stored in the GeoManager for a set period of time
	Int_t numberOfTracks = fManager->GetNtracks();
	for (Int_t trackid = 0; trackid < numberOfTracks; trackid++) {
		// Get Track from list
		TVirtualGeoTrack* track = fManager->GetTrack(trackid);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
		// Set Current Track -- needed by fData
		fManager->SetCurrentTrack(track);
		// Initialise track - Sets navigator's current point/direction/node to that of the particle
		fManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
			particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
		// Propagate track
		static_cast<TUCNGeoNavigator*>(fManager->GetCurrentNavigator())->PropagateTrack(track, runTime, maxStepTime);
		// Add Track to data tree
//		fData->AddParticle(particle);
		fData->AddTrack(track);
		// Reset Track to release memory
		track->ResetTrack();
//		cout << "test" << trackid << endl;
	}
	
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRunManager::PropagateAllTracks(Int_t steps, Double_t maxStepTime)
{
// -- Propagate all tracks stored in the GeoManager for a set number of steps
	Int_t numberOfTracks = fManager->GetNtracks();
	for (Int_t trackid = 0; trackid < numberOfTracks; trackid++) {
		// Get Track from list
		TVirtualGeoTrack* track = fManager->GetTrack(trackid);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
		// Set Current Track -- needed by fData
		fManager->SetCurrentTrack(track);
		// Initialise track - Sets navigator's current point/direction/node to that of the particle
		fManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
			particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
		// Propagate track
		static_cast<TUCNGeoNavigator*>(fManager->GetCurrentNavigator())->PropagateTrack(track, steps, maxStepTime);
		// Add Track to data tree
		fData->AddParticle(particle);
		// Reset Track to release memory
		track->ResetTrack();
	}
	
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRunManager::PropagateTrack(Double_t runTime, Double_t maxStepTime, Int_t trackIndex)
{
// -- Propagate single track stored in the GeoManager for a set period of time
	Int_t numberOfTracks = fManager->GetNtracks();
	assert(trackIndex >= 0 && trackIndex < numberOfTracks);
	
	// Get Track from list
	TVirtualGeoTrack* track = fManager->GetTrack(trackIndex);
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	// Set Current Track -- needed by fData
	fManager->SetCurrentTrack(track);
	// Initialise track - Sets navigator's current point/direction/node to that of the particle
	fManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
		particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
	// Propagate track
	static_cast<TUCNGeoNavigator*>(fManager->GetCurrentNavigator())->PropagateTrack(track, runTime, maxStepTime);
	// Add Track to data tree
	fData->AddParticle(particle);
	// Reset Track to free memory
	track->ResetTrack();
	
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRunManager::PropagateTrack(Int_t steps, Double_t maxStepTime, Int_t trackIndex)
{
// -- Propagate single track stored in the GeoManager for a set number of steps
	Int_t numberOfTracks = fManager->GetNtracks();
	assert(trackIndex >= 0 && trackIndex < numberOfTracks);
	
	// Get Track from list
	TVirtualGeoTrack* track = fManager->GetTrack(trackIndex);
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	// Set Current Track -- needed by fData
	fManager->SetCurrentTrack(track);
	// Initialise track - Sets navigator's current point/direction/node to that of the particle
	fManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
		particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
	// Propagate track
	static_cast<TUCNGeoNavigator*>(fManager->GetCurrentNavigator())->PropagateTrack(track, steps, maxStepTime);
	// Add Track to data tree
	fData->AddParticle(particle);
	// Reset Track to free memory
	track->ResetTrack();
	
	return kTRUE;
}

//_____________________________________________________________________________
void TUCNRunManager::DrawParticles(TCanvas* canvas, TPolyMarker3D* points)
{
	canvas->Draw();
	
	// -- Draw Volumes
	fManager->GetTopVolume()->Draw("ogl");
	fManager->SetVisLevel(4);
	fManager->SetVisOption(0);
	
	// -- Number of particles
	Int_t particles = fManager->GetNtracks();
	
	// -- Draw Particles
	for (Int_t i = 0; i < particles; i++) {
		TUCNParticle* particle = static_cast<TUCNParticle*>(this->GetTrack(i)->GetParticle());
		points->SetPoint(i, particle->Vx(), particle->Vy(), particle->Vz());
	}
	points->SetMarkerColor(2);
	points->SetMarkerStyle(6);
	points->Draw();
}

//_____________________________________________________________________________
void	TUCNRunManager::DrawTrack(TCanvas* canvas, TVirtualGeoTrack* track)
{
	canvas->Draw();
	// -- Draw Volumes
	fManager->GetTopVolume()->Draw();
	fManager->SetVisLevel(4);
	fManager->SetVisOption(0);
	// -- Draw Track
	track->Draw();
}

//_____________________________________________________________________________
void	TUCNRunManager::WriteOutData(TFile* file)
{
	assert(file->IsOpen());
	cout << "Writing data to file: " << file->GetName() << endl;
	fData->Write();
	cout << "WriteOutData completed" << endl;		
}

//_____________________________________________________________________________
TGeoTrack* TUCNRunManager::GetTrack(Int_t trackID)
{
	TGeoTrack* track = 0;
	assert(trackID < fData->GetTracks()->GetEntries() && trackID >= 0);
	fData->GetTracks()->SetBranchAddress("Tracks", &track);
	fData->GetTracks()->GetEntry(trackID);
	return track;
}