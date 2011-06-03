// UCN Particle class
// Author: Matthew Raso-Barnett  19/01/2009
// Re-write: 19/02/2010

#ifndef PARTICLE_H
#define PARTICLE_H

#include <list>
#include <string>

#include "TObject.h"
#include "TVector3.h"
#include "TDirectory.h"

#include "Point.h"
#include "Spin.h"
#include "State.h"
#include "Observable.h"

#include "Constants.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Particle - Particle class designed for UCN physics               //
//    This class breaks from the ROOT model of Particle as an element      //
//    of a track class. This paradigm suits high energy physics where      //
//    particle decay and then they wish to follow the decay products.      //
//    For UCN, there is only beta decay, and for our interests that is     //
//    an end state of the particle, not something which we wish to track.  //
//    For this reason, as well as the convenience of having full control   //
//    of the class interface, we here create a UCNParticle and UCNTrack    //
//    class. This will also reduce unecessary downcasting down from a      //
//    TParticle.                                                           //                                                                     
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
class FieldManager;
class GravField;
class Run;
class Boundary;

class TGeoNode;
class TGeoMatrix;
class TGeoNavigator;
class TTree;

//class ParticleState;

class Particle : public TObject, public Observable
{
private:
   // -- Members
   unsigned int fId;        // Particle's number (assigned when its created to help keep track of it)
   Point       fPos;
   TVector3    fVel;
   
   unsigned int  fRandomSeed;  // The seed of TRandom when the particle began to propagate
   
   // State
   friend class State;
   State      *fState;
   
   // Spin
   Spin         fSpin;
   
   // State Change
   void     ChangeState(State* state);
   
   // Stepping
   
   // Reflection
   void     SpecularBounce(const Double_t* norm);
   void     DiffuseBounce(TGeoNavigator* navigator, const Double_t* norm);
   Double_t DiffuseProbability(const Boundary* boundary, const Double_t* normal) const;
   
public:
   // -- Constructors
   Particle();
   Particle(unsigned int id, Point& position, TVector3& vel);
   Particle(const Particle &part);

   // -- Destructor
   virtual ~Particle();

   // -- Methods
   //_____________________________________________________________________________
   // Get/Setters
   unsigned int         Id()     const {return fId;}
   Double_t             X()      const {return fPos.X();}
   Double_t             Y()      const {return fPos.Y();}
   Double_t             Z()      const {return fPos.Z();}
   Double_t             T()      const {return fPos.T();}
   Double_t             Vx()     const {return fVel.X();}
   Double_t             Vy()     const {return fVel.Y();}
   Double_t             Vz()     const {return fVel.Z();}
   Double_t             V()      const {return fVel.Mag();}
   Double_t             Energy() const {return TMath::Power(V(), 2.0)*(0.5*Neutron::mass_eV_c2);}
   Double_t             P()      const {return this->V()*Neutron::mass_eV_c;}
   Double_t             Px()     const {return this->Vx()*Neutron::mass_eV_c;}
   Double_t             Py()     const {return this->Vy()*Neutron::mass_eV_c;}
   Double_t             Pz()     const {return this->Vz()*Neutron::mass_eV_c;}
   Double_t             Nx()     const {return (this->V() != 0. ? this->Vx()/this->V() : 0.);}
   Double_t             Ny()     const {return (this->V() != 0. ? this->Vy()/this->V() : 0.);}
   Double_t             Nz()     const {return (this->V() != 0. ? this->Vz()/this->V() : 0.);}
   Double_t             Theta()  const {return fVel.Theta();}
   Double_t             Phi()    const {return fVel.Phi();}

   const Point&         GetPoint() const {return fPos;}
   const TVector3&      GetVelocity() const {return fVel;}
   
   void                 SetId(const unsigned int id) {fId = id;}
   void                 SetPosition(const Double_t x, const Double_t y, const Double_t z, 
                                       const Double_t t);
   void                 SetVelocity(const Double_t vx, const Double_t vy, const Double_t vz);
   void                 Print(const Option_t* option="") const;
   
   // Random Seed Storage 
   // (Used for setting the random number generator to the exact point
   // when simulation was started)
   void                 SetRandomSeed(const Int_t seed)        {fRandomSeed = seed;}
   unsigned int         GetRandomSeed() const {return fRandomSeed;}
   
   // -- State
   const State*         GetState()     {return fState;}
   
   // -- Spin
   const Spin&          GetSpin() const {return fSpin;}
   void                 Polarise(const TVector3& axis, const Bool_t up);
   void                 PrecessSpin(const TVector3& field, const Double_t precessTime);
   Bool_t               IsSpinUp(const TVector3& axis) const;
   
   // -- Propagation
   Bool_t               Propagate(Run* run);
   void                 Move(const Double_t stepTime, const Run* run);
   void                 UpdateCoordinates(const TGeoNavigator* navigator);
   Bool_t               Reflect(const Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath);
   
   Bool_t               WillDecay(const Double_t timeInterval);
   
   void                 IsDetected();
   void                 IsLost();
   void                 IsAbsorbed();
   void                 IsAnomalous();
   
   // -- Observers
   virtual void         NotifyObservers(const Point& point, const TVector3& velocity, const std::string& context);
   
   // -- Output to file
   void                 SaveState(Run* run);
   
   ClassDef(Particle,1)   // Ultra-Cold Neutron
};

#endif  /*PARTICLE_H*/
