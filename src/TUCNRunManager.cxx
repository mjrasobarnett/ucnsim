// TUCNRunManager
// Author: Matthew Raso-Barnett  01/08/2009
#include <iostream>
#include <stdexcept>

#include "TMath.h"

#include "TUCNRunManager.h"

#include "Units.h"

using std::cout;
using std::endl;
using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(TUCNRunManager)

//_____________________________________________________________________________
TUCNRunManager::TUCNRunManager()
{
// -- Default constructor.
   Info("TUCNRunManager", "Constructor");
} 

//_____________________________________________________________________________
TUCNRunManager::TUCNRunManager(const TUCNParticle& part)
{
// -- Copy Constructor
//	Info("TUCNRunManager", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNRunManager& TUCNRunManager::operator=(const TUCNRunManager& part)
{
// --assignment operator
	
	return *this;
}

//______________________________________________________________________________
TUCNRunManager::~TUCNRunManager()
{ 
// -- Destructor	
	Info("TUCNRunManager", "Destructor");
}



//_____________________________________________________________________________
Double_t TUCNRunManager::DetermineNextStepTime(TUCNParticle* particle, const Double_t runTime)			 
{
// Placeholder for method to calculate the next step time depending on electric/magnetic field environment
	if (this->GetStepTime() == 0.0) {
		this->SetStepTime(0.01*Units::s);
	}
	
	// -- Check if we will reach the maximum runtime of the track. If so propagate only until this time.
	if (particle->T() > (runTime - this->GetStepTime())) {
		this->SetStepTime((runTime - particle->T()) + fgTolerance);
	}
	return this->GetStepTime();
}

//_____________________________________________________________________________
void TUCNRunManager::SetStepTime(Double_t stepTime) 
{
// 	
	if (stepTime <= 0.0) {
		Error("SetStepTime", "StepTime %f , cannot be set to zero or a negative value. StepTime will be set to the default value of 0.01s", stepTime);
		fStepTime = 0.01*Units::s;
	} else{
		fStepTime = stepTime;
	}
}

//_____________________________________________________________________________
Bool_t TUCNRunManager::PropagateTrack(TVirtualGeoTrack* track, const Double_t runTime, const Double_t maxStepTime)
{
// Propagate track through geometry until it is either stopped or the runTime has been reached
// Track passed MUST REFERENCE A TUCNPARTICLE as its particle type. 
// In future I will attempt to guarentee this. (4/06/09) 

// UNITS:: runTime, stepTime in Seconds

	Int_t stepNumber;
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());

#ifdef VERBOSE_MODE				
	Info("PropagateForSetTime","Starting Run - Max time (seconds): %f", runTime);
#endif
	
	// -- Check that Particle has not been initialised inside a boundary or detector		
	if (static_cast<TUCNGeoMaterial*>(this->GetCurrentNode()->GetMedium()->GetMaterial())->IsTrackingMaterial() == kFALSE) {
		cout << this->GetCurrentNode()->GetMedium()->GetName() << endl;
		throw runtime_error("In TUCNRunManager::PropagateTrack - particle initialised inside a boundary");
	}
	
	// -- Get the Grav Field
	TUCNGravField* field = fUCNGeometry->GetGravField();
	
	///////////////////////////////////	
	// -- Propagation Loop
	///////////////////////////////////
	for (stepNumber = 1 ; stepNumber < fgMaxSteps ; stepNumber++) {
		
		#ifdef VERBOSE_MODE		
			cout << "STEP " << stepNumber << "\t" << particle->T() << " s" << "\t" << this->GetCurrentNode()->GetName() << endl;	
		#endif
		
		// -- Calculate the Next StepTime
		this->SetStepTime(maxStepTime); // Start with the default value
		this->DetermineNextStepTime(particle, runTime); // Check if there are any factors that will reduce this time
		
		// -- Find time to reach next boundary and step along parabola
		#ifdef VERBOSE_MODE	
			cout << "------------------- START OF STEP ----------------------" << endl;
			cout << "Initial Steptime (s): " << this->GetStepTime() << endl;
			cout << "Initial Vertex (m): " << "X:" << particle->Vx() << "\t" << "Y:" << particle->Vy()  << "\t" << "Z:" << particle->Vz() << "\t" << "T:" << particle->T() << endl;
			cout << "Initial Dir: " << "X:" << particle->DirX()  << "\t" << "Y:" << particle->DirY()  << "\t" << "Z:" << particle->DirZ() << "\t" << "Mag:" << particle->Dir() << endl;
			cout << "Initial Vel (m/s): " << "X:" << particle->VelocityX()  << "\t" << "Y:" << particle->VelocityY()  << "\t" << "Z:" << particle->VelocityZ() << "\t" << "V:" << particle->Velocity() << endl;
			cout << "Initial Mom (eV): " << "X:" << particle->Px()  << "\t" << "Y:" << particle->Py()  << "\t" << "Z:" << particle->Pz() << "\t" << "P:" << particle->P() << endl;
			cout << "Initial Volume: "    << this->GetCurrentVolume()->GetName() << endl;
			cout << "Initial Energy (neV): "    << particle->Energy() /Units::neV << endl;
			cout << "-----------------------------" << endl;
			cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
			cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
			cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
			cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
			cout << "-----------------------------" << endl << endl;
		#endif
		
		
		if (field == NULL) {
			this->FindNextBoundaryAndStep(this->GetStepTime());
			this->UpdateTrack(track, this->GetStepTime());
			// TODO: UPDATE CLASS DATA MEMBERS NOW LIKE fUCNNextNode
			
		} else {
			// -- Update Particle is called by FindNext...AlongParabola so no need to repeat that here
			this->FindNextBoundaryAndStepAlongParabola(track, field, this->GetStepTime());
		}
		
			
		#ifdef VERBOSE_MODE	
			cout << "------------------- END OF STEP ----------------------" << endl;
			cout << "Final Steptime (s): " << this->GetStepTime() << endl;
			cout << "Final Vertex (m): " << "X:" << particle->Vx() << "\t" << "Y:" << particle->Vy()  << "\t" << "Z:" << particle->Vz() << "\t" << "T:" << particle->T() << endl;
			cout << "Final Dir: " << "X:" << particle->DirX()  << "\t" << "Y:" << particle->DirY()  << "\t" << "Z:" << particle->DirZ() << "\t" << "Mag:" << particle->Dir() << endl;
			cout << "Final Vel (m/s): " << "X:" << particle->VelocityX()  << "\t" << "Y:" << particle->VelocityY()  << "\t" << "Z:" << particle->VelocityZ() << "\t" << "V:" << particle->Velocity() << endl;
			cout << "Final Mom (eV): " << "X:" << particle->Px()  << "\t" << "Y:" << particle->Py()  << "\t" << "Z:" << particle->Pz() << "\t" << "P:" << particle->P() << endl;
			cout << "Final Volume: "    << this->GetCurrentVolume()->GetName() << endl;
			cout << "Final Energy (neV): "    << particle->Energy() /Units::neV << endl;
			cout << "-----------------------------" << endl;
			cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
			cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
			cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
			cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
			cout << "-----------------------------" << endl << endl;
		#endif	
		
		TUCNGeoMaterial* currentMaterial = static_cast<TUCNGeoMaterial*>(this->GetCurrentNode()->GetMedium()->GetMaterial());
		
		// -- Determine what to do if we are on a boundary
		// -- Is Track on the surface of a boundary?
		if (currentMaterial->IsTrackingMaterial() == kFALSE) {
			// -- Are we on the surface of a detector?
			if (currentMaterial->IsDetectorMaterial() == kTRUE) {
				// -- Was particle detected?
				Double_t prob = gRandom->Uniform(0.0,1.0);
				if (prob < currentMaterial->DetectionEfficiency()) {	
					particle->Detected(kTRUE);  // Set detected flag
				} else {	
				#ifdef VERBOSE_MODE	
					cout << "------------------- BOUNCE ----------------------" << endl;
					cout << "Current medium: " << this->GetCurrentNode()->GetMedium()->GetName() << endl;
				#endif
					// -- Make a Bounce
					this->Bounce(track);	
					// -- Cross Boundary and Locate
					this->CrossBoundaryAndLocate(kFALSE, this->GetCurrentNode()); 	
					this->UpdateTrack(track);
				#ifdef VERBOSE_MODE	
					cout << "-------------------------------------------------" << endl;
					cout << "Final medium: " << this->GetCurrentNode()->GetMedium()->GetName() << endl;
					cout << "-------------------------------------------------" << endl << endl;
				#endif
				}
			// -- Was particle lost to boundary (absorbed/upscattered) ?
			} else if (kFALSE) {	
				particle->Lost(kTRUE); // Set lost flag
			// -- Are we outside the geometry heirarchy we have built - ie: in TOP
			} else if (currentMaterial->IsBlackhole() == kTRUE) {
				particle->Lost(kTRUE);
			//	throw runtime_error("In TUCNRunManager::PropagateTrack - track has entered TOP/Blackhole!");
			} else {
				#ifdef VERBOSE_MODE	
					cout << "------------------- BOUNCE ----------------------" << endl;
					cout << "Current Node: " << this->GetCurrentNode()->GetName() << endl;
				#endif	
				// -- Make a Bounce
				this->Bounce(track);
				// -- Cross Boundary and Locate
				if (this->IsUCNStepEntering() == kTRUE) {
					#ifdef VERBOSE_MODE	
						cout << "Search Upwards - Step entering a volume so we must step back out" << endl;
						cout << this->GetCurrentNode()->GetVolume()->GetNextNodeIndex() << endl;
					#endif
					this->CrossBoundaryAndLocate(kFALSE, this->GetCurrentNode()); 	
						
				} else {
					#ifdef VERBOSE_MODE
						cout << "Search Downwards - Step exited a volume so we must re-enter" << endl;
						cout << this->GetCurrentNode()->GetVolume()->GetNextNodeIndex() << endl;
					#endif
					this->CrossBoundaryAndLocate(kTRUE, this->GetCurrentNode());
				}
				this->UpdateTrack(track);
				#ifdef VERBOSE_MODE	
					cout << "-------------------------------------------------" << endl;
					cout << "Final Node: " << this->GetCurrentNode()->GetName() << endl;
					cout << "-------------------------------------------------" << endl << endl;
				#endif
			}
		}
		
		// -- Determine destination
		// -- Has lost flag been set?
		if (particle->Lost() == kTRUE) {
			break; // -- End Propagtion Loop
		// -- Has detected flag been set?
		} else if (particle->Detected() == kTRUE) {
			break; // -- End Propagation Loop
		// -- Has particle decayed within steptime?
		} else if (particle->WillDecay(this->GetStepTime()) == kTRUE) {
			particle->Decayed(kTRUE);// Set Decay Flag
			break; // -- End Propagation Loop
		// -- Have we reached the maximum runtime?
		} else if (particle->T() >= runTime) {
			break; // -- End Propagation Loop
		}
	// -- END OF PROPAGATION LOOP
	}	
							
	cout << "FINAL STATUS: " << "Track: " << track->GetId() << "\t" << "Steps taken: " << stepNumber << "\t";
	cout << "Time: " << particle->T() << "s" << "\t" << "Final Medium: " << this->GetCurrentNode()->GetMedium()->GetName() << "\t";
	cout << "Bounces: " << particle->Bounces() << "\t" << "Diffuse: " << particle->DiffuseBounces() << "\t" << "Specular: " << particle->SpecularBounces() << endl;
	
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNRunManager::PropagateTrack(TVirtualGeoTrack* track, const Int_t steps, const Double_t maxStepTime)
{
// Propagate track through geometry until it is either stopped or the max steps has been reached
// Track passed MUST REFERENCE A TUCNPARTICLE as its particle type. 
// In future I will attempt to guarentee this. (4/06/09) 

	Int_t stepNumber;
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());

#ifdef VERBOSE_MODE				
	Info("PropagateTrack","Starting Run - Max steps: %i", steps);
#endif
	
	// -- Check that Particle has not been initialised inside a boundary or detector		
	if (static_cast<TUCNGeoMaterial*>(this->GetCurrentNode()->GetMedium()->GetMaterial())->IsTrackingMaterial() == kFALSE) {
		cout << this->GetCurrentNode()->GetMedium()->GetName() << endl;
		throw runtime_error("In TUCNRunManager::PropagateTrack - particle initialised inside a boundary");
	}
	
	// -- Get the Grav Field
	TUCNGravField* field = fUCNGeometry->GetGravField();
	
	if (steps <= 0) {
		Error("PropagateTrack", "No. of Steps must be greater than or equal to 1");
		return kFALSE;
	}
	
	///////////////////////////////////	
	// -- Propagation Loop
	///////////////////////////////////
	for (stepNumber = 1 ; stepNumber <= steps ; stepNumber++) {
		
		#ifdef VERBOSE_MODE		
			cout << "STEP " << stepNumber << "\t" << particle->T() << " s" << "\t" << this->GetCurrentNode()->GetName() << endl;	
		#endif
		
		// -- Define the next steptime
		this->SetStepTime(maxStepTime);
		
		// -- Find time to reach next boundary and step along parabola
		#ifdef VERBOSE_MODE	
			cout << "------------------- START OF STEP ----------------------" << endl;
			cout << "Initial Steptime (s): " << this->GetStepTime() << endl;
			cout << "Initial Vertex (m): " << "X:" << particle->Vx() << "\t" << "Y:" << particle->Vy()  << "\t" << "Z:" << particle->Vz() << "\t" << "T:" << particle->T() << endl;
			cout << "Initial Dir: " << "X:" << particle->DirX()  << "\t" << "Y:" << particle->DirY()  << "\t" << "Z:" << particle->DirZ() << "\t" << "Mag:" << particle->Dir() << endl;
			cout << "Initial Vel (m/s): " << "X:" << particle->VelocityX()  << "\t" << "Y:" << particle->VelocityY()  << "\t" << "Z:" << particle->VelocityZ() << "\t" << "V:" << particle->Velocity() << endl;
			cout << "Initial Mom (eV): " << "X:" << particle->Px()  << "\t" << "Y:" << particle->Py()  << "\t" << "Z:" << particle->Pz() << "\t" << "P:" << particle->P() << endl;
			cout << "Initial Volume: "    << this->GetCurrentVolume()->GetName() << endl;
			cout << "Initial Energy (neV): "    << particle->Energy() /Units::neV << endl;
			cout << "-----------------------------" << endl;
			cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
			cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
			cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
			cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
			cout << "-----------------------------" << endl << endl;
		#endif
		
		if (field == NULL) {
			this->FindNextBoundaryAndStep(this->GetStepTime());
			this->UpdateTrack(track, this->GetStepTime());
			// TODO: UPDATE CLASS DATA MEMBERS NOW LIKE fUCNNextNode
			
		} else {
			// -- Update Particle is called by FindNext...AlongParabola so no need to repeat that here
			this->FindNextBoundaryAndStepAlongParabola(track, field, this->GetStepTime());
		}
			
		#ifdef VERBOSE_MODE	
			cout << "------------------- END OF STEP ----------------------" << endl;
			cout << "Final Steptime (s): " << this->GetStepTime() << endl;
			cout << "Final Vertex (m): " << "X:" << particle->Vx() << "\t" << "Y:" << particle->Vy()  << "\t" << "Z:" << particle->Vz() << "\t" << "T:" << particle->T() << endl;
			cout << "Final Dir: " << "X:" << particle->DirX()  << "\t" << "Y:" << particle->DirY()  << "\t" << "Z:" << particle->DirZ() << "\t" << "Mag:" << particle->Dir() << endl;
			cout << "Final Vel (m/s): " << "X:" << particle->VelocityX()  << "\t" << "Y:" << particle->VelocityY()  << "\t" << "Z:" << particle->VelocityZ() << "\t" << "V:" << particle->Velocity() << endl;
			cout << "Final Mom (eV): " << "X:" << particle->Px()  << "\t" << "Y:" << particle->Py()  << "\t" << "Z:" << particle->Pz() << "\t" << "P:" << particle->P() << endl;
			cout << "Final Volume: "    << this->GetCurrentVolume()->GetName() << endl;
			cout << "Final Energy (neV): "    << particle->Energy() /Units::neV << endl;
			cout << "-----------------------------" << endl;
			cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
			cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
			cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
			cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
			cout << "-----------------------------" << endl << endl;
		#endif	
		
		TUCNGeoMaterial* currentMaterial = static_cast<TUCNGeoMaterial*>(this->GetCurrentNode()->GetMedium()->GetMaterial());
		
		// -- Determine what to do if we are on a boundary
		// -- Is Track on the surface of a boundary?
		if (currentMaterial->IsTrackingMaterial() == kFALSE) {
			// -- Are we on the surface of a detector?
			if (currentMaterial->IsDetectorMaterial() == kTRUE) {
				// -- Was particle detected?
				Double_t prob = gRandom->Uniform(0.0,1.0);
				if (prob < currentMaterial->DetectionEfficiency()) {	
					particle->Detected(kTRUE);  // Set detected flag
				} else {	
				#ifdef VERBOSE_MODE	
					cout << "------------------- BOUNCE ----------------------" << endl;
					cout << "Current medium: " << this->GetCurrentNode()->GetMedium()->GetName() << endl;
				#endif
					// -- Make a Bounce
					this->Bounce(track);	
					// -- Cross Boundary and Locate
					this->CrossBoundaryAndLocate(kFALSE, this->GetCurrentNode()); 	
					this->UpdateTrack(track);
				#ifdef VERBOSE_MODE	
					cout << "-------------------------------------------------" << endl;
					cout << "Final medium: " << this->GetCurrentNode()->GetMedium()->GetName() << endl;
					cout << "-------------------------------------------------" << endl << endl;
				#endif
				}
			// -- Was particle lost to boundary (absorbed/upscattered) ?
			} else if (kFALSE) {	
				particle->Lost(kTRUE); // Set lost flag
			// -- Are we outside the geometry heirarchy we have built - ie: in TOP
			} else if (currentMaterial->IsBlackhole() == kTRUE) {
				particle->Lost(kTRUE);
			//	throw runtime_error("In TUCNRunManager::PropagateTrack - track has entered TOP/Blackhole!");
			} else {
				TGeoNode* currentNode = this->GetCurrentNode();
				#ifdef VERBOSE_MODE	
					cout << "------------------- BOUNCE ----------------------" << endl;
					cout << "Current Node: " << currentNode->GetName() << endl;
				#endif	
				// -- Make a Bounce
				this->Bounce(track);
				
				// -- Locate particle
				this->FindNode();
			
			/*	// -- Cross Boundary and Locate
				if (this->IsUCNStepEntering() == kTRUE) {
					#ifdef VERBOSE_MODE	
						cout << "Search Upwards - Step entering a volume so we must step back out" << endl;
						cout << this->GetCurrentNode()->GetVolume()->GetNextNodeIndex() << endl;
					#endif
					this->CrossBoundaryAndLocate(kFALSE, this->GetCurrentNode()); 	
						
				} else {
					#ifdef VERBOSE_MODE
						cout << "Search Downwards - Step exited a volume so we must re-enter" << endl;
						cout << this->GetCurrentNode()->GetVolume()->GetNextNodeIndex() << endl;
					#endif
					this->CrossBoundaryAndLocate(kTRUE, this->GetCurrentNode());
				}
			*/	
			//	this->UpdateTrack(track);
				
				TGeoNode* finalNode = this->GetCurrentNode();
				#ifdef VERBOSE_MODE	
					cout << "-------------------------------------------------" << endl;
					cout << "Final Node: " << finalNode->GetName() << endl;
					cout << "-------------------------------------------------" << endl << endl;
				#endif
				
				if (finalNode == currentNode) {
					cout << "Initial Node: " << currentNode->GetName() << "\t" << "Final Node: " << finalNode->GetName() << endl;
					break;
				}
				
				
			}
		}
		
		if (static_cast<TUCNGeoMaterial*>(this->GetCurrentNode()->GetMedium()->GetMaterial())->IsTrackingMaterial() == kFALSE) {
			cout << "Step: " << stepNumber << "\t" << "Current Node: " << this->GetCurrentNode()->GetName() << endl;
			break;
		}
		
		// -- Determine destination
		// -- Has lost flag been set?
		if (particle->Lost() == kTRUE) {
			break; // -- End Propagtion Loop
		// -- Has detected flag been set?
		} else if (particle->Detected() == kTRUE) {
			break; // -- End Propagation Loop
		// -- Has particle decayed within steptime?
		} else if (particle->WillDecay(this->GetStepTime()) == kTRUE) {
			particle->Decayed(kTRUE);// Set Decay Flag
			break; // -- End Propagation Loop
		}
	// -- END OF PROPAGATION LOOP
	}	
							
	cout << "FINAL STATUS: " << "Track: " << track->GetId() << "\t" << "Steps taken: " << stepNumber << "\t";
	cout << "Time: " << particle->T() << "s" << "\t" << "Final Medium: " << this->GetCurrentNode()->GetMedium()->GetName() << "\t";
	cout << "Bounces: " << particle->Bounces() << "\t" << "Diffuse: " << particle->DiffuseBounces() << "\t" << "Specular: " << particle->SpecularBounces() << endl;
	
	return kTRUE;
}

