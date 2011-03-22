// UCN Observer class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <sstream>
#include <cassert>

#include "Observer.h"

#include "FieldArray.h"
#include "Particle.h"
#include "RunConfig.h"
#include "Data.h"
#include "Clock.h"
#include "Algorithms.h"

#include "TKey.h"
#include "TClass.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Observer                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace Algorithms;

ClassImp(Observer);

//_____________________________________________________________________________
Observer::Observer()
         :TNamed()
{
   // Constructor
   Info("Observer","Default Constructor");
}

//_____________________________________________________________________________
Observer::Observer(const string name)
         :TNamed(name,name)
{
   // Constructor
   Info("Observer","Default Constructor");
}

//_____________________________________________________________________________
Observer::Observer(const Observer& other)
         :TNamed(other)
{
   // Copy Constructor
   Info("Observer","Copy Constructor");
}

//_____________________________________________________________________________
Observer::~Observer()
{
   // Destructor
   Info("Observer","Destructor");
}


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    SpinObserver                                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(SpinObserver)

//_____________________________________________________________________________
SpinObserver::SpinObserver(const std::string name, double measureInterval)
             :Observer(name),
              fSpinData(NULL),
              fMeasInterval(measureInterval),
              fLastMeasurementTime(Clock::Instance()->GetTime())
{
   // Constructor
   Info("SpinObserver","Default Constructor");
}

//_____________________________________________________________________________
SpinObserver::SpinObserver(const SpinObserver& other)
                 :Observer(other),
                  fSpinData(other.fSpinData),
                  fMeasInterval(other.fMeasInterval),
                  fLastMeasurementTime(other.fLastMeasurementTime)
{
   // Copy Constructor
   Info("SpinObserver","Copy Constructor");
}

//_____________________________________________________________________________
SpinObserver::~SpinObserver()
{
   // Destructor
   Info("SpinObserver","Destructor");
   if (fSpinData != NULL) delete fSpinData;
}

//_____________________________________________________________________________
void SpinObserver::RecordEvent(const Point& /*point*/, const TVector3& /*velocity*/, const std::string& context)
{
   // -- Record the current spin state
   if (context == Context::Spin) {
      // The Spin context means that spin state has just changed.
      // First check whether it is time to make a Spin measurement
      double currentTime = Clock::Instance()->GetTime();
      if (Precision::IsEqual(currentTime, (fLastMeasurementTime + fMeasInterval))) {
         const Particle* particle = dynamic_cast<const Particle*>(fSubject);
         // Make a copy of the current spin state
         const Spin* spin = new Spin(particle->GetSpin());
         // Insert copy into data structure
         fSpinData->insert(pair<Double_t,const Spin*>(particle->T(), spin));
         // Update stored value of last measurement
         fLastMeasurementTime = currentTime;
      }
   } else if (context == Context::Creation) {
      // The creation context signifies that the particle has just been instantiated so we
      // shall make a measurement of its initial state
      const Particle* particle = dynamic_cast<const Particle*>(fSubject);
      const Spin* spin = new Spin(particle->GetSpin());
      fSpinData->insert(pair<Double_t,const Spin*>(particle->T(), spin));
   }
}

