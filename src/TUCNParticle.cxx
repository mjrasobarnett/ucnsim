// UCN class
// Author: Matthew Raso-Barnett  19/01/2009
#include <iostream>
#include <cassert>

#include "Constants.h"
#include "Units.h"

#include "TUCNParticle.h"
#include "TParticlePDG.h"

using namespace std;

ClassImp(TUCNParticle) 

const Int_t 	TUCNParticle::fgNeutronPDGCode;  //Static Const
const Double_t TUCNParticle::fgDiffuseProb;

//______________________________________________________________________________
TUCNParticle::TUCNParticle()
  				 :TParticle()				
{
//	-- Default constructor
//	Info("TUCNParticle", "Default Constructor");
	this->SetPdgCode(fgNeutronPDGCode);
	fDecayed = kFALSE;
	fLost = kFALSE;
	fDetected = kFALSE;
	fDistance = 0.0;
	fBounces = 0;
	fSpecularBounces = 0;
	fDiffuseBounces = 0;
	fAvgMagField = 0.;
}


//______________________________________________________________________________
TUCNParticle::TUCNParticle(Double_t* pos, Double_t* mom, Double_t kineticEnergy, Double_t startTime)
  				 :TParticle() 
{
//	-- Constructor
//	Info("TUCNParticle", "Constructor");
	this->SetPdgCode(fgNeutronPDGCode);
	this->SetProductionVertex(pos[0], pos[1], pos[2], startTime);
	this->SetMomentum(mom[0], mom[1], mom[2], kineticEnergy);
	fDecayed = kFALSE;
	fLost = kFALSE;
	fDetected = kFALSE;
	fDistance = 0.0;
	fBounces = 0;
	fSpecularBounces = 0;
	fDiffuseBounces = 0;
	fAvgMagField = 0.;
}

//_____________________________________________________________________________
TUCNParticle::TUCNParticle(const TUCNParticle& part)
             :TParticle(part)
{
// -- Copy Constructor
//	Info("TUCNParticle", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNParticle& TUCNParticle::operator=(const TUCNParticle& part)
{
// --assignment operator
	if(this!=&part) {
     TParticle::operator=(part);
	}
	return *this;
}

//______________________________________________________________________________
TUCNParticle::~TUCNParticle()
{ 
// -- Destructor	
//	Info("TUCNParticle", "Destructor");
}

//______________________________________________________________________________
Bool_t TUCNParticle::WillDecay(Double_t timeInterval)
{
	// Placeholder for method to calculate probability particle will decay within timeInterval, and then roll the dice!
	timeInterval = 0.;
	return kFALSE;
}

//______________________________________________________________________________
Double_t TUCNParticle::Dir() const
{
// Returns x direction
	Double_t dirX = this->DirX();
	Double_t dirY = this->DirY();
	Double_t dirZ = this->DirZ();
	Double_t mag = TMath::Sqrt(dirX*dirX + dirY*dirY + dirZ*dirZ);
	return mag;
}

//______________________________________________________________________________
Double_t TUCNParticle::DirX() const
{
// Returns x direction
	if (this->P() == 0.) {
		return 0.;
	}
	Double_t dirX = this->Px()/this->P();
	return dirX;
}

//______________________________________________________________________________
Double_t TUCNParticle::DirY() const
{
// Returns y direction
	if (this->P() == 0.) {
		return 0.;
	}
	Double_t dirY = this->Py()/this->P();
	return dirY;
}

//______________________________________________________________________________
Double_t TUCNParticle::DirZ() const
{
// Returns z direction
	if (this->P() == 0.) {
		return 0.;
	}
	Double_t dirZ = this->Pz()/this->P();
	return dirZ;
}

//______________________________________________________________________________
Double_t TUCNParticle::Velocity() const
{
// Returns velocity in units of M/S
	Double_t mass_c2 = this->Mass_GeV_c2();
	Double_t velocity = TMath::Sqrt(2.0*(this->Energy())/mass_c2);
	return velocity;
}

//______________________________________________________________________________
Double_t TUCNParticle::VelocityX() const
{
// Returns velocity in units of M/S
	return this->Velocity()*this->DirX();
}

//______________________________________________________________________________
Double_t TUCNParticle::VelocityY() const
{
// Returns velocity in units of M/S
	return this->Velocity()*this->DirY();
}

//______________________________________________________________________________
Double_t TUCNParticle::VelocityZ() const
{
// Returns velocity in units of M/S
	return this->Velocity()*this->DirZ();
}

//______________________________________________________________________________
Double_t TUCNParticle::Mass_GeV() const
{
// Returns neutron mass in units of GeV
	const Double_t mass_GeV = (fParticlePDG->Mass()*Units::GeV);
	return mass_GeV;
}

//______________________________________________________________________________
Double_t TUCNParticle::Mass_GeV_c() const
{
// Returns neutron mass in units of GeV/c
	const Double_t mass_GeV = (fParticlePDG->Mass()*Units::GeV);
	Double_t mass_GeV_c  = mass_GeV/(Constants::c_light);
	return mass_GeV_c;
}

//______________________________________________________________________________
Double_t TUCNParticle::Mass_GeV_c2() const
{
// Returns neutron mass in units of GeV/c^2
	const Double_t mass_GeV = (fParticlePDG->Mass()*Units::GeV);
	Double_t mass_GeV_c2 = mass_GeV/(Constants::c_squared);
	return mass_GeV_c2;
}

//______________________________________________________________________________
Double_t TUCNParticle::SampleMagField(const TUCNMagField* magfield, const Int_t stepNumber)
{
// Adds current field to average field
	assert(magfield != NULL);
	const Double_t pos[3] = {this->Vx(), this->Vy(), this->Vz()};
	Double_t currentField = magfield->FieldStrength(pos);
	Double_t totalField = (this->AvgMagField())*(stepNumber-1);
	totalField += currentField;
	assert(stepNumber != 0);
	this->AvgMagField(totalField/stepNumber);
	return this->AvgMagField();
}
