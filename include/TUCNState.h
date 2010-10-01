// UCN Particle State class
// Author: Matthew Raso-Barnett  
// 19/02/2010

#ifndef TUCNSTATE_H
#define TUCNSTATE_H

#include "TUCNParticle.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNState - Following the State design pattern, a Particle           //
//    state handles state specific behaviour, such as when a UCN is        //
//    detected, decays, or lost to the boundary in some way. The state     //
//    pattern allows us to code specific behaviour to these states rather  //
//    than relying on Boolean flags that must be checked constantly.       //                                             
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
class TUCNParticle;
class TUCNFieldManager;
class TUCNGravField;
class TUCNRun;
class TGeoNode;
class TGeoMatrix;
class TGeoNavigator;

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
   virtual Bool_t    LocateInGeometry(TUCNParticle* particle, TGeoNavigator* navigator,
                           const TGeoNode* initialNode, const TGeoMatrix* initialMatrix,
                           const TGeoNode* crossedNode);
   virtual void      UpdateParticle(TUCNParticle* particle, const TGeoNavigator* navigator,
                        const Double_t timeInterval=0., const TUCNGravField* gravField=0);
   virtual Bool_t    SaveState(TUCNRun* run, TUCNParticle* particle) = 0;
   virtual void      Detected(TUCNParticle* particle);
   virtual void      Lost(TUCNParticle* particle);
   virtual void      Absorbed(TUCNParticle* particle);
   virtual void      Bad(TUCNParticle* particle);
   
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
   virtual Bool_t    AttemptRelocationIntoCurrentNode(TGeoNavigator* navigator, 
                           const TGeoNode* initialNode, const TGeoMatrix* initialMatrix,
                           const TGeoNode* crossedNode);
   
   // Wall reflection
   virtual Bool_t    FindBoundaryNormal(Double_t* normal, TGeoNavigator* navigator,
                                    const TGeoNode* crossedNode);

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
   virtual Bool_t    LocateInGeometry(TUCNParticle* particle, TGeoNavigator* navigator,
                           const TGeoNode* initialNode, const TGeoMatrix* initialMatrix,
                           const TGeoNode* crossedNode);
   
   virtual Bool_t    SaveState(TUCNRun* run, TUCNParticle* particle);
   virtual void      Detected(TUCNParticle* particle);
   virtual void      Lost(TUCNParticle* particle);
   virtual void      Absorbed(TUCNParticle* particle);
   virtual void      Bad(TUCNParticle* particle);
   
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
   virtual Bool_t    SaveState(TUCNRun* run, TUCNParticle* particle);
   
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
   virtual Bool_t    SaveState(TUCNRun* run, TUCNParticle* particle);
   
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
   virtual Bool_t    SaveState(TUCNRun* run, TUCNParticle* particle);   
   
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
   virtual Bool_t    SaveState(TUCNRun* run, TUCNParticle* particle);
   
   ClassDef(TUCNLost,1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNBad -                                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNBad : public TUCNState
{
public:
   // -- Constructors
   TUCNBad();
   TUCNBad(const TUCNBad&);
   TUCNBad& operator=(const TUCNBad&);
   virtual ~TUCNBad();
   
   // -- Propagation
   virtual Bool_t    SaveState(TUCNRun* run, TUCNParticle* particle);
   
   ClassDef(TUCNBad,1)
};

#endif  /*TUCNSTATE_H*/

