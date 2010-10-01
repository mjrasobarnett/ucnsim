// TUCNParabolicMagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_TUCNParabolicMagField
#define ROOT_TUCNParabolicMagField

#include "TUCNMagField.h"
#include "TVector3.h"
#include "TUCNParticle.h"
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
   
   virtual void GetFieldVector(const TVector3& pos, TVector3& field) const;
   
   virtual Bool_t Interact(TUCNParticle& particle, const Double_t stepTime) const;
   
   ClassDef(TUCNParabolicMagField, 1)  // parabolic mag field class
};

#endif