//_____________________________________________________________________________
void TUCNRunManager::UpdateTrack(TVirtualGeoTrack* track, Double_t timeInterval)
{
// Take the particle and update its position, momentum, time and energy with the current properties stored in the Navigator
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	
	#ifdef VERBOSE_MODE
		Info("UpdateTrack", "Initial X: %f, Y: %f, Z: %f, T: %f", particle->Vx(), particle->Vy(), particle->Vz(), particle->T());
		Info("UpdateTrack", "Initial Px: %f, Py: %f, Pz: %f, E: %f", particle->Px(), particle->Py(), particle->Pz(), particle->Energy()/Units::neV);
	#endif
	
	const Double_t* pos = this->GetCurrentPoint();
	const Double_t* dir = this->GetCurrentDirection(); 
	const TUCNGravField* field = fUCNGeometry->GetGravField();
	
	Double_t heightClimbed = 0.0;
	Double_t gravPotentialEnergy = 0.0;
	
	if (field != NULL) {
		// Determine the height of our particle in the global coordinate system of TOP.
		// Take the dot product of the point vector with the field direction unit vector to get the height of this point in the gravitational field.
		// This assumes a convention that 'height' is measured along an axis that INCREASES in the opposite direction to the field direction vector
		// (which is usually 'downwards')
		heightClimbed = -1.0*((pos[0] - particle->Vx())*field->Nx() + (pos[1] - particle->Vy())*field->Ny() + (pos[2] - particle->Vz())*field->Nz());
		gravPotentialEnergy = particle->Mass_GeV_c2()*field->GravAcceleration()*heightClimbed;
	}
	
	// Determine current Kinetic energy of particle given the height climbed in graviational field
	Double_t kineticEnergy = particle->Energy() - gravPotentialEnergy;
	
	// Detemine current momentum
	Double_t momentum = TMath::Sqrt(2.0*particle->Mass_GeV()*kineticEnergy);
	Double_t mom[3] = {momentum*dir[0], momentum*dir[1], momentum*dir[2]};
	
	// Update particle
	particle->SetProductionVertex(pos[0], pos[1], pos[2], particle->T() + timeInterval);
	particle->SetMomentum(mom[0], mom[1], mom[2], kineticEnergy);

	// Update track
	track->AddPoint(particle->Vx(), particle->Vy(), particle->Vz(), particle->T());
	
	#ifdef VERBOSE_MODE
		Info("UpdateTrack", "Height climbed: %f, Kinetic Energy Gained(Lost): %f", heightClimbed, -gravPotentialEnergy/Units::neV);
		Info("UpdateTrack", "Final X: %f, Y: %f, Z: %f, T: %f", particle->Vx(), particle->Vy(), particle->Vz(), particle->T());
		Info("UpdateTrack", "Final Px: %f, Py: %f, Pz: %f, E: %f", particle->Px(), particle->Py(), particle->Pz(), particle->Energy()/Units::neV);
	#endif
}


