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
class TUCNState;
class TUCNGeoMaterial;
class TUCNFieldManager;
class TUCNGravField;
class TUCNRun;

class TGeoNode;
class TGeoMatrix;
class TGeoNavigator;

//class TUCNParticleState;

class TUCNParticle : public TObject
{
protected:
   // -- Members
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
   
   Int_t       fRandomSeed;         // The seed of TRandom when the particle began to propagate
   
//   Double_t    fAvgMagField;           // Average Magnetic field seen by the neutron as it travels
//   Int_t       fFieldPointsSampled;    // Number of times field has been sampled
   
   // -- State
   friend class TUCNState;
   TUCNState      *fState;
   
   // -- Methods
   //_____________________________________________________________________________
   // State Change
   void        ChangeState(TUCNState* state);
  
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
   
   // State
   TUCNState*           GetState()     {return fState;}
   Bool_t               RegisterState(TUCNRun* run);
   
   // -- Propagation
   Bool_t      Propagate(TUCNRun* run, TGeoNavigator* navigator, TUCNFieldManager* fieldManager);
   
   ClassDef(TUCNParticle,1)   // Ultra-Cold Neutron
};



/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNState - Following the State design pattern, a Particle           //
//    state handles state specific behaviour, such as when a UCN is        //
//    detected, decays, or lost to the boundary in some way. The state     //
//    pattern allows us to code specific behaviour to these states rather  //
//    than relying on Boolean flags that must be checked constantly.       //                                             
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNState : public TObject
{
protected:
   void  ChangeState(TUCNParticle* particle, TUCNState* state);
   
public:
   // -- Constructors
   TUCNState();
   TUCNState(const TUCNState&);
   TUCNState& operator=(const TUCNState&);
   virtual ~TUCNState();
   
   // -- Propagation
   virtual Bool_t    Propagate(TUCNParticle* particle, TUCNRun* run,
                                    TGeoNavigator* navigator, TUCNFieldManager* fieldManager);
   virtual void      UpdateParticle(TUCNParticle* particle, const TGeoNavigator* navigator,
                        const Double_t timeInterval=0., const TUCNGravField* gravField=0);
   virtual Bool_t    RegisterState(TUCNRun* run) = 0;
   
   ClassDef(TUCNState,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNPropagating -                                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNPropagating : public TUCNState
{
protected:
   Bool_t      fIsStepEntering;
   Bool_t      fIsStepExiting;
   Bool_t      fIsOnBoundary;     //! flag that current point is on some boundary
   
   // Step Time calculation
   virtual Double_t  DetermineNextStepTime(TUCNParticle* particle, const Double_t maxStepTime,
                                          const Double_t runTime=0.);
   // Propagation
   virtual Bool_t    MakeStep(Double_t stepTime, TUCNParticle* particle, TGeoNavigator* navigator,
                                          TUCNFieldManager* fieldManager);
   // Boundary Finding
   virtual TGeoNode* ParabolicBoundaryFinder(Double_t& stepTime, TUCNParticle* particle,
                                          TGeoNavigator* navigator, TGeoNode* crossedNode,
                                          TUCNGravField* field);
   virtual TGeoNode* ParabolicDaughterBoundaryFinder(Double_t& stepTime, TGeoNavigator* navigator,
                                    Double_t* point, Double_t* velocity, Double_t* field,
                                    Int_t &idaughter, Bool_t compmatrix=kFALSE);
   
   // Error checking when moving between volumes
   virtual Bool_t    LocateInGeometry(TUCNParticle* particle, TGeoNavigator* navigator,
                           const TGeoNode* initialNode, const TGeoMatrix* initialMatrix,
                           const TGeoNode* crossedNode);
   virtual Bool_t    AttemptRelocationIntoCurrentNode(TGeoNavigator* navigator, 
                           const TGeoNode* initialNode, const TGeoMatrix* initialMatrix,
                           const TGeoNode* crossedNode);
   
   // Wall reflection
   virtual Bool_t    Bounce(TUCNParticle* particle, TGeoNavigator* navigator, 
                              const Double_t* normal, const TUCNGeoMaterial* wallMaterial);
   virtual Bool_t    SpecularBounce(Double_t* dir, const Double_t* norm);
   virtual Bool_t    DiffuseBounce(const TGeoNavigator* navigator, Double_t* dir, 
                                    const Double_t* norm);
   virtual Bool_t    FindBoundaryNormal(Double_t* normal, TGeoNavigator* navigator,
                                    const TGeoNode* crossedNode);
   virtual Double_t  DiffuseProbability(const Double_t diffuseCoeff, const Double_t* normal,
                                                   const Double_t fermiPotential) const;
   
   // Decay/Loss Probability calculations
   virtual Bool_t    IsLostToWall(const TUCNParticle* particle, const TUCNGeoMaterial* wall,
                                          const Double_t* normal) const;
   virtual Bool_t    WillDecay(const Double_t timeInterval);
   
public:
   // -- Constructors
   TUCNPropagating();
   TUCNPropagating(const TUCNPropagating&);
   TUCNPropagating& operator=(const TUCNPropagating&);
   virtual ~TUCNPropagating();
   
   // -- Propagation
   virtual Bool_t    Propagate(TUCNParticle* particle, TUCNRun* run,
                                    TGeoNavigator* navigator, TUCNFieldManager* fieldManager);
   virtual Bool_t    RegisterState(TUCNRun* run);
   
   
   
   ClassDef(TUCNPropagating,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNDecayed -                                                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNDecayed : public TUCNState
{   
public:
   // -- Constructors
   TUCNDecayed();
   TUCNDecayed(const TUCNDecayed&);
   TUCNDecayed& operator=(const TUCNDecayed&);
   virtual ~TUCNDecayed();
   
   // -- Propagation
   virtual Bool_t    RegisterState(TUCNRun* run);
   
   ClassDef(TUCNDecayed,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNAbsorbed -                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNAbsorbed : public TUCNState
{
public:
   // -- Constructors
   TUCNAbsorbed();
   TUCNAbsorbed(const TUCNAbsorbed&);
   TUCNAbsorbed& operator=(const TUCNAbsorbed&);
   virtual ~TUCNAbsorbed();
   
   // -- Propagation
   virtual Bool_t    RegisterState(TUCNRun* run);
   
   ClassDef(TUCNAbsorbed,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNDetected -                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNDetected : public TUCNState
{
public:
   // -- Constructors
   TUCNDetected();
   TUCNDetected(const TUCNDetected&);
   TUCNDetected& operator=(const TUCNDetected&);
   virtual ~TUCNDetected();
   
   // -- Propagation
   virtual Bool_t    RegisterState(TUCNRun* run);
   
   ClassDef(TUCNDetected,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNLost -                                                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNLost : public TUCNState
{
public:
   // -- Constructors
   TUCNLost();
   TUCNLost(const TUCNLost&);
   TUCNLost& operator=(const TUCNLost&);
   virtual ~TUCNLost();
   
   // -- Propagation
   virtual Bool_t    RegisterState(TUCNRun* run);
   
   ClassDef(TUCNLost,1)
};

#endif  /*TUCNPARTICLE_H*/
