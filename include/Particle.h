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

class TGeoNode;
class TGeoMatrix;
class TGeoNavigator;

//class ParticleState;

class Particle : public TObject, public Observable
{
private:
   // -- Members
   Int_t       fId;        // Particle's number (assigned when its created to help keep track of it)
   Point       fPos;
   TVector3    fMom;
   Double_t    fE;         // kinetic energy
   Double_t    fDistance;  // Distance travelled
   
   Int_t       fRandomSeed;         // The seed of TRandom when the particle began to propagate
   
   // State
   friend class State;
   State      *fState;
   
   // Spin
   Spin         fSpin;
   
   // State Change
   void             ChangeState(State* state);
   
public:
   // -- Constructors
   Particle();
   Particle(Int_t id, Point& position, TVector3& mom, Double_t energy);
   Particle(const Particle &part);

   // -- Destructor
   virtual ~Particle();

   // -- Methods
   //_____________________________________________________________________________
   // Get/Setters
   Int_t                Id()     const {return fId;}
   Double_t             X()      const {return fPos.X();}
   Double_t             Y()      const {return fPos.Y();}
   Double_t             Z()      const {return fPos.Z();}
   Double_t             T()      const {return fPos.T();}
   Double_t             Px()     const {return fMom.X();}
   Double_t             Py()     const {return fMom.Y();}
   Double_t             Pz()     const {return fMom.Z();}
   Double_t             P()      const {return fMom.Mag();}
   Double_t             Energy() const {return fE;}
   Double_t             V()      const {return this->P()/Neutron::mass_eV_c;}
   Double_t             Vx()     const {return this->Px()/Neutron::mass_eV_c;}
   Double_t             Vy()     const {return this->Py()/Neutron::mass_eV_c;}
   Double_t             Vz()     const {return this->Pz()/Neutron::mass_eV_c;}
   Double_t             Nx()     const {return (this->P() != 0. ? this->Px()/this->P() : 0.);}
   Double_t             Ny()     const {return (this->P() != 0. ? this->Py()/this->P() : 0.);}
   Double_t             Nz()     const {return (this->P() != 0. ? this->Pz()/this->P() : 0.);}
   Double_t             Theta()  const {return fMom.Theta();}
   Double_t             Phi()    const {return fMom.Phi();}
   
   void                 SetId(const Int_t id) {fId = id;}
   void                 SetPosition(const Double_t x, const Double_t y, const Double_t z, 
                                       const Double_t t);
   void                 SetMomentum(const Double_t px, const Double_t py, const Double_t pz, 
                                       const Double_t energy);
   void                 Print(const Option_t* option="") const;
   
   // Random Seed Storage 
   // (Used for setting the random number generator to the exact point
   // when simulation was started)
   void                 SetRandomSeed(const Int_t seed)        {fRandomSeed = seed;}
   Int_t                GetRandomSeed() const                  {return fRandomSeed;}
   
   // Distance Counter
   Double_t             Distance() const                       {return fDistance;}
   inline void          IncreaseDistance(Double_t stepsize)    {fDistance += stepsize;}
   
   // State
   State*               GetState()     {return fState;}
   
   // Spin
   const Spin&          GetSpin() const {return fSpin;}
   void                 Polarise(const TVector3& axis, const Bool_t up);
   void                 PrecessSpin(const TVector3& field, const Double_t precessTime);
   Bool_t               IsSpinUp(const TVector3& axis) const;
   
   // -- Propagation
   Bool_t               Propagate(Run* run);
   Bool_t               LocateInGeometry(Particle* particle, TGeoNavigator* navigator,
                           const TGeoNode* initialNode, const TGeoMatrix* initialMatrix,
                           const TGeoNode* crossedNode);
   Bool_t               WillDecay(const Double_t timeInterval);

   void                 IsDetected();
   void                 IsLost();
   void                 IsAbsorbed();
   void                 IsAnomalous();
   
   // -- Observers
   virtual void         NotifyObservers(const TObject* subject, const std::string& context);
   
   // -- Output to file
   void                 SaveState(Run* run);
   void                 WriteToFile(TDirectory* particleDir);
   
   ClassDef(Particle,1)   // Ultra-Cold Neutron
};

#endif  /*PARTICLE_H*/
