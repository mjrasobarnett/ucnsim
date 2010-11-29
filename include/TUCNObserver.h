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
#include <string>

#include "TObject.h"
#include "TVector3.h"
#include "TTree.h"
#include "TDirectory.h"

#include "TUCNObservables.h"


namespace Context {
   // Define List of contexts passed to observers to help them distinguish
   // whether the particle's state change is of interest to them
   const std::string Spin = "spin";
   const std::string SpecBounce = "specbounce";
   const std::string DiffBounce = "diffbounce";
   const std::string Step = "step";
}

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
   virtual void RecordEvent(const TUCNParticle& particle, const std::string& context) = 0;
   virtual void LoadExistingObservables(TDirectory* const particleDir) = 0;
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
   virtual void RecordEvent(const TUCNParticle& particle, const std::string& context);
   virtual void LoadExistingObservables(TDirectory* const particleDir);
   virtual void WriteToFile(TDirectory* const particleDir);
   
   ClassDef(TUCNSpinObserver, 1)
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNBounceObserver -                                                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNBounceObserver : public TUCNObserver
{
private:
   TUCNBounceObservables *fBounceObservables;
   
public:
   // -- Constructors
   TUCNBounceObserver();
   TUCNBounceObserver(const TUCNRunConfig& runConfig);
   TUCNBounceObserver(const TUCNBounceObserver&);
   TUCNBounceObserver& operator=(const TUCNBounceObserver&);
   virtual ~TUCNBounceObserver();
   
   virtual void RegisterInterest(TUCNParticle& particle);
   virtual void RecordEvent(const TUCNParticle& particle, const std::string& context);
   virtual void LoadExistingObservables(TDirectory* const particleDir);
   virtual void WriteToFile(TDirectory* const particleDir);
   
   ClassDef(TUCNBounceObserver, 1)
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNTrackObserver -                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNTrackObserver : public TUCNObserver
{
private:
   TUCNTrackObservables *fTrackObservables;
   
public:
   // -- Constructors
   TUCNTrackObserver();
   TUCNTrackObserver(const TUCNRunConfig& runConfig);
   TUCNTrackObserver(const TUCNTrackObserver&);
   TUCNTrackObserver& operator=(const TUCNTrackObserver&);
   virtual ~TUCNTrackObserver();
   
   virtual void RegisterInterest(TUCNParticle& particle);
   virtual void RecordEvent(const TUCNParticle& particle, const std::string& context);
   virtual void LoadExistingObservables(TDirectory* const particleDir);
   virtual void WriteToFile(TDirectory* const particleDir);
   
   ClassDef(TUCNTrackObserver, 1)
};

#endif /* TUCNOBSERVER_H */