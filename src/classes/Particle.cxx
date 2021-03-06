// UCN class
// Author: Matthew Raso-Barnett  19/01/2009
#include <iostream>
#include <iomanip>
#include <cassert>
#include <stdexcept>

#include "Constants.h"
#include "Units.h"

#include "Particle.h"
#include "Run.h"
#include "Data.h"
#include "Observer.h"
#include "Volume.h"
#include "Clock.h"
#include "ValidStates.h"

#include "TMath.h"
#include "TRandom.h"
#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoMatrix.h"
#include "TGeoVoxelFinder.h"
#include "TTree.h"

using namespace std;

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

ClassImp(Particle)

//______________________________________________________________________________
Particle::Particle()
             :TObject(), Observable(),
              fId(0), fPos(), fVel(),
              fState(NULL), fSpin(), fRndState(NULL)
{
   // -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Particle","Default Constructor");
   #endif
}


//______________________________________________________________________________
Particle::Particle(const unsigned int id, const Point pos, const TVector3 vel)
             :TObject(), Observable(),
              fId(id), fPos(pos), fVel(vel),
              fState(NULL), fSpin(), fRndState(NULL)
{
   // -- Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Particle","Constructor");
   #endif
   fState = new Propagating();
}

//_____________________________________________________________________________
Particle::Particle(const Particle& other)
             :TObject(other), Observable(other),
              fId(other.fId), fPos(other.fPos), fVel(other.fVel),
              fState(NULL), fSpin(other.fSpin), fRndState(NULL)
{
   // -- Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Particle","Copy Constructor");
   #endif
      if (other.fState) fState = (other.fState)->Clone();
      if (other.fRndState) fRndState = (other.fRndState)->Clone();
}

//_____________________________________________________________________________
Particle& Particle::operator=(const Particle& other) 
{
   //assignment operator
   #ifdef PRINT_CONSTRUCTORS
      Info("Particle", "Assignment");
   #endif
   if(this!=&other) {
      TObject::operator=(other);
      Observable::operator=(other);
      fId = other.fId;
      fPos = other.fPos;
      fVel = other.fVel;
      fSpin = other.fSpin;
      if (fState) delete fState;
      fState = (other.fState)->Clone();
      if (fRndState) delete fRndState;
      if (other.fRndState) fRndState = (other.fRndState)->Clone();
   }
   return *this;
}

//______________________________________________________________________________
Particle::~Particle()
{ 
   // -- Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Particle","Destructor");
   #endif
   if (fState) delete fState;
   if (fRndState) delete fRndState;
}

//______________________________________________________________________________
void Particle::ChangeState(State* state)
{
   // -- Store pointer to state
   fState = state;
}

//______________________________________________________________________________
void Particle::SetPosition(const Double_t x, const Double_t y, const Double_t z,
                                    const Double_t t)
{
   // Set current vertex to given coords
   fPos.SetPoint(x,y,z,t);
}

//______________________________________________________________________________
void Particle::SetVelocity(const Double_t vx, const Double_t vy, const Double_t vz)
{
   // Set current velocity and energy to given coords
   fVel.SetXYZ(vx,vy,vz);
}

//_____________________________________________________________________________
void Particle::Print(const Option_t* /*option*/) const
{
   cout << left << setw(15) << "Particle: " << setw(8) << this->Id() << "\t";
   cout << setw(4) << "T: " << setw(8) << this->T() << endl;
   cout << setw(15) << "Vertex (m): " << setw(4) << "X:" << setw(8) << this->X() << "\t";
   cout << setw(4) << "Y:" << setw(8) << this->Y() << "\t";
   cout << setw(4) << "Z:" << setw(8) << this->Z() << endl;
   cout << setw(15) << "Dir: " << setw(4) << "Nx:" << setw(8) << this->Nx()  << "\t";
   cout << setw(4) << "Ny:" << setw(8) << this->Ny() << "\t";
   cout << setw(4) << "Nz:" << setw(8) << this->Nz() << endl;
   cout << setw(15) << "Vel (m/s): " << setw(4) << "Vx:" << setw(8) << this->Vx()  << "\t";
   cout << setw(4) << "Vy:" << setw(8) << this->Vy() << "\t";
   cout << setw(4) << "Vz:" << setw(8) << this->Vz() << "\t";
   cout << setw(4) << "V:" << setw(8) << this->V() << endl;
   cout << setw(15) << "Mom (eV): " << setw(4) << "X:" << setw(8) << this->Px()  << "\t";
   cout << setw(4) << "Y:" << setw(8) << this->Py() << "\t";
   cout << setw(4) << "Z:" << setw(8) << this->Pz() << "\t";
   cout << setw(4) << "P:" << setw(8) << this->P() << endl;
   cout << setw(15) << "Energy (neV): " << setw(8) << this->Energy() / Units::neV << endl;
}

