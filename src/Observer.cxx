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
              fSpinData(NULL)
{
   // Constructor
   Info("SpinObserver","Default Constructor");
}

//_____________________________________________________________________________
SpinObserver::SpinObserver(const SpinObserver& other)
                 :Observer(other),
                  fSpinData(other.fSpinData)
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
      fSpinData = other.fSpinData;
   }
   return *this;
}

//_____________________________________________________________________________
SpinObserver::~SpinObserver()
{
   // Destructor
   Info("SpinObserver","Destructor");
   if (fSpinData != NULL) delete fSpinData;
}

//_____________________________________________________________________________
void SpinObserver::RecordEvent(const TObject* subject, const string& context)
{
   // -- Record the current spin state
   if (subject == fSubject && context == Context::Spin) {
      const Particle* particle = dynamic_cast<const Particle*>(subject);
      // Make a copy of the current spin state
      const Spin* spin = new Spin(particle->GetSpin());
      // Insert copy into data structure
      fSpinData->insert(pair<Double_t,const Spin*>(particle->T(), spin));
   }
}

//_____________________________________________________________________________
void SpinObserver::ResetData()
{
   // -- Delete current observables and create a new version in its place
   if (fSpinData != NULL) delete fSpinData; fSpinData = NULL;
   fSpinData = new SpinData();
}

//_____________________________________________________________________________
void SpinObserver::LoadExistingData(TDirectory* const particleDir)
{
   // -- Look for a SpinData object and if so load into memory
   particleDir->cd();
   // -- Loop on all entries of this directory
   TKey *key;
   TIter nextkey(particleDir->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("SpinData")) {
         if (fSpinData != NULL) delete fSpinData; fSpinData = NULL;
         fSpinData = dynamic_cast<SpinData*>(key->ReadObj());
         break;
      }
   }
}

//_____________________________________________________________________________
void SpinObserver::WriteToFile(TDirectory* const particleDir)
{
   // -- Write out the current observable to the provided directory
   particleDir->cd();
   fSpinData->Write("SpinData",TObject::kOverwrite);
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
                    fBounceData(NULL)
{
   // Constructor
   Info("BounceObserver","Default Constructor");
}

//_____________________________________________________________________________
BounceObserver::BounceObserver(const BounceObserver& other)
                 :Observer(other),
                  fBounceData(other.fBounceData)
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
      fBounceData = other.fBounceData;
   }
   return *this;
}

//_____________________________________________________________________________
BounceObserver::~BounceObserver()
{
   // Destructor
   Info("BounceObserver","Destructor");
   if (fBounceData != NULL) delete fBounceData;
}

//_____________________________________________________________________________
void BounceObserver::RecordEvent(const TObject* subject, const string& context)
{
   // -- Record the current polarisation
   if (subject == fSubject) {
      if (context == Context::SpecBounce) {
         fBounceData->RecordSpecular();
      } else if (context == Context::DiffBounce) {
         fBounceData->RecordDiffuse();
      }
   }
}

//_____________________________________________________________________________
void BounceObserver::ResetData()
{
   // -- Delete current observables and create a new version in its place
   if (fBounceData != NULL) delete fBounceData; fBounceData = NULL;
   fBounceData = new BounceData();
}

//_____________________________________________________________________________
void BounceObserver::LoadExistingData(TDirectory* const particleDir)
{
   // -- Look for a SpinData object and if so load into memory
   particleDir->cd();
   // -- Loop on all entries of this directory
   TKey *key;
   TIter nextkey(particleDir->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("BounceData")) {
         if (fBounceData != NULL) delete fBounceData; fBounceData = NULL;
         fBounceData = dynamic_cast<BounceData*>(key->ReadObj());
         break;
      }
   }
}

//_____________________________________________________________________________
void BounceObserver::WriteToFile(TDirectory* const particleDir)
{
   // -- Write out the current observable to the provided directory
   particleDir->cd();
   fBounceData->Write("BounceData",TObject::kOverwrite);
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
   if (fTrack != NULL) delete fTrack;
}

//_____________________________________________________________________________
void TrackObserver::RecordEvent(const TObject* subject, const string& context)
{
   // -- Record the current polarisation
   if (subject == fSubject && context == Context::Step) {
      const Particle* particle = dynamic_cast<const Particle*>(subject);
      fTrack->AddPoint(particle->X(), particle->Y(), particle->Z(), particle->T());
   }
}

//_____________________________________________________________________________
void TrackObserver::ResetData()
{
   // -- Delete current observables and create a new version in its place
   if (fTrack != NULL) delete fTrack; fTrack = NULL;
   fTrack = new Track();
}

//_____________________________________________________________________________
void TrackObserver::LoadExistingData(TDirectory* const particleDir)
{
   // -- Look for a SpinData object and if so load into memory
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


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    FieldObserver                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(FieldObserver)

//_____________________________________________________________________________
FieldObserver::FieldObserver()
              :Observer(),
               fFieldData(NULL)
{
   // Constructor
   Info("FieldObserver","Default Constructor");
}

//_____________________________________________________________________________
FieldObserver::FieldObserver(const FieldObserver& other)
              :Observer(other),
               fFieldData(other.fFieldData)
{
   // Copy Constructor
   Info("FieldObserver","Copy Constructor");
}

//_____________________________________________________________________________
FieldObserver& FieldObserver::operator=(const FieldObserver& other)
{
   // Assignment
   Info("FieldObserver","Assignment");
   if(this!=&other) {
      Observer::operator=(other);
      fFieldData = other.fFieldData;
   }
   return *this;
}

//_____________________________________________________________________________
FieldObserver::~FieldObserver()
{
   // Destructor
   Info("FieldObserver","Destructor");
   if (fFieldData != NULL) delete fFieldData;
}

//_____________________________________________________________________________
void FieldObserver::RecordEvent(const TObject* subject, const string& context)
{
   // -- Record the current Field 
   if (context == Context::MeasureField) {
      const FieldVertex* vertex = dynamic_cast<const FieldVertex*>(subject);
      const FieldVertex* copy = new FieldVertex(*vertex);
      fFieldData->push_back(copy);
   }
}

//_____________________________________________________________________________
void FieldObserver::ResetData()
{
   // -- Delete current observables and create a new version in its place
   if (fFieldData != NULL) delete fFieldData; fFieldData = NULL;
   fFieldData = new FieldData();
}

//_____________________________________________________________________________
void FieldObserver::LoadExistingData(TDirectory* const particleDir)
{
   // -- Look for a SpinData object and if so load into memory
   particleDir->cd();
   // -- Loop on all entries of this directory
   TKey *key;
   TIter nextkey(particleDir->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("FieldData")) {
         if (fFieldData != NULL) delete fFieldData; fFieldData = NULL;
         fFieldData = dynamic_cast<FieldData*>(key->ReadObj());
         break;
      }
   }
}

//_____________________________________________________________________________
void FieldObserver::WriteToFile(TDirectory* const particleDir)
{
   // -- Write out the current observable to the provided directory
   particleDir->cd();
   fFieldData->Write("FieldData",TObject::kOverwrite);
}

