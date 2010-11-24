// UCN Particle Observer class
// Author: Matthew Raso-Barnett  
// 19/11/2010

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    This file lists the following Observers:                             //
//    -  TUCNObserver                                                      //
//    -  TUCNSpinObserver                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef TUCNOBSERVER_H
#define TUCNOBSERVER_H

#include "TObject.h"
#include <map>
#include "TVector3.h"
#include "TUCNObservables.h"
#include "TTree.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNObserver -  Abstract Interface                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
class TUCNParticle;
class TUCNRunConfig;
class TUCNInterest;
class TUCNData;

class TUCNObserver : public TObject
{
public:
   
   virtual void RegisterInterest(TUCNParticle& particle) = 0;
   virtual void RecordEvent(const TUCNParticle& particle) = 0;
   
   virtual void Plot(TUCNData* data, TTree* tree) = 0;
   
   ClassDef(TUCNObserver, 1)
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNSpinObserver -                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNSpinObserver : public TUCNObserver
{
private:
   std::map<Int_t, TUCNSpinObservables*> fParticleData;
   TVector3 fMeasAxis;
   
   void PurgeContainer();
   
public:
   // -- Constructors
   TUCNSpinObserver();
   TUCNSpinObserver(const TUCNRunConfig& runConfig);
   TUCNSpinObserver(const TUCNSpinObserver&);
   TUCNSpinObserver& operator=(const TUCNSpinObserver&);
   virtual ~TUCNSpinObserver();
   
   virtual void RegisterInterest(TUCNParticle& particle);
   virtual void RecordEvent(const TUCNParticle& particle);
   
   virtual void Plot(TUCNData* data, TTree* tree);
   
   ClassDef(TUCNSpinObserver, 1)
};

#endif /* TUCNOBSERVER_H */