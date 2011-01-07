// UCN Particle State class
// Author: Matthew Raso-Barnett  
// 19/02/2010

#ifndef STATE_H
#define STATE_H

#include "TObject.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    State - Following the State design pattern, a Particle           //
//    state handles state specific behaviour, such as when a UCN is        //
//    detected, decays, or lost to the boundary in some way. The state     //
//    pattern allows us to code specific behaviour to these states rather  //
//    than relying on Boolean flags that must be checked constantly.       //                                             
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
class Particle;
class FieldManager;
class GravField;
class Run;
class TGeoNode;
class TGeoMatrix;
class TGeoNavigator;

class State : public TObject
{
protected:
   void  ChangeState(Particle* particle, State* state);
   
public:
   // -- Constructors
   State();
   State(const State&);
   State& operator=(const State&);
   virtual ~State();
   
   // -- Propagation
   virtual Bool_t    Propagate(Particle* particle, Run* run);
   virtual Bool_t    LocateInGeometry(Particle* particle, TGeoNavigator* navigator,
                           const TGeoNode* initialNode, const TGeoMatrix* initialMatrix,
                           const TGeoNode* crossedNode);
   virtual void      UpdateParticle(Particle* particle, const TGeoNavigator* navigator,
                        const Double_t timeInterval=0., const GravField* gravField=0);
   virtual Bool_t    SaveState(Run* run, Particle* particle) = 0;
   virtual void      IsDetected(Particle* particle);
   virtual void      IsDecayed(Particle* particle);
   virtual void      IsLost(Particle* particle);
   virtual void      IsAbsorbed(Particle* particle);
   virtual void      IsAnomalous(Particle* particle);
   
   ClassDef(State,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Propagating -                                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class Propagating : public State
{
protected:
   Bool_t      fIsStepEntering;
   Bool_t      fIsStepExiting;
   Bool_t      fIsOnBoundary;     //! flag that current point is on some boundary
   
   // Step Time calculation
   virtual Double_t  DetermineNextStepTime(Particle* particle, const Double_t maxStepTime,
                                          const Double_t runTime=0.);
   // Propagation
   virtual Bool_t    MakeStep(Double_t stepTime, Particle* particle, Run* run);
   
   // Boundary Finding
   virtual TGeoNode* ParabolicBoundaryFinder(Double_t& stepTime, Particle* particle,
                                          TGeoNavigator* navigator, TGeoNode* crossedNode,
                                          const GravField* const field);
   virtual TGeoNode* ParabolicDaughterBoundaryFinder(Double_t& stepTime, TGeoNavigator* navigator,
                                    Double_t* point, Double_t* velocity, Double_t* field,
                                    Int_t &idaughter, Bool_t compmatrix=kFALSE);
   
   // Error checking when moving between volumes
   virtual Bool_t    AttemptRelocationIntoCurrentNode(TGeoNavigator* navigator, 
                           const TGeoNode* initialNode, const TGeoMatrix* initialMatrix,
                           const TGeoNode* crossedNode);
   
   // Wall reflection
   virtual Bool_t    FindBoundaryNormal(Double_t* normal, TGeoNavigator* navigator,
                                    const TGeoNode* crossedNode);

   virtual Bool_t    WillDecay(const Double_t timeInterval);
   
public:
   // -- Constructors
   Propagating();
   Propagating(const Propagating&);
   Propagating& operator=(const Propagating&);
   virtual ~Propagating();
   
   // -- Propagation
   virtual Bool_t    Propagate(Particle* particle, Run* run);
   virtual Bool_t    LocateInGeometry(Particle* particle, TGeoNavigator* navigator,
                           const TGeoNode* initialNode, const TGeoMatrix* initialMatrix,
                           const TGeoNode* crossedNode);
   
   virtual Bool_t    SaveState(Run* run, Particle* particle);
   virtual void      IsDetected(Particle* particle);
   virtual void      IsDecayed(Particle* particle);
   virtual void      IsLost(Particle* particle);
   virtual void      IsAbsorbed(Particle* particle);
   virtual void      IsAnomalous(Particle* particle);
   
   ClassDef(Propagating,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Decayed -                                                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class Decayed : public State
{   
public:
   // -- Constructors
   Decayed();
   Decayed(const Decayed&);
   Decayed& operator=(const Decayed&);
   virtual ~Decayed();
   
   // -- Propagation
   virtual Bool_t    SaveState(Run* run, Particle* particle);
   
   ClassDef(Decayed,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Absorbed -                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class Absorbed : public State
{
public:
   // -- Constructors
   Absorbed();
   Absorbed(const Absorbed&);
   Absorbed& operator=(const Absorbed&);
   virtual ~Absorbed();
   
   // -- Propagation
   virtual Bool_t    SaveState(Run* run, Particle* particle);
   
   ClassDef(Absorbed,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Detected -                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class Detected : public State
{
public:
   // -- Constructors
   Detected();
   Detected(const Detected&);
   Detected& operator=(const Detected&);
   virtual ~Detected();
   
   // -- Propagation
   virtual Bool_t    SaveState(Run* run, Particle* particle);   
   
   ClassDef(Detected,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Lost -                                                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class Lost : public State
{
public:
   // -- Constructors
   Lost();
   Lost(const Lost&);
   Lost& operator=(const Lost&);
   virtual ~Lost();
   
   // -- Propagation
   virtual Bool_t    SaveState(Run* run, Particle* particle);
   
   ClassDef(Lost,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Anomalous -                                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class Anomalous : public State
{
public:
   // -- Constructors
   Anomalous();
   Anomalous(const Anomalous&);
   Anomalous& operator=(const Anomalous&);
   virtual ~Anomalous();
   
   // -- Propagation
   virtual Bool_t    SaveState(Run* run, Particle* particle);
   
   ClassDef(Anomalous,1)
};

#endif  /*STATE_H*/

