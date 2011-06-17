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
         :TNamed(other),
          fSubject(other.fSubject)
{
   // Copy Constructor
   Info("Observer","Copy Constructor");
}

//_____________________________________________________________________________
Observer& Observer::operator=(const Observer& other)
{
   Info("Observer","Assignment");
   if(this!=&other) {
      Observer::operator=(other);
      fSubject = other.fSubject;
   }
   return *this;
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

ClassImp(SpinObserver);

//_____________________________________________________________________________
SpinObserver::SpinObserver(const std::string name, double measureInterval)
             :Observer(name),
              fSpinData(NULL),
              fMeasInterval(measureInterval),
              fLastMeasurementTime(Clock::Instance()->GetTime())
{
   // Constructor
   Info("SpinObserver","Default Constructor");
   fSpinData = new SpinData();
}

//_____________________________________________________________________________
SpinObserver::SpinObserver(const SpinObserver& other)
             :Observer(other),
              fSpinData(NULL),
              fMeasInterval(other.fMeasInterval),
              fLastMeasurementTime(other.fLastMeasurementTime)
{
   // Copy Constructor
   Info("SpinObserver","Copy Constructor");
   if (other.fSpinData) fSpinData = new SpinData(*(other.fSpinData));
}

//_____________________________________________________________________________
SpinObserver& SpinObserver::operator=(const SpinObserver& other)
{
   // Assignment
   Info("SpinObserver","Assignment");
   if(this!=&other) {
      Observer::operator=(other);
      if (fSpinData) delete fSpinData;
      fSpinData = new SpinData(*(other.fSpinData));
      fMeasInterval = other.fMeasInterval;
      fLastMeasurementTime = other.fLastMeasurementTime;
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
void SpinObserver::Reset()
{
   // -- Delete current observables and create a new version in its place
   fLastMeasurementTime = 0.0;
   if (fSpinData != NULL) delete fSpinData; fSpinData = NULL;
   fSpinData = new SpinData();
}

//_____________________________________________________________________________
void SpinObserver::WriteToFile(Data& data)
{
   // -- Write out the current observer's data to the observer's branch on the tree
   data.WriteObjectToTree(fSpinData, fSpinData->GetName());
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    BounceObserver                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(BounceObserver);

//_____________________________________________________________________________
BounceObserver::BounceObserver(const std::string name)
                   :Observer(name),
                    fBounceData(NULL)
{
   // Constructor
   Info("BounceObserver","Default Constructor");
   fBounceData = new BounceData();
}

//_____________________________________________________________________________
BounceObserver::BounceObserver(const BounceObserver& other)
                 :Observer(other),
                  fBounceData(NULL)
{
   // Copy Constructor
   Info("BounceObserver","Copy Constructor");
   if (other.fBounceData) fBounceData = new BounceData(*(other.fBounceData));
}

//_____________________________________________________________________________
BounceObserver& BounceObserver::operator=(const BounceObserver& other)
{
   // Assignment
   Info("BounceObserver","Assignment");
   if(this!=&other) {
      Observer::operator=(other);
      if (fBounceData) delete fBounceData;
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
void BounceObserver::Reset()
{
   // -- Delete current observables and create a new version in its place
   if (fBounceData != NULL) delete fBounceData; fBounceData = NULL;
   fBounceData = new BounceData();
}

//_____________________________________________________________________________
void BounceObserver::WriteToFile(Data& data)
{
   // -- Write out the current observer's data to the observer's branch on the tree
   data.WriteObjectToTree(fBounceData, fBounceData->GetName());
}



/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TrackObserver                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TrackObserver);

//_____________________________________________________________________________
TrackObserver::TrackObserver(const std::string name, double measInterval)
                   :Observer(name),
                    fTrack(NULL),
                    fMeasInterval(measInterval),
                    fLastMeasurementTime(0.0)
{
   // Constructor
   Info("TrackObserver","Default Constructor");
   fTrack = new Track();
}

//_____________________________________________________________________________
TrackObserver::TrackObserver(const TrackObserver& other)
                 :Observer(other),
                  fTrack(NULL),
                  fMeasInterval(other.fMeasInterval),
                  fLastMeasurementTime(other.fLastMeasurementTime)
                  
{
   // Copy Constructor
   Info("TrackObserver","Copy Constructor");
   if (other.fTrack) fTrack = new Track(*(other.fTrack));
}

//_____________________________________________________________________________
TrackObserver& TrackObserver::operator=(const TrackObserver& other)
{
   // Assignment
   Info("TrackObserver","Assignment");
   if(this!=&other) {
      Observer::operator=(other);
      if (fTrack) delete fTrack;
      fTrack = other.fTrack;
      fMeasInterval = other.fMeasInterval;
      fLastMeasurementTime = other.fLastMeasurementTime;
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
void TrackObserver::Reset()
{
   // -- Delete current observables and create a new version in its place
   fLastMeasurementTime = 0.0;
   if (fTrack != NULL) delete fTrack; fTrack = NULL;
   fTrack = new Track();
}

//_____________________________________________________________________________
void TrackObserver::WriteToFile(Data& data)
{
   // -- Write out the current observer's data to the observer's branch on the tree
   data.WriteObjectToTree(fTrack, fTrack->GetName());
}


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    FieldObserver                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(FieldObserver);

//_____________________________________________________________________________
FieldObserver::FieldObserver(const std::string name, const double measureInterval)
              :Observer(name),
               fFieldData(NULL),
               fMeasInterval(measureInterval),
               fLastMeasurementTime(Clock::Instance()->GetTime())
{
   // Constructor
   Info("FieldObserver","Default Constructor");
   fFieldData = new FieldData(name);
}

//_____________________________________________________________________________
FieldObserver::FieldObserver(const FieldObserver& other)
             :Observer(other),
              fFieldData(NULL),
              fMeasInterval(other.fMeasInterval),
              fLastMeasurementTime(other.fLastMeasurementTime)
{
   // Copy Constructor
   Info("FieldObserver","Copy Constructor");
   if (other.fFieldData) fFieldData = new FieldData(*(other.fFieldData));
}

//_____________________________________________________________________________
FieldObserver& FieldObserver::operator=(const FieldObserver& other)
{
   // Assignment
   Info("FieldObserver","Assignment");
   if(this!=&other) {
      Observer::operator=(other);
      if (fFieldData) delete fFieldData;
      fFieldData = new FieldData(*(other.fFieldData));
      fMeasInterval = other.fMeasInterval;
      fLastMeasurementTime = other.fLastMeasurementTime;
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
void FieldObserver::Reset()
{
   // -- Delete current observables and create a new version in its place
   fLastMeasurementTime = 0.0;
   if (fFieldData != NULL) delete fFieldData; fFieldData = NULL;
   fFieldData = new FieldData(this->GetName());
}

//_____________________________________________________________________________
void FieldObserver::WriteToFile(Data& data)
{
   // -- Write out the current observer's data to the observer's branch on the tree
   data.WriteObjectToTree(fFieldData, fFieldData->GetName());
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    PopulationObserver                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(PopulationObserver);

//_____________________________________________________________________________
PopulationObserver::PopulationObserver(const std::string name, const double measureInterval)
              :Observer(name),
               fPopulationData(NULL),
               fMeasInterval(measureInterval),
               fLastMeasurementTime(Clock::Instance()->GetTime())
{
   // Constructor
   Info("PopulationObserver","Default Constructor");
   fPopulationData = new PopulationData();
}

//_____________________________________________________________________________
PopulationObserver::PopulationObserver(const PopulationObserver& other)
             :Observer(other),
              fPopulationData(NULL),
              fMeasInterval(other.fMeasInterval),
              fLastMeasurementTime(other.fLastMeasurementTime)
{
   // Copy Constructor
   Info("PopulationObserver","Copy Constructor");
   if (other.fPopulationData) fPopulationData = new PopulationData(*(other.fPopulationData));
}

//_____________________________________________________________________________
PopulationObserver& PopulationObserver::operator=(const PopulationObserver& other)
{
   // Assignment
   Info("PopulationObserver","Assignment");
   if(this!=&other) {
      Observer::operator=(other);
      if (fPopulationData) delete fPopulationData;
      fPopulationData = new PopulationData(*(other.fPopulationData));
      fMeasInterval = other.fMeasInterval;
      fLastMeasurementTime = other.fLastMeasurementTime;
   }
   return *this;
}

//_____________________________________________________________________________
PopulationObserver::~PopulationObserver()
{
   // Destructor
   Info("PopulationObserver","Destructor");
   if (fPopulationData != NULL) delete fPopulationData;
}

//_____________________________________________________________________________
void PopulationObserver::RecordEvent(const Point& point, const TVector3& velocity, const std::string& context)
{
   // -- Record the current Population at the current point
   if (context == Context::Population) {
      // The population context means that its time to record the particle population.
      // First check whether it is time to make a population measurement
      double currentTime = Clock::Instance()->GetTime();
      if (Precision::IsEqual(currentTime, (fLastMeasurementTime + fMeasInterval))) {
         // Get the current state of the particle
         const Particle* particle = dynamic_cast<const Particle*>(fSubject);
         const string stateName = particle->GetState().GetName();
         // Update Population data
         fPopulationData->Fill(currentTime, stateName);
         // Update stored value of last measurement
         fLastMeasurementTime = currentTime;
      }
   } else if (context == Context::Creation) {
      // The creation context signifies that the particle has just been instantiated so we
      // shall make a measurement of its initial state
      const Particle* particle = dynamic_cast<const Particle*>(fSubject);
      const string stateName = particle->GetState().GetName();
      fPopulationData->Fill(Clock::Instance()->GetTime(), stateName);
   } 
}

//_____________________________________________________________________________
void PopulationObserver::Reset()
{
   // -- Reset the time of the last measurement to the beginning
   fLastMeasurementTime = 0.0;
}

//_____________________________________________________________________________
void PopulationObserver::WriteToFile(Data& data)
{
   // -- Write out the current observer's data to the observer's branch on the tree
   data.WriteObjectToFile(fPopulationData);
}