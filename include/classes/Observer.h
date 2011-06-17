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

#include "TNamed.h"

#include "SpinData.h"
#include "BounceData.h"
#include "FieldData.h"
#include "Track.h"
#include "PopulationData.h"

namespace Context {
   // Define List of contexts passed to observers to help them distinguish
   // whether the particle's state change is of interest to them
   const std::string Creation = "creation";
   const std::string Step = "step";
   const std::string Spin = "spin";
   const std::string MagField = "magfield";
   const std::string SpecBounce = "specbounce";
   const std::string DiffBounce = "diffbounce";
   const std::string Population = "population";
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Observer -  Abstract Interface                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
class Point;
class Data;

class Observer : public TNamed
{
protected:
   const TObject* fSubject;
   
public:
   Observer();
   Observer(const std::string name);
   Observer(const Observer&);
   virtual ~Observer();
   
   virtual void DefineSubject(const TObject* subject) {fSubject = subject;}
   virtual void RecordEvent(const Point& point, const TVector3& velocity, const std::string& context) = 0;
   virtual void ResetData() = 0;
   virtual void WriteToFile(Data& data) = 0;
   
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
   SpinObserver(const std::string name, double measureInterval);
   SpinObserver(const SpinObserver&);
   virtual ~SpinObserver();
   
   virtual void RecordEvent(const Point& point, const TVector3& velocity, const std::string& context);
   virtual void ResetData();
   virtual void WriteToFile(Data& data);
   
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
   BounceObserver(const std::string name);
   BounceObserver(const BounceObserver&);
   BounceObserver& operator=(const BounceObserver&);
   virtual ~BounceObserver();
   
   virtual void RecordEvent(const Point& point, const TVector3& velocity, const std::string& context);
   virtual void ResetData();
   virtual void WriteToFile(Data& data);
   
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
   TrackObserver(const std::string name, double measureInterval);
   TrackObserver(const TrackObserver&);
   virtual ~TrackObserver();
   
   virtual void RecordEvent(const Point& point, const TVector3& velocity, const std::string& context);
   virtual void ResetData();
   virtual void WriteToFile(Data& data);
   
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
   FieldObserver(const std::string name, const double measureInterval);
   FieldObserver(const FieldObserver&);
   virtual ~FieldObserver();
   
   virtual void RecordEvent(const Point& point, const TVector3& velocity, const std::string& context);
   virtual void ResetData();
   virtual void WriteToFile(Data& data);
   
   ClassDef(FieldObserver, 1)
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    PopulationObserver -                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class PopulationObserver : public Observer
{
private:
   PopulationData* fPopulationData;
   double fMeasInterval;
   double fLastMeasurementTime;
   
public:
   // -- Constructors
   PopulationObserver(const std::string name, const double measureInterval);
   PopulationObserver(const PopulationObserver&);
   PopulationObserver& operator=(const PopulationObserver&);
   virtual ~PopulationObserver();
   
   virtual void RecordEvent(const Point& point, const TVector3& velocity, const std::string& context);
   virtual void ResetData();
   virtual void WriteToFile(Data& data);
   
   ClassDef(PopulationObserver, 1)
};

#endif /* OBSERVER_H */
