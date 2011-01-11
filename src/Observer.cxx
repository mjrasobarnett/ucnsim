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
void SpinObserver::RecordEvent(const TObject* subject, const string& context)
{
   // -- Record the current polarisation
   if (subject == fSubject && context == Context::Spin) {
      const Particle* particle = dynamic_cast<const Particle*>(subject);
      fSpinObservables->insert(pair<Double_t,Bool_t>(particle->T(), particle->IsSpinUp(fMeasAxis)));
   }
}

//_____________________________________________________________________________
void SpinObserver::ResetObservables()
{
   // -- Delete current observables and create a new version in its place
   if (fSpinObservables != NULL) delete fSpinObservables; fSpinObservables = NULL;
   fSpinObservables = new SpinObservables();
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
void BounceObserver::RecordEvent(const TObject* subject, const string& context)
{
   // -- Record the current polarisation
   if (subject == fSubject) {
      if (context == Context::SpecBounce) {
         fBounceObservables->RecordSpecular();
      } else if (context == Context::DiffBounce) {
         fBounceObservables->RecordDiffuse();
      }
   }
}

//_____________________________________________________________________________
void BounceObserver::ResetObservables()
{
   // -- Delete current observables and create a new version in its place
   if (fBounceObservables != NULL) delete fBounceObservables; fBounceObservables = NULL;
   fBounceObservables = new BounceObservables();
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
                    fTrack(NULL)
{
   // Constructor
   Info("TrackObserver","Default Constructor");
}

//_____________________________________________________________________________
TrackObserver::TrackObserver(const RunConfig& /*runConfig*/)
                 :Observer(),
                  fTrack(NULL)
{
   // Constructor
   Info("TrackObserver","Constructor");
}

//_____________________________________________________________________________
TrackObserver::TrackObserver(const TrackObserver& other)
                 :Observer(other),
                  fTrack(other.fTrack)
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
      fTrack = other.fTrack;
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
void TrackObserver::RecordEvent(const TObject* subject, const string& context)
{
   // -- Record the current polarisation
   if (subject == fSubject && context == Context::Step) {
      const Particle* particle = dynamic_cast<const Particle*>(subject);
      fTrack->AddVertex(particle->X(), particle->Y(), particle->Z(), particle->T());
   }
}

//_____________________________________________________________________________
void TrackObserver::ResetObservables()
{
   // -- Delete current observables and create a new version in its place
   if (fTrack != NULL) delete fTrack; fTrack = NULL;
   fTrack = new Track();
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
      if (cl->InheritsFrom("Track")) {
         if (fTrack != NULL) delete fTrack; fTrack = NULL;
         fTrack = dynamic_cast<Track*>(key->ReadObj());
         break;
      }
   }
}

//_____________________________________________________________________________
void TrackObserver::WriteToFile(TDirectory* const particleDir)
{
   // -- Write out the current observable to the provided directory
   particleDir->cd();
   fTrack->Write("Track",TObject::kOverwrite);
}
