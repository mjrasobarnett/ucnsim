// UCN Particle State class
// Author: Matthew Raso-Barnett  
// 19/08/2010

#ifndef TUCNSPIN_H
#define TUCNSPIN_H

#include "TObject.h"
#include "TVector3.h"
#include "TComplex.h"


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNSpinor - Quantum mechanical Spinor                               //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNSpinor : public TObject
{
private:
   TComplex fUp;  // Spinor component for Spin 'Up'
   TComplex fDown; // Spinor component for Spin 'Down'
   
public:
   // -- Constructors
   TUCNSpinor();
   TUCNSpinor(const TUCNSpinor&);
   TUCNSpinor& operator=(const TUCNSpinor&);
   virtual ~TUCNSpinor();
   
   // -- Set initial polarisation
   void PolariseUp(const TVector3& axis);
   void PolariseDown(const TVector3& axis);
   
   Bool_t Precess(const TVector3& avgMagField, const Double_t precessTime);
   Double_t CalculateProbSpinUp(const TVector3& axis) const;
   
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(TUCNSpinor, 1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNSpin - Spin vector object                                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNSpin : public TObject
{
private:
   TUCNSpinor fSpinor;
   
   Double_t CalculateProbSpinUp(const TVector3& axis) const;
   
public:
   // -- Constructors
   TUCNSpin();
   TUCNSpin(const TUCNSpin&);
   TUCNSpin& operator=(const TUCNSpin&);
   virtual ~TUCNSpin();
   
   // -- Methods
   Bool_t Precess(const TVector3& avgMagField, const Double_t precessTime);
   Bool_t IsSpinUp(const TVector3& axis) const;
   
   // -- Set initial polarisation
   Bool_t Polarise(const TVector3& axis, const Bool_t up);
   
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(TUCNSpin, 1)
};

#endif  /*TUCNSPIN_H*/
