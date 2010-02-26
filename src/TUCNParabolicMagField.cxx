// TUCNParabolicMagField
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include "TMath.h"
#include "TUCNParticle.h"
#include "TUCNGravField.h"

#include "TUCNParabolicMagField.h"

using std::cout;
using std::endl;

ClassImp(TUCNParabolicMagField)

//_____________________________________________________________________________
TUCNParabolicMagField::TUCNParabolicMagField()
							 :TUCNMagField()
{
// Default constructor.
	Info("TUCNParabolicMagField", "Dummy Constructor");
} 

//_____________________________________________________________________________
TUCNParabolicMagField::TUCNParabolicMagField(const char *name, Double_t maxB, Double_t parabolicGradient, Double_t fieldRadius)
							 :TUCNMagField(name)
{
// Default constructor.
	Info("TUCNParabolicMagField", "Constructor");
	fBMax = maxB;
	fParabolicGradient = parabolicGradient;
	fFieldRadius = fieldRadius;
	Info("TUCNParabolicMagField", "Field Created with BMax: %f, ParabolicGradient: %f, FieldRadius: %f",fBMax,fParabolicGradient, fFieldRadius);
} 

//_____________________________________________________________________________
TUCNParabolicMagField::TUCNParabolicMagField(const TUCNParabolicMagField& mf)
							 :TUCNMagField(mf),
							  fBMax(mf.fBMax),
							  fParabolicGradient(mf.fParabolicGradient),
							  fFieldRadius(mf.fFieldRadius)
{
// Copy constructor.
} 

//_____________________________________________________________________________
TUCNParabolicMagField &TUCNParabolicMagField::operator=(const TUCNParabolicMagField& mf)
{
// Assignment.
   if(this != &mf) {
		TUCNMagField::operator=(mf);
		fBMax = mf.fBMax;
		fParabolicGradient = mf.fParabolicGradient;
		fFieldRadius = mf.fFieldRadius;
	}
	return *this;
}

//_____________________________________________________________________________
TUCNParabolicMagField::~TUCNParabolicMagField()
{
// Destructor.
	Info("TUCNParabolicMagField", "Destructor");
}   

//_____________________________________________________________________________
void TUCNParabolicMagField::Field(const Double_t* pos, Double_t* field)
{
	// B(r,z) = Bmax - A(r^2)/(R^2) with A=(0.1)Bmax
	
	// For now we assume we are in the local coordinates of the cylinder
	Double_t r = TMath::Sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
	Double_t theta = TMath::ACos(pos[0]/r);
	field[0] = this->FieldStrength(pos)*r*TMath::Cos(theta);
	field[1] = this->FieldStrength(pos)*r*TMath::Sin(theta); 
}   

//_____________________________________________________________________________
Double_t TUCNParabolicMagField::FieldStrength(const Double_t* pos) const
{
	// B(r,z) = Bmax - A(r^2)/(R^2) with A=(0.1)Bmax
	
	// For now we assume we are in the local coordinates of the cylinder
	Double_t r = TMath::Sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
	Double_t B = fBMax - (fParabolicGradient*r*r)/(fFieldRadius*fFieldRadius);
	return B;
}

//_____________________________________________________________________________
Double_t TUCNParabolicMagField::IntegratedField(const Double_t stepTime, const TUCNParticle* initialState, const TUCNGravField* /*gravField*/) const
{
	// Calculate the integrated field strength along the path defined by stepTime and initialState
	// This only works for vertical Tube setup currently
	
	// -- Definitions for calculation
	Double_t integratedField = 0.0;
	Double_t alpha = fParabolicGradient/(fFieldRadius*fFieldRadius);
	Double_t x0 = initialState->Vx();
	Double_t vx = initialState->VelocityX();
	Double_t y0 = initialState->Vy();
	Double_t vy = initialState->VelocityY();
	
	Double_t r = TMath::Sqrt(x0*x0 + y0*y0);
//	cout << "r/R: " << r/fFieldRadius << "(r/R)^2: " << r*r/(fFieldRadius*fFieldRadius) << endl;
	
	// -- Calculate integrated field
	
	// Currently this is wrong. Needs to be corrected for the LOCAL field vector, and gx, gy, not just g. 
	// For now we just assume the volume is set-up as normal, and use the non-grav field case (equivalent to vertical cylinder with grav field along z)
/*	if (gravField) {
		// Intgrate along gravitational trajectory
		Double_t g = gravField->GravAcceleration();
		Double_t term1 = (fBMax - alpha*(x0*x0 + y0*y0))*stepTime;
		Double_t term2 = -1.*alpha*(x0*vx + y0*vy)*stepTime*stepTime;
		Double_t term3 = -1.*(alpha/3.)*(vx*vx + vy*vy - g*(x0 + y0))*TMath::Power(stepTime, 3.0);
		Double_t term4 = (alpha*g/4.)*(vx + vy)*TMath::Power(stepTime, 4.0);
		Double_t term5 = -1.*(alpha*g/10.)*TMath::Power(stepTime, 5.0);
		
		integratedField = term1 + term2 + term3 + term4 + term5;
		return integratedField;
		
	} else {
		// Integrate along straight line
		Double_t term1 = (fBMax - alpha*(x0*x0 + y0*y0))*stepTime;
		Double_t term2 = -1.*alpha*(x0*vx + y0*vy)*stepTime*stepTime;
		Double_t term3 = -1.*(alpha/3.)*(vx*vx + vy*vy)*TMath::Power(stepTime, 3.0);
		
		integratedField = term1 + term2 + term3;
		return integratedField;
	}
*/
	
	// Integrate along straight line
	Double_t term1 = (fBMax - alpha*(x0*x0 + y0*y0))*stepTime;
	Double_t term2 = -1.*alpha*(x0*vx + y0*vy)*stepTime*stepTime;
	Double_t term3 = -1.*(alpha/3.)*(vx*vx + vy*vy)*TMath::Power(stepTime, 3.0);
	
	integratedField = term1 + term2 + term3;
	return integratedField;
}