//_____________________________________________________________________________
Bool_t Particle::Propagate(Run* run)
{
   // -- Call State-dependent propagate method
   if (!fState) fState = new Propagating();
   return fState->Propagate(this,run);
   return true;
}

//_____________________________________________________________________________
void Particle::Move(const Double_t stepTime, const Run* run)
{
   // -- Move Particle by specified steptime and interact with any fields 
   // -- that may be present
   Double_t interval = run->GetRunConfig().SpinStepTime();
   // Check if the spin step time is larger than the geometric step time.
   if (interval >= stepTime || interval <= 0.0) {
      interval = stepTime;
   }
   #ifdef VERBOSE_MODE
      cout << "-------------------------------------------" << endl;
      cout << "-- Move Particle along trajectory -- " << endl;
      cout << "Interval: " << interval << "\t" << "StepTime: " << stepTime << endl;
      cout << "----------" << endl;
      cout << setw(10) << "Initial - " << setw(4) << "X: " << setw(10) << this->X() << "\t";
      cout << setw(4) << "Y: " << setw(10) << this->Y() << "\t";
      cout << setw(4) << "Z: " << setw(10) << this->Z() << endl;
      cout << setw(10) << "Initial - " << setw(4) << "Vx: " << setw(10) << this->Vx() << "\t";
      cout << setw(4) << "Vy: " << setw(10) << this->Vy() << "\t";
      cout << setw(4) << "Vz: " << setw(10) << this->Vz() << endl;
      cout << setw(4) << "E: " << setw(10) << this->Energy()/Units::neV << endl;
      cout << "----------" << endl;
   #endif
   // Fetch the Gravitational Field if it exists, and store field's components
   const GravField* const gravity = run->GetExperiment().GetGravField();
   TVector3 gravField(0.,0.,0.);
   if (gravity != NULL) {
      gravField.SetXYZ(gravity->Gx(), gravity->Gy(), gravity->Gz());
   }
   // Make multiple small steps, of size 'interval', until we have made
   // one step of size 'stepTime'
   const Double_t end = this->T() + stepTime;
   while (this->T() < end) {
      // Check if we will reach the end of stepTime within the next small step
      if (this->T() + interval > end) {interval = end - this->T();} 
      // Get current positions
      TVector3 pos(this->X(), this->Y(), this->Z());
      TVector3 vel(this->Vx(), this->Vy(), this->Vz());
      // Calculate position halfway along step
      const Double_t halfInterval = interval*0.5;
      TVector3 halfwayPos(pos);
      TVector3 halfwayVel(vel);
      for (int i=0;i<3;i++) {
         halfwayPos[i] += vel[i]*halfInterval + 0.5*gravField[i]*halfInterval*halfInterval;
         halfwayVel[i] += gravField[i]*halfInterval;
      }
      const Double_t halfwayTime = this->T()+halfInterval;
      Point halfwayPoint(halfwayPos,halfwayTime);
      // Calculate final position
      const Double_t finalTime = this->T()+interval;
      for (int i=0;i<3;i++) {
         pos[i] += vel[i]*interval + 0.5*gravField[i]*interval*interval;
         vel[i] += gravField[i]*interval;
      }
      // Update Clock
      Clock::Instance()->Tick(interval);
      // Update Particle
      this->SetPosition(pos[0],pos[1],pos[2],finalTime);
      this->SetVelocity(vel[0],vel[1],vel[2]);
      // Measure the magnetic field at the halfway point along step
      const TVector3 field = run->GetExperiment().GetMagField(halfwayPoint,halfwayVel);
      // Notify observers of new field state
      this->NotifyObservers(halfwayPoint, halfwayVel, Context::MagField);
      // Precess spin about measured magnetic field
      this->PrecessSpin(field, interval);
   }
   #ifdef VERBOSE_MODE
      cout << setw(10) << "Final - " << setw(4) << "X: " << setw(10) << this->X() << "\t";
      cout << setw(4) << "Y: " << setw(10) << this->Y() << "\t";
      cout << setw(4) << "Z: " << setw(10) << this->Z() << endl;
      cout << setw(10) << "Final - " << setw(4) << "Vx: " << setw(10) << this->Vx() << "\t";
      cout << setw(4) << "Vy: " << setw(10) << this->Vy() << "\t";
      cout << setw(4) << "Vz: " << setw(10) << this->Vz() << endl;
      cout << setw(4) << "E: " << setw(10) << this->Energy()/Units::neV << endl;
      cout << "---------------------------------------" << endl;
   #endif
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Notify Observers of Step Completion
   this->NotifyObservers(this->GetPoint(), this->GetVelocity(), Context::Step);
}

