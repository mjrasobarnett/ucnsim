// ParabolicMagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_ParabolicMagField
#define ROOT_ParabolicMagField

#include "MagField.h"
#include "TVector3.h"
#include "Particle.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// ParabolicMagField                                                  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
/*
class ParabolicMagField : public MagField {
private:
   Double_t fBMax;   // Field along z-axis, r = 0
   Double_t fParabolicGradient;  // Parabolic Gradient: A, in the formula
   Double_t fFieldRadius;  // Radius of the containing vessel
   
   void GetFieldVector(const TVector3& pos, TVector3& field) const;
   
protected:
   ParabolicMagField(const ParabolicMagField&);
   ParabolicMagField& operator=(const ParabolicMagField&);
   
public:
   ParabolicMagField();
   ParabolicMagField(const std::string& name, const Double_t& maxB, const Double_t& gradient, const Double_t& radius);
   virtual ~ParabolicMagField();
   
   virtual Bool_t Contains(const TVector3& point) const;
   
   
   virtual Bool_t Interact(Particle& particle, const Double_t stepTime) const;
   
   ClassDef(ParabolicMagField, 1)  // parabolic mag field class
};
*/
#endif