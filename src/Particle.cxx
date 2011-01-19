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
      // Update particle
      this->SetPosition(pos[0],pos[1],pos[2],this->T()+interval);
      this->SetVelocity(vel[0],vel[1],vel[2]);
      // Get magfield at this position and precess spin
      const MagField* magfield = run->GetFieldManager()->GetMagField(pos);
      if (magfield != NULL) {this->PrecessSpin(magfield->GetField(pos), interval);}
      #ifdef VERBOSE_MODE
         cout << "-------------------------------------------" << endl;
         cout << "Move -- Final X: " << this->X() << "\t" << "Y: " << this->Y();
         cout << "\t" <<  "Z: " << this->Z() << "\t" <<  "T: " << this->T() << endl;
         cout << "Move -- Final Vx: " << this->Vx() << "\t" << "Vy: " << this->Py();
         cout << "\t" <<  "Vz: " << this->Pz() << "\t";
         cout <<  "E: " << this->Energy()/Units::neV << endl;
      #endif
   }
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
Bool_t Particle::LocateInGeometry(TGeoNavigator* navigator,
                        const TGeoNode* initialNode, const TGeoMatrix* initialMatrix,
                        const TGeoNode* crossedNode)
{
   return fState->LocateInGeometry(this,navigator,initialNode,initialMatrix,crossedNode);
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

