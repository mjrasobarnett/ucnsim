// UCN Observer class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <sstream>
#include <cassert>

#include "Observer.h"

#include "Particle.h"
#include "RunConfig.h"
#include "Data.h"

#include "TKey.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Observer                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

using namespace std;

ClassImp(Observer)

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    SpinObserver                                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(SpinObserver)

//_____________________________________________________________________________
SpinObserver::SpinObserver()
                 :Observer(),
                  fSpinObservables(NULL)
{
   // Constructor
   Info("SpinObserver","Default Constructor");
}

//_____________________________________________________________________________
SpinObserver::SpinObserver(const RunConfig& runConfig)
                 :Observer(),
                  fSpinObservables(NULL)
{
   // Constructor
   Info("SpinObserver","Constructor");
   fMeasAxis = runConfig.PolarisationAxis();
}

//_____________________________________________________________________________
SpinObserver::SpinObserver(const SpinObserver& other)
                 :Observer(other),
                  fSpinObservables(other.fSpinObservables),
                  fMeasAxis(other.fMeasAxis)
{
   // Copy Constructor
   Info("SpinObserver","Copy Constructor");
}

//_____________________________________________________________________________
SpinObserver& SpinObserver::operator=(const SpinObserver& other)
{
   // Assignment
   Info("SpinObserver","Assignment");
   if(this!=&other) {
      Observer::operator=(other);
      fSpinObservables = other.fSpinObservables;
      fMeasAxis = other.fMeasAxis;
   }
   return *this;
}

//_____________________________________________________________________________
SpinObserver::~SpinObserver()
{
   // Destructor
   Info("SpinObserver","Destructor");
}

//_____________________________________________________________________________
void SpinObserver::RegisterInterest(Particle& particle)
{
   // -- Register as an observer with the particle
   if (fSpinObservables != NULL) delete fSpinObservables; fSpinObservables = NULL;
   fSpinObservables = new SpinObservables();
   particle.Attach(this);
}

//_____________________________________________________________________________
void SpinObserver::RecordEvent(const Particle& particle, const string& context)
{
   // -- Record the current polarisation
   if (context == Context::Spin) {
      fSpinObservables->insert(pair<Double_t, Bool_t>(particle.T(), particle.IsSpinUp(fMeasAxis)));
   }
}

//_____________________________________________________________________________
void SpinObserver::LoadExistingObservables(TDirectory* const particleDir)
{
   // -- Look for a SpinObservables object and if so load into memory
   particleDir->cd();
   // -- Loop on all entries of this directory
   TKey *key;
   TIter nextkey(particleDir->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("SpinObservables")) {
         if (fSpinObservables != NULL) delete fSpinObservables; fSpinObservables = NULL;
         fSpinObservables = dynamic_cast<SpinObservables*>(key->ReadObj());
         break;
      }
   }
}

//_____________________________________________________________________________
void SpinObserver::WriteToFile(TDirectory* const particleDir)
{
   // -- Write out the current observable to the provided directory
   particleDir->cd();
   fSpinObservables->Write("SpinObservables",TObject::kOverwrite);
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    BounceObserver                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(BounceObserver)

//_____________________________________________________________________________
BounceObserver::BounceObserver()
                   :Observer(),
                    fBounceObservables(NULL)
{
   // Constructor
   Info("BounceObserver","Default Constructor");
}

//_____________________________________________________________________________
BounceObserver::BounceObserver(const RunConfig& /*runConfig*/)
                 :Observer(),
                  fBounceObservables(NULL)
{
   // Constructor
   Info("BounceObserver","Constructor");
}

//_____________________________________________________________________________
BounceObserver::BounceObserver(const BounceObserver& other)
                 :Observer(other),
                  fBounceObservables(other.fBounceObservables)
{
   // Copy Constructor
   Info("BounceObserver","Copy Constructor");
}

//_____________________________________________________________________________
BounceObserver& BounceObserver::operator=(const BounceObserver& other)
{
   // Assignment
   Info("BounceObserver","Assignment");
   if(this!=&other) {
      Observer::operator=(other);
      fBounceObservables = other.fBounceObservables;
   }
   return *this;
}

//_____________________________________________________________________________
BounceObserver::~BounceObserver()
{
   // Destructor
   Info("BounceObserver","Destructor");
}

//_____________________________________________________________________________
void BounceObserver::RegisterInterest(Particle& particle)
{
   // -- Register as an observer with the particle
   if (fBounceObservables != NULL) delete fBounceObservables; fBounceObservables = NULL;
   fBounceObservables = new BounceObservables();
   particle.Attach(this);
}

//_____________________________________________________________________________
void BounceObserver::RecordEvent(const Particle& particle, const string& context)
{
   // -- Record the current polarisation
   if (context == Context::SpecBounce) {
      fBounceObservables->RecordSpecular();
   } else if (context == Context::DiffBounce) {
      fBounceObservables->RecordDiffuse();
   }
}

//_____________________________________________________________________________
void BounceObserver::LoadExistingObservables(TDirectory* const particleDir)
{
   // -- Look for a SpinObservables object and if so load into memory
   particleDir->cd();
   // -- Loop on all entries of this directory
   TKey *key;
   TIter nextkey(particleDir->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("BounceObservables")) {
         if (fBounceObservables != NULL) delete fBounceObservables; fBounceObservables = NULL;
         fBounceObservables = dynamic_cast<BounceObservables*>(key->ReadObj());
         break;
      }
   }
}

