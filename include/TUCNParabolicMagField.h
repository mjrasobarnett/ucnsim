// TUCNParabolicMagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_TUCNParabolicMagField
#define ROOT_TUCNParabolicMagField

#include "TUCNMagField.h"
#include "TVector3.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TUCNParabolicMagField                                                  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNParabolicMagField : public TUCNMagField {
private:
   Double_t fBMax;   // Field along z-axis, r = 0
   Double_t fParabolicGradient;  // Parabolic Gradient: A, in the formula
   Double_t fFieldRadius;  // Radius of the containing vessel
   
protected:
   TUCNParabolicMagField(const TUCNParabolicMagField&);
   TUCNParabolicMagField& operator=(const TUCNParabolicMagField&);
   
public:
   TUCNParabolicMagField();
   TUCNParabolicMagField(const std::string& name, const Double_t& maxB, const Double_t& gradient, const Double_t& radius);
   virtual ~TUCNParabolicMagField();
   
   virtual Double_t Bx(const TVector3& pos) const;
   virtual Double_t By(const TVector3& pos) const;
   virtual Double_t Bz(const TVector3& pos) const;
   virtual Double_t B(const TVector3& pos) const;
   
   virtual void GetFieldVector(const TVector3& pos, TVector3& field) const;
   
   ClassDef(TUCNParabolicMagField, 1)  // parabolic mag field class
};

#endif