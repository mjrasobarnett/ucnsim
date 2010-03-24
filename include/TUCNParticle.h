// UCN Particle class
// Author: Matthew Raso-Barnett  19/01/2009
// Re-write: 19/02/2010

#include "TObject.h"

#ifndef TUCNPARTICLE_H
#define TUCNPARTICLE_H

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNParticle - Particle class designed for UCN physics               //
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
class TUCNGeoMaterial;
class TUCNFieldManager;
class TUCNGravField;
class TUCNRun;

class TGeoNode;
class TGeoNavigator;

//class TUCNParticleState;

class TUCNParticle : public TObject
{
private:
   // -- private members
   static const Double_t   fgMass = 939.56563e+6;  // Units of MeV
   static const Double_t   fgLifetime = 885.7;     // Units s
   
   Double_t    fX;         // x coord
   Double_t    fY;         // y coord
   Double_t    fZ;         // z coord
   Double_t    fT;         // Time travelled
   
   Double_t    fPx;        // x component of momentum
   Double_t    fPy;        // y component of momentum
   Double_t    fPz;        // z component of momentum
   Double_t    fE;         // kinetic energy

   Double_t    fDistance;  // Distance travelled

   Int_t       fId;        // Particle's number (assigned when its created to help keep track of it)
   
   Int_t       fBounces;            // Number of Bounces made from Wall
   Int_t       fSpecularBounces;    // Number of such bounces that are specular
   Int_t       fDiffuseBounces;     // Number of such bounces that are diffuse
   Int_t       fRandomSeed;  // The seed of TRandom when the particle began to propagate
   
//   Double_t    fAvgMagField;           // Average Magnetic field seen by the neutron as it travels
//   Int_t       fFieldPointsSampled;    // Number of times field has been sampled
   Bool_t      fDecayed;
   Bool_t      fLostToBoundary;
   Bool_t      fDetected;
   
   
   TGeoNode*   fUCNNextNode;
   Double_t    fStepTime;
   Bool_t      fUCNIsStepEntering;
   Bool_t      fUCNIsStepExiting;
   Bool_t      fUCNIsOutside;        //! flag that current point is outside geometry
   Bool_t      fUCNIsOnBoundary;     //! flag that current point is on some boundary
   
   // -- State
//   friend class TUCNParticleState;
//   TUCNParticleState    *fState;
   
   // -- Private Methods
   //_____________________________________________________________________________
   
   // State Change
//   void        ChangeState(TUCNParticleState* state);
   
   // Decay Probability
   Bool_t               IsLostToWall(const TUCNGeoMaterial* wall, const Double_t* normal) const;
   Double_t             DiffuseProbability(const Double_t diffuseCoeff, const Double_t* normal,
                                                   const Double_t fermiPotential) const;
   Bool_t               WillDecay(const Double_t timeInterval);

   // Propagation
   Bool_t      MakeStep(Double_t stepTime, TGeoNavigator* navigator, TUCNFieldManager* fieldManager);
   
   TGeoNode*   ParabolicBoundaryFinder(Double_t& stepTime, TGeoNavigator* navigator, TUCNGravField* field);
   TGeoNode*   ParabolicDaughterBoundaryFinder(TGeoNavigator* navigator, Double_t* point,
                  Double_t* velocity, Double_t* field, Int_t &idaughter, Bool_t compmatrix=kFALSE);
   
   Double_t    DetermineNextStepTime(const Double_t maxStepTime, const Double_t runTime=0.);   
   Double_t    GetStepTime() const {return fStepTime;}
   void        SetStepTime(Double_t stepTime); 
   
   
   Bool_t      Bounce(TGeoNavigator* navigator, const Double_t* normal, const TUCNGeoMaterial* wallMaterial);
   Bool_t      SpecularBounce(Double_t* dir, const Double_t* norm);
   Bool_t      DiffuseBounce(const TGeoNavigator* navigator, Double_t* dir, const Double_t* norm);
   void        Update(const TGeoNavigator* navigator, const Double_t timeInterval=0., const TUCNGravField* gravField=0);

   Bool_t      FindBoundaryNormal(Double_t* normal, TGeoNavigator* navigator);
   
   
   Bool_t      IsUCNOnBoundary() const {return fUCNIsOnBoundary;}
   Bool_t      IsUCNStepEntering() const {return fUCNIsStepEntering;}
   Bool_t      IsUCNStepExiting() const {return fUCNIsStepExiting;}
   Bool_t      IsUCNOutside() const {return fUCNIsOutside;}
   
public:
   // -- Constructors
   TUCNParticle();
   TUCNParticle(Int_t id, Double_t* pos, Double_t* mom, Double_t energy, Double_t t=0);
   TUCNParticle(const TUCNParticle &part);
   TUCNParticle& operator=(const TUCNParticle&);