//_____________________________________________________________________________
void Particle::UpdateCoordinates(const TGeoNavigator* navigator)
{
   // Take the Navigator's internal state (position, direction) and set particle's to this
   #ifdef VERBOSE_MODE
      cout << "-------------------------------------------" << endl;
      cout << "-- Update Particle's state with that of the Navigator --" << endl;
      cout << setw(10) << "Initial - " << setw(4) << "X: " << setw(10) << this->X() << "\t";
      cout << setw(4) << "Y: " << setw(10) << this->Y() << "\t";
      cout << setw(4) << "Z: " << setw(10) << this->Z() << endl;
      cout << setw(10) << "Initial - " << setw(4) << "Vx: " << setw(10) << this->Vx() << "\t";
      cout << setw(4) << "Vy: " << setw(10) << this->Vy() << "\t";
      cout << setw(4) << "Vz: " << setw(10) << this->Vz() << endl;
      cout << setw(4) << "E: " << setw(10) << this->Energy()/Units::neV << endl;
      cout << "----------" << endl;
   #endif
   const Double_t* pos = navigator->GetCurrentPoint();
   const Double_t* dir = navigator->GetCurrentDirection();
   const Double_t  vel = this->V();
   // Update particle
   this->SetPosition(pos[0], pos[1], pos[2], this->T());
   this->SetVelocity(vel*dir[0], vel*dir[1], vel*dir[2]);
   #ifdef VERBOSE_MODE
      cout << setw(10) << "Final - " << setw(4) << "X: " << setw(10) << this->X() << "\t";
      cout << setw(4) << "Y: " << setw(10) << this->Y() << "\t";
      cout << setw(4) << "Z: " << setw(10) << this->Z() << endl;
      cout << setw(10) << "Final - " << setw(4) << "Vx: " << setw(10) << this->Vx() << "\t";
      cout << setw(4) << "Vy: " << setw(10) << this->Vy() << "\t";
      cout << setw(4) << "Vz: " << setw(10) << this->Vz() << endl;
      cout << setw(4) << "E: " << setw(10) << this->Energy()/Units::neV << endl;
      cout << "-------------------------------------------" << endl;
   #endif
}

//______________________________________________________________________________
Bool_t Particle::WillDecay(const Double_t timeInterval)
{
   // Calculate probability particle will decay within timeInterval, and then roll the dice!
   Double_t probDecay = (timeInterval/Neutron::lifetime);
   if (gRandom->Uniform(0.0, 1.0) < probDecay) { return kTRUE; }
   return kFALSE;
}


//_____________________________________________________________________________
void Particle::IsDetected()
{
   fState->IsDetected(this);
}

//_____________________________________________________________________________
void Particle::IsLost()
{
   fState->IsLost(this);
}

//_____________________________________________________________________________
void Particle::IsAbsorbed()
{
   fState->IsAbsorbed(this);
}

