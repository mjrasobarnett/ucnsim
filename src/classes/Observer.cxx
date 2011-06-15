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
#include "TTree.h"
#include "TBranch.h"

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

//_____________________________________________________________________________
void Observer::WriteDataToTree(TTree* tree, TObject* data)
{
   // -- Write out the provided data to a branch on the provided tree, named after
   // -- the classname of the data
   TBranch* branch = tree->GetBranch(data->ClassName());
   if (branch == NULL) {
      // -- If there is no branch yet, create one
      Info("WriteToTree","Creating Branch %s in output tree", data->ClassName());
      branch = tree->Branch(data->ClassName(), data->ClassName(), &data, 32000,0);
   }
   branch->Fill();
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
void SpinObserver::WriteToTree(TTree* tree)
{
   // -- Write out the current observer's data to the observer's branch on the tree
   this->WriteDataToTree(tree, fSpinData);
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
void BounceObserver::WriteToTree(TTree* tree)
{
   // -- Write out the current observer's data to the observer's branch on the tree
   this->WriteDataToTree(tree, fBounceData);
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
         fTrack->AddPoint(particle->GetPoint());
      } else if (Precision::IsEqual(currentTime, (fLastMeasurementTime + fMeasInterval))) {
         const Particle* particle = dynamic_cast<const Particle*>(fSubject);
         fTrack->AddPoint(particle->GetPoint());
         // Update stored value of last measurement
         fLastMeasurementTime = currentTime;
      } else {
         // Do nothing
      }
   }  else if (context == Context::Creation) {
      // The creation context signifies that the particle has just been instantiated so we
      // shall make a measurement of its initial state
      const Particle* particle = dynamic_cast<const Particle*>(fSubject);
      fTrack->AddPoint(particle->GetPoint());
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
void TrackObserver::WriteToTree(TTree* tree)
{
   // -- Write out the current observer's data to the observer's branch on the tree
   this->WriteDataToTree(tree, fTrack);
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
void FieldObserver::WriteToTree(TTree* tree)
{
   // -- Write out the current observer's data to the observer's branch on the tree
   this->WriteDataToTree(tree, fFieldData);
}

