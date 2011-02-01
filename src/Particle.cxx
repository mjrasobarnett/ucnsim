// UCN class
// Author: Matthew Raso-Barnett  19/01/2009
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Constants.h"
#include "Units.h"

#include "Particle.h"
#include "Run.h"
#include "Observer.h"
#include "Volume.h"
#include "Clock.h"

#include "TMath.h"
#include "TRandom.h"
#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoMatrix.h"
#include "TGeoVoxelFinder.h"

using namespace std;

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

ClassImp(Particle)

//______________________________________________________________________________
Particle::Particle()
             :TObject(), Observable(),
              fId(0), fPos(), fVel(), fE(0.),
              fRandomSeed(0), fState(NULL), fSpin()
{
   // -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Particle","Default Constructor");
   #endif
}


//______________________________________________________________________________
Particle::Particle(Int_t id, Point& pos, TVector3& mom, Double_t energy)
             :TObject(), Observable(),
              fId(id), fPos(pos), fVel(mom), fE(energy),
              fRandomSeed(0), fSpin()
{
   // -- Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Particle","Constructor");
   #endif
   fState = new Propagating();
}

//_____________________________________________________________________________
Particle::Particle(const Particle& p)
             :TObject(p), Observable(p),
              fId(p.fId), fPos(p.fPos), fVel(p.fVel), fE(p.fE),
              fRandomSeed(p.fRandomSeed), fState(p.fState), fSpin(p.fSpin)
{
   // -- Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Particle","Copy Constructor");
   #endif
}

//______________________________________________________________________________
Particle::~Particle()
{ 
   // -- Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Particle","Destructor");
   #endif
   if (fState) delete fState;
}

//______________________________________________________________________________
void Particle::ChangeState(State* state)
{
   // -- Store pointer to state
   fState = state;
}

//______________________________________________________________________________
void Particle::SaveState(Run* run)
{
   // Ask States to register themselves in the run to keep track of numbers
   fState->SaveState(run, this);
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
   fE = TMath::Power(this->P(), 2.0) / (2.0*Neutron::mass_eV);
}

//_____________________________________________________________________________
void Particle::Print(const Option_t* /*option*/) const
{
   cout << "Particle: " << this->Id() << "\t" << "Time Elapsed: " << this->T() << endl;
   cout << "Vertex (m): " << "X:" << this->X() << "\t" << "Y:" << this->Y();
   cout << "\t" << "Z:" << this->Z() << endl;
   cout << "Dir: " << "Nx:" << this->Nx()  << "\t" << "Ny:" << this->Ny();
   cout << "\t" << "Nz:" << this->Nz() << endl;
   cout << "Vel (m/s): " << "Vx:" << this->Vx()  << "\t" << "Vy:" << this->Vy();
   cout << "\t" << "Vz:" << this->Vz() << "\t" << "V:" << this->V() << endl;
   cout << "Mom (eV): " << "X:" << this->Px()  << "\t" << "Y:" << this->Py();
   cout << "\t" << "Z:" << this->Pz() << "\t" << "P:" << this->P() << endl;
   cout << "Energy (neV): " << this->Energy() / Units::neV << endl;
}