//_____________________________________________________________________________
void Particle::IsAnomalous()
{
   fState->IsAnomalous(this);
}

//_____________________________________________________________________________
void Particle::Polarise(const TVector3& axis, const Bool_t up)
{
   // -- Set Particle's spin state to be an eigenvector of 'axis', with eigenvalue defined by 'up'
   fSpin.Polarise(axis,up);
}

//_____________________________________________________________________________
void Particle::PrecessSpin(const TVector3& field, const Double_t precessTime)
{
   // -- Precess spin about field for time defined by precessTime 
   fSpin.Precess(field,precessTime);
   // Notify Observers of spin state change
   NotifyObservers(this->GetPoint(), this->GetVelocity(), Context::Spin);
}

//_____________________________________________________________________________
Bool_t Particle::IsSpinUp(const TVector3& axis) const
{
   // -- Return whether Particle is in Spin up state defined along provided axis
   return this->GetSpin().IsSpinUp(axis);
}

//_____________________________________________________________________________
Bool_t Particle::Reflect(const Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath)
{
   // -- Reflect particle
   #ifdef VERBOSE_MODE
      cout << "------------------- BOUNCE ----------------------" << endl;
      cout << "Boundary Node: " << navigator->GetCurrentNode()->GetName() << endl;
   #endif
   TGeoNode* boundaryNode = navigator->GetCurrentNode();
   TGeoMatrix* boundaryMatrix = navigator->GetCurrentMatrix();
   const Boundary* boundary = dynamic_cast<const Boundary*>(boundaryNode->GetVolume());
   //------------------------------------------------------
   // Reflect
   // -- Normal Vector
   // Check if the normal vector is actually pointing in the wrong direction  
   // (wrong means pointing along the direction of the track, rather than in the opposite direction)
   // This will actually be the case nearly all (if not all) of the time,
   // because of the way ROOT calculates the normal
   Double_t norm[3] = {normal[0], normal[1], normal[2]};
   Double_t dotProduct = this->Nx()*norm[0] + this->Ny()*norm[1] + this->Nz()*norm[2];
   // If so, reflect the normal to get the correct direction
   if (dotProduct > 0.) { for (Int_t i=0; i<3; i++) {norm[i] = -norm[i];} }
   // -- Calculate Probability of diffuse reflection
   Double_t diffuseProbability = this->DiffuseProbability(boundary, norm);
   // Determine Reflection Type 
   if (gRandom->Uniform(0.0,1.0) <= diffuseProbability) {
      // -- Diffuse Bounce
      this->DiffuseBounce(navigator, norm);
      this->NotifyObservers(this->GetPoint(), this->GetVelocity(), Context::DiffBounce);
   } else {
      // -- Specular Bounce
      this->SpecularBounce(norm);
      this->NotifyObservers(this->GetPoint(), this->GetVelocity(), Context::SpecBounce);
   }
   // Update Navigator
   navigator->SetCurrentDirection(this->Nx(), this->Ny(), this->Nz());
   //------------------------------------------------------
   // Change navigator state back to the initial node after bounce
   if (navigator->cd(initialPath) == kFALSE) {
      Error("MakeStep","Unable to cd to initial node after bounce!");
      throw runtime_error("Unable to cd to initial node");
   }
   // Attempt to locate particle within the current node
   Bool_t located = fState->LocateInGeometry(this, navigator, boundaryNode, boundaryMatrix, crossedNode);
   if (located == kFALSE) {
      #ifdef VERBOSE_MODE
         cout << "Error - After Bounce - Unable to locate particle correctly in Geometry"<< endl;
      #endif
      // Change state
      this->IsAnomalous();
      throw runtime_error("Unable to locate particle uniquely in correct volume");
   }
   #ifdef VERBOSE_MODE
      cout << "-------------------------------------------------" << endl;
      cout << "Final Node: " << navigator->GetCurrentNode()->GetName() << endl;
      cout << "-------------------------------------------------" << endl << endl;
   #endif
   // End of Bounce. We should have returned to the original node, 
   // and guarenteed that the current point is located within it.
   return kTRUE;
}

