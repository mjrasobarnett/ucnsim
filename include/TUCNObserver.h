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
#include "TUCNRunConfig.h"
#include "TUCNParticle.h"
#include "TH1F.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNObserver -  Abstract Interface                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNObserver : public TObject
{
private:
   
public:
   // -- Constructors
   TUCNObserver();
   TUCNObserver(const TUCNObserver&);
   TUCNObserver& operator=(const TUCNObserver&);
   virtual ~TUCNObserver();
   
   virtual void RecordEvent(const TUCNParticle& particle) = 0;
   
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
   TH1F* fSpinUpHist;
   TH1F* fSpinDownHist;
   TVector3 fMeasAxis;
   
public:
   // -- Constructors
   TUCNSpinObserver();
   TUCNSpinObserver(const TUCNRunConfig& runConfig);
   TUCNSpinObserver(const TUCNSpinObserver&);
   TUCNSpinObserver& operator=(const TUCNSpinObserver&);
   virtual ~TUCNSpinObserver();
   
   virtual void RecordEvent(const TUCNParticle& particle);
   
   ClassDef(TUCNObserver, 1)
};

#endif /* TUCNOBSERVER_H */