//_____________________________________________________________________________
void SpinObserver::ResetData()
{
   // -- Delete current observables and create a new version in its place
   fLastMeasurementTime = 0.0;
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
BounceObserver::BounceObserver(const std::string name)
                   :Observer(name),
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
void BounceObserver::RecordEvent(const Point& /*point*/, const TVector3& /*velocity*/, const std::string& context)

{
   // -- If context indicates a bounce was made, increment counters.
   if (context == Context::SpecBounce) {
         fBounceData->RecordSpecular();
   } else if (context == Context::DiffBounce) {
         fBounceData->RecordDiffuse();
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
TrackObserver::TrackObserver(const std::string name, double measInterval)
                   :Observer(name),
                    fTrack(NULL),
                    fMeasInterval(measInterval),
                    fLastMeasurementTime(0.0)
{
   // Constructor
   Info("TrackObserver","Default Constructor");
}

//_____________________________________________________________________________
TrackObserver::TrackObserver(const TrackObserver& other)
                 :Observer(other),
                  fTrack(other.fTrack),
                  fMeasInterval(other.fMeasInterval),
                  fLastMeasurementTime(other.fLastMeasurementTime)
{
   // Copy Constructor
   Info("TrackObserver","Copy Constructor");
}

//_____________________________________________________________________________
TrackObserver::~TrackObserver()
{
   // Destructor
   Info("TrackObserver","Destructor");
   if (fTrack != NULL) delete fTrack;
}

//_____________________________________________________________________________
void TrackObserver::RecordEvent(const Point& /*point*/, const TVector3& /*velocity*/, const std::string& context)
{
   // -- Record the current polarisation
   if (context == Context::Step) {
      double currentTime = Clock::Instance()->GetTime();
      // If no measurement interval is set, we record every step
      if (fMeasInterval == 0.0) {
         const Particle* particle = dynamic_cast<const Particle*>(fSubject);
         fTrack->AddPoint(particle->X(), particle->Y(), particle->Z(), particle->T());
      } else if (Precision::IsEqual(currentTime, (fLastMeasurementTime + fMeasInterval))) {
         const Particle* particle = dynamic_cast<const Particle*>(fSubject);
         fTrack->AddPoint(particle->X(), particle->Y(), particle->Z(), particle->T());
         // Update stored value of last measurement
         fLastMeasurementTime = currentTime;
      } else {
         // Do nothing
      }
   }  else if (context == Context::Creation) {
      // The creation context signifies that the particle has just been instantiated so we
      // shall make a measurement of its initial state
      const Particle* particle = dynamic_cast<const Particle*>(fSubject);
      fTrack->AddPoint(particle->X(), particle->Y(), particle->Z(), particle->T());
   }
}

//_____________________________________________________________________________
void TrackObserver::ResetData()
{
   // -- Delete current observables and create a new version in its place
   fLastMeasurementTime = 0.0;
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
FieldObserver::FieldObserver(const std::string name, const double measureInterval)
              :Observer(name),
               fFieldData(NULL),
               fMeasInterval(measureInterval),
               fLastMeasurementTime(Clock::Instance()->GetTime())
{
   // Constructor
   Info("FieldObserver","Default Constructor");
}

//_____________________________________________________________________________
FieldObserver::FieldObserver(const FieldObserver& other)
              :Observer(other),
               fFieldData(other.fFieldData),
               fMeasInterval(other.fMeasInterval),
               fLastMeasurementTime(other.fLastMeasurementTime)
{
   // Copy Constructor
   Info("FieldObserver","Copy Constructor");
}

//_____________________________________________________________________________
FieldObserver::~FieldObserver()
{
   // Destructor
   Info("FieldObserver","Destructor");
   if (fFieldData != NULL) delete fFieldData;
}

//_____________________________________________________________________________
void FieldObserver::RecordEvent(const Point& point, const TVector3& velocity, const std::string& context)
{
   // -- Record the current Field at the current point
   if (context == Context::MagField) {
      // Calculate whether it is time to make a field measurement
      double currentTime = Clock::Instance()->GetTime();
      if (Precision::IsEqual(currentTime,(fLastMeasurementTime + fMeasInterval))) {
         // Make measurement
         const TVector3 field = dynamic_cast<const FieldArray*>(fSubject)->GetMagField(point,velocity);
         const FieldVertex* fieldvertex = new FieldVertex(point, field);
         fFieldData->push_back(fieldvertex);
         // Update stored value of last measurement
         fLastMeasurementTime = currentTime;
      }
   } else if (context == Context::Creation) {
      // The creation context signifies that the particle has just been instantiated so we
      // shall make a measurement of the initial state
      const TVector3 field = dynamic_cast<const FieldArray*>(fSubject)->GetMagField(point,velocity);
      const FieldVertex* fieldvertex = new FieldVertex(point, field);
      fFieldData->push_back(fieldvertex);
   }
}

//_____________________________________________________________________________
void FieldObserver::ResetData()
{
   // -- Delete current observables and create a new version in its place
   fLastMeasurementTime = 0.0;
   if (fFieldData != NULL) delete fFieldData; fFieldData = NULL;
   fFieldData = new FieldData(this->GetName());
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
   fFieldData->Write("FieldData");
}

