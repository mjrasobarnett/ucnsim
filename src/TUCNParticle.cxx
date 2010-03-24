// UCN class
// Author: Matthew Raso-Barnett  19/01/2009
#include <iostream>
#include <cassert>

#include "Constants.h"
#include "Units.h"

#include "TUCNParticle.h"

#include "TUCNGeoMaterial.h"
#include "TMath.h"
#include "TRandom.h"

using namespace std;

ClassImp(TUCNParticle)

const Double_t    TUCNParticle::fgMass;      // Neutron Mass
const Double_t    TUCNParticle::fgLifetime;   // Neutron Lifetime

//______________________________________________________________________________
TUCNParticle::TUCNParticle()
             :TObject(),
              fX(0.), fY(0.), fZ(0.), fT(0.), fPx(0.), fPy(0.), fPz(0.), fE(0.),
              fDistance(0.), fId(0), fBounces(0), fSpecularBounces(0), fDiffuseBounces(0),
              fRandomSeed(0), fDecayed(kFALSE), fLostToBoundary(kFALSE), fDetected(kFALSE)
{
// -- Default constructor
//   fState = TUCNStatePropagate::Instance();
}


//______________________________________________________________________________
TUCNParticle::TUCNParticle(Int_t id, Double_t* pos, Double_t* mom, Double_t energy, Double_t t)
             :TObject(),
              fX(pos[0]), fY(pos[1]), fZ(pos[2]), fT(t), fPx(mom[0]), fPy(mom[1]), fPz(mom[2]),
              fE(energy), fDistance(0.), fId(id), fBounces(0), fSpecularBounces(0),
              fDiffuseBounces(0), fRandomSeed(0), fDecayed(kFALSE), fLostToBoundary(kFALSE),
              fDetected(kFALSE)
{
// -- Constructor
//   fState = TUCNStatePropagate::Instance();
}

//_____________________________________________________________________________
TUCNParticle::TUCNParticle(const TUCNParticle& part)
             :TObject(part),
              fX(part.fX), fY(part.fY), fZ(part.fZ), fT(part.fT), fPx(part.fPx), fPy(part.fPy),
              fPz(part.fPz), fE(part.fE), fDistance(part.fDistance), fId(part.fId),
              fBounces(part.fBounces), fSpecularBounces(part.fSpecularBounces),
              fDiffuseBounces(part.fDiffuseBounces), fRandomSeed(part.fRandomSeed),
              fDecayed(part.fDecayed), fLostToBoundary(part.fLostToBoundary),
              fDetected(part.fDetected)
{
// -- Copy Constructor
}

//_____________________________________________________________________________
TUCNParticle& TUCNParticle::operator=(const TUCNParticle& part)
{
// -- assignment operator
   if(this!=&part) {
      TObject::operator=(part);
      fX = part.fX;
      fY = part.fY;
      fZ = part.fZ;
      fT = part.fT;
      fPx = part.fPx;
      fPy = part.fPy;
      fPz = part.fPz;
      fE = part.fE;
      fDistance = part.fDistance;
      fId = part.fId;
      fBounces = part.fBounces;
      fSpecularBounces = part.fSpecularBounces;
      fDiffuseBounces = part.fDiffuseBounces;
      fRandomSeed = part.fRandomSeed;
      fDecayed = part.fDecayed;
      fLostToBoundary = part.fLostToBoundary;
      fDetected = part.fDetected;
   }
   return *this;
}

//______________________________________________________________________________
TUCNParticle::~TUCNParticle()
{ 
// -- Destructor	
}

//______________________________________________________________________________
Double_t TUCNParticle::P() const
{
   return TMath::Sqrt(fPx*fPx+fPy*fPy+fPz*fPz);
}

//______________________________________________________________________________
Double_t TUCNParticle::Mass_Kg() const
{
   return this->Mass_eV_c2()*Units::e_SI;
}

//______________________________________________________________________________
Double_t TUCNParticle::Mass_eV_c() const
{
   return this->Mass_eV()/Constants::c_light;
}

//______________________________________________________________________________
Double_t TUCNParticle::Mass_eV_c2() const
{
   return this->Mass_eV()/Constants::c_squared;
}

//______________________________________________________________________________
Double_t TUCNParticle::Rho() const
{
   return TMath::Sqrt(fX*fX+fY*fY+fZ*fZ);
}