//_____________________________________________________________________________
Bool_t TUCNRunManager::Bounce(TVirtualGeoTrack* track)
{
	// -- Get particle
	TUCNParticle* ucnparticle = static_cast<TUCNParticle*>(track->GetParticle());
	
	// -- Direction Vector
	Double_t dir[3] = {(this->GetCurrentDirection())[0], (this->GetCurrentDirection())[1], (this->GetCurrentDirection())[2]};
	
	// -- Normal Vector
	const Double_t *normal = this->FindUCNNormal();
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
	Double_t diffuseProbability = ucnparticle->DiffuseProbability(); // Need to add some kind of law to determine probability
	
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
	this->SetCurrentDirection(dir);
	
	// -- Make a micro-step off boundary
	const Double_t timestep = 100000*TGeoShape::Tolerance();
	Double_t currentPoint[3] = {this->GetCurrentPoint()[0], this->GetCurrentPoint()[1], this->GetCurrentPoint()[2]};
	Double_t currentField[3] = {fUCNGeometry->GetGravField()->Gx(), fUCNGeometry->GetGravField()->Gy(), fUCNGeometry->GetGravField()->Gz()};

//	cout << TUCNParabola::Instance()->ArcLength(currentVelocity, currentField, timestep) << endl;

	currentPoint[0] += norm[0]*timestep + 0.5*currentField[0]*timestep*timestep; 
   currentPoint[1] += norm[1]*timestep + 0.5*currentField[1]*timestep*timestep; 
   currentPoint[2] += norm[2]*timestep + 0.5*currentField[2]*timestep*timestep;	
	this->SetCurrentPoint(currentPoint);

	// -- Update Track
	this->UpdateTrack(track);	

	// -- Update Bounce Counter
	ucnparticle->MadeBounce();
	
	return kTRUE;
}	

