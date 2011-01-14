// UCN Particle Observables classes
// Author: Matthew Raso-Barnett  
// 22/11/2010

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    This file lists the following Observables:                           //                                                   
//    -  SpinObserver                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef OBSERVABLES_H
#define OBSERVABLES_H

#include <map>
#include "TObject.h"
#include "TPolyLine3D.h"
#include "FieldVertex.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    SpinObservables -                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class SpinObservables : public std::map<Double_t, Bool_t>, public TObject
{
private:

public:
   SpinObservables();
   SpinObservables(const SpinObservables&);
   SpinObservables& operator=(const SpinObservables&);
   virtual ~SpinObservables();
   
   ClassDef(SpinObservables, 1)
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    BounceObservables -                                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class BounceObservables : public TObject
{
private:
   Int_t fSpecular;
   Int_t fDiffuse;
   
public:
   BounceObservables();
   BounceObservables(const BounceObservables&);
   BounceObservables& operator=(const BounceObservables&);
   virtual ~BounceObservables();
   
   void RecordSpecular() {fSpecular++;}
   void RecordDiffuse() {fDiffuse++;}
   
   Int_t CountSpecular() const {return fSpecular;}
   Int_t CountDiffuse() const {return fDiffuse;}
   Int_t CountTotal() const {return (fDiffuse+fSpecular);}
   
   ClassDef(BounceObservables, 1)
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    FieldObservables -                                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class FieldObservables : public std::vector<const FieldVertex*>, public TObject
{
private:
   void PurgeContainer();
   
public:
   FieldObservables();
   FieldObservables(const FieldObservables&);
   virtual ~FieldObservables();
   
   ClassDef(FieldObservables, 1)
};

#endif /*OBSERVABLES*/