//_____________________________________________________________________________
void Particle::SpecularBounce(const Double_t* norm)
{
   #ifdef VERBOSE_MODE
      cout << "----------------------------" << endl;
      cout << "Specular Bounce" << endl;
      cout << "BEFORE - nx: " << this->Nx() <<"\t"<< "ny: " << this->Ny();
      cout << "\t" << "nz: " << this->Nz() << endl;
      cout << "normx: " << norm[0] <<"\t"<<"normy: "<< norm[1] <<"\t"<<"normz: "<< norm[2] << endl;
   #endif
   Double_t vel = this->V();
   Double_t dir[3] = {this->Nx(), this->Ny(), this->Nz()};
   Double_t dotProduct = dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2];	
   // Reflection Law for Specular Reflection
   for (Int_t i=0; i<3; i++) {dir[i] = dir[i] - 2.0*dotProduct*norm[i];}
   #ifdef VERBOSE_MODE
      cout << "AFTER - nx: " << dir[0] <<"\t"<< "ny: " << dir[1] <<"\t"<< "nz: " << dir[2] << endl;
   #endif
   // Update Particle direction   
   this->SetVelocity(vel*dir[0], vel*dir[1], vel*dir[2]);
}

//_____________________________________________________________________________
void Particle::DiffuseBounce(TGeoNavigator* navigator, const Double_t* norm)
{
   #ifdef VERBOSE_MODE
      cout << "----------------------------" << endl;
      cout << "Diffuse Bounce" << endl;
      cout << "BEFORE - nx: " << this->Nx() <<"\t"<< "ny: " << this->Ny();
      cout << "\t" << "nz: " << this->Ny() << endl;
      cout << "normx: "<< norm[0] <<"\t"<<"normy: "<< norm[1] <<"\t"<<"normz: "<< norm[2] << endl;
   #endif
   
   // First we need to pick random angles to choose the orientation of our diffuse direction vector. 
   // Correct method for UCN physics though is to weight these angles towards the poles by adding
   // an extra cos(theta). Derivation of how to pick these angles is in notes
   // Overall solid angle used is dOmega = cos(theta)sin(theta) dtheta dphi
   Double_t phi = gRandom->Uniform(0.0, 1.0)*2*TMath::Pi();
   // We do not want the full range of theta from 0 to pi/2 however.
   // An angle of pi/2 would imply moving off the boundary exactly parallel to
   // the current surface.Therefore we should restrict theta to a slightly smaller
   // proportion of angles - letting u be between 0 and 0.499, ensures theta
   // is between 0 and ~89 degrees. 
   Double_t u = gRandom->Uniform(0.0, 0.499);
   // We ignore the negative sqrt term when selecting theta,
   // since we are only interested in theta between 0 and pi/2 
   // (negative root provides the pi/2 to pi branch) 
   Double_t theta = TMath::ACos(TMath::Sqrt(1.0 - 2*u)); 
   
   // Calculate local normal vector	
   Double_t lnorm[3] = {0.,0.,0.};
   navigator->MasterToLocalVect(norm, lnorm);
   TVector3 localNorm(lnorm[0], lnorm[1], lnorm[2]);
   assert(TMath::Abs(localNorm.Mag() - 1.0) < TGeoShape::Tolerance());
   
   // ------------------------------------------------------------------
   // -- FIND A SINGLE ARBITRARY VECTOR PERPENDICULAR TO THE LOCAL NORMAL
   // Define a preferred direction in our coordinate system - usually the z-direction
   // - that we want to be perpendicular to the normal vector of our surface
   TVector3 upAxis(0.,0.,1.);
   
   // Here we check to make sure that the upAxis we chose is not parallel to the normal vector.
   // If it is, we try another one, x. 
   if (TMath::Abs(upAxis.Dot(localNorm)) > TGeoShape::Tolerance()) {
      upAxis.SetXYZ(1.,0.,0.);
      if (TMath::Abs(upAxis.Dot(localNorm)) > TGeoShape::Tolerance()) {
         // In the (hopefully) unlikely case that localNorm has components in the x and z
         // directions. This has been observed when including the bend geometry. This suggests
         // that in the local coordinate system in this instance, the z-axis is not along the
         // vertical as we usually assume. Thus including this check covers our backs. 
         upAxis.SetXYZ(0.,1.,0.);
         if (TMath::Abs(upAxis.Dot(localNorm)) > TGeoShape::Tolerance()) {
            cout << "Axis X: " << upAxis.X() << "\t" <<  "Y: " << upAxis.Y() << "\t";
            cout <<   "Z: " << upAxis.Z() << endl;
            cout << "LocalNorm X: " << localNorm.X() << "\t" <<  "Y: " << localNorm.Y() << "\t";
            cout <<   "Z: " << localNorm.Z() << endl;
            Error("DiffuseBounce","Could not find an axis perpendicular to normal.");
            throw runtime_error("Could not find an axis perpendicular to normal");
         }
      }
   }
   // Calculate the cross product of the 'up' vector with our normal vector to get a vector
   // guaranteed to be perpendicular to the normal. 
   // This is the vector we will want to rotate around initially. 
   TVector3 perpAxis = localNorm.Cross(upAxis); 
   assert(TMath::Abs(perpAxis.Mag() - 1.0) < TGeoShape::Tolerance());
   // ------------------------------------------------------------------
   
   // Rotate normal vector about perpendicular axis by angle theta. 
   // Using Rodrigues' formula derived in notes. 
   TVector3 rotatedNorm(0.,0.,0.);
   {
      // name term2 and term3 just refer to the terms as they appear in the Rodrigues' formula
      TVector3 term2 = localNorm - (localNorm.Dot(perpAxis))*perpAxis; 
      TVector3 term3 = perpAxis.Cross(localNorm);
      rotatedNorm = localNorm + (TMath::Cos(theta) - 1.0)*term2 + TMath::Sin(theta)*term3;
   }
   assert(TMath::Abs(rotatedNorm.Mag() - 1.0) < TGeoShape::Tolerance());
   
   // Rotate the newly rotated Normal vector, rotatedNorm, by angle phi, this time
   // rotating about the original normal vector, norm.
   TVector3 direction(0.,0.,0.);
   {
      TVector3 term3 = localNorm.Cross(rotatedNorm);
      TVector3 term2 = rotatedNorm - (rotatedNorm.Dot(localNorm))*localNorm;
      direction = rotatedNorm + (TMath::Cos(phi) - 1.0)*term2 + TMath::Sin(phi)*term3;
   }
   assert(TMath::Abs(direction.Mag() - 1.0) < TGeoShape::Tolerance()); 
   
   // Convert the direction vector back to the global frame
   Double_t ldir[3] = {direction.X(), direction.Y(), direction.Z()};
   Double_t dir[3];
   navigator->LocalToMasterVect(ldir, dir);
   
   assert(TMath::Abs(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2] - 1.0) < TGeoShape::Tolerance());
   // Assert that our final direction is not perpendicular to the normal.
   // This could result in escaping the boundary
   assert(TMath::Abs(dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2]) > TGeoShape::Tolerance()); 
   
   #ifdef VERBOSE_MODE	
      cout << "AFTER - nx: " << dir[0] <<"\t"<< "nx: " << dir[1] <<"\t"<< "nx: " << dir[2] << endl;
   #endif
   
   // Update Particle Direction
   this->SetVelocity(this->V()*dir[0], this->V()*dir[1], this->V()*dir[2]);
}

//______________________________________________________________________________
Double_t Particle::DiffuseProbability(const Boundary* boundary, const Double_t* normal) const
{
   // Calculate the probability of making a diffuse bounce
   Double_t cosTheta = TMath::Abs(this->Nx()*normal[0] + this->Ny()*normal[1] + this->Nz()*normal[2]);
   Double_t energyPerp = this->Energy()*cosTheta*cosTheta;
   Double_t diffProb = boundary->GetRoughness()*energyPerp/boundary->FermiPotential();
   assert(diffProb <= 1.0 && diffProb >= 0.0);
   //return diffProb;
   return diffProb;
}

//______________________________________________________________________________
void Particle::SaveRandomGeneratorState(const TRandom3State& rndState)
{
   fRndState = new TRandom3State(rndState);
}