//_____________________________________________________________________________
Bool_t TUCNRunManager::SpecularBounce(Double_t* dir, const Double_t* norm)
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
Bool_t TUCNRunManager::DiffuseBounce(Double_t* dir, const Double_t* norm)
{
	#ifdef VERBOSE_MODE	
		cout << "----------------------------" << endl;
		cout << "Diffuse Bounce" << endl;
		cout << "BEFORE - nx: " << dir[0] << "\t" << "nx: " << dir[1] << "\t" << "nx: " << dir[2] << endl;
		cout << "normx: " << norm[0] << "\t" << "normx: " << norm[1] << "\t" << "normx: " << norm[2] << endl;
	#endif	
	
	// First we need to pick random angles to choose the orientation of our diffuse direction vector. 
	// Correct method for UCN physics though is to weight these angles towards the poles by adding an extra cos(theta) - derivation of how to pick these angles is in notes
	Double_t phi = gRandom->Uniform(0.0, 1.0)*2*TMath::Pi();
	Double_t u = gRandom->Uniform(0.0, 0.5);		
	Double_t theta = TMath::ACos(TMath::Sqrt(1.0 - 2*u)); // We ignore the negative sqrt term, since we are only interested in theta between 0 and pi/2 
	
	// Calculate local normal vector	
	Double_t lnorm[3] = {0.,0.,0.};
	this->MasterToLocalVect(norm, lnorm);
	TVector3 localNorm(lnorm[0], lnorm[1], lnorm[2]);
	assert(TMath::Abs(localNorm.Mag() - 1.0) < fgTolerance);
		
	// Define a preferred direction in our coordinate system - usually the z-direction - that we want to be perpendicular to the normal vector of our surface
	TVector3 upAxis(0.,0.,1.);
		
	// Here we check to make sure that the upAxis we chose is not parallel to the normal vector. If it is, we try another one, x. 
	if (TMath::Abs(upAxis.Dot(localNorm)) > fgTolerance) {
		upAxis.SetXYZ(1.,0.,0.);
		if (TMath::Abs(upAxis.Dot(localNorm)) > fgTolerance) {
			throw runtime_error("In TUCNRunManager::DiffuseBounce - Could not find an axis perpendicular to normal. Normal is parallel to z and x axes!!!");
		}
	}
	
	// Calculate the cross product of the 'up' vector with our normal vector to get a vector guaranteed to be perpendicular to the normal. 
	// This is the vector we will want to rotate around initially. 
	TVector3 perpAxis = localNorm.Cross(upAxis); 
	assert(TMath::Abs(perpAxis.Mag() - 1.0) < fgTolerance);
	
	
	// Rotate normal vector about perpendicular axis by angle theta. Using Rodrigues' formula derived in notes. 
	TVector3 rotatedNorm(0.,0.,0.);
	{
		TVector3 term2 = localNorm - (localNorm.Dot(perpAxis))*perpAxis; // name term2 and term3 just refer to the terms as they appear in the Rodrigues' formula
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
	this->LocalToMasterVect(ldir, dir);
	
	assert(TMath::Abs(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2] - 1.0) < fgTolerance);
	// Assert that our final direction is not perpendicular to the normal. This could result in escaping the boundary
	assert(TMath::Abs(dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2]) > fgTolerance); 
	
	#ifdef VERBOSE_MODE	
		cout << "AFTER - nx: " << dir[0] << "\t" << "nx: " << dir[1] << "\t" << "nx: " << dir[2] << endl;
	#endif
	
	return kTRUE;
}
