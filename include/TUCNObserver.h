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

#include <map>

#include "TObject.h"
#include "TVector3.h"
#include "TTree.h"
#include "TDirectory.h"

#include "TUCNObservables.h"

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
   
   virtual void WriteToFile(TDirectory* particleDir) = 0;
      
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
   TUCNSpinObservables *fSpinObservables;
   TVector3 fMeasAxis;
   
public:
   // -- Constructors
   TUCNSpinObserver();
   TUCNSpinObserver(const TUCNRunConfig& runConfig);
   TUCNSpinObserver(const TUCNSpinObserver&);
   TUCNSpinObserver& operator=(const TUCNSpinObserver&);
   virtual ~TUCNSpinObserver();
   
   virtual void RegisterInterest(TUCNParticle& particle);
   virtual void RecordEvent(const TUCNParticle& particle);
   
   virtual void WriteToFile(TDirectory* particleDir);
   
   ClassDef(TUCNSpinObserver, 1)
};

#endif /* TUCNOBSERVER_H */