   // -- Destructor
   virtual ~TUCNParticle();

   // -- Methods
   //_____________________________________________________________________________
   // Get/Setters
   Int_t                Id()     const {return fId;}
   Double_t             X()      const {return fX;}
   Double_t             Y()      const {return fY;}
   Double_t             Z()      const {return fZ;}
   Double_t             T()      const {return fT;}
   Double_t             Px()     const {return fPx;}
   Double_t             Py()     const {return fPy;}
   Double_t             Pz()     const {return fPz;}
   Double_t             P()      const;
   Double_t             Energy() const {return fE;}
   Double_t             V()      const {return this->P()/this->Mass_eV_c();}
   Double_t             Vx()     const {return this->Px()/this->Mass_eV_c();}
   Double_t             Vy()     const {return this->Py()/this->Mass_eV_c();}
   Double_t             Vz()     const {return this->Pz()/this->Mass_eV_c();}
   Double_t             Nx()     const {return (this->P() != 0. ? this->Px()/this->P() : 0.);}
   Double_t             Ny()     const {return (this->P() != 0. ? this->Py()/this->P() : 0.);}
   Double_t             Nz()     const {return (this->P() != 0. ? this->Pz()/this->P() : 0.);}
   Double_t             Mass_Kg()      const;
   Double_t             Mass_eV()      const {return fgMass;}
   Double_t             Mass_eV_c()    const;
   Double_t             Mass_eV_c2()   const;
   Double_t             Rho()    const;
   Double_t             Theta()  const;
   Double_t             Phi()    const;
   Double_t             Lifetime()     const {return fgLifetime;} 
   
   void                 SetVertex(const Double_t x, const Double_t y, const Double_t z, 
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
   
   // Bounce Counters
   void                 MadeBounce()                           {fBounces++;}
   void                 MadeSpecularBounce()                   {fSpecularBounces++;}
   void                 MadeDiffuseBounce()                    {fDiffuseBounces++;}
   Int_t                Bounces()                              {return fBounces;}
   Int_t                SpecularBounces()                      {return fSpecularBounces;}
   Int_t                DiffuseBounces()                       {return fDiffuseBounces;}
   
   // -- Mag Field Tracking
//   void                 SampleMagField(const Double_t integratedField, const Double_t stepTime);
//   Double_t             AvgMagField() const {return fAvgMagField;}

   Bool_t   LostToBoundary()  {return fLostToBoundary;}
   void     LostToBoundary(Bool_t lost)   {fLostToBoundary = lost;}
   Bool_t   Detected()  {return fDetected;}
   void     Detected(Bool_t detected)   {fDetected = detected;}
   Bool_t   Decayed()  {return fDecayed;}
   void     Decayed(Bool_t decayed)   {fDecayed = decayed;}
   
   // -- Propagation
   Bool_t      Propagate(TUCNRun* run, TGeoNavigator* navigator, TUCNFieldManager* fieldManager);
   

   ClassDef(TUCNParticle,1)   // Ultra-Cold Neutron
};



/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNParticleState - Following the State design pattern, a Particle   //
//    state handles state specific behaviour, such as when a UCN is        //
//    detected, decays, or lost to the boundary in some way. The state     //
//    pattern allows us to code specific behaviour to these states rather  //
//    than relying on Boolean flags that must be checked constantly.       //                                             
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
/*
class TUCNState : public TNamed
{
public:
   
   
protected:
   
   
   ClassDef(TUCNState,1)
};


/////////////////////////////////////////////////////////////////////////////

class TUCNPropagating : public TUCNState
{
   
   ClassDef(TUCNPropagating,1)
};


/////////////////////////////////////////////////////////////////////////////

class TUCNDecayed : public TUCNState
{
   
   ClassDef(TUCNDecayed,1)
};

/////////////////////////////////////////////////////////////////////////////

class TUCNAbsorbed : public TUCNState
{
   
   ClassDef(TUCNAbsorbed,1)
};

/////////////////////////////////////////////////////////////////////////////

class TUCNLost : public TUCNState
{
   
   ClassDef(TUCNLost,1)
};
*/
#endif  /*TUCNPARTICLE_H*/
