// UCN class
// Author: Matthew Raso-Barnett  19/01/2009
#include <iostream>
#include <cassert>

#include "Constants.h"
#include "Units.h"

#include "TUCNParticle.h"
#include "TUCNGeoMaterial.h"
#include "TRandom.h"
#include "TParticlePDG.h"

using namespace std;

ClassImp(TUCNParticle) 

const Int_t 	TUCNParticle::fgNeutronPDGCode;  //Static Const

//______________________________________________________________________________
TUCNParticle::TUCNParticle()
  				 :TParticle()				
{
//	-- Default constructor
//	Info("TUCNParticle", "Default Constructor");
	this->SetPdgCode(fgNeutronPDGCode);
	fDecayed = kFALSE;
	fLostToBoundary = kFALSE;
	fDetected = kFALSE;
	fDistance = 0.0;
	fBounces = 0;
	fSpecularBounces = 0;
	fDiffuseBounces = 0;
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
	fLostToBoundary = kFALSE;
	fDetected = kFALSE;
	fDistance = 0.0;
	fBounces = 0;
	fSpecularBounces = 0;
	fDiffuseBounces = 0;
}

//_____________________________________________________________________________
TUCNParticle::TUCNParticle(const TUCNParticle& part)
             :TParticle(part),
				  fDecayed(part.fDecayed),
				  fLostToBoundary(part.fLostToBoundary),
				  fDetected(part.fDetected),
				  fDistance(part.fDistance),
				  fBounces(part.fBounces),
				  fSpecularBounces(part.fSpecularBounces),
				  fDiffuseBounces(part.fDiffuseBounces),
				  fAvgMagField(part.fAvgMagField)
{
// -- Copy Constructor
	Info("TUCNParticle", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNParticle& TUCNParticle::operator=(const TUCNParticle& part)
{
// --assignment operator
	if(this!=&part) {
		TParticle::operator=(part);
		fDecayed = part.fDecayed;
		fLostToBoundary = part.fLostToBoundary;
		fDetected = part.fDetected;
		fDistance = part.fDistance;
		fBounces = part.fBounces;
		fSpecularBounces = part.fSpecularBounces;
		fDiffuseBounces = part.fDiffuseBounces;
		fAvgMagField = part.fAvgMagField;
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
// Returns direction mag (1?)
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
Bool_t TUCNParticle::IsLostToWall(TUCNGeoMaterial* wall, const Double_t* normal) const
{
// Calculate whether particle will be absorbed/upscattered by the wall
	return kFALSE;
	Double_t fermiPotential = wall->FermiPotential();
	Double_t eta = wall->Eta();
//	cout << "Material: " << wall->GetName() << "\t" << "eta: " << eta << "\t" <<  "fermiPotential: " << fermiPotential << "\t" << endl;
//	cout << "nx: " << this->DirX() << "\t" << "ny: " << this->DirY() << "\t" << "nz: " << this->DirZ() << endl;
//	cout << "normx: " << normal[0] << "\t" << "normy: " << normal[1] << "\t" << "normz: " << normal[2] << endl;	
	// Take dot product of two unit vectors - normal and direction vector - to give the angle between them
	Double_t cosTheta = TMath::Abs(this->DirX()*normal[0] + this->DirY()*normal[1] + this->DirZ()*normal[2]);
	Double_t energyPerp = this->Energy()*cosTheta*cosTheta;
	if (energyPerp >= fermiPotential) {
		return kTRUE;
	}
	Double_t lossProb = 2.*eta*(TMath::Sqrt(energyPerp/(fermiPotential - energyPerp)));
//	cout << "Loss Prob: " << lossProb << "\t" << "EnergyPerp: " << energyPerp/Units::neV << "\t" <<  "cosTheta: " << cosTheta << "\t" << endl;
	// roll dice to see whether particle is lost
	Double_t diceRoll = gRandom->Uniform(0.0, 1.0);
	if (diceRoll <= lossProb) {
//		cout << "DiceRoll: " << diceRoll << "\t" << "LossProb: " << lossProb << endl;
		return kTRUE;
	} else {
		return kFALSE;
	}
}

//______________________________________________________________________________
Double_t	TUCNParticle::DiffuseProbability(const Double_t diffuseCoeff, const Double_t* normal, const Double_t fermiPotential) const
{
	// Calculate the probability of making a diffuse bounce - according to formula (from Mike P) prob ~ diffuseCoeff*(Eperp/V)
	Double_t cosTheta = TMath::Abs(this->DirX()*normal[0] + this->DirY()*normal[1] + this->DirZ()*normal[2]);
	Double_t energyPerp = this->Energy()*cosTheta*cosTheta;
	Double_t diffProb = diffuseCoeff*energyPerp/fermiPotential;
	assert(diffProb <= 1.0 && diffProb >= 0.0);
	//return diffProb;
	return diffuseCoeff;
}

//______________________________________________________________________________
void TUCNParticle::SampleMagField(const Double_t integratedField, const Double_t stepTime)
{
	// Adds current field to average field
	Double_t timeAveragedField = integratedField/stepTime;
	fAvgMagField.push_back(timeAveragedField);
}

//______________________________________________________________________________
Double_t	TUCNParticle::AvgMagField()
{
	Double_t totalAvgField = 0.;
	for (UInt_t i = 0; i < fAvgMagField.size(); i++) {
		totalAvgField += fAvgMagField.at(i);
	}
	Double_t avgField = (totalAvgField/(static_cast<Double_t>(fAvgMagField.size())));
	return avgField;
}

//______________________________________________________________________________
Double_t	TUCNParticle::AvgMagFieldError(Double_t avgMagField)
{
	Double_t value = 0.;
	UInt_t n = fAvgMagField.size();
	for (UInt_t i = 0; i < n; i++) {
		value += TMath::Power(avgMagField - fAvgMagField.at(i), 2.0);
	}
	Double_t stdError = TMath::Sqrt(value/static_cast<Double_t>(n*(n-1)));
	return stdError;
}
