/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TRandom3a
#define ROOT_TRandom3a

/**
* TRandom3a
* random number generator class: Mersenne Twistor
* This is a copy of TRandom3, which just adds the ability to access
* the internal state of the generator
**/

#ifndef ROOT_TRandom
#include "TRandom.h"
#endif

class TRandom3State : public TObject {
private:
   UInt_t   fMt[624];
   Int_t    fCount624;
   
   friend class TRandom3a;

public:
   TRandom3State();
   TRandom3State(const UInt_t* mt, const Int_t count624);
   virtual ~TRandom3State();
   
   virtual TRandom3State* Clone() const;
   
   ClassDef(TRandom3State,1)
};


class TRandom3a : public TRandom {
private:
   UInt_t   fMt[624];
   Int_t    fCount624;

public:
   TRandom3a(UInt_t seed=4357);
   virtual ~TRandom3a();
   virtual  Double_t  Rndm(Int_t i=0);
   virtual  void      RndmArray(Int_t n, Float_t *array);
   virtual  void      RndmArray(Int_t n, Double_t *array);
   virtual  void      SetSeed(UInt_t seed=0);
   
   TRandom3State      GetState() const;
   void               SetState(const TRandom3State& state);
   
   ClassDef(TRandom3a,1)  //Random number generator: Mersenne Twistor
};

#endif
