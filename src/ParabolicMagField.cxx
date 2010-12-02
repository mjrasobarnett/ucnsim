// ParabolicMagField
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include "TMath.h"
#include "ParabolicMagField.h"

using std::string;

ClassImp(ParabolicMagField)

//_____________________________________________________________________________
ParabolicMagField::ParabolicMagField()
                      :MagField()
{
// Default constructor.
   Info("ParabolicMagField", "Dummy Constructor");
} 

//_____________________________________________________________________________
ParabolicMagField::ParabolicMagField(const string& name, const Double_t& bMax, const Double_t& gradient, const Double_t& radius)
                      :MagField(name)
{
// Default constructor.
   Info("ParabolicMagField", "Constructor");
   fBMax = bMax;
   fParabolicGradient = gradient;
   fFieldRadius = radius;
} 

//_____________________________________________________________________________
ParabolicMagField::ParabolicMagField(const ParabolicMagField& other)
                      :MagField(other),
                       fBMax(other.fBMax),
                       fParabolicGradient(other.fParabolicGradient),
                       fFieldRadius(other.fFieldRadius)
{
// Copy constructor.
} 

//_____________________________________________________________________________
ParabolicMagField &ParabolicMagField::operator=(const ParabolicMagField& other)
{
// Assignment.
   if(this != &other) {
      MagField::operator=(other);
      fBMax = other.fBMax;
      fParabolicGradient = other.fParabolicGradient;
      fFieldRadius = other.fFieldRadius;
   }
   return *this;
}

//_____________________________________________________________________________
ParabolicMagField::~ParabolicMagField()
{
// Destructor.
   Info("ParabolicMagField", "Destructor");
}   

//_____________________________________________________________________________
Bool_t ParabolicMagField::Contains(const TVector3& /*point*/) const
{
   return kFALSE;
}

//_____________________________________________________________________________
void ParabolicMagField::GetFieldVector(const TVector3& /*pos*/, TVector3& /*field*/) const
{
   // B(r,z) = Bmax - A(r^2)/(R^2) with A=(0.1)Bmax
   // For now we assume we are in the local coordinates of the cylinder
/*   Double_t r = TMath::Sqrt(pos.X()*pos.X() + pos.Y()*pos.Y());
   Double_t theta = TMath::ACos(pos.X()/r);
   Double_t bMag = this->B(pos);
*/
}

//______________________________________________________________________________
Bool_t ParabolicMagField::Interact(Particle& /*particle*/, const Double_t /*stepTime*/) const
{
   
   return kTRUE;
}

/*
//_____________________________________________________________________________
Double_t ParabolicMagField::IntegratedField(const Double_t stepTime, const Particle* initialState, const GravField* gravField) const
{
   // Calculate the integrated field strength along the path defined by stepTime and initialState
   // This only works for vertical Tube setup currently
   
   // -- Definitions for calculation
   Double_t integratedField = 0.0;
   Double_t alpha = fParabolicGradient/(fFieldRadius*fFieldRadius);
   Double_t x0 = initialState->X();
   Double_t vx = initialState->Vx();
   Double_t y0 = initialState->Y();
   Double_t vy = initialState->Vy();   
   
   // Integrate along straight line
   Double_t term1 = (fBMax - alpha*(x0*x0 + y0*y0))*stepTime;
   Double_t term2 = -1.*alpha*(x0*vx + y0*vy)*stepTime*stepTime;
   Double_t term3 = -1.*(alpha/3.)*(vx*vx + vy*vy)*TMath::Power(stepTime, 3.0);
   
   integratedField = term1 + term2 + term3;
   return integratedField;
}
*/
