// UCN class
// Author: Matthew Raso-Barnett  19/02/2010
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Constants.h"
#include "Units.h"

#include "TUCNState.h"

#include "TMath.h"
#include "TRandom.h"

#include "TUCNRun.h"
#include "TUCNVolume.h"
#include "TUCNFieldManager.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"
#include "TUCNGeoBBox.h"
#include "TUCNParabola.h"
#include "TUCNMagField.h"

#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoMatrix.h"
#include "TGeoVoxelFinder.h"

using namespace std;

//#define VERBOSE_MODE

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNState                                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNState)

//_____________________________________________________________________________
TUCNState::TUCNState()
          :TObject()
{
   // Constructor
//   Info("TUCNState","Default Constructor");
}

//_____________________________________________________________________________
TUCNState::TUCNState(const TUCNState& s)
          :TObject(s)
{
   // Copy Constructor
//   Info("TUCNState","Copy Constructor");
}

//_____________________________________________________________________________
TUCNState& TUCNState::operator=(const TUCNState& s)
{
   // Assignment
//   Info("TUCNState","Assignment");
   if(this!=&s) {
      TObject::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
TUCNState::~TUCNState()
{
   // Destructor
//   Info("TUCNState","Destructor");
}

//_____________________________________________________________________________
void TUCNState::ChangeState(TUCNParticle* particle, TUCNState* newState)
{
   // -- Change the particle's state pointer to the new state
   particle->ChangeState(newState);
   // Delete ourselves (previous state) before we finish
   delete this;
}

//_____________________________________________________________________________
Bool_t TUCNState::Propagate(TUCNParticle* /*particle*/, TUCNRun* /*run*/, TGeoNavigator* /*navigator*/, TUCNFieldManager* /*fieldManager*/)
{
   // Default behaviour - don't propagate
   return kFALSE;
}

//_____________________________________________________________________________
Bool_t TUCNState::LocateInGeometry(TUCNParticle* /*particle*/, TGeoNavigator* /*navigator*/, const TGeoNode* /*initialNode*/, const TGeoMatrix* /*initialMatrix*/, const TGeoNode* /*crossedNode*/)
{
   return kTRUE;
}

//_____________________________________________________________________________
void TUCNState::UpdateParticle(TUCNParticle* particle, const TGeoNavigator* navigator, const Double_t timeInterval, const TUCNGravField* gravField)
{
   // -- Take the particle and update its position, momentum, time and energy
   // -- with the current properties stored in the Navigator
   #ifdef VERBOSE_MODE
      cout << "-------------------------------------------" << endl;
      cout << "Time step: " << timeInterval << endl;
		cout << "Update -- Initial X: " << particle->X() << "\t" << "Y: " << particle->Y();
      cout << "\t" <<  "Z: " << particle->Z() << "\t" <<  "T: " << particle->T() << endl;
      cout << "Update -- Initial PX: " << particle->Px() << "\t" << "PY: " << particle->Py();
      cout << "\t" <<  "PZ: " << particle->Pz() << "\t";
      cout <<  "E: " << particle->Energy()/Units::neV << endl;
   #endif
   
   const Double_t* pos = navigator->GetCurrentPoint();
   const Double_t* dir = navigator->GetCurrentDirection(); 
   Double_t heightClimbed = 0.0;
   Double_t gravPotentialEnergy = 0.0;
   
   if (gravField) {
      // Determine the height of our particle in the global coordinate system of TOP.
      // Take the dot product of the point vector with the field direction unit vector
      // to get the height of this point in the gravitational field.
      // This assumes a convention that 'height' is measured along an axis that INCREASES
      // in the opposite direction to the field direction vector (which is usually 'downwards')
      heightClimbed = -1.0*((pos[0] - particle->X())*gravField->Nx() + (pos[1] - particle->Y())*gravField->Ny() + (pos[2] - particle->Z())*gravField->Nz());
      gravPotentialEnergy = Neutron::mass_eV_c2*gravField->GravAcceleration()*heightClimbed;
   }
   
   // Determine current Kinetic energy of particle given the height climbed in graviational field
   Double_t kineticEnergy = particle->Energy() - gravPotentialEnergy;
   
   // Detemine current momentum
   Double_t momentum = TMath::Sqrt(2.0*Neutron::mass_eV*kineticEnergy);
   Double_t mom[3] = {momentum*dir[0], momentum*dir[1], momentum*dir[2]};
   
   // Update particle
   particle->SetVertex(pos[0], pos[1], pos[2], particle->T() + timeInterval);
   particle->SetMomentum(mom[0], mom[1], mom[2], kineticEnergy);
   particle->IncreaseDistance(navigator->GetStep()); // Increase the distance travelled
   
   #ifdef VERBOSE_MODE
      cout << "Update -- Height climbed: " << heightClimbed << "\t";
      cout << "Kinetic Energy Gained(Lost): " << -gravPotentialEnergy/Units::neV << endl;
      cout << "Update -- Final X: " << particle->X() << "\t" << "Y: " << particle->Y();
      cout << "\t" <<  "Z: " << particle->Z() << "\t" <<  "T: " << particle->T() << endl;
      cout << "Update -- Final PX: " << particle->Px() << "\t" << "PY: " << particle->Py();
      cout << "\t" <<  "PZ: " << particle->Pz() << "\t";
      cout <<  "E: " << particle->Energy()/Units::neV << endl;
      cout << "-------------------------------------------" << endl;
   #endif
}

//_____________________________________________________________________________
void TUCNState::Detected(TUCNParticle* /*particle*/)
{
   // Default do nothing
}

//_____________________________________________________________________________
void TUCNState::Lost(TUCNParticle* /*particle*/)
{
   // Default do nothing
}

//_____________________________________________________________________________
void TUCNState::Absorbed(TUCNParticle* /*particle*/)
{
   // Default do nothing
}

//_____________________________________________________________________________
void TUCNState::Bad(TUCNParticle* /*particle*/)
{
   // Default do nothing
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNPropagating                                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNPropagating)

//_____________________________________________________________________________
TUCNPropagating::TUCNPropagating()
                :TUCNState(),
                 fIsStepEntering(kFALSE),
                 fIsStepExiting(kFALSE),
                 fIsOnBoundary(kFALSE)
{
   // Constructor
//   Info("TUCNPropagating","Constructor");
}

//_____________________________________________________________________________
TUCNPropagating::TUCNPropagating(const TUCNPropagating& s)
                :TUCNState(s),
                 fIsStepEntering(s.fIsStepEntering),
                 fIsStepExiting(s.fIsStepExiting),
                 fIsOnBoundary(s.fIsOnBoundary)
{
   // Copy Constructor
//   Info("TUCNPropagating","Copy Constructor");
}

//_____________________________________________________________________________
TUCNPropagating& TUCNPropagating::operator=(const TUCNPropagating& s)
{
   // Assignment.
//   Info("TUCNPropagating","Assignment");
   if(this!=&s) {
      TUCNState::operator=(s);
      fIsStepEntering = s.fIsStepEntering;
      fIsStepExiting = s.fIsStepExiting;
      fIsOnBoundary = s.fIsOnBoundary;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNPropagating::~TUCNPropagating()
{
   // Destructor
//   Info("TUCNPropagating","Destructor");
}

//______________________________________________________________________________
Bool_t TUCNPropagating::SaveState(TUCNRun* run, TUCNParticle* particle)
{
   // Register in Run what final state we are
//   run->GetData()->AddPropagatingParticleState(particle);
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNPropagating::Propagate(TUCNParticle* particle, TUCNRun* run, TGeoNavigator* navigator, TUCNFieldManager* fieldManager)
{
   // Propagate track through geometry until it is either stopped or the runTime has been reached
   // Track passed MUST REFERENCE A TUCNPARTICLE as its particle type. 
   // UNITS:: runTime, stepTime in Seconds

   ///////////////////////////////////////////////////////////////////////////////////////	
   // -- 1. Initialise Track
   // Initialise track - Sets navigator's current point/direction/node to that of the particle
   navigator->InitTrack(particle->X(), particle->Y(), particle->Z(), particle->Nx(), particle->Ny(), particle->Nz());
   
   #ifdef VERBOSE_MODE
      cout << "Propagate - Starting Run - Max time (seconds): " <<  run->RunTime() << endl;
   #endif
   
   // -- Check that Particle has not been initialised inside a boundary or detector
//   TUCNMaterial* material = static_cast<TUCNMaterial*>(
//                                 navigator->GetCurrentVolume()->GetMaterial());
//   if (material->IsTrackingMaterial() == kFALSE) {
//      cout << "Particle: " << particle->Id() << " initialised inside boundary of ";
//      cout << navigator->GetCurrentVolume()->GetName() << endl;
//      return kFALSE;
//   }
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- 2. Propagation Loop
   Int_t stepNumber;
   for (stepNumber = 1 ; ; stepNumber++) {
      #ifdef VERBOSE_MODE
         cout << endl << "-------------------------------------------------------" << endl;
         cout << "STEP " << stepNumber << "\t" << particle->T() << " s" << "\t";
         cout << navigator->GetCurrentNode()->GetName() << endl;	
      #endif
      // -- Calculate the Next StepTime (i.e: are there any factors that reduce the maximum
      // -- step size before we work out boundary distance)
      Double_t stepTime = this->DetermineNextStepTime(particle, run->MaxStepTime(), run->RunTime());

      // -- Make a step
      if (this->MakeStep(stepTime, particle, navigator, fieldManager) == kFALSE) {
         // -- Particle has reached a final state (decay,detected)
         break; // -- End Propagation Loop
      }
      
      // -- Check whether we reached the runtime
      if (particle->T() >= run->RunTime()) {
         break; // -- End Propagation Loop
      }
   }
   // -- END OF PROPAGATION LOOP
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNPropagating::MakeStep(Double_t stepTime, TUCNParticle* particle, TGeoNavigator* navigator, TUCNFieldManager* fieldManager)
{
   // -- Find time to reach next boundary and step along parabola
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Step 1 - Get the initial parameters
   ///////////////////////////////////////////////////////////////////////////////////////
   const TUCNGravField* const gravField = fieldManager->GetGravField();
      
   // -- Store the Initial Node and Initial Matrix
   const TGeoNode* initialNode = navigator->GetCurrentNode();
   TGeoHMatrix initMatrix = *(navigator->GetCurrentMatrix()); // Copy the initial matrix here
   TGeoMatrix* initialMatrix = &initMatrix; // Hold pointer to the stored matrix
   
   // -- Determine the current local coordinates
   Double_t* currentGlobalPoint = 0;
   Double_t initialLocalPoint[3] = {0.,0.,0.};
   
   currentGlobalPoint = const_cast<Double_t*>(navigator->GetCurrentPoint());
   initialMatrix->MasterToLocal(currentGlobalPoint,&initialLocalPoint[0]);
   
   #ifdef VERBOSE_MODE
      cout << endl << "------------------- START OF STEP ----------------------" << endl;
      particle->Print(); // Print state
      cout << "Steptime (s): " << stepTime << endl;
      cout << "-----------------------------" << endl;
      cout << "Navigator's Initial Node: " << initialNode->GetName() << endl;
      cout << "Initial Matrix: " << endl;
      initialMatrix->Print();
      initMatrix.Print();
      cout << "Is On Boundary?  " << fIsOnBoundary << endl;
      cout << "Is Step Entering?  " << fIsStepEntering << endl;
      cout << "Is Step Exiting?  " << fIsStepExiting << endl;
      cout << "-----------------------------" << endl << endl;
   #endif
     
   // -- We now should be sure we have begun in the current volume. 	
   // -- Save Path to current node - we will want to return to this in the event we make a bounce
   const char* initialPath = navigator->GetPath();
   #ifdef VERBOSE_MODE	
      cout << "Current PATH: " << initialPath << endl;
   #endif
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- STEP 2 - Find Next Boundary
   ///////////////////////////////////////////////////////////////////////////////////////      
   // Store pointer to boundary node - i.e: the node which contains the boundary we just crossed
   // This will sometimes be different from the final node, such as when we cross from a daughter
   // into the parent volume. 
   TGeoNode* crossedNode = navigator->GetCurrentNode();
   // Choice of propagation algorithm depends on whether there is a grav field or not
   if (gravField == NULL) {
      // CASE 1; No Gravitational Field - Straight line tracking
      if (navigator->FindNextBoundaryAndStep(stepTime) == NULL) {
         #ifdef VERBOSE_MODE
            Error("MakeStep", "MakeStep has failed to find the next node");
         #endif
         this->ChangeState(particle, new TUCNBad());
         throw runtime_error("Failed to find next node");
      }
      // (Re-)Calculate the time travelled (approximately) from the navigator's stepsize
      Double_t timeTravelled = (particle->V() == 0. ? 0. : navigator->GetStep()/particle->V()); 
      stepTime = timeTravelled;
   } else {
      // CASE 2; Grav Field present - tracking along parabolic trajectories
      // -- Propagate Point by StepTime along Parabola
      TGeoNode* nextNode = this->ParabolicBoundaryFinder(stepTime, particle, navigator, crossedNode, gravField);
      if (nextNode == NULL) {
         #ifdef VERBOSE_MODE
            Error("MakeStep", "MakeStep has failed to find the next node");
         #endif
         this->ChangeState(particle, new TUCNBad());
         throw runtime_error("Failed to find next node");
      }
      // Assert that the returned node is also the current node
      assert(nextNode == navigator->GetCurrentNode());
   }
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Step 3 - Sample Magnetic Field if there is one
   ///////////////////////////////////////////////////////////////////////////////////////
   // Navigator's state now corresponds to the next position of the particle. Before we move the
   // particle to this position, calculate the motion of the spin vector along this trajectory
   const TVector3 initialPosition(particle->X(), particle->Y(), particle->Z());
   const string initialVolumeName = initialNode->GetVolume()->GetName();
   const TUCNMagField* const magField = fieldManager->GetMagField(initialPosition, initialVolumeName);
   if (magField != NULL) {
      magField->Interact(*particle, stepTime);
   }
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Step 4 - Update Particle to final position
   ///////////////////////////////////////////////////////////////////////////////////////
   this->UpdateParticle(particle, navigator, stepTime, gravField);
   #ifdef VERBOSE_MODE	
      cout << endl << "------------------- AFTER STEP ----------------------" << endl;
      particle->Print(); // Print verbose
      cout << "Steptime (s): " << stepTime << endl;
      cout << "-----------------------------" << endl;
      cout << "Navigator's Current Node: " << navigator->GetCurrentNode()->GetName() << endl;
      cout << "Is On Boundary?  " << fIsOnBoundary << endl;
      cout << "Is Step Entering?  " << fIsStepEntering << endl;
      cout << "Is Step Exiting?  " << fIsStepExiting << endl;
      cout << "-----------------------------" << endl << endl;
   #endif
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- STEP 5 - Now we need to determine where we have ended up, and to examine whether
   // -- the current volume is the point's true container
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Attempt to locate particle within the current node
   Bool_t locatedParticle = this->LocateInGeometry(particle, navigator, initialNode, initialMatrix, crossedNode);
   if (locatedParticle == kFALSE) {
      #ifdef VERBOSE_MODE
         cout << "Error - After Step - Unable to locate particle correctly in Geometry" << endl;
      #endif
      this->ChangeState(particle, new TUCNBad());
      throw runtime_error("Unable to locate particle uniquely in correct volume");
   }
   // -- We should now have propagated our point by some stepsize and be inside the correct volume 
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Step 6 - Determine whether we collided with a wall, decayed, or any other
   // -- state change occured
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Check whether particle has decayed in the last step
   if(this->WillDecay(stepTime) == kTRUE) {
      this->ChangeState(particle, new TUCNDecayed());
      return kFALSE;
   }
   
   // -- Get the normal vector to the boundary
   Double_t normal[3] = {0.,0.,0.};
   if (gravField) {
      this->FindBoundaryNormal(normal, navigator, crossedNode);
   } else {
      throw runtime_error("Need to sort out behaviour of normals in particle");
      //normal = navigator->FindNormal();
   }
   
   // -- Interact with Boundary
   TUCNVolume* currentVolume = static_cast<TUCNVolume*>(navigator->GetCurrentVolume());
   if (currentVolume->Interact(particle, normal, navigator, crossedNode, initialPath) == kFALSE) {
      // Particle reached a final state
      return kFALSE;
   }
   
   // End of MakeStep.
   return kTRUE;
}

//_____________________________________________________________________________
Double_t TUCNPropagating::DetermineNextStepTime(TUCNParticle* particle, const Double_t maxStepTime, const Double_t runTime)
{
   // Placeholder for method to calculate the next step time depending on 
   // electric/magnetic field environment
   // Start with the maximum stepTime 
   
   // Check if we will reach the maximum runtime of the track. If so propagate only until this time.
   if (runTime == 0.0) {
      return 0.0;
   } else if (particle->T() > (runTime - maxStepTime)) {
      return ((runTime - particle->T()) + TGeoShape::Tolerance());
   } else {
      return maxStepTime;
   }
}

//_____________________________________________________________________________
TGeoNode* TUCNPropagating::ParabolicBoundaryFinder(Double_t& stepTime, TUCNParticle* particle, TGeoNavigator* navigator, TGeoNode* crossedNode, const TUCNGravField* const field)
{
// Compute distance to next boundary within STEPMAX. If no boundary is found,
// propagate current point along current direction with fStep=STEPMAX. Otherwise
// propagate with fStep=SNEXT (distance to boundary) and locate/return the next 
// node.
   
   // -- Get the global coordinates
   Double_t globalField[3] 	 = {field->Gx(), field->Gy(), field->Gz()}; 
   Double_t globalPoint[3] 	 = {particle->X(), particle->Y(), particle->Z()};
   Double_t globalDir[3]	 	 = {particle->Nx(), particle->Ny(), particle->Nz()};
   Double_t globalVelocity[3]  = {particle->Vx(), particle->Vy(), particle->Vz()};
   
   Double_t currentField[3]	 = {globalField[0], globalField[1], globalField[2]};
   Double_t currentPoint[3]    = {globalPoint[0], globalPoint[1], globalPoint[2]};
   Double_t currentDir[3]      = {globalDir[0], globalDir[1], globalDir[2]};
   Double_t currentVelocity[3] = {globalVelocity[0], globalVelocity[1], globalVelocity[2]};
   
   // Compute maximum stepsize
   const Double_t stepMax = TUCNParabola::Instance()->ArcLength(globalVelocity, globalField, stepTime);
   
   // -- Some initialisations
   static Int_t icount = 0;
   icount++;
   Int_t nextindex;
   Bool_t is_assembly;
   fIsStepExiting  = kFALSE;
   fIsStepEntering = kFALSE;
   TGeoNode *skip;
   
   // -- Store time step and step distance -- these two should always be convertible
   navigator->SetStep(stepMax);
   Double_t snext = TGeoShape::Big();
   Double_t tnext = TGeoShape::Big();
   
   #ifdef VERBOSE_MODE
      cout << "PBF - Starting StepTime: " << stepTime << "\t";
      cout << "Starting StepSize: " << navigator->GetStep() << endl;
      cout << "PBF - Velocity: " << particle->V() << endl;
      cout << "PBF - Global Field: X: " << globalField[0] << "\t" << "Y: " << globalField[1];
      cout << "\t" << "Z: " << globalField[2] << endl;
      cout << "PBF - Global Point: X: " << globalPoint[0] << "\t" << "Y: " << globalPoint[1];
      cout << "\t" << "Z: " << globalPoint[2] << endl;
      cout << "PBF - Global Dir: X: "   << globalDir[0] << "\t" << "Y: " << globalDir[1];
      cout << "\t" << "Z: " << globalDir[2] << endl;	
      cout << "PBF - Global Velocity: X: " << globalVelocity[0] << "\t" << "Y: ";
      cout << globalVelocity[1] << "\t" << "Z: " << globalVelocity[2] << endl;	
   #endif
   
   navigator->GetHMatrix()->CopyFrom(navigator->GetCurrentMatrix());
   
   // *********************************************************************
   // -- BRANCH 2
   // -- Get point and direction in local coordinate frame and calculate
   // -- distance to boundary of current node.
   // *********************************************************************
   Double_t localPoint[3], localVelocity[3], localField[3]; // Containers for the local coords
   Int_t icrossed = -2;
   navigator->GetCurrentMatrix()->MasterToLocal(currentPoint, &localPoint[0]);
   navigator->GetCurrentMatrix()->MasterToLocalVect(currentVelocity, &localVelocity[0]);
   navigator->GetCurrentMatrix()->MasterToLocalVect(currentField, &localField[0]);
   TGeoVolume *vol = navigator->GetCurrentNode()->GetVolume();
   
   // -- Find distance to exiting current node
   tnext = static_cast<TUCNGeoBBox*>(vol->GetShape())->TimeFromInsideAlongParabola(localPoint, localVelocity, localField, stepTime, fIsOnBoundary); 
   if (tnext <= 0.0) {
      #ifdef VERBOSE_MODE
         Error("ParabolicBoundaryFinder", "Failed to find boundary");
      #endif
      return NULL;
   }
   snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
   crossedNode = navigator->GetCurrentNode();
   
   #ifdef VERBOSE_MODE
      cout << "PBF - Branch 2. Find distance to exit current Volume: " << vol->GetName() << endl;
      cout << "PBF - Local Field: X: " << localField[0] << "\t" << "Y: " << localField[1] << "\t";
      cout << "Z: " << localField[2] << endl;
      cout << "PBF - Time to boundary: " << tnext << "\t";
      cout << "Distance to Boundary: " << snext << endl;
      cout << "PBF - Abs(snext - fStep): " << TMath::Abs(snext - navigator->GetStep()) << endl;
   #endif
   // -- If distance to exiting current node is <= Tolerance value (1e-10)
   // -- make a small step by navigator tolerance value
   if (snext <= TGeoShape::Tolerance()) {
      snext = TGeoShape::Tolerance();
      tnext = TGeoShape::Tolerance();
      navigator->SetStep(snext);
      stepTime = tnext;
      fIsOnBoundary = kTRUE;
      fIsStepEntering = kFALSE;
      fIsStepExiting = kTRUE;
      skip = navigator->GetCurrentNode();
      
      // -- Move global point by Tolerance value
      currentPoint[0] += navigator->GetStep()*currentDir[0]; 
      currentPoint[1] += navigator->GetStep()*currentDir[1]; 
      currentPoint[2] += navigator->GetStep()*currentDir[2];	
      navigator->SetCurrentPoint(currentPoint);
      is_assembly = navigator->GetCurrentNode()->GetVolume()->IsAssembly();
      if (!(navigator->GetLevel()) && !is_assembly) {
         // fUCNIsOutside = kTRUE ;
         return 0;
      }
      if (navigator->GetCurrentNode()->IsOffset()) {
         Warning("ParabolicBoundaryFinder","Deprecated CrossDivisionCell().");
         throw runtime_error("In TUCNParticle PBF Need to call CrossDivisionCell but can't!");
      }
      // -- Cross Boundary and return new volume
      if (navigator->GetLevel()) {
         navigator->CdUp(); 
      } else {
         skip = 0;
      }
      return navigator->CrossBoundaryAndLocate(kFALSE, skip);
   }
   
   // -- If distance to exiting current node is smaller than proposed Stepsize
   // -- then set our stepsize fStep to navigator distance (snext)
   if (snext < navigator->GetStep() - TGeoShape::Tolerance()) {
      icrossed = -1;
      navigator->SetStep(snext);
      stepTime = tnext;
      fIsStepEntering = kFALSE;
      fIsStepExiting = kTRUE;
   }
   
   // Find next daughter boundary for the current volume
   Int_t idaughter = -1;
   TGeoNode *crossed = this->ParabolicDaughterBoundaryFinder(stepTime, navigator, localPoint, localVelocity, localField, idaughter, kTRUE);
   if (crossed) {
      fIsStepExiting = kFALSE;
      icrossed = idaughter;
      fIsStepEntering = kTRUE;
      // If we crossed a daughter volume, set this node to be the new crossedNode,
      // since in this case it is the daughter's boundary we are crossing, rather than
      // the current volume's boundary.
      crossedNode = crossed;
   }
   // -- If we are in an overlapping node, return an error as we are no longer supporting this.
   // -- Geometries must be constructed with no overlaps beyond common boundaries.
   if (navigator->GetNmany()) {
      Error("PBF","In overlapping node - implementation of this removed");
      return 0;
   }
   // *********************************************************************
   // -- BRANCH 3
   // -- Updating the Particle's position and momentum
   // *********************************************************************
   const Double_t timestep = stepTime;
   #ifdef VERBOSE_MODE
      cout << "PBF - Branch 3. Updating Global Point. fTimeStep: " << stepTime;
      cout << "\t" << "fStep: " << navigator->GetStep() << endl;
      cout << "PBF - Initial X: " << currentPoint[0] << "\t" << "Y: " << currentPoint[1] <<  "\t";
      cout << "Z: " << currentPoint[2] << endl;
      cout << "PBF - Initial Vx: " << currentVelocity[0] << "\t" << "Vy: " << currentVelocity[1];
      cout <<  "\t" << "Vz: " << currentVelocity[2] << endl;
      cout << "PBF - Initial Gx: " << currentField[0] << "\t" << "Gy: " << currentField[1] <<  "\t";
      cout << "Gz: " << currentField[2] << endl;
      cout << "PBF - Sqrt(X^2 + Y^2): " << TMath::Sqrt(TMath::Power(currentPoint[0],2) + TMath::Power(currentPoint[1],2)) << endl;
      cout << "PBF - Sqrt(X^2 + Z^2): " << TMath::Sqrt(TMath::Power(currentPoint[0],2) + TMath::Power(currentPoint[2],2)) << endl;
   #endif
   // -- Update Position and Direction according to the timestep
   for (Int_t i = 0; i < 3; i++) {
      currentPoint[i] += currentVelocity[i]*timestep + 0.5*currentField[i]*timestep*timestep;
      currentVelocity[i] += currentField[i]*timestep;
   }
   // Calculate the magnitude of velocity
   Double_t velocityMag = TMath::Sqrt((TMath::Power(currentVelocity[0],2) + TMath::Power(currentVelocity[1],2) + TMath::Power(currentVelocity[2],2)));
   // Check that velocity is not zero
   assert(velocityMag != 0.); 
   // Update Current Point
   navigator->SetCurrentPoint(currentPoint);
   // Update Current Direction
   for (Int_t i = 0; i < 3; i++) currentDir[i] = currentVelocity[i]/velocityMag;
   navigator->SetCurrentDirection(currentDir);
   #ifdef VERBOSE_MODE
      cout << "PBF - Final X: " << currentPoint[0] << "\t" << "Y: " << currentPoint[1] <<  "\t";
      cout << "Z: " << currentPoint[2] << endl;
      cout << "PBF - Final Vx: " << currentVelocity[0] << "\t" << "Vy: " << currentVelocity[1] <<  "\t" << "Vz: " << currentVelocity[2] << endl;
      cout << "PBF - Field Gx: " << currentField[0] << "\t" << "Gy: " << currentField[1] <<  "\t";
      cout << "Gz: " << currentField[2] << endl;
      cout << "PBF - Sqrt(X^2 + Y^2): " << TMath::Sqrt(TMath::Power(currentPoint[0],2) + TMath::Power(currentPoint[1],2)) << endl;
      cout << "PBF - Sqrt(X^2 + Z^2): " << TMath::Sqrt(TMath::Power(currentPoint[0],2) + TMath::Power(currentPoint[2],2)) << endl;
   #endif
   // *********************************************************************
   // -- BRANCH 4
   // -- Final check on results of above
   // *********************************************************************
   TGeoNode *current = 0;
   if (icrossed == -2) {
      // Nothing crossed within stepMax -> propagate and return same location   
      #ifdef VERBOSE_MODE
         cout << "PBF - Branch 4. Nothing crossed within stepMax. Propagating point and returning same location." << endl;
      #endif
      fIsOnBoundary = kFALSE;
      return navigator->GetCurrentNode();
   }
   fIsOnBoundary = kTRUE;
   if (icrossed == -1) {
      #ifdef VERBOSE_MODE
         cout << "PBF - Branch 4. On Boundary. Crossing boundary and locating." << endl;
         cout << "PBF - Get Level: " << navigator->GetLevel() << endl;
         cout << "PBF - Current Node: " << navigator->GetCurrentNode()->GetName() << endl;
      #endif
      skip = navigator->GetCurrentNode();
      is_assembly = navigator->GetCurrentNode()->GetVolume()->IsAssembly();
      if (!(navigator->GetLevel()) && !is_assembly) {
         // fUCNIsOutside = kTRUE;
         return 0;
      }   
      if (navigator->GetCurrentNode()->IsOffset()) {
         throw runtime_error("In PBF Need to call CrossDivisionCell but can't!");
         //return navigator->CrossDivisionCell();
      }
      if (navigator->GetLevel()) {
         navigator->CdUp();
      } else {
         skip = 0;
      }
      TGeoNode* finalNode = navigator->CrossBoundaryAndLocate(kFALSE, skip);
      #ifdef VERBOSE_MODE
         cout << "PBF - Branch 4. Crossing boundary Upwards." << endl;
         cout << "PBF - Final Node: " << finalNode->GetName() << endl;
         cout << "---------------------------------------" << endl;
      #endif
      return finalNode;
   }
   current = navigator->GetCurrentNode();
   navigator->CdDown(icrossed);
   nextindex = navigator->GetCurrentNode()->GetVolume()->GetNextNodeIndex();
   while (nextindex>=0) {
      current = navigator->GetCurrentNode();
      navigator->CdDown(nextindex);
      nextindex = navigator->GetCurrentNode()->GetVolume()->GetNextNodeIndex();
   }
   #ifdef VERBOSE_MODE
      cout << "PBF - Branch 4. Crossing boundary Downwards." << endl;
   #endif
   return navigator->CrossBoundaryAndLocate(kTRUE, current);
}

//_____________________________________________________________________________
TGeoNode* TUCNPropagating::ParabolicDaughterBoundaryFinder(Double_t& stepTime, TGeoNavigator* navigator, Double_t* point, Double_t* velocity, Double_t* field, Int_t &idaughter, Bool_t compmatrix)
{
// Computes as fStep the distance to next daughter of the current volume. 
// The point and direction must be converted in the coordinate system of the current volume.
// The proposed step limit is fStep.
   
   // -- First Get the current local and global fields
   Double_t motherField[3] = {field[0], field[1], field[2]}; 
   Double_t motherPoint[3] = {point[0], point[1], point[2]};
   Double_t motherVelocity[3] = {velocity[0], velocity[1], velocity[2]};
   
   #ifdef VERBOSE_MODE		
      cout << "PDBF - Mother Local Field: X: " << motherField[0] << "\t" << "Y: ";
      cout << motherField[1] << "\t" << "Z: " << motherField[2] << endl;
   #endif
   
   // -- Initialising some important parameters
   Double_t tnext = TGeoShape::Big();
   Double_t snext = TGeoShape::Big();
   idaughter = -1; // nothing crossed
   TGeoNode *nodefound = 0;
   // This has been added because we do not have access to fGlobalMatrix in TGeoNavigator
   TGeoHMatrix* globalMatrix = navigator->GetCurrentMatrix(); 
   
   // Get number of daughters. If no daughters we are done.
   TGeoVolume *vol = navigator->GetCurrentNode()->GetVolume();
   Int_t nd = vol->GetNdaughters();
   
   #ifdef VERBOSE_MODE		
      cout << "PDBF - Volume: " << vol->GetName() << "\t";
      cout << "Checking number of Daughters: " << nd << endl;
   #endif
      
   if (!nd) return 0;  // No daughter 
   if (gGeoManager->IsActivityEnabled() && !vol->IsActiveDaughters()) {
      cout << "Warning PDBF - IsActivityEnabled in geomanager" << endl;
      return 0;
   }
   
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
         current->MasterToLocal(&motherPoint[0], &localPoint[0]);
         current->MasterToLocalVect(&motherVelocity[0], &localVelocity[0]);
         current->MasterToLocalVect(&motherField[0], &localField[0]);
         #ifdef VERBOSE_MODE
            cout << "PDBF - Divided Volume. Calculate distance from outside to first cell." << endl;
            cout << "PDBF - Volume: " << current->GetName() << endl;
            cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t";
            cout << "Z: " << localField[2] << endl;
         #endif
         tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, stepTime, fIsOnBoundary);  
         if (tnext <= 0.0) {
            Error("ParabolicDaughterBoundaryFinder", "Failed to find boundary");
            return NULL;
         }
         snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         if (snext < (navigator->GetStep() - TGeoShape::Tolerance())) {
            if (compmatrix) {
               navigator->GetHMatrix()->CopyFrom(globalMatrix);
               navigator->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fIsStepExiting  = kFALSE;
            fIsStepEntering = kTRUE;
            navigator->SetStep(snext);
            stepTime = tnext;
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
         #ifdef VERBOSE_MODE		
            cout << "PDBF - Divided Volume. Calculate distance from outside to last cell." << endl;
            cout << "PDBF - Volume: " << current->GetName() << endl;
            cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t";
            cout << "Z: " << localField[2] << endl;
         #endif
         tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, stepTime, fIsOnBoundary);
         if (tnext <= 0.0) {
            Error("FindNextBoundaryAlongParabola", "Failed to find boundary");
            return NULL;
         }
         snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         if (snext < (navigator->GetStep() - TGeoShape::Tolerance())) {
            if (compmatrix) {
               navigator->GetHMatrix()->CopyFrom(globalMatrix);
               navigator->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fIsStepExiting  = kFALSE;
            fIsStepEntering = kTRUE;
            navigator->SetStep(snext);
            stepTime = tnext;
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
         if (voxels && voxels->IsSafeVoxel(motherPoint, i, navigator->GetStep())) continue;
         current->MasterToLocal(motherPoint, localPoint);
         current->MasterToLocalVect(motherVelocity, localVelocity);
         current->MasterToLocalVect(motherField, localField);
         if (current->IsOverlapping() && current->GetVolume()->Contains(localPoint)) continue;
         #ifdef VERBOSE_MODE		
            cout << "PDBF - Only Few daughters. Calculate distance from outside to all." << endl;
            cout << "PDBF - Volume: " << current->GetName() << endl;
            cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t";
            cout << "Z: " << localField[2] << endl;
            #endif
         tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, stepTime, fIsOnBoundary);
         if (tnext <= 0.0) {
            Error("FindNextBoundaryAlongParabola", "Failed to find boundary");
            return NULL;
         }
         snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         if (snext < (navigator->GetStep() - TGeoShape::Tolerance())) {
            #ifdef VERBOSE_MODE		
            cout << "PDBF - Distance to daughter is less than distance to current volume." << endl;
            #endif
            indnext = current->GetVolume()->GetNextNodeIndex();
            if (compmatrix) {
               navigator->GetHMatrix()->CopyFrom(globalMatrix);
               navigator->GetHMatrix()->Multiply(current->GetMatrix());
            }    
            fIsStepExiting  = kFALSE;
            fIsStepEntering = kTRUE;
            navigator->SetStep(snext);
            stepTime = tnext;
            nodefound = current;   
            idaughter = i;   
            while (indnext>=0) {
               current = current->GetDaughter(indnext);
               if (compmatrix) navigator->GetHMatrix()->Multiply(current->GetMatrix());
               nodefound = current;
               indnext = current->GetVolume()->GetNextNodeIndex();
            }
         }
      }
      #ifdef VERBOSE_MODE		
         cout << "PDBF - Return nearest Node found if any." << endl;
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
            cout << "PDBF - Volume is voxelized. Calculate distance from outside to voxel." << endl;
            cout << "PDBF - Volume: " << current->GetName() << endl;
            cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t";
            cout << "Z: " << localField[2] << endl;
         #endif
         tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, stepTime, fIsOnBoundary);
         if (tnext <= 0.0) {
            Error("FindNextBoundaryAlongParabola", "Failed to find boundary");
            return NULL;
         }
         snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         sumchecked++;
//         printf("checked %d from %d : snext=%g\n", sumchecked, nd, snext);
         if (snext < (navigator->GetStep() - TGeoShape::Tolerance())) {
            indnext = current->GetVolume()->GetNextNodeIndex();
            if (compmatrix) {
               navigator->GetHMatrix()->CopyFrom(globalMatrix);
               navigator->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fIsStepExiting  = kFALSE;
            fIsStepEntering = kTRUE;
            navigator->SetStep(snext);
            stepTime = tnext;
            nodefound = current;
            idaughter = vlist[i];
            while (indnext>=0) {
               current = current->GetDaughter(indnext);
               if (compmatrix) navigator->GetHMatrix()->Multiply(current->GetMatrix());
               nodefound = current;
               indnext = current->GetVolume()->GetNextNodeIndex();
            }
         }
      }
   }
   return nodefound;
}

//_____________________________________________________________________________
Bool_t TUCNPropagating::LocateInGeometry(TUCNParticle* particle, TGeoNavigator* navigator, const TGeoNode* initialNode, const TGeoMatrix* initialMatrix, const TGeoNode* crossedNode)
{
   // -- We want to determine whether the current coordinates of our particle is
   // -- located in the correct volume in the geometry. If this isn't the case - 
   // -- for example, if we are sitting on the boundary, but just inside/outside where
   // -- we expect to be, perhaps because our boundary finder has returned a time to
   // -- boundary that is out by a small amount - then we make a number of small
   // -- microsteps, along the normal vector of the boundary, back to where we should be.
   // -- If these microsteps still fail to locate the particle, we return an error.
   
   // - 1. Find the current Point, Node and Matrix
   Double_t* currentGlobalPoint = const_cast<Double_t*>(navigator->GetCurrentPoint());
   TGeoNode* currentNode = navigator->GetCurrentNode();
   TGeoMatrix* currentMatrix = navigator->GetCurrentMatrix();
   #ifdef VERBOSE_MODE
      cout << "Locating Current Point in Geometry..." << endl;
      cout << "Current Node: " << currentNode->GetName() << endl;
      cout << "Initial Node: " << initialNode->GetName() << endl;
      
   #endif
   // - 1.1 Find current point, local to the current matrix
   Double_t currentLocalPoint[3] = {0.,0.,0.};
   currentMatrix->MasterToLocal(currentGlobalPoint,&currentLocalPoint[0]);
   // - 1.2 Find current point, local to the initial matrix
   Double_t tempLocalPoint[3] = {0.,0.,0.};
   initialMatrix->MasterToLocal(currentGlobalPoint,&tempLocalPoint[0]);
   
   // -- If we are in the same node as before, then we have not crossed any boundary
   if (currentNode == initialNode) {
      #ifdef VERBOSE_MODE
         cout << "Particle didn't not cross a boundary in last step." << endl;
         cout << "Current Node: " << currentNode->GetName() << endl;
         cout << "Initial Node: " << initialNode->GetName() << endl;
         currentMatrix->Print();
         initialMatrix->Print();
      #endif
      // -- Assert that we didn't cross a boundary
      assert(fIsOnBoundary == kFALSE);
      // -- If the returned node is the same as before, the matrices should match up
//      assert(currentMatrix == initialMatrix); -- This no longer works since we make a copy of the
//      the initial matrix at the start, and the current is taken from cache, which leads to some
//      problem here. The matrices are for all intents and purposes the same, but doing equality on
//      them here fails in some way.
   }
   
   // - 2. Now check that current point is 'exclusively' located within the current volume
   // - We want to know whether the current node contains the current point, and whether it is
   // - the ONLY node that contains that point!
   if (navigator->IsSameLocation(currentGlobalPoint[0], currentGlobalPoint[1], currentGlobalPoint[2], kFALSE) == kFALSE) {
      // Point is not contained exclusively in Current Node
      #ifdef VERBOSE_MODE
         cout.precision(16);
         cout << "------------------------------------------------------" << endl;
         cout << "Warning! Current Point is not exclusively contained in Current Node!" << endl;
         cout << "Current Global Point: " << endl;
         cout << "X:" << currentGlobalPoint[0] << "\t" << "Y:" << currentGlobalPoint[1] << "\t";
         cout << "Z:" << currentGlobalPoint[2] << endl;
         cout << "X:" << particle->X() << "\t" << "Y:" << particle->Y() << "\t";
         cout << "Z:" << particle->Z() << endl;
         cout << "NX:" << navigator->GetCurrentDirection()[0] << "\t" << "NY:" << navigator->GetCurrentDirection()[1] << "\t";
         cout << "NZ:" << navigator->GetCurrentDirection()[2] << endl;
         cout << "NX:" << particle->Nx() << "\t" << "NY:" << particle->Ny() << "\t";
         cout << "NZ:" << particle->Nz() << endl;
         
         
         cout << "-----------------------------" << endl;
         cout << "Current Node: " << currentNode->GetName() << endl;
         cout << "Current Matrix: " << endl;
         currentMatrix->Print();
         currentNode->GetMatrix()->Print();
         cout << "Coords local to Final Node: " << endl;
         cout << "X:" << currentLocalPoint[0] << "\t" << "Y:" << currentLocalPoint[1] << "\t";
         cout << "Z:" << currentLocalPoint[2] << endl;
         cout << "Sqrt(X^2 + Y^2): ";
         cout << TMath::Sqrt(TMath::Power(currentLocalPoint[0],2) + TMath::Power(currentLocalPoint[1],2)) << endl;
         cout << "Sqrt(X^2 + Z^2): ";
         cout << TMath::Sqrt(TMath::Power(currentLocalPoint[0],2) + TMath::Power(currentLocalPoint[2],2)) << endl;
         cout << "Does Current Node Contain Current Point? ";
         cout << currentNode->GetVolume()->GetShape()->Contains(currentLocalPoint) << endl;
         cout << "-----------------------------" << endl;
         cout << "Initial Node: " << initialNode->GetName() << endl;
         cout << "Initial Matrix: " << endl;
         initialMatrix->Print();
         initialNode->GetMatrix()->Print();
         cout << "Coords local to Initial Node: " << endl;
         cout << "X:" << tempLocalPoint[0] << "\t" << "Y:" << tempLocalPoint[1] << "\t";
         cout << "Z:" << tempLocalPoint[2] << endl;
         cout << "Sqrt(X^2 + Y^2): ";
         cout << TMath::Sqrt(TMath::Power(tempLocalPoint[0],2) + TMath::Power(tempLocalPoint[1],2)) << endl;
         cout << "Sqrt(X^2 + Z^2): ";
         cout << TMath::Sqrt(TMath::Power(tempLocalPoint[0],2) + TMath::Power(tempLocalPoint[2],2)) << endl;
         cout << "Does Initial Node Contain Current Point? ";
         cout << initialNode->GetVolume()->GetShape()->Contains(tempLocalPoint) << endl;
         cout << "-----------------------------" << endl;
         cout << "Initial Node is Parent of Current Node? ";
         cout << (currentNode->GetMotherVolume() == initialNode->GetVolume() ? 1 : 0) << endl;
         cout << "Initial Node is Daughter of Current Node? ";
         cout << (initialNode->GetMotherVolume() == currentNode->GetVolume() ? 1 : 0) << endl;
         cout << "------------------------------------------------------" << endl;
      #endif
      // We must now try to understand why we are not where we should be, and if necessary
      // make a small shift to the particle to put it in the correct volume.
      Bool_t locatedParticle = this->AttemptRelocationIntoCurrentNode(navigator, initialNode, initialMatrix, crossedNode);
      
      // Update particle after attempted relocation
      this->UpdateParticle(particle, navigator);
      
      // Check whether shift has helped to locate particle in correct volume
      if (locatedParticle == kFALSE) {
         #ifdef VERBOSE_MODE
            cout << "Error - Attempt to relocate particle inside current volume has failed. ";
            cout << "Particle must be debugged to determine what has gone wrong"<< endl;
         #endif
         return kFALSE;
      } else {
         // Otherwise we should have successfully moved our particle into the correct volume
         #ifdef VERBOSE_MODE
            cout << "Shift was succesfull. Particle is located correctly." << endl;
         #endif
         return kTRUE;
      } 
   } else {
      // Particle is correctly located in current volume without any shifting required
      #ifdef VERBOSE_MODE
         cout << "Particle is located correctly. Nothing to be done." << endl;
      #endif
      return kTRUE;
   } 
}

//_____________________________________________________________________________
Bool_t TUCNPropagating::AttemptRelocationIntoCurrentNode(TGeoNavigator* navigator, const TGeoNode* initialNode, const TGeoMatrix* initialMatrix, const TGeoNode* crossedNode)
{
   // At this point we know that the point is not exclusively contained by the current volume.
   // There could be 2 reasons for this:
   // (1.) The point is not contained by the current volume at all
   // (2.) The point is contained by the current volume, but one of its daughters 
   //      also contains the current point.
   // 

   ///////////////////////////////////////////////////////////////////////////////////////
   // CASE (1) = If this case applies then there are three posibilities:
   // 1a.) We are inside the initial node still, which is the parent of the current node
   //      (Likely right after a bounce when initial node is the wall, and current node
   //       has just been set to the inner tracking region)
   //      -- This scenario is largely benevolant and we should proceed
   // 1b.) We are inside the initial node still, which is the daughter of the current node
   //      (Likely in the case that we just made a step, but failed to cross the boundary
   //       for some reason)
   //      -- This scenario is not desirable and for now we will raise and error
   // 1c.) We are in neither the current, nor the initial node.
   //      -- Major problem here.
   ///////////////////////////////////////////////////////////////////////////////////////
   
   // - Find the current Point, Node and Matrix
   Double_t* currentGlobalPoint = const_cast<Double_t*>(navigator->GetCurrentPoint());
   TGeoNode* currentNode = navigator->GetCurrentNode();
   TGeoMatrix* currentMatrix = navigator->GetCurrentMatrix();
   // - Find current point, local to the current matrix
   Double_t currentLocalPoint[3] = {0.,0.,0.};
   currentMatrix->MasterToLocal(currentGlobalPoint,&currentLocalPoint[0]);
   // - Find current point, local to the initial matrix
   Double_t tempLocalPoint[3] = {0.,0.,0.};
   initialMatrix->MasterToLocal(currentGlobalPoint,&tempLocalPoint[0]);
   // - Check whether point is contained by current node
   if (currentNode->GetVolume()->GetShape()->Contains(currentLocalPoint) == kFALSE) {
      // Current point not inside current volume
      #ifdef VERBOSE_MODE
         cout << "Warning - Current Point is not contained in Current Node: ";
         cout << currentNode->GetName() << endl;
      #endif
      // Check for whether we are still inside the initial node
      // i.e: cases (1a.) and (1b.) -- see above comments
      if (initialNode->GetVolume()->GetShape()->Contains(tempLocalPoint) == kTRUE) {   
         // Case (1a.)
         if (currentNode->GetMotherVolume() == initialNode->GetVolume()) {
            // Initial Node is parent of current node -- likely we just made a bounce
            // We will want to try to make micro-steps into the current volume.
            assert(fIsOnBoundary == kTRUE);
         } else if (initialNode->GetMotherVolume() == currentNode->GetVolume()) {
            // Current node is parent of initial node -- likely we just made a step onto boundary
            // For now we want to catch this case. Generally we want our boundary finder to always
            // put us over the boundary, so if this case occurs I want to know about it.
            assert(fIsOnBoundary == kTRUE);
            #ifdef VERBOSE_MODE
               cout << "Error - Current point still contained by initial node" << endl;
               cout << "Initial Node: " << initialNode->GetName() << endl;
               cout << "Does Initial Node Contain Current Point? ";
               cout << initialNode->GetVolume()->GetShape()->Contains(tempLocalPoint) << endl;
               cout << "Actual Node: " << navigator->FindNode()->GetName() << endl;
            #endif
            return kFALSE;
         } else {
            // Initial and current node are unrelated -- likely that they are sitting next to
            // each other and have a common boundary. (Think of the source tube segments)
            // We will want to try to make micro-steps into the current volume.
            #ifdef VERBOSE_MODE
               cout << "Initial and Current nodes are unrelated." << endl;
            #endif
         }
      } else {
         // Not inside initial node either! Case (1c.)
         #ifdef VERBOSE_MODE
            cout << "Error - Particle not contained in Initial Node either!" << endl;
            cout << "Actual Node: " << navigator->FindNode()->GetName() << endl;
         #endif
         return kFALSE;
      }
   }
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // CASE (2) - There must be a daughter volume that contains the point as well.
   // If we are sitting right on the boundary, so that the current point is still
   // contained by the initialnode, lets try making a tiny step along the normal vector
   // of the boundary, aligned along the direction we are currently moving.
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Get the normal vector to the boundary
   const Double_t* dir = navigator->GetCurrentDirection();
   Double_t normal[3] = {0.,0.,0.};
   this->FindBoundaryNormal(&normal[0], navigator, crossedNode);
   // Check if the normal vector is pointing along or against our current path
   Double_t dotProduct = dir[0]*normal[0] + dir[1]*normal[1] + dir[2]*normal[2];
   if (dotProduct < 0.) {
      // Normal is pointing in the opposite direction to our current track
      // so reflect the normal to get the correct direction
      #ifdef VERBOSE_MODE
         cout << "Reversing Normal to align with Current Direction" << endl;
      #endif
      normal[0] = -normal[0];
      normal[1] = -normal[1];
      normal[2] = -normal[2];
   }
   #ifdef VERBOSE_MODE
      cout << "Normal To Boundary aligned with Current Direction: " << endl;
      cout << "X:" << normal[0] <<"\t"<< "Y:" << normal[1] <<"\t"<< "Z:" << normal[2] << endl;
   #endif
   // Make up to 100 micro-steps, each of size 1E-10 -- (equivalent to a 1E-8 step).
   // This size of step defines a range that under which we assume there could be inaccuracies
   // in our boundary finder. Thus for this strategy to work, we need to ensure that we can
   // find the correct time to the next boundary to within 1E-8. Thus
   // our boundaries form a ±1E-8 zone, rather than a purely discrete line
   for (Int_t stepNumber = 1; stepNumber <= 100; stepNumber++) {
      // Increment the current point
      for (Int_t i = 0; i < 3; i++) {currentGlobalPoint[i] += normal[i]*TGeoShape::Tolerance();} 
      // Update point in the navigator
      navigator->SetCurrentPoint(currentGlobalPoint);
      // Get the new current point, local to the current volume
      currentMatrix->MasterToLocal(currentGlobalPoint, currentLocalPoint);
      // Get the new current point, local to the initial volume for comparison
      initialMatrix->MasterToLocal(currentGlobalPoint, tempLocalPoint);
      #ifdef VERBOSE_MODE
         cout << stepNumber << "\t" << "Making micro-step along current direction to try and locate particle within correct volume." << endl;
         cout << "Current Node: " << currentNode->GetName() << endl;
         cout << "Current point after micro-step: ";
         cout << "X:" << currentGlobalPoint[0] << "\t" << "Y:" << currentGlobalPoint[1] << "\t";
         cout << "Z:" << currentGlobalPoint[2] << endl;
         cout << "Current point local to current volume after micro-step: ";
         cout << "X:" << currentLocalPoint[0] << "\t" << "Y:" << currentLocalPoint[1] << "\t";
         cout << "Z:" << currentLocalPoint[2] << endl;
         cout << "Sqrt(X^2 + Y^2): ";
         cout << TMath::Sqrt(TMath::Power(currentLocalPoint[0],2) + TMath::Power(currentLocalPoint[1],2)) << endl;
         cout << "Sqrt(X^2 + Z^2): ";
         cout << TMath::Sqrt(TMath::Power(currentLocalPoint[0],2) + TMath::Power(currentLocalPoint[2],2)) << endl;
         cout << "Current Node Contains Point? ";
         cout << currentNode->GetVolume()->GetShape()->Contains(currentLocalPoint) << endl;
         cout << "Initial Node Contains Point? ";
         cout << initialNode->GetVolume()->GetShape()->Contains(tempLocalPoint) << endl;
      #endif
      // Check whether this new point is exclusively located in the current volume
      if (navigator->IsSameLocation(currentGlobalPoint[0], currentGlobalPoint[1], currentGlobalPoint[2], kFALSE) == kTRUE) {
         // Point is correctly located - exit the loop
         #ifdef VERBOSE_MODE
            cout << "Particle is now correctly located in: " << currentNode->GetName() << endl;
         #endif   
         return kTRUE;
      }
      // Otherwise, try another microstep
   }
   
   // Loop has finished and we have not been able to push the particle back into the
   // correct volume. This suggests a major problem so we highlight the error and return
   // a failure.
   #ifdef VERBOSE_MODE
      cout << "Point is STILL not exclusively contained in Current Node after relocation" << endl;
      cout << "Current Node: " << currentNode->GetName() << endl;
      cout << "Initial Node: " << initialNode->GetName() << endl;
      cout << "Actual Node: " << navigator->FindNode()->GetName() << endl;
   #endif
   return kFALSE;
}

//_____________________________________________________________________________
Bool_t TUCNPropagating::FindBoundaryNormal(Double_t* normal, TGeoNavigator* navigator, const TGeoNode* crossedNode)
{
// Computes normal to the crossed boundary, assuming that the current point
// is close enough to the boundary. This method is the same as ROOT's except that we are using
// our own currentNode be used exclusively instead of TGeoNavigator::FindNormal when gravity is present.

   if (crossedNode == NULL) {
      Error("FindBoundaryNormal","No boundary has been set");
      return kFALSE;
   }
   Double_t local[3];
   Double_t ldir[3];
   Double_t lnorm[3];
   navigator->GetHMatrix()->MasterToLocal(navigator->GetCurrentPoint(), local);
   navigator->GetHMatrix()->MasterToLocalVect(navigator->GetCurrentDirection(), ldir);
   crossedNode->GetVolume()->GetShape()->ComputeNormal(local, ldir, lnorm);
   navigator->GetHMatrix()->LocalToMasterVect(lnorm, normal);
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TUCNPropagating::WillDecay(const Double_t /*timeInterval*/)
{
   // Placeholder for method to calculate probability particle will decay within timeInterval, and then roll the dice!
   return kFALSE;
}

//_____________________________________________________________________________
void TUCNPropagating::Detected(TUCNParticle* particle)
{
   this->ChangeState(particle, new TUCNDetected());
}

//_____________________________________________________________________________
void TUCNPropagating::Lost(TUCNParticle* particle)
{
   this->ChangeState(particle, new TUCNLost());
}

//_____________________________________________________________________________
void TUCNPropagating::Absorbed(TUCNParticle* particle)
{
   this->ChangeState(particle, new TUCNAbsorbed());
}

//_____________________________________________________________________________
void TUCNPropagating::Bad(TUCNParticle* particle)
{
   this->ChangeState(particle, new TUCNBad());
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNDecayed                                                          //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNDecayed)

//_____________________________________________________________________________
TUCNDecayed::TUCNDecayed()
            :TUCNState()
{
   // Constructor
//   Info("TUCNDecayed","Constructor");
}

//_____________________________________________________________________________
TUCNDecayed::TUCNDecayed(const TUCNDecayed& s)
            :TUCNState(s)
{
   // Copy Constructor
//   Info("TUCNDecayed","Copy Constructor");
}

//_____________________________________________________________________________
TUCNDecayed& TUCNDecayed::operator=(const TUCNDecayed& s)
{
   // Assignment
//   Info("TUCNDecayed","Assignment");
   if(this!=&s) {
      TUCNState::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
TUCNDecayed::~TUCNDecayed()
{
   // Destructor
//   Info("TUCNDecayed","Destructor");
}

//______________________________________________________________________________
Bool_t TUCNDecayed::SaveState(TUCNRun* run, TUCNParticle* particle)
{
   // Register in Run what final state we are
//   run->GetData()->AddDecayedParticleState(particle);
   return kTRUE;
}


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNAbsorbed                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNAbsorbed)

//_____________________________________________________________________________
TUCNAbsorbed::TUCNAbsorbed()
             :TUCNState()
{
   // Constructor
//   Info("TUCNAbsorbed","Constructor");
}

//_____________________________________________________________________________
TUCNAbsorbed::TUCNAbsorbed(const TUCNAbsorbed& s)
             :TUCNState(s)
{
   // Copy Constructor
//   Info("TUCNAbsorbed","Copy Constructor");
}

//_____________________________________________________________________________
TUCNAbsorbed& TUCNAbsorbed::operator=(const TUCNAbsorbed& s)
{
   // Assignment
//   Info("TUCNAbsorbed","Assignment");
   if(this!=&s) {
      TUCNState::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
TUCNAbsorbed::~TUCNAbsorbed()
{
   // Destructor
//   Info("TUCNAbsorbed","Destructor");
}

//______________________________________________________________________________
Bool_t TUCNAbsorbed::SaveState(TUCNRun* run, TUCNParticle* particle)
{
   // Register in Run what final state we are
//   run->GetData()->AddAbsorbedParticleState(particle);
   return kTRUE;
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNDetected                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNDetected)

//_____________________________________________________________________________
TUCNDetected::TUCNDetected()
             :TUCNState()
{
   // Constructor
//   Info("TUCNDetected","Constructor");
}

//_____________________________________________________________________________
TUCNDetected::TUCNDetected(const TUCNDetected& s)
             :TUCNState(s)
{
   // Copy Constructor
//   Info("TUCNDetected","Copy Constructor");
}

//_____________________________________________________________________________
TUCNDetected& TUCNDetected::operator=(const TUCNDetected& s)
{
   // Assignment
//   Info("TUCNDetected","Assignment");
   if(this!=&s) {
      TUCNState::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
TUCNDetected::~TUCNDetected()
{
   // Destructor
//   Info("TUCNDetected","Destructor");
}

//______________________________________________________________________________
Bool_t TUCNDetected::SaveState(TUCNRun* run, TUCNParticle* particle)
{
   // Register in Run what final state we are
//   run->GetData()->AddDetectedParticleState(particle);
   return kTRUE;
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNLost                                                             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNLost)

//_____________________________________________________________________________
TUCNLost::TUCNLost()
         :TUCNState()
{
   // Constructor
//   Info("TUCNLost","Constructor");
}

//_____________________________________________________________________________
TUCNLost::TUCNLost(const TUCNLost& s)
         :TUCNState(s)
{
   // Copy Constructor
//   Info("TUCNLost","Copy Constructor");
}

//_____________________________________________________________________________
TUCNLost& TUCNLost::operator=(const TUCNLost& s)
{
   // Assignment
//   Info("TUCNLost","Assignment");
   if(this!=&s) {
      TUCNState::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
TUCNLost::~TUCNLost()
{
   // Destructor
//   Info("TUCNLost","Destructor");
}

//______________________________________________________________________________
Bool_t TUCNLost::SaveState(TUCNRun* run, TUCNParticle* particle)
{
   // Register in Run what final state we are
//   run->GetData()->AddLostParticleState(particle);
   return kTRUE;
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNBad                                                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNBad)

//_____________________________________________________________________________
TUCNBad::TUCNBad()
        :TUCNState()
{
   // Constructor
//   Info("TUCNBad","Constructor");
}

//_____________________________________________________________________________
TUCNBad::TUCNBad(const TUCNBad& s)
        :TUCNState(s)
{
   // Copy Constructor
//   Info("TUCNBad","Copy Constructor");
}

//_____________________________________________________________________________
TUCNBad& TUCNBad::operator=(const TUCNBad& s)
{
   // Assignment
//   Info("TUCNBad","Assignment");
   if(this!=&s) {
      TUCNState::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
TUCNBad::~TUCNBad()
{
   // Destructor
//   Info("TUCNLost","Destructor");
}

//______________________________________________________________________________
Bool_t TUCNBad::SaveState(TUCNRun* run, TUCNParticle* particle)
{
   // Register in Run what final state we are
//   run->GetData()->AddBadParticleState(particle);
   return kTRUE;
}