//______________________________________________________________________________
Double_t TUCNParticle::Theta() const
{
   return (fPz==0)?TMath::PiOver2():TMath::ACos(fPz/P());
}

//______________________________________________________________________________
Double_t TUCNParticle::Phi() const
{
   return TMath::Pi()+TMath::ATan2(-fPy,-fPx);
}

//______________________________________________________________________________
void TUCNParticle::SetVertex(const Double_t x, const Double_t y, const Double_t z,
                                    const Double_t t)
{
   // Set current vertex to given coords
   fX = x; fY = y; fZ = z; fT = t;
}

//______________________________________________________________________________
void TUCNParticle::SetMomentum(const Double_t px, const Double_t py, const Double_t pz,
                                    const Double_t energy)
{
   // Set current momentum to given coords
   fPx = px; fPy = py; fPz = pz; fE = energy;
}

//______________________________________________________________________________
Bool_t TUCNParticle::IsLostToWall(const TUCNGeoMaterial* wall, const Double_t* normal) const
{
// Calculate whether particle will be absorbed/upscattered by the wall
   Double_t fermiPotential = wall->FermiPotential();
   Double_t eta = wall->Eta();
   if (eta == 0.0) {
      // No wall losses implemented
      return kFALSE;
   }
// cout << "Material: " << wall->GetName() << "\t" << "eta: " << eta << "\t" <<  "fermiPotential: " << fermiPotential << "\t" << endl;
// cout << "nx: " << this->DirX() << "\t" << "ny: " << this->DirY() << "\t" << "nz: " << this->DirZ() << endl;
// cout << "normx: " << normal[0] << "\t" << "normy: " << normal[1] << "\t" << "normz: " << normal[2] << endl;	

   // Take dot product of two unit vectors - normal and direction - to give the angle between them
   Double_t cosTheta = TMath::Abs(this->Nx()*normal[0] + this->Ny()*normal[1] + this->Nz()*normal[2]);
   Double_t energyPerp = this->Energy()*cosTheta*cosTheta;
   if (energyPerp >= fermiPotential) {
      return kTRUE;
   }
   Double_t lossProb = 2.*eta*(TMath::Sqrt(energyPerp/(fermiPotential - energyPerp)));
// cout << "Loss Prob: " << lossProb << "\t" << "EnergyPerp: " << energyPerp/Units::neV << "\t" <<  "cosTheta: " << cosTheta << "\t" << endl;
   // roll dice to see whether particle is lost
   Double_t diceRoll = gRandom->Uniform(0.0, 1.0);
   if (diceRoll <= lossProb) {
//    cout << "DiceRoll: " << diceRoll << "\t" << "LossProb: " << lossProb << endl;
      return kTRUE;
   } else {
      return kFALSE;
   }
}

//______________________________________________________________________________
Double_t TUCNParticle::DiffuseProbability(const Double_t diffuseCoeff, const Double_t* /*normal*/,
                                             const Double_t /*fermiPotential*/) const
{
   // Calculate the probability of making a diffuse bounce - according to formula (from Mike P) prob ~ diffuseCoeff*(Eperp/V)
/*   Double_t cosTheta = TMath::Abs(this->DirX()*normal[0] + this->DirY()*normal[1] + this->DirZ()*normal[2]);
   Double_t energyPerp = this->Energy()*cosTheta*cosTheta;
   Double_t diffProb = diffuseCoeff*energyPerp/fermiPotential;
   assert(diffProb <= 1.0 && diffProb >= 0.0);
   //return diffProb;
*/ // For now just use fixed coefficient until this is properly checked
   return diffuseCoeff;
}

//______________________________________________________________________________
Bool_t TUCNParticle::WillDecay(const Double_t /*timeInterval*/)
{
   // Placeholder for method to calculate probability particle will decay within timeInterval, and then roll the dice!
   return kFALSE;
}






/*//______________________________________________________________________________
void TUCNParticle::SampleMagField(const Double_t integratedField, const Double_t stepTime)
{
   // Adds current field to average field
   Double_t timeAveragedField = integratedField/stepTime;
   fAvgMagField = (fAvgMagField*fFieldPointsSampled + timeAveragedField)/(++fFieldPointsSampled);
}
*/
