// TUCNParticleCloud
// Author: Matthew Raso-Barnett  19/01/2009
#include <iostream>
#include <stdexcept>
#include <fstream>

#include "TMath.h"
#include "TObjArray.h"
#include "TRandom3.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"

#include "TUCNGeoManager.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"
#include "Constants.h"
#include "Units.h"

#include "TUCNParticleCloud.h"

using std::runtime_error;
using std::cout;
using std::endl;

//#define VERBOSE_MODE

ClassImp(TUCNParticleCloud) 

//______________________________________________________________________________
TUCNParticleCloud::TUCNParticleCloud()
{
//	-- Default constructor
	Info("TUCNParticleCloud", "Dummy Constructor");
	fParticles = NULL;
	fUCNGeometry = NULL;
}

//______________________________________________________________________________
TUCNParticleCloud::TUCNParticleCloud(TUCNGeoManager* geom)
{
//	-- Default constructor
	Info("TUCNParticleCloud", "Constructor");
	fParticles = new TObjArray(256);
	fUCNGeometry = geom;
}

//_____________________________________________________________________________
TUCNParticleCloud::TUCNParticleCloud(const TUCNParticleCloud& part)
              		:TObject(part),
               	 fParticles(part.fParticles),
               	 fUCNGeometry(part.fUCNGeometry)
{
// -- Copy Constructor
	Info("TUCNParticleCloud", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNParticleCloud& TUCNParticleCloud::operator=(const TUCNParticleCloud& part)
{
// --assignment operator
	if(this!=&part) {
     TObject::operator=(part);
     fParticles = part.fParticles;
	  fUCNGeometry = part.fUCNGeometry;
	}
	return *this;
}

//______________________________________________________________________________
TUCNParticleCloud::~TUCNParticleCloud()
{ 
// -- Destructor	
	Info("TUCNParticleCloud", "Destructor");
	if (fParticles) {fParticles->Delete(); delete fParticles;}
}

//______________________________________________________________________________
TObjArray* TUCNParticleCloud::UniformMonoEnergeticDistribution(Int_t particles, Double_t totalEnergy, TGeoVolume* volume, TGeoMatrix* matrix)
{
//	Fill the TObjArray with monoenergetic particles, uniformly distributed inside the volume passed. 
// Directions will be random on the unit sphere with an additional cos(theta) weighting the directions towards the poles. 

// Warning: It does not at this time (04/06/2009) seem possible to extract the TGeoMatrix used to position a volume in the overall geometry heirarchy
// directly from the TGeoVolume object itself. If it does become possible, then this will be added, but for now, the overall matrix used (could be a multiple
// of matrices multiplied together) to position this volume in TOP must be supplied for this method to properly fill an arbitrarily translated, rotated volume.
	
//	ofstream out3("initialangles-1-1.txt");
//	out3 << "i" << "\t" << "dx"<< "\t" << "dy" << "\t" << "dz" << "\t" << "theta" << "\t" << "phi" << "\t" << endl;
	
	if (particles <= 0) {
		throw runtime_error("In TUCNParticleCloud::UniformMonoEnergeticDistribution - Number of particles to initialise is zero.");
	} 
	
	// If there is a gravitational field we will need to later determine the kinetic energy from the TotalEnergy provided. 
	Bool_t fieldPresent = kFALSE;
	Double_t g = 0.0;
	Double_t fieldDir[3] = {0.,0.,0.};
	if (this->fUCNGeometry->GetGravField() != NULL) {
		fieldPresent = kTRUE;
		g = (this->fUCNGeometry->GetGravField()->GravAcceleration());
		fieldDir[0] = this->fUCNGeometry->GetGravField()->Nx(); 
		fieldDir[1] = this->fUCNGeometry->GetGravField()->Ny();
		fieldDir[2] = this->fUCNGeometry->GetGravField()->Nz();
	}
	
	// Determine the dimensions of the shape's bounding box, that we wish to initialise inside
	TGeoShape* shape = volume->GetShape();
	TGeoBBox*  boundingBox = (TGeoBBox*)shape;
	const Double_t boxWallX = boundingBox->GetDX();
	const Double_t boxWallY = boundingBox->GetDY();
	const Double_t boxWallZ = boundingBox->GetDZ();
	const Double_t boxOriginX = (boundingBox->GetOrigin())[0];
	const Double_t boxOriginY = (boundingBox->GetOrigin())[1];
	const Double_t boxOriginZ = (boundingBox->GetOrigin())[2];
	
	#ifdef VERBOSE_MODE
		Info("UniformMonoEnergeticDistribution", "Bounding Box - X: %f, Y: %f, Z: %f, in volume, %s", boxWallX, boxWallY, boxWallZ, volume->GetName());
		Info("UniformMonoEnergeticDistribution", "Bounding Box Origin - X: %f, Y: %f, Z: %f", boxOriginX, boxOriginY, boxOriginZ);
	#endif
	
	for (Int_t i = 0; i < particles; i++) {
		// Initialise important parameters
		TGeoNode* currentNode = NULL;
		Double_t point[3] = {0.,0.,0.}, lpoint[3] = {0.,0.,0.}, dir[3] = {0.,0.,0.}, mom[3] = {0.,0.,0.};
		Double_t kineticEnergy = 0.0, momentum = 0.0, height = 0.0, startTime = 0.0;
		
		// Create a particle object on heap
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
			this->Geometry()->SetCurrentPoint(point);
			currentNode = this->Geometry()->FindNode();
						
			if (currentNode == NULL) {
				Warning("UniformMonoEnergeticDistribution", "Point - X: %f, Y: %f, Z: %f,   not found in geometry!", point[0], point[1], point[2]);
			} else if (currentNode->GetVolume() != volume) {
				currentNode = NULL;
			}
		}
		#ifdef VERBOSE_MODE
			Info("UniformMonoEnergeticDistribution", "Number: %i 	Point- X: %f, Y: %f, Z: %f, in volume, %s", i, point[0], point[1], point[2], currentNode->GetVolume()->GetName());
		#endif
		
		// Determine a random direction vector on the unit sphere weighted by an additional cos(theta). Phi ranges from 0 to 2*Pi, Theta from 0 to Pi.
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
		if(TMath::Abs(TMath::Sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]) - 1.0) > 1.E-10){
			throw runtime_error("In TUCNParticleCloud::UniformMonoEnergeticDistribution - Direction vector is not normalised!");
		}
		
		// If there is a grav field we need to calculate the height of our particle in the gravitational potential 
		//in order to get the particle's kinetic energy from the defined total energy. 
		if (fieldPresent == kTRUE) {
			// Determine the height of our particle in the global coordinate system of TOP.
			// Take the dot product of the point vector with the field direction unit vector to get the height of this point in the gravitational field.
			// This assumes a convention that 'height' is measured along an axis that INCREASES in the opposite direction to the field direction vector
			// (which is usually 'downwards')
			height = -1.0*(point[0]*fieldDir[0] + point[1]*fieldDir[1] + point[2]*fieldDir[2]);
			#ifdef VERBOSE_MODE
//				Info("UniformMonoEnergeticDistribution", "Height: %f", height); 
			#endif
		}
		
		// Kinetic Energy
		kineticEnergy = totalEnergy - particle->Mass_GeV_c2()*g*height;
		if (kineticEnergy <= 0.0) {
			Info("UniformMonoEnergeticDistribution", "Height: %f, KE: %f", height, kineticEnergy/Units::neV); 
			throw runtime_error("In TUCNParticleCloud::UniformMonoEnergeticDistribution - Attempted to initialise particles with too small a total energy for the height of container. Kinetic Energy < 0!");
		}
		
		// Momentum
		momentum = TMath::Sqrt(2.0*particle->Mass_GeV()*kineticEnergy);
		mom[0] = momentum*dir[0];
		mom[1] = momentum*dir[1];
		mom[2] = momentum*dir[2];
		
		#ifdef VERBOSE_MODE
			Info("UniformMonoEnergeticDistribution", "Px: %f, 	Py: %f, Pz: %f", mom[0]/Units::eV, mom[1]/Units::eV, mom[2]/Units::eV); 
			Info("UniformMonoEnergeticDistribution", "TotalE: %f, 	KE: %f, 		Momentum: %f", totalEnergy/Units::neV, kineticEnergy/Units::neV, momentum/Units::eV); 
		#endif
		
		// Initialise particle and store in TObjArray
		particle->SetProductionVertex(point[0], point[1], point[2], startTime);
		particle->SetMomentum(mom[0], mom[1], mom[2], kineticEnergy);
		this->GetListOfParticles()->Add(particle);
		
		// Write angle data out to file
//		out3 << i << "\t" << dir[0] << "\t" << dir[1] << "\t" << dir[2] << "\t" << theta << "\t" << phi << "\t" << endl;
	}
	return this->GetListOfParticles();
}

//______________________________________________________________________________
TUCNParticle* TUCNParticleCloud::GetParticle(Int_t id) const 
{
	if (this->GetListOfParticles() == NULL) {
		Error("GetParticle", "No particles have been initialised!");
		return 0;
	} else if (id >= this->GetListOfParticles()->GetEntriesFast()) {
		Error("GetParticle", "No particle at index: %i. Only %i particles in array", id, this->GetListOfParticles()->GetEntriesFast());
		return 0;
	} 
	return static_cast<TUCNParticle*>(this->GetListOfParticles()->At(id));
}
