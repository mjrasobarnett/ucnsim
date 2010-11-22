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
#include "TH1F.h"
#include "TVector3.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNObserver -  Abstract Interface                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
class TUCNParticle;
class TUCNRunConfig;
class TUCNInterest;

class TUCNObserver : public TObject
{
public:
   
   virtual void RecordEvent(const TUCNParticle& particle) = 0;
   virtual void Plot() = 0;
   
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
   virtual void Plot();
   
   ClassDef(TUCNSpinObserver, 1)
};

#endif /* TUCNOBSERVER_H */