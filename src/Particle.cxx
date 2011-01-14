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
              fId(0), fPos(), fMom(), fE(0.),
              fDistance(0.), fRandomSeed(0), fState(NULL), fSpin()
{
   // -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Particle","Default Constructor");
   #endif
}


//______________________________________________________________________________
Particle::Particle(Int_t id, Point& pos, TVector3& mom, Double_t energy)
             :TObject(), Observable(),
              fId(id), fPos(pos), fMom(mom), fE(energy),
              fDistance(0.), fRandomSeed(0), fSpin()
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
              fId(p.fId), fPos(p.fPos), fMom(p.fMom), fE(p.fE), fDistance(p.fDistance),
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
const Spin& Particle::GetSpin() const
{
   return fSpin;
}

//______________________________________________________________________________
void Particle::SetPosition(const Double_t x, const Double_t y, const Double_t z,
                                    const Double_t t)
{
   // Set current vertex to given coords
   fPos.SetPoint(x,y,z,t);
}

//______________________________________________________________________________
void Particle::SetMomentum(const Double_t px, const Double_t py, const Double_t pz,
                                    const Double_t energy)
{
   // Set current momentum to given coords
   fMom.SetXYZ(px,py,pz); fE = energy;
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
Bool_t Particle::LocateInGeometry(Particle* particle, TGeoNavigator* navigator, const TGeoNode* initialNode, const TGeoMatrix* initialMatrix, const TGeoNode* crossedNode)
{
   return fState->LocateInGeometry(particle, navigator, initialNode, initialMatrix, crossedNode);
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

