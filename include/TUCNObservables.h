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

#endif /*TUCNOBSERVABLES*/