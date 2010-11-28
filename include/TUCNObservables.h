// UCN Particle Observables classes
// Author: Matthew Raso-Barnett  
// 22/11/2010

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    This file lists the following Observables:                           //                                                   
//    -  TUCNSpinObserver                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef TUCNOBSERVABLES_H
#define TUCNOBSERVABLES_H

#include "TObject.h"
#include <map>

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNSpinObservables -                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNSpinObservables : public std::map<Double_t, Bool_t>, public TObject
{
private:

public:
   TUCNSpinObservables();
   TUCNSpinObservables(const TUCNSpinObservables&);
   TUCNSpinObservables& operator=(const TUCNSpinObservables&);
   virtual ~TUCNSpinObservables();
   
   ClassDef(TUCNSpinObservables, 1)
};

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNBounceObservables -                                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNBounceObservables : public TObject
{
private:
   Int_t fSpecular;
   Int_t fDiffuse;
   
public:
   TUCNBounceObservables();
   TUCNBounceObservables(const TUCNBounceObservables&);
   TUCNBounceObservables& operator=(const TUCNBounceObservables&);
   virtual ~TUCNBounceObservables();
   
   void RecordSpecular() {fSpecular++;}
   void RecordDiffuse() {fDiffuse++;}
   
   Int_t CountSpecular() const {return fSpecular;}
   Int_t CountDiffuse() const {return fDiffuse;}
   Int_t CountTotal() const {return (fDiffuse+fSpecular);}
   
   ClassDef(TUCNBounceObservables, 1)
};

#endif /*TUCNOBSERVABLES*/