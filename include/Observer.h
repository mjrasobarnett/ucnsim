// UCN Particle Observer class
// Author: Matthew Raso-Barnett  
// 19/11/2010

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    This file lists the following Observers:                             //
//    -  Observer                                                      //
//    -  SpinObserver                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef OBSERVER_H
#define OBSERVER_H

#include <map>
#include <string>

#include "TObject.h"
#include "TVector3.h"
#include "TTree.h"
#include "TDirectory.h"

#include "Observables.h"


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
//    Observer -  Abstract Interface                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
class Particle;
class RunConfig;
class Interest;
class Data;

class Observer : public TObject
{
public:
   
   virtual void RegisterInterest(Particle& particle) = 0;
   virtual void RecordEvent(const Particle& particle, const std::string& context) = 0;
   virtual void LoadExistingObservables(TDirectory* const particleDir) = 0;
   virtual void WriteToFile(TDirectory* particleDir) = 0;
      
   ClassDef(Observer, 1)
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    SpinObserver -                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class SpinObserver : public Observer
{
private:
   SpinObservables *fSpinObservables;
   TVector3 fMeasAxis;
   
public:
   // -- Constructors
   SpinObserver();
   SpinObserver(const RunConfig& runConfig);
   SpinObserver(const SpinObserver&);
   SpinObserver& operator=(const SpinObserver&);
   virtual ~SpinObserver();
   
   virtual void RegisterInterest(Particle& particle);
   virtual void RecordEvent(const Particle& particle, const std::string& context);
   virtual void LoadExistingObservables(TDirectory* const particleDir);
   virtual void WriteToFile(TDirectory* const particleDir);
   
   ClassDef(SpinObserver, 1)
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    BounceObserver -                                                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class BounceObserver : public Observer
{
private:
   BounceObservables *fBounceObservables;
   
public:
   // -- Constructors
   BounceObserver();
   BounceObserver(const RunConfig& runConfig);
   BounceObserver(const BounceObserver&);
   BounceObserver& operator=(const BounceObserver&);
   virtual ~BounceObserver();
   
   virtual void RegisterInterest(Particle& particle);
   virtual void RecordEvent(const Particle& particle, const std::string& context);
   virtual void LoadExistingObservables(TDirectory* const particleDir);
   virtual void WriteToFile(TDirectory* const particleDir);
   
   ClassDef(BounceObserver, 1)
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TrackObserver -                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TrackObserver : public Observer
{
private:
   TrackObservables *fTrackObservables;
   
public:
   // -- Constructors
   TrackObserver();
   TrackObserver(const RunConfig& runConfig);
   TrackObserver(const TrackObserver&);
   TrackObserver& operator=(const TrackObserver&);
   virtual ~TrackObserver();
   
   virtual void RegisterInterest(Particle& particle);
   virtual void RecordEvent(const Particle& particle, const std::string& context);
   virtual void LoadExistingObservables(TDirectory* const particleDir);
   virtual void WriteToFile(TDirectory* const particleDir);
   
   ClassDef(TrackObserver, 1)
};

#endif /* OBSERVER_H */