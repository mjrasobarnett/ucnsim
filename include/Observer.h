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

#include "SpinData.h"
#include "BounceData.h"
#include "FieldData.h"
#include "Track.h"

namespace Context {
   // Define List of contexts passed to observers to help them distinguish
   // whether the particle's state change is of interest to them
   const std::string Spin = "spin";
   const std::string SpecBounce = "specbounce";
   const std::string DiffBounce = "diffbounce";
   const std::string Step = "step";
   const std::string MeasureField = "measurefield";
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
protected:
   const TObject* fSubject;
   
public:
   
   virtual void DefineSubject(const TObject* subject) {fSubject = subject;}
   virtual void InitialReading(const TObject* subject) = 0;
   virtual void RecordEvent(const TObject* subject, const std::string& context) = 0;
   virtual void ResetData() = 0;
   virtual void LoadExistingData(TDirectory* const particleDir) = 0;
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
   SpinData *fSpinData;
   double fMeasInterval;
   double fLastMeasurementTime;
   
public:
   // -- Constructors
   SpinObserver(double measureInterval);
   SpinObserver(const SpinObserver&);
   virtual ~SpinObserver();
   
   virtual void InitialReading(const TObject* subject);
   virtual void RecordEvent(const TObject* subject, const std::string& context);
   virtual void ResetData();
   virtual void LoadExistingData(TDirectory* const particleDir);
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
   BounceData *fBounceData;
   
public:
   // -- Constructors
   BounceObserver();
   BounceObserver(const BounceObserver&);
   BounceObserver& operator=(const BounceObserver&);
   virtual ~BounceObserver();
   
   virtual void InitialReading(const TObject* subject);
   virtual void RecordEvent(const TObject* subject, const std::string& context);
   virtual void ResetData();
   virtual void LoadExistingData(TDirectory* const particleDir);
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
   Track *fTrack;
   double fMeasInterval;
   double fLastMeasurementTime;
   
public:
   // -- Constructors
   TrackObserver(double measureInterval);
   TrackObserver(const TrackObserver&);
   virtual ~TrackObserver();
   
   virtual void InitialReading(const TObject* subject);
   virtual void RecordEvent(const TObject* subject, const std::string& context);
   virtual void ResetData();
   virtual void LoadExistingData(TDirectory* const particleDir);
   virtual void WriteToFile(TDirectory* const particleDir);
   
   ClassDef(TrackObserver, 1)
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    FieldObserver -                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class FieldObserver : public Observer
{
private:
   FieldData *fFieldData;
   double fMeasInterval;
   double fLastMeasurementTime;
   
public:
   // -- Constructors
   FieldObserver(double measureInterval);
   FieldObserver(const FieldObserver&);
   virtual ~FieldObserver();
   
   virtual void InitialReading(const TObject* subject);
   virtual void RecordEvent(const TObject* subject, const std::string& context);
   virtual void ResetData();
   virtual void LoadExistingData(TDirectory* const particleDir);
   virtual void WriteToFile(TDirectory* const particleDir);
   
   ClassDef(FieldObserver, 1)
};


#endif /* OBSERVER_H */
