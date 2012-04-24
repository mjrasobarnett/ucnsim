// UCN Particle State class
// Author: Matthew Raso-Barnett  
// 19/08/2010

#ifndef SPIN_H
#define SPIN_H

#include "TObject.h"
#include "TVector3.h"
#include "TComplex.h"


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Spinor - Quantum mechanical Spinor                               //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class Spinor : public TObject
{
private:
   Double_t fUpRe, fUpIm;  // Spinor components for Spin 'Up'
   Double_t fDownRe, fDownIm; // Spinor components for Spin 'Down'
   
public:
   // -- Constructors
   Spinor();
   Spinor(const Spinor&);
   Spinor& operator=(const Spinor&);
   virtual ~Spinor();
   
   // -- Set initial polarisation
   void PolariseUp(const TVector3& axis);
   void PolariseDown(const TVector3& axis);
   
   Bool_t Precess(const TVector3& avgMagField, const Double_t precessTime);
   Double_t CalculateProbSpinUp(const TVector3& axis) const;
   
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(Spinor, 1)
};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Spin - Spin vector object                                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class Spin : public TObject
{
private:
   Spinor fSpinor;
   
public:
   // -- Constructors
   Spin();
   Spin(const Spin&);
   Spin& operator=(const Spin&);
   virtual ~Spin();
   
   // -- Methods
   Bool_t Precess(const TVector3& avgMagField, const Double_t precessTime);
   Bool_t IsSpinUp(const TVector3& axis) const;
   Double_t CalculateProbSpinUp(const TVector3& axis) const;
   
   // -- Set initial polarisation
   void Polarise(const TVector3& axis, const Bool_t up);
   void PolariseRandomly();   
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(Spin, 1)
};

#endif  /*SPIN_H*/
