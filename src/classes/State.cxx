// UCN class
// Author: Matthew Raso-Barnett  19/02/2010
#include <iostream>
#include <iomanip>
#include <cassert>
#include <stdexcept>

#include "State.h"

#include "TMath.h"
#include "TRandom.h"

#include "Run.h"
#include "RunConfig.h"
#include "Volume.h"
#include "FieldManager.h"
#include "GravField.h"
#include "Particle.h"
#include "Box.h"
#include "Parabola.h"
#include "MagField.h"
#include "Observer.h"
#include "Clock.h"

#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoMatrix.h"
#include "TGeoVoxelFinder.h"

#include "Constants.h"
#include "Units.h"
#include "DataFileHierarchy.h"

using namespace std;

//#define VERBOSE_MODE

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    State                                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(State)

//_____________________________________________________________________________
State::State()
          :TObject()
{
   // Constructor
//   Info("State","Default Constructor");
}

//_____________________________________________________________________________
State::State(const State& s)
          :TObject(s)
{
   // Copy Constructor
//   Info("State","Copy Constructor");
}

//_____________________________________________________________________________
State& State::operator=(const State& s)
{
   // Assignment
//   Info("State","Assignment");
   if(this!=&s) {
      TObject::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
State::~State()
{
   // Destructor
//   Info("State","Destructor");
}

//_____________________________________________________________________________
void State::ChangeState(Particle* particle, State* newState)
{
   // -- Change the particle's state pointer to the new state
   particle->ChangeState(newState);
   // Delete ourselves (previous state) before we finish
   delete this;
}

//_____________________________________________________________________________
Bool_t State::Propagate(Particle* /*particle*/, Run* /*run*/)
{
   // Default behaviour - don't propagate
   return kFALSE;
}

//_____________________________________________________________________________
Bool_t State::LocateInGeometry(Particle* /*particle*/, TGeoNavigator* /*navigator*/, const TGeoNode* /*initialNode*/, const TGeoMatrix* /*initialMatrix*/, const TGeoNode* /*crossedNode*/)
{
   return kTRUE;
}

//_____________________________________________________________________________
void State::IsDetected(Particle* /*particle*/)
{
   // Default do nothing
}

//_____________________________________________________________________________
void State::IsDecayed(Particle* /*particle*/)
{
   // Default do nothing
}

//_____________________________________________________________________________
void State::IsLost(Particle* /*particle*/)
{
   // Default do nothing
}

//_____________________________________________________________________________
void State::IsAbsorbed(Particle* /*particle*/)
{
   // Default do nothing
}

//_____________________________________________________________________________
void State::IsAnomalous(Particle* /*particle*/)
{
   // Default do nothing
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Propagating                                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(Propagating)

//_____________________________________________________________________________
Propagating::Propagating()
                :State(),
                 fIsStepEntering(kFALSE),
                 fIsStepExiting(kFALSE),
                 fIsOnBoundary(kFALSE)
{
   // Constructor
//   Info("Propagating","Constructor");
}

//_____________________________________________________________________________
Propagating::Propagating(const Propagating& s)
                :State(s),
                 fIsStepEntering(s.fIsStepEntering),
                 fIsStepExiting(s.fIsStepExiting),
                 fIsOnBoundary(s.fIsOnBoundary)
{
   // Copy Constructor
//   Info("Propagating","Copy Constructor");
}

//_____________________________________________________________________________
Propagating& Propagating::operator=(const Propagating& s)
{
   // Assignment.
//   Info("Propagating","Assignment");
   if(this!=&s) {
      State::operator=(s);
      fIsStepEntering = s.fIsStepEntering;
      fIsStepExiting = s.fIsStepExiting;
      fIsOnBoundary = s.fIsOnBoundary;
   }
   return *this;
}

//_____________________________________________________________________________
Propagating::~Propagating()
{
   // Destructor
//   Info("Propagating","Destructor");
}

//______________________________________________________________________________
Bool_t Propagating::SaveState(Run* run, Particle* particle)
{
   // Register in Run what final state we are
   run->GetData()->SaveParticle(particle, Folders::propagating);
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t Propagating::Propagate(Particle* particle, Run* run)
{
   // -- Propagate particle through geometry until it is either stopped by some decay process
   // -- or the runTime defined int he RunConfig has been reached
   #ifdef VERBOSE_MODE
      cout << "Propagate - Starting Run - Max time (seconds): " <<  run->RunTime() << endl;
   #endif
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Initialise TGeoNavigator
   // InitTrack sets navigator's current point/direction/node to that of the particle
   TGeoNavigator* const navigator = run->GetExperiment().GetNavigator();
   navigator->InitTrack(particle->X(), particle->Y(), particle->Z(), particle->Nx(), particle->Ny(), particle->Nz());
   // -- Check that Particle has not been initialised inside a boundary or detector
   const Volume* vol = dynamic_cast<const Volume*>(navigator->GetCurrentVolume());
   if (vol->IsTrackingVolume() == kFALSE) {
      cout << "Particle: " << particle->Id() << " initialised inside boundary of ";
      cout << navigator->GetCurrentVolume()->GetName() << endl;
      return kFALSE;
   }   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Propagation Loop
   for (Int_t stepNumber = 1 ; ; stepNumber++) {
      #ifdef VERBOSE_MODE
         cout << endl;
         cout << "-------------------------------------------------------" << endl;
         cout << "STEP " << stepNumber << endl;	
         cout << "-------------------------------------------------------" << endl;
      #endif
      // Check clock for time to the next event. Set this as the maximum step time
      Double_t stepTime = Clock::Instance()->GetTimeToNextEvent();
      // -- Check if run has ended
      if (stepTime == 0.0) break;
      // -- Make a step
      if (this->MakeStep(stepTime, particle, run) == kFALSE) {
         // -- Particle has reached a final state (decay,detected)
         break; // -- End Propagation Loop
      }
   }
   // -- END OF PROPAGATION LOOP
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t Propagating::MakeStep(Double_t stepTime, Particle* particle, Run* run)
{
   // -- Find time to reach next boundary and step along parabola
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Get the initial parameters
   ///////////////////////////////////////////////////////////////////////////////////////
   TGeoNavigator* navigator = run->GetExperiment().GetNavigator();
   const GravField* const gravField = run->GetExperiment().GetGravField();
      
   // -- Store the initial particle's position
   const TVector3 initialPosition(particle->X(), particle->Y(), particle->Z());
   // -- Save initial node and its name
   const TGeoNode* initialNode = navigator->GetCurrentNode();
   const string initialVolumeName = initialNode->GetVolume()->GetName();
   // -- Save Initial node's matrix
   TGeoHMatrix initMatrix = *(navigator->GetCurrentMatrix()); // Copy the initial matrix here
   const TGeoMatrix* initialMatrix = &initMatrix; // Hold pointer to the stored matrix
   // -- Save Path to current node - we will want to return to this in the event we make a bounce
   const char* initialPath = navigator->GetPath();
   #ifdef VERBOSE_MODE
      cout << "------------------- START OF STEP ----------------------" << endl;
      particle->Print(); // Print state
      cout << "Steptime (s): " << stepTime << endl;
      cout << "-----------------------------" << endl;
      cout << "Navigator's Initial Node: " << initialNode->GetName() << endl;
      cout << "Initial Node PATH: " << initialPath << endl;
      cout << "Initial Matrix: " << endl;
      initialMatrix->Print();
      cout << "Is On Boundary?  " << fIsOnBoundary << endl;
      cout << "Is Step Entering?  " << fIsStepEntering << endl;
      cout << "Is Step Exiting?  " << fIsStepExiting << endl;
      cout << "-----------------------------" << endl;
   #endif
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Find Next Boundary
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
         this->IsAnomalous(particle);
         throw runtime_error("Failed to find next node");
      }
      // (Re-)Calculate the time travelled (approximately) from the navigator's stepsize
      Double_t timeTravelled = (particle->V() == 0. ? 0. : navigator->GetStep()/particle->V()); 
      stepTime = timeTravelled;
   } else {
      // CASE 2; Grav Field present - tracking along parabolic trajectories
      // -- Propagate Point by StepTime along Parabola
      TGeoNode* nextNode = this->ParabolicBoundaryFinder(stepTime, *particle, navigator, crossedNode, gravField);
      if (nextNode == NULL) {
         #ifdef VERBOSE_MODE
            Error("MakeStep", "MakeStep has failed to find the next node");
         #endif
         this->IsAnomalous(particle);;
         throw runtime_error("Failed to find next node");
      }
      // Assert that the returned node is also the current node
      assert(nextNode == navigator->GetCurrentNode());
   }
   
   // -- We should now have propagated our point by some stepsize and be inside the correct volume 
   if (fIsStepEntering == kTRUE) {
      // If current node is the daughter of the initial node, then we need to set the current node
      // as the crossed node, so that we use its normal vector for making reflections off of
      crossedNode = navigator->GetCurrentNode();
      #ifdef VERBOSE_MODE
         cout << "Particle has entered a Daughter volume of the initial volume: " << crossedNode->GetName() << endl;
      #endif
   } else if (fIsStepExiting == kTRUE) {
      // We are exiting mother volume
      // crossedNode = initialNode --> therefore no change to be made
      #ifdef VERBOSE_MODE
         cout << "Particle has exited the boundary of the initial volume: " << crossedNode->GetName() << endl;
      #endif
   } else {
      // Neither Exiting mother nor entering daughter. Hopefully its because we didnt hit any boundaries
      #ifdef VERBOSE_MODE
         cout << "Particle has not hit any boundaries in this step" << endl;
      #endif
   }
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Move Particle to next position.
   // -- Along the way, if a MagField is present, precess the spin
   ///////////////////////////////////////////////////////////////////////////////////////
   particle->Move(stepTime, run);
   #ifdef VERBOSE_MODE	
      cout << "------------------- AFTER STEP ----------------------" << endl;
      particle->Print(); // Print verbose
      cout << "Steptime (s): " << stepTime << endl;
      cout << "-----------------------------" << endl;
      cout << "Navigator's Current Node: " << navigator->GetCurrentNode()->GetName() << endl;
      cout << "Crossed Node: " << crossedNode->GetName() << endl;
      cout << "Is On Boundary?  " << fIsOnBoundary << endl;
      cout << "Is Step Entering?  " << fIsStepEntering << endl;
      cout << "Is Step Exiting?  " << fIsStepExiting << endl;
      cout << "-----------------------------" << endl << endl;
   #endif
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Now we need to determine where we have ended up, and to examine whether
   // -- the current volume is the point's true container
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Attempt to locate particle within the current node
   Bool_t locatedParticle = this->LocateInGeometry(particle, navigator, initialNode, initialMatrix, crossedNode);
   if (locatedParticle == kFALSE) {
      #ifdef VERBOSE_MODE
         cout << "Error - After Step - Unable to locate particle correctly in Geometry" << endl;
      #endif
      this->IsAnomalous(particle);;
      throw runtime_error("Unable to locate particle uniquely in correct volume");
   }
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Determine whether we collided with a wall, decayed, or any other
   // -- state change occured
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Check whether particle has decayed in the last step
   if(this->WillDecay(stepTime) == kTRUE) {
      this->IsDecayed(particle);
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
   Volume* currentVolume = static_cast<Volume*>(navigator->GetCurrentVolume());
   if (currentVolume->Interact(particle, normal, navigator, crossedNode, initialPath, run->GetRunConfig()) == kFALSE) {
      // Particle reached a final state
      return kFALSE;
   }
   // End of MakeStep.
   return kTRUE;
}

//_____________________________________________________________________________
Double_t Propagating::DetermineNextStepTime(const Particle& particle, const RunConfig& runConfig)
{
   // -- Calculate the maximum size of the next geometric step of the particle, according to
   // -- the specified run parameters
   if (runConfig.RunTime() == 0.0) {return 0.0;}
   // Check if we will reach the maximum runtime of the track. If so propagate only until this time.
   if (particle.T() > (runConfig.RunTime() - runConfig.MaxStepTime())) {
      return ((runConfig.RunTime() - particle.T()) + 1.E-10);
   }
   // Else return the predefined max step time
   return runConfig.MaxStepTime();
}

//_____________________________________________________________________________
TGeoNode* Propagating::ParabolicBoundaryFinder(Double_t& stepTime, const Particle& particle, TGeoNavigator* navigator, TGeoNode* crossedNode, const GravField* const field)
{
// Compute distance to next boundary within STEPMAX. If no boundary is found,
// propagate current point along current direction with fStep=STEPMAX. Otherwise
// propagate with fStep=SNEXT (distance to boundary) and locate/return the next 
// node.
   
   // -- Get the global coordinates
   Double_t globalField[3]     = {field->Gx(), field->Gy(), field->Gz()}; 
   Double_t globalPoint[3]     = {particle.X(), particle.Y(), particle.Z()};
   Double_t globalDir[3]       = {particle.Nx(), particle.Ny(), particle.Nz()};
   Double_t globalVelocity[3]  = {particle.Vx(), particle.Vy(), particle.Vz()};
   
   Double_t currentField[3]    = {globalField[0], globalField[1], globalField[2]};
   Double_t currentPoint[3]    = {globalPoint[0], globalPoint[1], globalPoint[2]};
   Double_t currentDir[3]      = {globalDir[0], globalDir[1], globalDir[2]};
   Double_t currentVelocity[3] = {globalVelocity[0], globalVelocity[1], globalVelocity[2]};
   
   // Compute maximum stepsize
   const Double_t stepMax = Parabola::Instance()->ArcLength(globalVelocity, globalField, stepTime);
   
   // -- Some initialisations
   fIsStepExiting  = kFALSE;
   fIsStepEntering = kFALSE;
   TGeoNode *skip;
   
   // -- Store time step and step distance -- these two should always be convertible
   navigator->SetStep(stepMax);
   Double_t snext = TGeoShape::Big();
   Double_t tnext = TGeoShape::Big();
   
   #ifdef VERBOSE_MODE
      cout << "-----------------------------" << endl;
      cout << "-- STARTING PBF --" << endl;
      cout << setw(20) << "StepTime: " << setw(10) << stepTime << "\t";
      cout << setw(20) << "StepSize: " << setw(10) << navigator->GetStep() << endl;
      cout << setw(20) << "Global Field - " << setw(4) << "X: " << setw(10) << globalField[0] << "\t";
      cout << setw(4) << "Y: " << setw(10) << globalField[1] << "\t";
      cout << setw(4) << "Z: " << setw(10) << globalField[2] << endl;
      cout << setw(20) << "Global Point - " << setw(4) << "X: " << setw(10) << globalPoint[0] << "\t";
      cout << setw(4) << "Y: " << setw(10) << globalPoint[1] << "\t";
      cout << setw(4) << "Z: " << setw(10) << globalPoint[2] << endl;
      cout << setw(20) << "Global Dir - " << setw(4) << "X: " << setw(10) << globalDir[0] << "\t";
      cout << setw(4) << "Y: " << setw(10) << globalDir[1] << "\t";
      cout << setw(4) << "Z: " << setw(10) << globalDir[2] << endl;	
      cout << setw(20) << "Global Velocity - " << setw(4) << "X: " << setw(10) << globalVelocity[0] << "\t";
      cout << setw(4) << "Y: " << setw(10) << globalVelocity[1] << "\t";
      cout << setw(4) << "Z: " << setw(10) << globalVelocity[2] << endl;	
   #endif
   
   navigator->GetHMatrix()->CopyFrom(navigator->GetCurrentMatrix());
   
   // *********************************************************************
   // -- Get point and direction in local coordinate frame and calculate
   // -- distance to boundary of current node.
   // *********************************************************************
   Double_t localPoint[3], localVelocity[3], localField[3]; // Containers for the local coords
   navigator->GetCurrentMatrix()->MasterToLocal(currentPoint, &localPoint[0]);
   navigator->GetCurrentMatrix()->MasterToLocalVect(currentVelocity, &localVelocity[0]);
   navigator->GetCurrentMatrix()->MasterToLocalVect(currentField, &localField[0]);
   TGeoVolume *vol = navigator->GetCurrentNode()->GetVolume();
   
   // -- Find distance to exiting current node
   #ifdef VERBOSE_MODE
      cout << "Finding distance to exit current node: " << vol->GetName() << endl;
   #endif
   tnext = static_cast<Box*>(vol->GetShape())->TimeFromInside(localPoint, localVelocity, localField, stepTime, fIsOnBoundary); 
   if (tnext <= 0.0) {
      Error("ParabolicBoundaryFinder", "Failed to find boundary of current node from inside");
      return NULL;
   }
   snext = Parabola::Instance()->ArcLength(localVelocity, localField, tnext);
   crossedNode = navigator->GetCurrentNode();
   #ifdef VERBOSE_MODE
      cout << "Time to nearest boundary of " << crossedNode->GetName() << ": " << tnext << endl;
      cout << "Proposed Step Time: " << stepTime << endl;
   #endif
   
   // -- If distance to exiting current node is <= Tolerance value (1e-10)
   // -- make a small step by navigator tolerance value
   if (snext <= TGeoShape::Tolerance()) {
      #ifdef VERBOSE_MODE
         cout << "Warning: Distance is within Shape Tolerance level." << endl;
      #endif
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
      navigator->SetStep(snext);
      stepTime = tnext;
      fIsStepEntering = kFALSE;
      fIsStepExiting = kTRUE;
      #ifdef VERBOSE_MODE
         cout << "Boundary of " << crossedNode->GetName() << " is within range of proposed step size." << endl;
         cout << "Updated Step Time: " << stepTime << endl;
      #endif
   }
   
   // *********************************************************************
   // -- Check if we will intersect any daughters of current volume before
   // -- reaching current volume's boundary
   // *********************************************************************
   #ifdef VERBOSE_MODE
      cout << "Check daughter volumes of " << crossedNode->GetName() << " to see if there are any further intersection" << endl;
   #endif
   Int_t daughterIndex = -1;
   TGeoNode *crossed = this->ParabolicDaughterBoundaryFinder(stepTime, navigator, localPoint, localVelocity, localField, daughterIndex, kTRUE);
   if (crossed) {
      #ifdef VERBOSE_MODE
         cout << "Particle will intersect " << crossed->GetName() << " volume first." << endl;
      #endif
      fIsStepExiting = kFALSE;
      fIsStepEntering = kTRUE;
      // If we crossed a daughter volume, set this node to be the new crossedNode,
      // since in this case it is the daughter's boundary we are crossing, rather than
      // the current volume's boundary.
      crossedNode = crossed;
   }
   // *********************************************************************
   // -- Update the navigator's state to intersection point
   // *********************************************************************
   const Double_t timestep = stepTime;
   #ifdef VERBOSE_MODE
      cout << "-----------------------------" << endl;
      cout << "Intersection found with " << crossedNode->GetName() << endl;
      cout << "Updating Navigator's State to point of intersection..." << endl;
      cout << "Final Time Step (s): " << stepTime << "\t";
      cout << "Step Size (m): " << navigator->GetStep() << endl;
      cout << setw(20) << "Initial Point - " << setw(4) << "X: " << setw(10) << currentPoint[0] << "\t";
      cout << setw(4) << "Y: " << setw(10) << currentPoint[1] << "\t";
      cout << setw(4) << "Z: " << setw(10) << currentPoint[2] << endl;
      cout << setw(20) << "Initial Velocity - " << setw(4) << "X: " << setw(10) << currentVelocity[0] << "\t";
      cout << setw(4) << "Y: " << setw(10) << currentVelocity[1] << "\t";
      cout << setw(4) << "Z: " << setw(10) << currentVelocity[2] << endl;	
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
      cout << "---------" << endl;
      cout << setw(20) << "Final Point - " << setw(4) << "X: " << setw(10) << currentPoint[0] << "\t";
      cout << setw(4) << "Y: " << setw(10) << currentPoint[1] << "\t";
      cout << setw(4) << "Z: " << setw(10) << currentPoint[2] << endl;
      cout << setw(20) << "Final Velocity - " << setw(4) << "X: " << setw(10) << currentVelocity[0] << "\t";
      cout << setw(4) << "Y: " << setw(10) << currentVelocity[1] << "\t";
      cout << setw(4) << "Z: " << setw(10) << currentVelocity[2] << endl;	
   #endif
   // *********************************************************************
   // -- Return Final Node
   // *********************************************************************
   TGeoNode *current = 0;
   if (fIsStepExiting == false && fIsStepEntering == false) {
      // Nothing crossed within stepMax -> propagate and return same location
      #ifdef VERBOSE_MODE
         cout << "Nothing crossed within step." << endl;
      #endif
      fIsOnBoundary = kFALSE;
      return navigator->GetCurrentNode();
   }
   fIsOnBoundary = kTRUE;
   if (fIsStepExiting == true) {
      #ifdef VERBOSE_MODE
         cout << "Reached a boundary of current volume (or daughter of current volume) within step." << endl;
         cout << "Now crossing boundary and determining what our next volume is..." << endl;
         cout << "Current Node: " << navigator->GetCurrentNode()->GetName() << endl;
         cout << "Get Level: " << navigator->GetLevel() << endl;
      #endif
      skip = navigator->GetCurrentNode();
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
         cout << "Crossing boundary. Navigating Upwards in Node Hierarchy." << endl;
         cout << "Final Node: " << finalNode->GetName() << endl;
         cout << "---------------------------------------" << endl;
      #endif
      return finalNode;
   }
   current = navigator->GetCurrentNode();
   // Make a daughter of current node current  
   navigator->CdDown(daughterIndex);
   TGeoNode* finalNode = navigator->CrossBoundaryAndLocate(kTRUE, current);
   #ifdef VERBOSE_MODE
      cout << "Crossing boundary. Navigating Downwards in Node Hierarchy." << endl;
      cout << "Final Node: " << finalNode->GetName() << endl;
      cout << "---------------------------------------" << endl;
   #endif
   return finalNode;
}

//_____________________________________________________________________________
TGeoNode* Propagating::ParabolicDaughterBoundaryFinder(Double_t& stepTime, TGeoNavigator* navigator, Double_t* point, Double_t* velocity, Double_t* field, Int_t &idaughter, Bool_t compmatrix)
{
// Computes as fStep the distance to next daughter of the current volume. 
// The point and direction must be converted in the coordinate system of the current volume.
// The proposed step limit is fStep.
   
   // -- First Get the current local and global fields
   Double_t motherField[3] = {field[0], field[1], field[2]}; 
   Double_t motherPoint[3] = {point[0], point[1], point[2]};
   Double_t motherVelocity[3] = {velocity[0], velocity[1], velocity[2]};
   
   #ifdef VERBOSE_MODE
      cout << "-----------------------------" << endl;
      cout << "-- STARTING PDBF --" << endl;
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
      cout << "Mother Volume: " << vol->GetName() << "\t";
      cout << "Number of Daughters: " << nd << endl;
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
      cout << "Divided Volume. Calculate distance from outside to first cell." << endl;
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
            cout << "Volume: " << current->GetName() << endl;
         #endif
         tnext = static_cast<Box*>(current->GetVolume()->GetShape())->TimeFromOutside(localPoint, localVelocity, localField, stepTime, fIsOnBoundary);  
         if (tnext <= 0.0) {
            Error("ParabolicDaughterBoundaryFinder", "Failed to find boundary");
            return NULL;
         }
         snext = Parabola::Instance()->ArcLength(localVelocity, localField, tnext);
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
            cout << "Divided Volume. Calculate distance from outside to last cell." << endl;
            cout << "Volume: " << current->GetName() << endl;
         #endif
         tnext = static_cast<Box*>(current->GetVolume()->GetShape())->TimeFromOutside(localPoint, localVelocity, localField, stepTime, fIsOnBoundary);
         if (tnext <= 0.0) {
            Error("FindNextBoundary", "Failed to find boundary");
            return NULL;
         }
         snext = Parabola::Instance()->ArcLength(localVelocity, localField, tnext);
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
      #ifdef VERBOSE_MODE		
         cout << "Only Few daughters. Calculate distance from outside to all." << endl;
      #endif
      for (i=0; i<nd; i++) {
         current = vol->GetNode(i);
         if (gGeoManager->IsActivityEnabled() && !current->GetVolume()->IsActive()) continue;
         current->cd();
         if (voxels && voxels->IsSafeVoxel(motherPoint, i, navigator->GetStep())) continue;
         current->MasterToLocal(motherPoint, localPoint);
         current->MasterToLocalVect(motherVelocity, localVelocity);
         current->MasterToLocalVect(motherField, localField);
         if (current->IsOverlapping() && current->GetVolume()->Contains(localPoint)) continue;
         tnext = static_cast<Box*>(current->GetVolume()->GetShape())->TimeFromOutside(localPoint, localVelocity, localField, stepTime, fIsOnBoundary);
         if (tnext <= 0.0) {
            Error("FindNextBoundary", "Failed to find boundary");
            return NULL;
         }
         snext = Parabola::Instance()->ArcLength(localVelocity, localField, tnext);
         #ifdef VERBOSE_MODE
            cout << "Daughter Volume: " << current->GetName() << endl;
            cout << "Time to nearest Daughter boundary: " << tnext << endl;
         #endif
         if (snext < (navigator->GetStep() - TGeoShape::Tolerance())) {
            #ifdef VERBOSE_MODE
            cout << "Distance to daughter boundary is less than distance to mother volume." << endl;
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
         if (nodefound) cout << "Nearest Daughter Node: " << nodefound->GetName() << endl;
      #endif
      return nodefound;
   }
   // if current volume is voxelized, first get current voxel
   Int_t ncheck = 0;
   Int_t sumchecked = 0;
   Int_t *vlist = 0;
   voxels->SortCrossedVoxels(motherPoint, motherVelocity);
   cout << "Volume is voxelized. Calculate distance from outside to voxel." << endl;
   while ((sumchecked<nd) && (vlist=voxels->GetNextVoxel(motherPoint, motherVelocity, ncheck))) {
      for (i=0; i<ncheck; i++) {
         current = vol->GetNode(vlist[i]);
         if (gGeoManager->IsActivityEnabled() && !current->GetVolume()->IsActive()) continue;
         current->cd();
         current->MasterToLocal(motherPoint, localPoint);
         current->MasterToLocalVect(motherVelocity, localVelocity);
         current->MasterToLocalVect(&motherField[0], &localField[0]);
         if (current->IsOverlapping() && current->GetVolume()->Contains(localPoint)) continue;
         tnext = static_cast<Box*>(current->GetVolume()->GetShape())->TimeFromOutside(localPoint, localVelocity, localField, stepTime, fIsOnBoundary);
         if (tnext <= 0.0) {
            Error("FindNextBoundary", "Failed to find boundary");
            return NULL;
         }
         snext = Parabola::Instance()->ArcLength(localVelocity, localField, tnext);
         #ifdef VERBOSE_MODE
            cout << "Daughter Volume: " << current->GetName() << endl;
            cout << "Time to nearest Daughter: " << tnext << endl;
         #endif
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
Bool_t Propagating::LocateInGeometry(Particle* particle, TGeoNavigator* navigator, const TGeoNode* initialNode, const TGeoMatrix* initialMatrix, const TGeoNode* crossedNode)
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
         cout << "Particle didn't cross a boundary in last step." << endl;
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
         cout << "Point in Coords local to Final Node: " << endl;
         cout << "X:" << currentLocalPoint[0] << "\t" << "Y:" << currentLocalPoint[1] << "\t";
         cout << "Z:" << currentLocalPoint[2] << endl;
         TGeoBBox* currentShape = dynamic_cast<TGeoBBox*>(currentNode->GetVolume()->GetShape());
         Double_t currentVolGlobalOrigin[3] = {0.,0.,0.};
         Double_t currentVolPosLocalBoundary[3] = {currentShape->GetDX(),currentShape->GetDY(),currentShape->GetDZ()};
         Double_t currentVolNegLocalBoundary[3] = {-currentShape->GetDX(),-currentShape->GetDY(),-currentShape->GetDZ()};
         Double_t currentVolPosGlobalBoundary[3] = {0.,0.,0.};
         Double_t currentVolNegGlobalBoundary[3] = {0.,0.,0.};
         currentNode->GetMatrix()->LocalToMaster(currentShape->GetOrigin(), currentVolGlobalOrigin);
         currentNode->GetMatrix()->LocalToMaster(currentVolPosLocalBoundary, currentVolPosGlobalBoundary);
         currentNode->GetMatrix()->LocalToMaster(currentVolNegLocalBoundary, currentVolNegGlobalBoundary);
         cout << "Final Node's Origin in Global Coords: " << endl;
         cout << "X:" << currentVolGlobalOrigin[0] << "\t" << "Y:" << currentVolGlobalOrigin[1] << "\t";
         cout << "Z:" << currentVolGlobalOrigin[2] << endl;
         cout << "Final Node's Boundaries in Global Coords: " << endl;
         cout << "+X: " << currentVolPosGlobalBoundary[0] << "\t -X: " << currentVolNegGlobalBoundary[0] << endl;
         cout << "+Y: " << currentVolPosGlobalBoundary[1] << "\t -Y: " << currentVolNegGlobalBoundary[1] << endl;
         cout << "+Z: " << currentVolPosGlobalBoundary[2] << "\t -Z: " << currentVolNegGlobalBoundary[2] << endl;         
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
         cout << "Point in Coords local to Initial Node: " << endl;
         cout << "X:" << tempLocalPoint[0] << "\t" << "Y:" << tempLocalPoint[1] << "\t";
         cout << "Z:" << tempLocalPoint[2] << endl;
         TGeoBBox* initialShape = dynamic_cast<TGeoBBox*>(initialNode->GetVolume()->GetShape());
         Double_t initialVolGlobalOrigin[3] = {0.,0.,0.};
         Double_t initialVolPosLocalBoundary[3] = {initialShape->GetDX(),initialShape->GetDY(),initialShape->GetDZ()};
         Double_t initialVolNegLocalBoundary[3] = {-initialShape->GetDX(),-initialShape->GetDY(),-initialShape->GetDZ()};
         Double_t initialVolPosGlobalBoundary[3] = {0.,0.,0.};
         Double_t initialVolNegGlobalBoundary[3] = {0.,0.,0.};
         initialNode->GetMatrix()->LocalToMaster(initialShape->GetOrigin(), initialVolGlobalOrigin);
         initialNode->GetMatrix()->LocalToMaster(initialVolPosLocalBoundary, initialVolPosGlobalBoundary);
         initialNode->GetMatrix()->LocalToMaster(initialVolNegLocalBoundary, initialVolNegGlobalBoundary);
         cout << "Final Node's Origin in Global Coords: " << endl;
         cout << "X:" << initialVolGlobalOrigin[0] << "\t" << "Y:" << initialVolGlobalOrigin[1] << "\t";
         cout << "Z:" << initialVolGlobalOrigin[2] << endl;
         cout << "Final Node's Boundaries in Global Coords: " << endl;
         cout << "+X: " << initialVolPosGlobalBoundary[0] << "\t -X: " << initialVolNegGlobalBoundary[0] << endl;
         cout << "+Y: " << initialVolPosGlobalBoundary[1] << "\t -Y: " << initialVolNegGlobalBoundary[1] << endl;
         cout << "+Z: " << initialVolPosGlobalBoundary[2] << "\t -Z: " << initialVolNegGlobalBoundary[2] << endl;         cout << "Sqrt(X^2 + Y^2): ";
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
      particle->UpdateCoordinates(navigator);
      
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
Bool_t Propagating::AttemptRelocationIntoCurrentNode(TGeoNavigator* navigator, const TGeoNode* initialNode, const TGeoMatrix* initialMatrix, const TGeoNode* crossedNode)
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
Bool_t Propagating::FindBoundaryNormal(Double_t* normal, TGeoNavigator* navigator, const TGeoNode* crossedNode)
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
Bool_t Propagating::WillDecay(const Double_t /*timeInterval*/)
{
   // Placeholder for method to calculate probability particle will decay within timeInterval, and then roll the dice!
   return kFALSE;
}

//_____________________________________________________________________________
void Propagating::IsDetected(Particle* particle)
{
   this->ChangeState(particle, new Detected());
}

//_____________________________________________________________________________
void Propagating::IsDecayed(Particle* particle)
{
   this->ChangeState(particle, new Decayed());
}

//_____________________________________________________________________________
void Propagating::IsLost(Particle* particle)
{
   this->ChangeState(particle, new Lost());
}

//_____________________________________________________________________________
void Propagating::IsAbsorbed(Particle* particle)
{
   this->ChangeState(particle, new Absorbed());
}

//_____________________________________________________________________________
void Propagating::IsAnomalous(Particle* particle)
{
   this->ChangeState(particle, new Anomalous());
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Decayed                                                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(Decayed)

//_____________________________________________________________________________
Decayed::Decayed()
            :State()
{
   // Constructor
//   Info("Decayed","Constructor");
}

//_____________________________________________________________________________
Decayed::Decayed(const Decayed& s)
            :State(s)
{
   // Copy Constructor
//   Info("Decayed","Copy Constructor");
}

//_____________________________________________________________________________
Decayed& Decayed::operator=(const Decayed& s)
{
   // Assignment
//   Info("Decayed","Assignment");
   if(this!=&s) {
      State::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
Decayed::~Decayed()
{
   // Destructor
//   Info("Decayed","Destructor");
}

//______________________________________________________________________________
Bool_t Decayed::SaveState(Run* run, Particle* particle)
{
   // Register in Run what final state we are
   run->GetData()->SaveParticle(particle, Folders::decayed);
   return kTRUE;
}


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Absorbed                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(Absorbed)

//_____________________________________________________________________________
Absorbed::Absorbed()
             :State()
{
   // Constructor
//   Info("Absorbed","Constructor");
}

//_____________________________________________________________________________
Absorbed::Absorbed(const Absorbed& s)
             :State(s)
{
   // Copy Constructor
//   Info("Absorbed","Copy Constructor");
}

//_____________________________________________________________________________
Absorbed& Absorbed::operator=(const Absorbed& s)
{
   // Assignment
//   Info("Absorbed","Assignment");
   if(this!=&s) {
      State::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
Absorbed::~Absorbed()
{
   // Destructor
//   Info("Absorbed","Destructor");
}

//______________________________________________________________________________
Bool_t Absorbed::SaveState(Run* run, Particle* particle)
{
   // Register in Run what final state we are
   run->GetData()->SaveParticle(particle, Folders::absorbed);
   return kTRUE;
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Detected                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(Detected)

//_____________________________________________________________________________
Detected::Detected()
             :State()
{
   // Constructor
//   Info("Detected","Constructor");
}

//_____________________________________________________________________________
Detected::Detected(const Detected& s)
             :State(s)
{
   // Copy Constructor
//   Info("Detected","Copy Constructor");
}

//_____________________________________________________________________________
Detected& Detected::operator=(const Detected& s)
{
   // Assignment
//   Info("Detected","Assignment");
   if(this!=&s) {
      State::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
Detected::~Detected()
{
   // Destructor
//   Info("Detected","Destructor");
}

//______________________________________________________________________________
Bool_t Detected::SaveState(Run* run, Particle* particle)
{
   // Register in Run what final state we are
   run->GetData()->SaveParticle(particle, Folders::detected);
   return kTRUE;
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Lost                                                             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(Lost)

//_____________________________________________________________________________
Lost::Lost()
         :State()
{
   // Constructor
//   Info("Lost","Constructor");
}

//_____________________________________________________________________________
Lost::Lost(const Lost& s)
         :State(s)
{
   // Copy Constructor
//   Info("Lost","Copy Constructor");
}

//_____________________________________________________________________________
Lost& Lost::operator=(const Lost& s)
{
   // Assignment
//   Info("Lost","Assignment");
   if(this!=&s) {
      State::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
Lost::~Lost()
{
   // Destructor
//   Info("Lost","Destructor");
}

//______________________________________________________________________________
Bool_t Lost::SaveState(Run* run, Particle* particle)
{
   // Register in Run what final state we are
   run->GetData()->SaveParticle(particle, Folders::lost);
   return kTRUE;
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Anomalous                                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(Anomalous)

//_____________________________________________________________________________
Anomalous::Anomalous()
    :State()
{
   // Constructor
//   Info("Anomalous","Constructor");
}

//_____________________________________________________________________________
Anomalous::Anomalous(const Anomalous& s)
        :State(s)
{
   // Copy Constructor
//   Info("Anomalous","Copy Constructor");
}

//_____________________________________________________________________________
Anomalous& Anomalous::operator=(const Anomalous& s)
{
   // Assignment
//   Info("Anomalous","Assignment");
   if(this!=&s) {
      State::operator=(s);   
   }
   return *this;
}

//_____________________________________________________________________________
Anomalous::~Anomalous()
{
   // Destructor
//   Info("Lost","Destructor");
}

//______________________________________________________________________________
Bool_t Anomalous::SaveState(Run* run, Particle* particle)
{
   // Register in Run what final state we are
   run->GetData()->SaveParticle(particle, Folders::anomalous);
   return kTRUE;
}

