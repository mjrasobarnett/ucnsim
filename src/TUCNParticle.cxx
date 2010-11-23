// UCN class
// Author: Matthew Raso-Barnett  19/01/2009
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Constants.h"
#include "Units.h"

#include "TUCNParticle.h"
#include "TUCNRun.h"

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

ClassImp(TUCNParticle)

const Double_t    TUCNParticle::fgMass;      // Neutron Mass
const Double_t    TUCNParticle::fgLifetime;   // Neutron Lifetime

//______________________________________________________________________________
TUCNParticle::TUCNParticle()
             :TObject(),
              fX(0.), fY(0.), fZ(0.), fT(0.), fPx(0.), fPy(0.), fPz(0.), fE(0.),
              fDistance(0.), fId(0), fBounces(0), fSpecularBounces(0), fDiffuseBounces(0),
              fRandomSeed(0), fState(NULL), fSpin()
{
   // -- Default constructor
   Info("TUCNParticle","Default Constructor");
}


//______________________________________________________________________________
TUCNParticle::TUCNParticle(Int_t id, Double_t* pos, Double_t* mom, Double_t energy, Double_t t)
             :TObject(),
              fX(pos[0]), fY(pos[1]), fZ(pos[2]), fT(t), fPx(mom[0]), fPy(mom[1]), fPz(mom[2]),
              fE(energy), fDistance(0.), fId(id), fBounces(0), fSpecularBounces(0),
              fDiffuseBounces(0), fRandomSeed(0), fSpin()
{
   // -- Constructor
   Info("TUCNParticle","Constructor");
   fState = new TUCNPropagating();
}

//_____________________________________________________________________________
TUCNParticle::TUCNParticle(const TUCNParticle& p)
             :TObject(p),
              fX(p.fX), fY(p.fY), fZ(p.fZ), fT(p.fT), fPx(p.fPx), fPy(p.fPy),
              fPz(p.fPz), fE(p.fE), fDistance(p.fDistance), fId(p.fId),
              fBounces(p.fBounces), fSpecularBounces(p.fSpecularBounces),
              fDiffuseBounces(p.fDiffuseBounces), fRandomSeed(p.fRandomSeed),
              fState(p.fState), fSpin(p.fSpin), fObservers(p.fObservers)
{
   // -- Copy Constructor
   Info("TUCNParticle","Copy Constructor");
}

//_____________________________________________________________________________
TUCNParticle& TUCNParticle::operator=(const TUCNParticle& p)
{
   // -- assignment operator
   Info("TUCNParticle","Assignment");
   if(this!=&p) {
      TObject::operator=(p);
      fX = p.fX;
      fY = p.fY;
      fZ = p.fZ;
      fT = p.fT;
      fPx = p.fPx;
      fPy = p.fPy;
      fPz = p.fPz;
      fE = p.fE;
      fDistance = p.fDistance;
      fId = p.fId;
      fBounces = p.fBounces;
      fSpecularBounces = p.fSpecularBounces;
      fDiffuseBounces = p.fDiffuseBounces;
      fRandomSeed = p.fRandomSeed;
      if (fState) delete fState; fState = NULL;
      fState = p.fState;
      fSpin = p.fSpin;
      fObservers = p.fObservers;
   }
   return *this;
}

//______________________________________________________________________________
TUCNParticle::~TUCNParticle()
{ 
   // -- Destructor
   Info("TUCNParticle","Destructor");
   if (fState) delete fState;
}

//______________________________________________________________________________
void TUCNParticle::ChangeState(TUCNState* state)
{
   // -- Store pointer to state
   fState = state;
}

//______________________________________________________________________________
Bool_t TUCNParticle::SaveState(TUCNRun* run)
{
   // Ask States to register themselves in the run to keep track of numbers
   if (fState) return fState->SaveState(run, this);
   return kFALSE;
}

//______________________________________________________________________________
Double_t TUCNParticle::P() const
{
   return TMath::Sqrt(fPx*fPx+fPy*fPy+fPz*fPz);
}

//______________________________________________________________________________
Double_t TUCNParticle::Mass_Kg() const
{
   return this->Mass_eV_c2()*Units::e_SI;
}

//______________________________________________________________________________
Double_t TUCNParticle::Mass_eV_c() const
{
   return this->Mass_eV()/Constants::c_light;
}