//_____________________________________________________________________________
Bool_t Particle::Propagate(Run* run)
{
   // -- Call State-dependent propagate method
   if (!fState) fState = new Propagating();
   return fState->Propagate(this,run);
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
   // Fetch the Gravitational Field if it exists, and store field's components
   const GravField* const gravity = run->GetFieldManager()->GetGravField();
   TVector3 gravField(0.,0.,0.);
   if (gravity != NULL) {
      gravField.SetXYZ(gravity->Gx(), gravity->Gy(), gravity->Gz());
   }
   // Make multiple small steps, of size 'interval', until we have made
   // one step of size 'stepTime'
   const Double_t end = this->T() + stepTime;
   while (this->T() < end) {
      #ifdef VERBOSE_MODE
         cout << "-------------------------------------------" << endl;
         cout << "Move -- Interval: " << interval << "\t" << "StepTime: " << stepTime << endl;
         cout << "Move -- Initial X: " << this->X() << "\t" << "Y: " << this->Y();
         cout << "\t" <<  "Z: " << this->Z() << "\t" <<  "T: " << this->T() << endl;
         cout << "Move -- Initial Vx: " << this->Vx() << "\t" << "Vy: " << this->Py();
         cout << "\t" <<  "Vz: " << this->Pz() << "\t";
         cout <<  "E: " << this->Energy()/Units::neV << endl;
      #endif
      // Check if we will reach the end of stepTime within the next small step
      if (this->T() + interval > end) {interval = end - this->T();} 
      // Get current positions
      TVector3 pos(this->X(), this->Y(), this->Z());
      TVector3 vel(this->Vx(), this->Vy(), this->Vz());
      // Calculate next coordinates
      for (int i=0;i<3;i++) {
         pos[i] += vel[i]*interval + 0.5*gravField[i]*interval*interval;
         vel[i] += gravField[i]*interval;
      }
      // Update Clock
      Clock::Instance()->Tick(interval);
      // Update Particle
      this->SetPosition(pos[0],pos[1],pos[2],this->T()+interval);
      this->SetVelocity(vel[0],vel[1],vel[2]);
      // Get magfield at this position and precess spin
      this->PrecessSpin(run->GetFieldManager()->GetMagField(pos), interval);
      #ifdef VERBOSE_MODE
         cout << "-------------------------------------------" << endl;
         cout << "Move -- Final X: " << this->X() << "\t" << "Y: " << this->Y();
         cout << "\t" <<  "Z: " << this->Z() << "\t" <<  "T: " << this->T() << endl;
         cout << "Move -- Final Vx: " << this->Vx() << "\t" << "Vy: " << this->Py();
         cout << "\t" <<  "Vz: " << this->Pz() << "\t";
         cout <<  "E: " << this->Energy()/Units::neV << endl;
      #endif
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Notify Observers of Step Completion
   this->NotifyObservers(this, Context::Step);
}

//_____________________________________________________________________________
void Particle::UpdateCoordinates(const TGeoNavigator* navigator)
{
   // Take the Navigator's internal state (position, direction) and set particle's to this
   #ifdef VERBOSE_MODE
      cout << "-------------------------------------------" << endl;
      cout << "Update -- Initial X: " << this->X() << "\t" << "Y: " << this->Y();
      cout << "\t" <<  "Z: " << this->Z() << "\t" <<  "T: " << this->T() << endl;
      cout << "Update -- Initial Vx: " << this->Vx() << "\t" << "Vy: " << this->Py();
      cout << "\t" <<  "Vz: " << this->Pz() << "\t";
      cout <<  "E: " << this->Energy()/Units::neV << endl;
   #endif
   const Double_t* pos = navigator->GetCurrentPoint();
   const Double_t* dir = navigator->GetCurrentDirection();
   const Double_t  vel = this->V();
   // Update particle
   this->SetPosition(pos[0], pos[1], pos[2], this->T());
   this->SetVelocity(vel*dir[0], vel*dir[1], vel*dir[2]);
   #ifdef VERBOSE_MODE
      cout << "Update -- Final X: " << this->X() << "\t" << "Y: " << this->Y();
      cout << "\t" <<  "Z: " << this->Z() << "\t" <<  "T: " << this->T() << endl;
      cout << "Update -- Final Vx: " << this->Vx() << "\t" << "Vy: " << this->Py();
      cout << "\t" <<  "Vz: " << this->Pz() << "\t";
      cout <<  "E: " << this->Energy()/Units::neV << endl;
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
   NotifyObservers(this, Context::Spin);
   // Notify Observers of current MagField
   FieldVertex fieldvertex(this->GetPosition(), field);
   NotifyObservers(&fieldvertex, Context::MeasureField);
}

//_____________________________________________________________________________
Bool_t Particle::IsSpinUp(const TVector3& axis) const
{
   // -- Return whether Particle is in Spin up state defined along provided axis
   return this->GetSpin().IsSpinUp(axis);
}

//_____________________________________________________________________________
void Particle::NotifyObservers(const TObject* subject, const std::string& context)
{
   // -- Notify Observers of change
   for(int i = 0; i < this->CountObservers(); i++) {
      Observer* observer = this->GetObserver(i);
      observer->RecordEvent(subject, context);
   }
}

//_____________________________________________________________________________
void Particle::WriteToFile(TDirectory* particleDir)
{
   // -- Write particle to given directory. Also tell the observers to write out the particle's
   // -- observables to the same directory.
   particleDir->cd();
   // First write out the observers to file
   this->WriteObserversToFile(particleDir);
   // Delete Observers
   this->DetachAll();
   assert(this->CountObservers() == 0);
   // Next write particle to file
   this->Write("Particle",TObject::kOverwrite);
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
      this->NotifyObservers(this, Context::DiffBounce);
   } else {
      // -- Specular Bounce
      this->SpecularBounce(norm);
      this->NotifyObservers(this, Context::SpecBounce);
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