//_____________________________________________________________________________
void BounceObserver::WriteToFile(TDirectory* const particleDir)
{
   // -- Write out the current observable to the provided directory
   particleDir->cd();
   fBounceObservables->Write("BounceObservables",TObject::kOverwrite);
}


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TrackObserver                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TrackObserver)

//_____________________________________________________________________________
TrackObserver::TrackObserver()
                   :Observer(),
                    fTrackObservables(NULL)
{
   // Constructor
   Info("TrackObserver","Default Constructor");
}

//_____________________________________________________________________________
TrackObserver::TrackObserver(const RunConfig& /*runConfig*/)
                 :Observer(),
                  fTrackObservables(NULL)
{
   // Constructor
   Info("TrackObserver","Constructor");
}

//_____________________________________________________________________________
TrackObserver::TrackObserver(const TrackObserver& other)
                 :Observer(other),
                  fTrackObservables(other.fTrackObservables)
{
   // Copy Constructor
   Info("TrackObserver","Copy Constructor");
}

//_____________________________________________________________________________
TrackObserver& TrackObserver::operator=(const TrackObserver& other)
{
   // Assignment
   Info("TrackObserver","Assignment");
   if(this!=&other) {
      Observer::operator=(other);
      fTrackObservables = other.fTrackObservables;
   }
   return *this;
}

//_____________________________________________________________________________
TrackObserver::~TrackObserver()
{
   // Destructor
   Info("TrackObserver","Destructor");
}

//_____________________________________________________________________________
void TrackObserver::RegisterInterest(Particle& particle)
{
   // -- Register as an observer with the particle
   if (fTrackObservables != NULL) delete fTrackObservables; fTrackObservables = NULL;
   fTrackObservables = new TrackObservables();
   particle.Attach(this);
}

//_____________________________________________________________________________
void TrackObserver::RecordEvent(const Particle& particle, const string& context)
{
   // -- Record the current polarisation
   if (context == Context::Step) {
      fTrackObservables->SetNextPoint(particle.X(), particle.Y(), particle.Z());
   }
}

//_____________________________________________________________________________
void TrackObserver::LoadExistingObservables(TDirectory* const particleDir)
{
   // -- Look for a SpinObservables object and if so load into memory
   particleDir->cd();
   // -- Loop on all entries of this directory
   TKey *key;
   TIter nextkey(particleDir->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TrackObservables")) {
         if (fTrackObservables != NULL) delete fTrackObservables; fTrackObservables = NULL;
         fTrackObservables = dynamic_cast<TrackObservables*>(key->ReadObj());
         break;
      }
   }
}

//_____________________________________________________________________________
void TrackObserver::WriteToFile(TDirectory* const particleDir)
{
   // -- Write out the current observable to the provided directory
   particleDir->cd();
   fTrackObservables->Write("TrackObservables",TObject::kOverwrite);
}