//______________________________________________________________________________
Double_t TUCNParticle::Mass_eV_c2() const
{
   return this->Mass_eV()/Constants::c_squared;
}

//______________________________________________________________________________
Double_t TUCNParticle::Rho() const
{
   return TMath::Sqrt(fX*fX+fY*fY+fZ*fZ);
}

//______________________________________________________________________________
Double_t TUCNParticle::Theta() const
{
   return (fPz==0)?TMath::PiOver2():TMath::ACos(fPz/P());
}

//______________________________________________________________________________
Double_t TUCNParticle::Phi() const
{
   return TMath::Pi()+TMath::ATan2(-fPy,-fPx);
}

//______________________________________________________________________________
const TUCNSpin& TUCNParticle::GetSpin() const
{
   return fSpin;
}

//______________________________________________________________________________
void TUCNParticle::SetVertex(const Double_t x, const Double_t y, const Double_t z,
                                    const Double_t t)
{
   // Set current vertex to given coords
   fX = x; fY = y; fZ = z; fT = t;
}

//______________________________________________________________________________
void TUCNParticle::SetMomentum(const Double_t px, const Double_t py, const Double_t pz,
                                    const Double_t energy)
{
   // Set current momentum to given coords
   fPx = px; fPy = py; fPz = pz; fE = energy;
}

//_____________________________________________________________________________
void TUCNParticle::Print(const Option_t* /*option*/) const
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
Bool_t TUCNParticle::Propagate(TUCNRun* run)
{
   // -- Call State-dependent propagate method
   if (!fState) fState = new TUCNPropagating();
   return fState->Propagate(this,run,run->Navigator(),run->FieldManager());
}

//_____________________________________________________________________________
Bool_t TUCNParticle::LocateInGeometry(TUCNParticle* particle, TGeoNavigator* navigator, const TGeoNode* initialNode, const TGeoMatrix* initialMatrix, const TGeoNode* crossedNode)
{
   return fState->LocateInGeometry(particle, navigator, initialNode, initialMatrix, crossedNode);
}

//______________________________________________________________________________
Bool_t TUCNParticle::WillDecay(const Double_t timeInterval)
{
   // Calculate probability particle will decay within timeInterval, and then roll the dice!
   Double_t probDecay = (timeInterval/this->Lifetime());
   if (gRandom->Uniform(0.0, 1.0) < probDecay) { return kTRUE; }
   return kFALSE;
}

//_____________________________________________________________________________
void TUCNParticle::Detected()
{
   fState->Detected(this);
}

//_____________________________________________________________________________
void TUCNParticle::Lost()
{
   fState->Lost(this);
}

//_____________________________________________________________________________
void TUCNParticle::Absorbed()
{
   fState->Absorbed(this);
}

//_____________________________________________________________________________
void TUCNParticle::Bad()
{
   fState->Bad(this);
}

//_____________________________________________________________________________
void TUCNParticle::Polarise(const TVector3& axis, const Bool_t up)
{
   // -- Set Particle's spin state to be an eigenvector of 'axis', with eigenvalue defined by 'up'
   fSpin.Polarise(axis,up);
}

//_____________________________________________________________________________
void TUCNParticle::PrecessSpin(const TVector3& field, const Double_t precessTime)
{
   // -- Precess spin about field for time defined by precessTime 
   fSpin.Precess(field,precessTime);
   // Notify Observers of spin state change
   this->NotifyObservers();
}

//_____________________________________________________________________________
Bool_t TUCNParticle::IsSpinUp(const TVector3& axis) const
{
   // -- Return whether Particle is in Spin up state defined along provided axis
   return this->GetSpin().IsSpinUp(axis);
}

//_____________________________________________________________________________
void TUCNParticle::Attach(TUCNObserver* observer)
{
   // -- Add an observer to the particle's list of observers
   fObservers.push_back(observer);
}

//_____________________________________________________________________________
void TUCNParticle::Detach(TUCNObserver* observer)
{
   // -- Remove an observer from the particle's list of observers
   fObservers.remove(observer);
}


//_____________________________________________________________________________
void TUCNParticle::NotifyObservers(/*const TUCNInterest& interest*/)
{
   // -- Notify Observers of change
   list<TUCNObserver*>::iterator listIter;
   for(listIter = fObservers.begin(); listIter != fObservers.end(); listIter++) {
      (*listIter)->RecordEvent(*this);
   }
}


