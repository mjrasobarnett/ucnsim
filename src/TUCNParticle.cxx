// UCN class
// Author: Matthew Raso-Barnett  19/01/2009
#include <iostream>
#include <cassert>
#include <iomanip>
#include <cassert>
#include <vector>
#include <stdexcept>

#include "Constants.h"
#include "Units.h"

#include "TUCNParticle.h"

#include "TUCNGeoMaterial.h"

#include "TMath.h"
#include "TRandom.h"
#include "TGeoNode.h"
#include "TGeoNavigator.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"


#include "TUCNRun.h"
#include "TUCNConfigFile.h"
#include "TUCNGeoMaterial.h"
#include "TUCNFieldManager.h"
#include "TUCNMagField.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"
#include "TUCNData.h"
#include "TUCNGeoBBox.h"
#include "TUCNParabola.h"

#include "TGeoNode.h"
#include "TGeoPatternFinder.h"
#include "TGeoVoxelFinder.h"
#include "TParticle.h"
#include "TGeoNavigator.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TPolyMarker3D.h"
#include "TGeoManager.h"
#include "TRandom.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TVirtualGeoTrack.h"

using namespace std;

//#define VERBOSE_MODE

ClassImp(TUCNParticle)

const Double_t    TUCNParticle::fgMass;      // Neutron Mass
const Double_t    TUCNParticle::fgLifetime;   // Neutron Lifetime

//______________________________________________________________________________
TUCNParticle::TUCNParticle()
             :TObject(),
              fX(0.), fY(0.), fZ(0.), fT(0.), fPx(0.), fPy(0.), fPz(0.), fE(0.),
              fDistance(0.), fId(0), fBounces(0), fSpecularBounces(0), fDiffuseBounces(0),
              fRandomSeed(0), fDecayed(kFALSE), fLostToBoundary(kFALSE), fDetected(kFALSE),
              fUCNNextNode(0), fStepTime(0), fUCNIsStepEntering(kFALSE), fUCNIsStepExiting(kFALSE),
              fUCNIsOutside(kFALSE), fUCNIsOnBoundary(kFALSE)
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
              fDetected(kFALSE), fUCNNextNode(0), fStepTime(0), fUCNIsStepEntering(kFALSE),
              fUCNIsStepExiting(kFALSE), fUCNIsOutside(kFALSE), fUCNIsOnBoundary(kFALSE)
{
// -- Constructor
//   fState = TUCNStatePropagate::Instance();
}

//_____________________________________________________________________________
TUCNParticle::TUCNParticle(const TUCNParticle& p)
             :TObject(p),
              fX(p.fX), fY(p.fY), fZ(p.fZ), fT(p.fT), fPx(p.fPx), fPy(p.fPy),
              fPz(p.fPz), fE(p.fE), fDistance(p.fDistance), fId(p.fId),
              fBounces(p.fBounces), fSpecularBounces(p.fSpecularBounces),
              fDiffuseBounces(p.fDiffuseBounces), fRandomSeed(p.fRandomSeed),
              fDecayed(p.fDecayed), fLostToBoundary(p.fLostToBoundary),
              fDetected(p.fDetected), fUCNNextNode(p.fUCNNextNode), fStepTime(p.fStepTime),
              fUCNIsStepEntering(p.fUCNIsStepEntering), fUCNIsStepExiting(p.fUCNIsStepExiting),
              fUCNIsOutside(p.fUCNIsOutside), fUCNIsOnBoundary(p.fUCNIsOnBoundary)
{
// -- Copy Constructor
}

//_____________________________________________________________________________
TUCNParticle& TUCNParticle::operator=(const TUCNParticle& p)
{
// -- assignment operator
   if(this!=&p) {
      TObject::operator=(p);
      fX = p.fX;
      fY = p.fY;
      fZ = p.fZ;
      fT = p.fT;
      fPx = p.fPx;
      fPy = p.fPy;
      fPz = p.fPz;
      fE = p.fE;
      fDistance = p.fDistance;
      fId = p.fId;
      fBounces = p.fBounces;
      fSpecularBounces = p.fSpecularBounces;
      fDiffuseBounces = p.fDiffuseBounces;
      fRandomSeed = p.fRandomSeed;
      fDecayed = p.fDecayed;
      fLostToBoundary = p.fLostToBoundary;
      fDetected = p.fDetected;
      fUCNNextNode = p.fUCNNextNode; 
      fStepTime = p.fStepTime;
      fUCNIsStepEntering = p.fUCNIsStepEntering;
      fUCNIsStepExiting = p.fUCNIsStepExiting;
      fUCNIsOutside = p.fUCNIsOutside;
      fUCNIsOnBoundary = p.fUCNIsOnBoundary;
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

//_____________________________________________________________________________
void  TUCNParticle::Print(const Option_t* /*option*/) const
{
   cout << "Particle: " << this->Id() << "\t" << "Time Elapsed: " << this->T() << endl;
   cout << "Vertex (m): " << "X:" << this->X() << "\t" << "Y:" << this->Y();
   cout << "\t" << "Z:" << this->Z() << endl;
   cout << "Dir: " << "Nx:" << this->Nx()  << "\t" << "Ny:" << this->Ny();
   cout << "\t" << "Nz:" << this->Nz() << endl;
   cout << "Vel (m/s): " << "Vx:" << this->Vx()  << "\t" << "Vy:" << this->Vy();
   cout << "\t" << "Vz:" << this->Vz() << "\t" << "V:" << this->V() << endl;
   cout << "Mom (eV): " << "X:" << this->Px()  << "\t" << "Y:" << this->Py();
   cout << "\t" << "Z:" << this->Pz() << "\t" << "P:" << this->P() << endl;
   cout << "Energy (neV): " << this->Energy() / Units::neV << endl;
}

/*//______________________________________________________________________________
void TUCNParticle::SampleMagField(const Double_t integratedField, const Double_t stepTime)
{
   // Adds current field to average field
   Double_t timeAveragedField = integratedField/stepTime;
   fAvgMagField = (fAvgMagField*fFieldPointsSampled + timeAveragedField)/(++fFieldPointsSampled);
}
*/


//_____________________________________________________________________________
Bool_t TUCNParticle::Propagate(TUCNRun* run, TGeoNavigator* navigator, TUCNFieldManager* fieldManager)
{
   // Propagate track through geometry until it is either stopped or the runTime has been reached
   // Track passed MUST REFERENCE A TUCNPARTICLE as its particle type. 
   // UNITS:: runTime, stepTime in Seconds

   ///////////////////////////////////////////////////////////////////////////////////////	
   // -- 1. Initialise Track
   // Initialise track - Sets navigator's current point/direction/node to that of the particle
   navigator->InitTrack(this->X(),this->Y(),this->Z(),this->Nx(),this->Ny(),this->Nz());                  
   #ifdef VERBOSE_MODE
      cout << "Propagate - Starting Run - Max time (seconds): " <<  run->RunTime() << endl;
   #endif
   
   // -- Check that Particle has not been initialised inside a boundary or detector		
   TUCNGeoMaterial* material = static_cast<TUCNGeoMaterial*>(
                                 navigator->GetCurrentVolume()->GetMaterial());
   if (material->IsTrackingMaterial() == kFALSE) {
      cout << "Particle: " << this->Id() << " initialised inside boundary of ";
      cout << navigator->GetCurrentVolume()->GetName() << endl;
      return kFALSE;
   }
   
   ///////////////////////////////////////////////////////////////////////////////////////	
   // -- 2. Propagation Loop
   Int_t stepNumber;
   for (stepNumber = 1 ; ; stepNumber++) {
      #ifdef VERBOSE_MODE		
         cout << endl << "-------------------------------------------------------" << endl;
         cout << "STEP " << stepNumber << "\t" << this->T() << " s" << "\t";
         cout << navigator->GetCurrentNode()->GetName() << endl;	
      #endif
      // -- Calculate the Next StepTime (i.e: are there any factors that reduce the maximum
      // -- step size before we work out boundary distance)
      Double_t stepTime = this->DetermineNextStepTime(run->MaxStepTime(), run->RunTime());

      // -- Make a step
      if (this->MakeStep(stepTime, navigator, fieldManager) == kFALSE) {
         Error("Propagate","Error in Step Number %i. Step Failed to complete.", stepNumber);
         return kFALSE;	
      }
      // -- Determine Particle destination
      // Has lost flag been set?
      if (this->LostToBoundary() == kTRUE) {
         run->IncrementLostToBoundary();
         break; // -- End Propagtion Loop
      // Has detected flag been set?
      } else if (this->Detected() == kTRUE) {
         run->IncrementDetected();
         break; // -- End Propagation Loop
      // Has particle decayed within steptime?
      } else if (this->WillDecay(this->GetStepTime()) == kTRUE) {
         run->IncrementDecayed();
         this->Decayed(kTRUE); // Set Decay Flag
         break; // -- End Propagation Loop
      // -- Have we reached the maximum runtime?
      } else if (this->T() >= run->RunTime()) {
         break; // -- End Propagation Loop
      }
   }
   // -- END OF PROPAGATION LOOP
//	cout << "FINAL STATUS: " << "Track: " << track->GetId() << "\t" << "Steps taken: " << stepNumber << "\t";
//	cout << "Time: " << particle->T() << "s" << "\t" << "Final Medium: " << navigator->GetCurrentNode()->GetMedium()->GetName() << "\t";
//	cout << "Bounces: " << particle->Bounces() << "\t" << "Diffuse: " << particle->DiffuseBounces() << "\t" << "Specular: " << particle->SpecularBounces() << endl;
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNParticle::MakeStep(Double_t stepTime, TGeoNavigator* navigator, TUCNFieldManager* fieldManager)
{
   // -- Find time to reach next boundary and step along parabola
   
   ///////////////////////////////////////////////////////////////////////////////////////	
   // -- Get the Fields
   TUCNGravField* gravField = 0;
   gravField = fieldManager->GravField();
   /*   
   TUCNMagField* magField = 0;
   // -- Check if we are to sample the mag field
   if (fSampleMagField == kTRUE) {
      magField = fieldManager->MagField();
      if (magField == NULL) {
         Error("PropagateTrack","Configuration asks to sample mag field, but no field can be found!");
         return kFALSE;
      }
   }
   */
   #ifdef VERBOSE_MODE	
      cout << endl << "------------------- START OF STEP ----------------------" << endl;
      this->Print(); // Print verbose
      cout << "Steptime (s): " << stepTime << endl;
      cout << "-----------------------------" << endl;
      cout << "Navigator's Current Node: " << navigator->GetCurrentNode()->GetName() << endl;
      cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
      cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
      cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
      cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
      cout << "-----------------------------" << endl << endl;
   #endif
   
   // -- Store the Initial Node and Initial Matrix
   TGeoNode* initialNode = 0;
   TGeoMatrix* initialMatrix = 0;
   TGeoNode* nextNode = 0;
   TGeoMatrix* nextMatrix = 0;
   
   initialNode = navigator->GetCurrentNode();
   initialMatrix = navigator->GetCurrentMatrix();
   
   // -- Determine the current local coordinates
   Double_t* currentGlobalPoint = 0;
   Double_t initialLocalPoint[3] = {0.,0.,0.};
   Double_t nextLocalPoint[3] = {0.,0.,0.};
   
   currentGlobalPoint = const_cast<Double_t*>(navigator->GetCurrentPoint());
   initialMatrix->MasterToLocal(currentGlobalPoint,&initialLocalPoint[0]);
   
   // -- Check that current point is located within the current volume
   if (!navigator->IsSameLocation(currentGlobalPoint[0], currentGlobalPoint[1], currentGlobalPoint[2], kFALSE)) {
      Error("MakeStep","1. Initial Point is not contained in Current Node, according to Navigator::IsSameLocation");
      cout << "Current Node: " << initialNode->GetName() << endl;
      cout << "Global Point: " << "X:" << currentGlobalPoint[0] << "\t";
      cout << "Y:" << currentGlobalPoint[1] << "\t" << "Z:" << currentGlobalPoint[2] << endl;
      cout << "Local Point: " << "X:" << initialLocalPoint[0] << "\t";
      cout << "Y:" << initialLocalPoint[1] << "\t" << "Z:" << initialLocalPoint[2] << endl;
      cout << "Current Volume Contains Local Point: ";
      cout << initialNode->GetVolume()->GetShape()->Contains(initialLocalPoint) << endl;
      cout << "Find Node Result: "    << navigator->FindNode()->GetName() << endl;
      return kFALSE;
   }

   if (!initialNode->GetVolume()->GetShape()->Contains(initialLocalPoint)) {
      Error("MakeStep","1. Initial Point is not contained in Current Node, according to Shape::Contains");
      cout << "Current Node: " << initialNode->GetName() << endl;
      cout << "Global Point: " << "X:" << currentGlobalPoint[0] << "\t";
      cout << "Y:" << currentGlobalPoint[1] << "\t" << "Z:" << currentGlobalPoint[2] << endl;
      cout << "Local Point: " << "X:" << initialLocalPoint[0] << "\t";
      cout << "Y:" << initialLocalPoint[1] << "\t" << "Z:" << initialLocalPoint[2] << endl;
      cout << "Current Volume Contains Local Point: ";
      cout << initialNode->GetVolume()->GetShape()->Contains(initialLocalPoint) << endl;
      cout << "Find Node Result: "    << navigator->FindNode()->GetName() << endl;
      return kFALSE;
   }
   
   // -- We now should be sure we have begun in the current volume. 	
   // -- Save Path to current node - we will want to return to this in the event we make a bounce
   const char* initialPath = navigator->GetPath();
   #ifdef VERBOSE_MODE	
      cout << "Current PATH: " << initialPath << endl;
   #endif
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Find Next Boundary
   if (gravField == NULL) {
      // CASE 1; No Gravitational Field - Straight line tracking
      if (navigator->FindNextBoundaryAndStep(stepTime) == NULL) {
         Error("MakeStep", "FindNextBoundaryAndStep has failed to find the next node");
         return kFALSE;
      }
      // (Re-)Calculate the time travelled (approximately) from the navigator's stepsize
      Double_t timeTravelled = (this->V() == 0. ? 0. : navigator->GetStep()/this->V()); 
      stepTime = timeTravelled;
   } else {
      // CASE 2; Grav Field present - tracking along parabolic trajectories
      // -- Propagate Point by StepTime along Parabola
      if (this->ParabolicBoundaryFinder(stepTime, navigator, gravField) == NULL) {
         Error("MakeStep", "FindNextBoundaryAndStepAlongParabola has failed to find the next node");
         return kFALSE;	
      }
   }
   
   stepTime = this->GetStepTime();
   // -- Update particle properties
   this->Update(navigator, stepTime, gravField);
   
   #ifdef VERBOSE_MODE	
      cout << endl << "------------------- AFTER STEP ----------------------" << endl;
      this->Print(); // Print verbose
      cout << "Steptime (s): " << stepTime << endl;
      cout << "-----------------------------" << endl;
      cout << "Navigator's Current Node: " << navigator->GetCurrentNode()->GetName() << endl;
      cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
      cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
      cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
      cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
      cout << "-----------------------------" << endl << endl;
   #endif
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Now we need to determine where we have ended up, and to examine whether
   // -- the current volume is the point's true container
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Get the returned Node and Matrix
   nextNode = navigator->GetCurrentNode();
   nextMatrix = navigator->GetCurrentMatrix();

   // -- If we are in the same node as before, then we have not crossed any boundary
   if (nextNode == initialNode) {
      // -- If the returned node is the same as before, the matrices should match up
      assert(nextMatrix == initialMatrix);
   }
   
   // -- Get the current coordinates
   currentGlobalPoint = const_cast<Double_t*>(navigator->GetCurrentPoint());
   nextMatrix->MasterToLocal(currentGlobalPoint,&nextLocalPoint[0]);
   Double_t tempLocalPoint[3] = {0.,0.,0.};
   initialMatrix->MasterToLocal(currentGlobalPoint,&tempLocalPoint[0]);
   
   #ifdef VERBOSE_MODE	
      cout << "-----------------------------" << endl;
      cout << "Final Node: " << nextNode->GetName() << endl;
      cout << "Coords local to Final Node: " << endl;
      cout << "X:" << nextLocalPoint[0] << "\t" << "Y:" << nextLocalPoint[1] << "\t";
      cout << "Z:" << nextLocalPoint[2] << endl;
      cout << "Final Node Contains Current Point: ";
      cout << nextNode->GetVolume()->GetShape()->Contains(nextLocalPoint) << endl;
      cout << "-----------------------------" << endl;
      cout << "Initial Node: " << initialNode->GetName() << endl;
      cout << "Coords local to Initial Node: " << endl;
      cout << "X:" << tempLocalPoint[0] << "\t" << "Y:" << tempLocalPoint[1] << "\t";
      cout << "Z:" << tempLocalPoint[2] << endl;
      cout << "Sqrt(X^2 + Y^2): ";
      cout << TMath::Sqrt(tempLocalPoint[0]*tempLocalPoint[0]+tempLocalPoint[1]*tempLocalPoint[1]) << endl;
      cout << "Initial Node Contains Current Point: ";
      cout << initialNode->GetVolume()->GetShape()->Contains(tempLocalPoint) << endl;
      cout << "-----------------------------" << endl;
      cout << "Initial is Parent of Final: ";
      cout << (nextNode->GetMotherVolume() == initialNode->GetVolume() ? 1 : 0) << endl;
      cout << "Initial is Daughter of Final: ";
      cout << (initialNode->GetMotherVolume() == nextNode->GetVolume() ? 1 : 0) << endl;
      cout << "-----------------------------" << endl << endl;
   #endif
   
   // -- Now check that current point is exclusively located within the current volume
   if ( !(navigator->IsSameLocation(currentGlobalPoint[0], currentGlobalPoint[1], currentGlobalPoint[2], kFALSE)) ) {
      #ifdef VERBOSE_MODE
         Warning("MakeStep","2. Next Point is not contained in Current Node, according to Navigator::IsSameLocation");
         cout << "Current Node: " << nextNode->GetName() << endl;
         cout << "Global Point: ";
         cout << "X:" << currentGlobalPoint[0] << "\t" << "Y:" << currentGlobalPoint[1] << "\t";
         cout << "Z:" << currentGlobalPoint[2] << endl;
         cout << "Local Point: ";
         cout << "X:" << nextLocalPoint[0] << "\t" << "Y:" << nextLocalPoint[1] << "\t";
         cout << "Z:" << nextLocalPoint[2] << endl;
         cout << "Sqrt(X^2 + Y^2): ";
         cout << TMath::Sqrt(TMath::Power(nextLocalPoint[0],2) + TMath::Power(nextLocalPoint[1],2)) << endl;
         cout << "Sqrt(X^2 + Z^2): ";
         cout << TMath::Sqrt(TMath::Power(nextLocalPoint[0],2) + TMath::Power(nextLocalPoint[2],2)) << endl;
         cout << "Current Volume Contains Local Point: ";
         cout << nextNode->GetVolume()->GetShape()->Contains(nextLocalPoint) << endl;
      #endif
      // At this point we know that the point is contained by the current volume,
      // but there must be a daughter volume that contains the point as well.
      // If we are sitting right on the boundary, so that the current point is still
      // contained by the initialnode, lets try making a tiny step along our current path,
      // so that we are within the current volume
      // To do this we shall use the normal vector of the current boundary
      Double_t dir[3] = {navigator->GetCurrentDirection()[0], navigator->GetCurrentDirection()[1], navigator->GetCurrentDirection()[2]};
      // -- Get the normal vector to the boundary
      Double_t normal[3] = {0.,0.,0.};
      this->FindBoundaryNormal(&normal[0], navigator);
      Double_t norm[3] = {normal[0], normal[1], normal[2]};
      // Check if the normal vector is pointing along or against our current path
      Double_t dotProduct = dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2];
      if (dotProduct < 0.) {
         // Normal is pointing in the opposite direction to our current track
         // so reflect the normal to get the correct direction
         norm[0] = -norm[0];
         norm[1] = -norm[1];
         norm[2] = -norm[2];
      }
      #ifdef VERBOSE_MODE
         cout << "Normal To Boundary aligned with Current Direction: " << endl;
         cout << "X:" << norm[0] << "\t" << "Y:" << norm[1] << "\t" << "Z:" << norm[2] << endl;
      #endif
      // Make up to 100 micro-steps (equivalent to a 1E-8 step). This size of step
      // should be sufficient to cover for inaccuracies in our boundary finder, and thus
      // our boundaries form a ±1E-8 zone, rather than a purely discrete line
      for (Int_t i = 1; i <= 100; i++) {
         Double_t point[3] = {navigator->GetCurrentPoint()[0], navigator->GetCurrentPoint()[1], navigator->GetCurrentPoint()[2]};
         point[0] += norm[0]*1.0*TGeoShape::Tolerance(); 
         point[1] += norm[1]*1.0*TGeoShape::Tolerance(); 
         point[2] += norm[2]*1.0*TGeoShape::Tolerance();
         // Update point in the navigator
         navigator->SetCurrentPoint(point);
         this->Update(navigator);
         currentGlobalPoint = const_cast<Double_t*>(navigator->GetCurrentPoint());
         initialMatrix->MasterToLocal(currentGlobalPoint,&nextLocalPoint[0]);
         #ifdef VERBOSE_MODE
            cout << i << "\t" << "Making micro-step along current direction to try and locate particle within correct volume." << endl;
            cout << "Global Point after micro-step: ";
            cout << "X:" << currentGlobalPoint[0] << "\t" << "Y:" << currentGlobalPoint[1] << "\t";
            cout << "Z:" << currentGlobalPoint[2] << endl;
            cout << "Point Local to initial volume after micro-step: ";
            cout << "X:" << nextLocalPoint[0] << "\t" << "Y:" << nextLocalPoint[1] << "\t";
            cout << "Z:" << nextLocalPoint[2] << endl;
            cout << "Sqrt(X^2 + Y^2): ";
            cout << TMath::Sqrt(TMath::Power(nextLocalPoint[0],2) + TMath::Power(nextLocalPoint[1],2)) << endl;
            cout << "Sqrt(X^2 + Z^2): ";
            cout << TMath::Sqrt(TMath::Power(nextLocalPoint[0],2) + TMath::Power(nextLocalPoint[2],2)) << endl;
         #endif
         if (navigator->IsSameLocation(currentGlobalPoint[0], currentGlobalPoint[1], currentGlobalPoint[2], kFALSE)) {
            break;
         }
         #ifdef VERBOSE_MODE
            cout << "Current Node: " << nextNode->GetName() << endl;
            cout << "Global Point: ";
            cout << "X:" << currentGlobalPoint[0] << "\t" << "Y:" << currentGlobalPoint[1] << "\t" << "Z:" << currentGlobalPoint[2] << endl;
            cout << "Current Volume Contains Local Point: ";
            cout << nextNode->GetVolume()->GetShape()->Contains(nextLocalPoint) << endl;
         #endif
      }
      
      if (!navigator->IsSameLocation(currentGlobalPoint[0], currentGlobalPoint[1], currentGlobalPoint[2], kFALSE)) {
         Error("MakeStep","2. Next Point is STILL not contained in Current Node, according to Navigator::IsSameLocation");
         cout << "Current Node: " << nextNode->GetName() << endl;
         cout << "Global Point: ";
         cout << "X:" << currentGlobalPoint[0] << "\t" << "Y:" << currentGlobalPoint[1] << "\t" << "Z:" << currentGlobalPoint[2] << endl;
         cout << "Current Volume Contains Local Point: " << nextNode->GetVolume()->GetShape()->Contains(nextLocalPoint) << endl;
         cout << "Find Node Result: "    << navigator->FindNode()->GetName() << endl;
         return kFALSE;
      }
      
      #ifdef VERBOSE_MODE
         cout << "Particle is now correctly located in: " << navigator->GetCurrentNode()->GetName() << endl;
      #endif
   }
   
   // -- We should now have propagated our point by some stepsize and be inside the correct volume 
   // -- Sample Magnetic Field if there is one	
// if (magField != NULL) {
//    const Double_t integratedField = magField->IntegratedField(this->GetStepTime(), particle, gravField);
//    particle->SampleMagField(integratedField, this->GetStepTime());	
// }
   
   // -- Get the current material we are in to determine what to do next
   TUCNGeoMaterial* currentMaterial = static_cast<TUCNGeoMaterial*>(navigator->GetCurrentVolume()->GetMedium()->GetMaterial());

   // -- Get the normal vector to the boundary
   Double_t normal[3] = {0.,0.,0.};
   if (gravField) {
      this->FindBoundaryNormal(normal,navigator);
   } else {
      throw runtime_error("Need to sort out behaviour of normals in particle");     
      //normal = navigator->FindNormal();
   }
   
   // -- Determine what to do if we are on a boundary
   // -- Is Track on the surface of a boundary?
   if (currentMaterial->IsTrackingMaterial() == kFALSE) {
      #ifdef VERBOSE_MODE	
         cout << "On Boundary - Deciding what action to take..." << endl;
      #endif
      // -- Are we on the surface of a detector?
      if (currentMaterial->IsDetectorMaterial() == kTRUE) {
         // -- Was particle detected?
         Double_t prob = gRandom->Uniform(0.0,1.0);
         if (prob <= currentMaterial->DetectionEfficiency()) {	
            // -- PARTICLE DETECTED
            this->Detected(kTRUE);  // Set detected flag
         } else {	
            // -- PARTICLE NOT-DETECTED.
            // -- Eventually we will change this to allow particle to track inside the detector
            cerr << "Not implemented yet" << endl;
            return kFALSE;
         }
      // -- Was particle lost to boundary (absorbed/upscattered) ?
      } else if (this->IsLostToWall(currentMaterial, normal)) {	
         this->LostToBoundary(kTRUE); // Set lost flag
      // -- Are we outside the geometry heirarchy we have built - ie: in TOP
      } else if (currentMaterial->IsBlackHole() == kTRUE) {
         Error("MakeStep","Particle is in `BlackHole' material region. Particle has been lost through the wall!");
         return kFALSE;
      } else {
         // -- PARTICLE ON SURFACE OF BOUNDARY
         TGeoNode* boundaryNode = navigator->GetCurrentNode();
         #ifdef VERBOSE_MODE	
            cout << "------------------- BOUNCE ----------------------" << endl;
            cout << "Boundary Node: " << boundaryNode->GetName() << endl;
            cout << "On Boundary? " << this->IsUCNOnBoundary() << endl;
         #endif	
         // -- Make a Bounce (note: bounce calls update track, so no need to do that here)
         this->Bounce(navigator, normal, currentMaterial);

         // -- cd back to the saved node before we made the step -- stored in 'initialPath'. 
         if (!navigator->cd(initialPath)) {
            Error("MakeStep","Unable to cd to initial node after bounce!");
            return kFALSE;
         }

         TGeoNode* finalNode = navigator->GetCurrentNode();
         TGeoMatrix* finalMatrix = navigator->GetCurrentMatrix();
         #ifdef VERBOSE_MODE	
            cout << "-------------------------------------------------" << endl;
            cout << "Final Node: " << finalNode->GetName() << endl;
            cout << "On Boundary? " << this->IsUCNOnBoundary() << endl;
            cout << "-------------------------------------------------" << endl << endl;
         #endif
         // -- Check if the particle is still registered as being on the boundary
         if (finalNode == boundaryNode) {
            cout << "Initial Node: " << boundaryNode->GetName() << "\t" << "Final Node: " << finalNode->GetName() << endl;
            return kFALSE;
         }
         // -- Check that the final point is within the final node
         Double_t finalLocalPoint[3] = {0.,0.,0.};
         currentGlobalPoint = const_cast<Double_t*>(navigator->GetCurrentPoint());
         finalMatrix->MasterToLocal(currentGlobalPoint,&finalLocalPoint[0]);
         // -- Check that current point is exclusively located within the current volume
         if (!navigator->IsSameLocation(currentGlobalPoint[0], currentGlobalPoint[1], currentGlobalPoint[2], kFALSE)) {
            #ifdef VERBOSE_MODE
               Warning("MakeStep","3. Final Point is not contained in Current Node, according to Navigator::IsSameLocation");
               cout << "Current Node: " << finalNode->GetName() << endl;
               cout << "Global Point: ";
               cout << "X:" << currentGlobalPoint[0] << "\t" << "Y:" << currentGlobalPoint[1];
               cout << "\t" << "Z:" << currentGlobalPoint[2] << endl;
               cout << "Local Point: ";
               cout << "X:" << finalLocalPoint[0] << "\t" << "Y:" << finalLocalPoint[1];
               cout << "\t" << "Z:" << finalLocalPoint[2] << endl;
               cout << "Sqrt(X^2 + Y^2): " << TMath::Sqrt(TMath::Power(finalLocalPoint[0],2) + TMath::Power(finalLocalPoint[1],2)) << endl;
               cout << "Sqrt(X^2 + Z^2): " << TMath::Sqrt(TMath::Power(finalLocalPoint[0],2) + TMath::Power(finalLocalPoint[2],2)) << endl;
               cout << "Current Volume Contains Local Point: ";
               cout << finalNode->GetVolume()->GetShape()->Contains(finalLocalPoint) << endl;
            #endif
            // Now, we know that the point is not in the volume that it should be, and that
            // volume may not actually contain the point. Either way, we will now make a microstep
            // back along the way we came (since the direction has been reversed).
            // To do this we shall use the normal vector of the current boundary
            Double_t dir[3] = {navigator->GetCurrentDirection()[0], navigator->GetCurrentDirection()[1], navigator->GetCurrentDirection()[2]};
            Double_t norm[3] = {normal[0], normal[1], normal[2]};
            // Check if the normal vector is pointing along or against our current path
            Double_t dotProduct = dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2];
            if (dotProduct < 0.) {
               // Normal is pointing in the opposite direction to our current track so reflect
               // the normal to get the correct direction
               norm[0] = -norm[0];
               norm[1] = -norm[1];
               norm[2] = -norm[2];
            }
            #ifdef VERBOSE_MODE
               cout << "Normal To Boundary aligned with Current Direction: " << endl;
               cout << "X:" << norm[0] << "\t" << "Y:"<< norm[1] << "\t" << "Z:" << norm[2] << endl;
            #endif
            for (Int_t i = 1; i <= 100; i++) {
               Double_t point[3] = {navigator->GetCurrentPoint()[0], navigator->GetCurrentPoint()[1], navigator->GetCurrentPoint()[2]};
               point[0] += norm[0]*1.0*TGeoShape::Tolerance(); 
               point[1] += norm[1]*1.0*TGeoShape::Tolerance(); 
               point[2] += norm[2]*1.0*TGeoShape::Tolerance();
               // Update point in the navigator
               navigator->SetCurrentPoint(point);
               this->Update(navigator);
               currentGlobalPoint = const_cast<Double_t*>(navigator->GetCurrentPoint());
               navigator->GetCurrentMatrix()->MasterToLocal(currentGlobalPoint,&nextLocalPoint[0]);
               #ifdef VERBOSE_MODE
                  cout << i << "\t" << "Making micro-step along current direction to try and locate particle within correct volume." << endl;
                  cout << "Global Point after micro-step: ";
                  cout << "X:" << currentGlobalPoint[0] << "\t" << "Y:" << currentGlobalPoint[1];
                  cout << "\t" << "Z:" << currentGlobalPoint[2] << endl;
                  cout << "Local Point after micro-step: ";
                  cout << "X:" << nextLocalPoint[0] << "\t" << "Y:" << nextLocalPoint[1];
                  cout << "\t" << "Z:" << nextLocalPoint[2] << endl;
                  cout << "Sqrt(X^2 + Y^2): " << TMath::Sqrt(TMath::Power(nextLocalPoint[0],2) + TMath::Power(nextLocalPoint[1],2)) << endl;
                  cout << "Sqrt(X^2 + Z^2): " << TMath::Sqrt(TMath::Power(nextLocalPoint[0],2) + TMath::Power(nextLocalPoint[2],2)) << endl;
               #endif
               if (navigator->IsSameLocation(currentGlobalPoint[0], currentGlobalPoint[1], currentGlobalPoint[2], kFALSE)) {
                  break;
               }
               #ifdef VERBOSE_MODE
                  cout << "Current Node: " << finalNode->GetName() << endl;
                  cout << "Global Point: ";
                  cout << "X:" << currentGlobalPoint[0] << "\t" << "Y:" << currentGlobalPoint[1];
                  cout << "\t" << "Z:" << currentGlobalPoint[2] << endl;
                  cout << "Current Volume Contains Local Point: ";
                  cout << finalNode->GetVolume()->GetShape()->Contains(finalLocalPoint) << endl;
               #endif
            }
            if (!navigator->IsSameLocation(currentGlobalPoint[0], currentGlobalPoint[1], currentGlobalPoint[2], kFALSE)) {
               Error("MakeStep","3. Final Point is STILL not contained in Current Node, according to Navigator::IsSameLocation");
               cout << "Current Node: " << finalNode->GetName() << endl;
               cout << "Global Point: ";
               cout << "X:" << currentGlobalPoint[0] << "\t" << "Y:" << currentGlobalPoint[1];
               cout << "\t" << "Z:" << currentGlobalPoint[2] << endl;
               cout << "Current Volume Contains Local Point: ";
               cout << finalNode->GetVolume()->GetShape()->Contains(finalLocalPoint) << endl;
               cout << "Find Node Result: "    << navigator->FindNode()->GetName() << endl;
               return kFALSE;
            }
            #ifdef VERBOSE_MODE
            cout << "Particle is now correctly located in: ";
            cout << navigator->GetCurrentNode()->GetName() << endl;
            #endif
         }
      // End of Bounce. We should have returned to the original node, and guarenteed that the current point is located within it.
      }
   }
   // Add Vertex to Track
// track->AddPoint(particle->Vx(), particle->Vy(), particle->Vz(), particle->T());
   // End of MakeStep.
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNParticle::FindBoundaryNormal(Double_t* normal, TGeoNavigator* navigator)
{
// Computes normal to the crossed boundary, assuming that the current point
// is close enough to the boundary. This method is the same as ROOT's except that we are using
// our own currentNode be used exclusively instead of TGeoNavigator::FindNormal when gravity is present.

   if (fUCNNextNode == NULL) {
      Error("FindUCNNormal","No next node set");
      return kFALSE;
   }
   
   Double_t local[3];
   Double_t ldir[3];
   Double_t lnorm[3];
   navigator->GetHMatrix()->MasterToLocal(navigator->GetCurrentPoint(), local);
   navigator->GetHMatrix()->MasterToLocalVect(navigator->GetCurrentDirection(), ldir);
   fUCNNextNode->GetVolume()->GetShape()->ComputeNormal(local, ldir, lnorm);
   navigator->GetHMatrix()->LocalToMasterVect(lnorm, normal);
   return kTRUE;
}

//_____________________________________________________________________________
TGeoNode* TUCNParticle::ParabolicDaughterBoundaryFinder(TGeoNavigator* navigator, Double_t* point, Double_t* velocity, Double_t* field, Int_t &idaughter, Bool_t compmatrix)
{
// Computes as fStep the distance to next daughter of the current volume. 
// The point and direction must be converted in the coordinate system of the current volume.
// The proposed step limit is fStep.
   
   // -- First Get the current local and global fields
   Double_t motherField[3] = {field[0], field[1], field[2]}; 
   Double_t motherPoint[3] = {point[0], point[1], point[2]};
   Double_t motherVelocity[3] = {velocity[0], velocity[1], velocity[2]};
   
   #ifdef VERBOSE_MODE		
      cout << "PDBF - Mother Local Field: X: " << motherField[0] << "\t" << "Y: ";
      cout << motherField[1] << "\t" << "Z: " << motherField[2] << endl;
   #endif
   
   // -- Initialising some important parameters
   Double_t tnext = TGeoShape::Big();
   Double_t snext = TGeoShape::Big();
   idaughter = -1; // nothing crossed
   TGeoNode *nodefound = 0;
   // This has been added because we do not have access to fGlobalMatrix in TGeoNavigator
   TGeoHMatrix* globalMatrix = navigator->GetCurrentMatrix(); 
   
   // Get number of daughters. If no daughters we are done.
   TGeoVolume *vol = navigator->GetCurrentNode()->GetVolume();
   Int_t nd = vol->GetNdaughters();
   
   #ifdef VERBOSE_MODE		
      cout << "PDBF - Volume: " << vol->GetName() << "\t";
      cout << "Checking number of Daughters: " << nd << endl;
   #endif
      
   if (!nd) return 0;  // No daughter 
   if (gGeoManager->IsActivityEnabled() && !vol->IsActiveDaughters()) {
      cout << "Warning PDBF - IsActivityEnabled in geomanager" << endl;
      return 0;
   }
   
   Double_t localPoint[3], localVelocity[3], localField[3];
   TGeoNode *current = 0;
   Int_t i=0;
   
   // ************************************************************************************
   // -- BRANCH 1
   // -- if current volume is divided, we are in the non-divided region. We
   // -- check first if we are inside a cell in which case compute distance to next cell
   // ************************************************************************************
	
   TGeoPatternFinder *finder = vol->GetFinder();
   if (finder) {
      Int_t ifirst = finder->GetDivIndex();
      Int_t ilast = ifirst+finder->GetNdiv()-1;
      current = finder->FindNode(motherPoint);
      if (current) {
         // Point inside a cell: find distance to next cell
         Int_t index = current->GetIndex();
         if ((index-1) >= ifirst) ifirst = index-1;
         else                     ifirst = -1;
         if ((index+1) <= ilast)  ilast  = index+1;
         else                     ilast  = -1;
      }
      if (ifirst>=0) {   
         current = vol->GetNode(ifirst);
         current->cd();
         current->MasterToLocal(&motherPoint[0], &localPoint[0]);
         current->MasterToLocalVect(&motherVelocity[0], &localVelocity[0]);
         current->MasterToLocalVect(&motherField[0], &localField[0]);
         #ifdef VERBOSE_MODE
            cout << "PDBF - Divided Volume. Calculate distance from outside to first cell." << endl;
            cout << "PDBF - Volume: " << current->GetName() << endl;
            cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t";
            cout << "Z: " << localField[2] << endl;
         #endif
         tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, this->GetStepTime(), fUCNIsOnBoundary);  
         if (tnext <= 0.0) {
            Error("ParabolicDaughterBoundaryFinder", "Failed to find boundary");
            return NULL;
         }
         snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         if (snext < (navigator->GetStep() - TGeoShape::Tolerance())) {
            if (compmatrix) {
               navigator->GetHMatrix()->CopyFrom(globalMatrix);
               navigator->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            navigator->SetStep(snext);
            this->SetStepTime(tnext);
            fUCNNextNode = current;
            nodefound = current;
            idaughter = ifirst;
         }
      }
      if (ilast==ifirst) return nodefound;
      if (ilast>=0) { 
         current = vol->GetNode(ilast);
         current->cd();
         current->MasterToLocal(&motherPoint[0], localPoint);
         current->MasterToLocalVect(&motherVelocity[0], localVelocity);
         current->MasterToLocalVect(&motherField[0], localField);
         #ifdef VERBOSE_MODE		
            cout << "PDBF - Divided Volume. Calculate distance from outside to last cell." << endl;
            cout << "PDBF - Volume: " << current->GetName() << endl;
            cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t";
            cout << "Z: " << localField[2] << endl;
         #endif
         tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, this->GetStepTime(), fUCNIsOnBoundary);
         if (tnext <= 0.0) {
            Error("FindNextBoundaryAlongParabola", "Failed to find boundary");
            return NULL;
         }
         snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         if (snext < (navigator->GetStep() - TGeoShape::Tolerance())) {
            if (compmatrix) {
               navigator->GetHMatrix()->CopyFrom(globalMatrix);
               navigator->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            navigator->SetStep(snext);
            this->SetStepTime(tnext);
            fUCNNextNode = current;
            nodefound = current;
            idaughter = ilast;
         }
      }   
      return nodefound;
   }
   // if only few daughters, check all and exit
   TGeoVoxelFinder *voxels = vol->GetVoxels();
   Int_t indnext;
   if (nd<5 || !voxels) {
      for (i=0; i<nd; i++) {
         current = vol->GetNode(i);
         if (gGeoManager->IsActivityEnabled() && !current->GetVolume()->IsActive()) continue;
         current->cd();
         if (voxels && voxels->IsSafeVoxel(motherPoint, i, navigator->GetStep())) continue;
         current->MasterToLocal(motherPoint, localPoint);
         current->MasterToLocalVect(motherVelocity, localVelocity);
         current->MasterToLocalVect(motherField, localField);
         if (current->IsOverlapping() && current->GetVolume()->Contains(localPoint)) continue;
         #ifdef VERBOSE_MODE		
            cout << "PDBF - Only Few daughters. Calculate distance from outside to all." << endl;
            cout << "PDBF - Volume: " << current->GetName() << endl;
            cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t";
            cout << "Z: " << localField[2] << endl;
            #endif
         tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, this->GetStepTime(), fUCNIsOnBoundary);
         if (tnext <= 0.0) {
            Error("FindNextBoundaryAlongParabola", "Failed to find boundary");
            return NULL;
         }
         snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         if (snext < (navigator->GetStep() - TGeoShape::Tolerance())) {
            #ifdef VERBOSE_MODE		
            cout << "PDBF - Distance to daughter is less than distance to current volume." << endl;
            #endif
            indnext = current->GetVolume()->GetNextNodeIndex();
            if (compmatrix) {
               navigator->GetHMatrix()->CopyFrom(globalMatrix);
               navigator->GetHMatrix()->Multiply(current->GetMatrix());
            }    
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            navigator->SetStep(snext);
            this->SetStepTime(tnext);
            fUCNNextNode = current;
            nodefound = fUCNNextNode;   
            idaughter = i;   
            while (indnext>=0) {
               current = current->GetDaughter(indnext);
               if (compmatrix) navigator->GetHMatrix()->Multiply(current->GetMatrix());
               fUCNNextNode = current;
               nodefound = current;
               indnext = current->GetVolume()->GetNextNodeIndex();
            }
         }
      }
      #ifdef VERBOSE_MODE		
         cout << "PDBF - Return nearest Node found if any." << endl;
      #endif
      return nodefound;
   }
   // if current volume is voxelized, first get current voxel
   Int_t ncheck = 0;
   Int_t sumchecked = 0;
   Int_t *vlist = 0;
   voxels->SortCrossedVoxels(motherPoint, motherVelocity);
   while ((sumchecked<nd) && (vlist=voxels->GetNextVoxel(motherPoint, motherVelocity, ncheck))) {
      for (i=0; i<ncheck; i++) {
         current = vol->GetNode(vlist[i]);
         if (gGeoManager->IsActivityEnabled() && !current->GetVolume()->IsActive()) continue;
         current->cd();
         current->MasterToLocal(motherPoint, localPoint);
         current->MasterToLocalVect(motherVelocity, localVelocity);
         current->MasterToLocalVect(&motherField[0], &localField[0]);
         if (current->IsOverlapping() && current->GetVolume()->Contains(localPoint)) continue;
         #ifdef VERBOSE_MODE		
            cout << "PDBF - Volume is voxelized. Calculate distance from outside to voxel." << endl;
            cout << "PDBF - Volume: " << current->GetName() << endl;
            cout << "Local Field: X: " << localField[0] << "\t" << "Y: " << localField[0] << "\t";
            cout << "Z: " << localField[2] << endl;
         #endif
         tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localField, this->GetStepTime(), fUCNIsOnBoundary);
         if (tnext <= 0.0) {
            Error("FindNextBoundaryAlongParabola", "Failed to find boundary");
            return NULL;
         }
         snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
         sumchecked++;
//         printf("checked %d from %d : snext=%g\n", sumchecked, nd, snext);
         if (snext < (navigator->GetStep() - TGeoShape::Tolerance())) {
            indnext = current->GetVolume()->GetNextNodeIndex();
            if (compmatrix) {
               navigator->GetHMatrix()->CopyFrom(globalMatrix);
               navigator->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            navigator->SetStep(snext);
            this->SetStepTime(tnext);
            fUCNNextNode = current;
            nodefound = fUCNNextNode;
            idaughter = vlist[i];
            while (indnext>=0) {
               current = current->GetDaughter(indnext);
               if (compmatrix) navigator->GetHMatrix()->Multiply(current->GetMatrix());
               fUCNNextNode = current;
               nodefound = current;
               indnext = current->GetVolume()->GetNextNodeIndex();
            }
         }
      }
   }
   return nodefound;
}

//_____________________________________________________________________________
TGeoNode* TUCNParticle::ParabolicBoundaryFinder(Double_t& stepTime, TGeoNavigator* navigator, TUCNGravField* field)
{
// Compute distance to next boundary within STEPMAX. If no boundary is found,
// propagate current point along current direction with fStep=STEPMAX. Otherwise
// propagate with fStep=SNEXT (distance to boundary) and locate/return the next 
// node.
   
   // -- Get the global coordinates
   Double_t globalField[3] 	 = {field->Gx(), field->Gy(), field->Gz()}; 
   Double_t globalPoint[3] 	 = {this->X(), this->Y(), this->Z()};
   Double_t globalDir[3]	 	 = {this->Nx(), this->Ny(), this->Nz()};
   Double_t globalVelocity[3]  = {this->Vx(), this->Vy(), this->Vz()};
   
   Double_t currentField[3]	 = {globalField[0], globalField[1], globalField[2]};
   Double_t currentPoint[3]    = {globalPoint[0], globalPoint[1], globalPoint[2]};
   Double_t currentDir[3]      = {globalDir[0], globalDir[1], globalDir[2]};
   Double_t currentVelocity[3] = {globalVelocity[0], globalVelocity[1], globalVelocity[2]};
   
   // Compute maximum stepsize
   const Double_t stepMax = TUCNParabola::Instance()->ArcLength(globalVelocity, globalField, stepTime);
   
   // -- Some initialisations
   static Int_t icount = 0;
   icount++;
   Int_t nextindex;
   Bool_t is_assembly;
   fUCNIsStepExiting  = kFALSE;
   fUCNIsStepEntering = kFALSE;
   TGeoNode *skip;
   
   // -- Store time step and step distance -- these two should always be convertible
   this->SetStepTime(stepTime);
   navigator->SetStep(stepMax);
   Double_t snext = TGeoShape::Big();
   Double_t tnext = TGeoShape::Big();
   
   #ifdef VERBOSE_MODE
      cout << "PBF - Starting StepTime: " << this->GetStepTime() << "\t";
      cout << "Starting StepSize: " << navigator->GetStep() << endl;
      cout << "PBF - Velocity: " << this->V() << endl;
      cout << "PBF - Global Field: X: " << globalField[0] << "\t" << "Y: " << globalField[1];
      cout << "\t" << "Z: " << globalField[2] << endl;
      cout << "PBF - Global Point: X: " << globalPoint[0] << "\t" << "Y: " << globalPoint[1];
      cout << "\t" << "Z: " << globalPoint[2] << endl;
      cout << "PBF - Global Dir: X: "   << globalDir[0] << "\t" << "Y: " << globalDir[1];
      cout << "\t" << "Z: " << globalDir[2] << endl;	
      cout << "PBF - Global Velocity: X: " << globalVelocity[0] << "\t" << "Y: ";
      cout << globalVelocity[1] << "\t" << "Z: " << globalVelocity[2] << endl;	
   #endif
   
   navigator->GetHMatrix()->CopyFrom(navigator->GetCurrentMatrix());
   
   // *********************************************************************
   // -- BRANCH 1
   // -- fIsOutside means outside TOP volume - ie: outside geometry
   // *********************************************************************	
   if (fUCNIsOutside) {
      Error("PBF", "Point is Outside Geometry. Stopping all propagation.");
      return 0;
   }
   
   // *********************************************************************
   // -- BRANCH 2
   // -- So we are not outside the geometry. Get point and direction in
   // -- local coordinate frame and calculate distance to boundary of current node.
   // *********************************************************************
   Double_t localPoint[3], localVelocity[3], localField[3]; // Containers for the local coords
   Int_t icrossed = -2;
   navigator->GetCurrentMatrix()->MasterToLocal(currentPoint, &localPoint[0]);
   navigator->GetCurrentMatrix()->MasterToLocalVect(currentVelocity, &localVelocity[0]);
   navigator->GetCurrentMatrix()->MasterToLocalVect(currentField, &localField[0]);
   TGeoVolume *vol = navigator->GetCurrentNode()->GetVolume();
   
   // -- Find distance to exiting current node
   tnext = static_cast<TUCNGeoBBox*>(vol->GetShape())->TimeFromInsideAlongParabola(localPoint, localVelocity, localField, this->GetStepTime(), fUCNIsOnBoundary); 
   if (tnext <= 0.0) {
      Error("FindNextBoundaryAlongParabola", "Failed to find boundary");
      return NULL;
   }
   snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
   fUCNNextNode = navigator->GetCurrentNode();
   #ifdef VERBOSE_MODE
      cout << "PBF - Branch 2. Find distance to exit current Volume: " << vol->GetName() << endl;
      cout << "PBF - Local Field: X: " << localField[0] << "\t" << "Y: " << localField[1] << "\t";
      cout << "Z: " << localField[2] << endl;
      cout << "PBF - Time to boundary: " << tnext << "\t";
      cout << "Distance to Boundary: " << snext << endl;
      cout << "PBF - Abs(snext - fStep): " << TMath::Abs(snext - navigator->GetStep()) << endl;
   #endif
   // -- If distance to exiting current node is <= Tolerance value (1e-10)
   // -- make a small step by navigator tolerance value
   if (snext <= TGeoShape::Tolerance()) {
      snext = TGeoShape::Tolerance();
      tnext = TGeoShape::Tolerance();
      navigator->SetStep(snext);
      this->SetStepTime(tnext);
      fUCNIsOnBoundary = kTRUE;
      fUCNIsStepEntering = kFALSE;
      fUCNIsStepExiting = kTRUE;
      skip = navigator->GetCurrentNode();
      
      // -- Move global point by Tolerance value
      currentPoint[0] += navigator->GetStep()*currentDir[0]; 
      currentPoint[1] += navigator->GetStep()*currentDir[1]; 
      currentPoint[2] += navigator->GetStep()*currentDir[2];	
      navigator->SetCurrentPoint(currentPoint);
      is_assembly = navigator->GetCurrentNode()->GetVolume()->IsAssembly();
      if (!(navigator->GetLevel()) && !is_assembly) {
         fUCNIsOutside = kTRUE ;
         return 0;
      }
      if (navigator->GetCurrentNode()->IsOffset()) {
         #ifdef VERBOSE_MODE
            Warning("FindNextBoundaryAndStepAlongParabola","Branch 2. fCurrentNode->IsOffset(). Entering CrossDivisionCell().");
         #endif
         throw runtime_error("In TUCNParticle PBF Need to call CrossDivisionCell but can't!");
      }
      // -- Cross Boundary and return new volume
      if (navigator->GetLevel()) {
         navigator->CdUp(); 
      } else {
         skip = 0;
      }
      return navigator->CrossBoundaryAndLocate(kFALSE, skip);
   }
   
   // -- If distance to exiting current node is smaller than proposed Stepsize
   // -- then set our stepsize fStep to navigator distance (snext)
   if (snext < navigator->GetStep() - TGeoShape::Tolerance()) {
      icrossed = -1;
      navigator->SetStep(snext);
      this->SetStepTime(tnext);
      fUCNIsStepEntering = kFALSE;
      fUCNIsStepExiting = kTRUE;
   }
   
   // Find next daughter boundary for the current volume
   Int_t idaughter = -1;
   TGeoNode *crossed = this->ParabolicDaughterBoundaryFinder(navigator, localPoint, localVelocity, localField, idaughter, kTRUE);
   if (crossed) {
      fUCNIsStepExiting = kFALSE;
      icrossed = idaughter;
      fUCNIsStepEntering = kTRUE;
   }
   
   TGeoNode *current = 0;
   
   // -- If we are in an overlapping node, return an error as we are no longer supporting this.
   // -- Geometries must be constructed with no overlaps beyond common boundaries.
   if (navigator->GetNmany()) {
      Error("PBF","In overlapping node - implementation of this removed");
      return 0;
   }
   // *********************************************************************
   // -- BRANCH 3
   // -- Updating the Particle's position and momentum
   // *********************************************************************
   const Double_t timestep = this->GetStepTime();
   #ifdef VERBOSE_MODE
      cout << "PBF - Branch 3. Updating Global Point. fTimeStep: " << this->GetStepTime();
      cout << "\t" << "fStep: " << navigator->GetStep() << endl;
      cout << "PBF - Initial X: " << currentPoint[0] << "\t" << "Y: " << currentPoint[1] <<  "\t";
      cout << "Z: " << currentPoint[2] << endl;
      cout << "PBF - Initial Vx: " << currentVelocity[0] << "\t" << "Vy: " << currentVelocity[1];
      cout <<  "\t" << "Vz: " << currentVelocity[2] << endl;
      cout << "PBF - Initial Gx: " << currentField[0] << "\t" << "Gy: " << currentField[1] <<  "\t";
      cout << "Gz: " << currentField[2] << endl;
      cout << "PBF - Sqrt(X^2 + Y^2): " << TMath::Sqrt(TMath::Power(currentPoint[0],2) + TMath::Power(currentPoint[1],2)) << endl;
      cout << "PBF - Sqrt(X^2 + Z^2): " << TMath::Sqrt(TMath::Power(currentPoint[0],2) + TMath::Power(currentPoint[2],2)) << endl;
   #endif
   // -- Update Position and Direction according to the timestep
   for (Int_t i = 0; i < 3; i++) {
      currentPoint[i] += currentVelocity[i]*timestep + 0.5*currentField[i]*timestep*timestep;
      currentVelocity[i] += currentField[i]*timestep;
   }
   // Calculate the magnitude of velocity
   Double_t velocityMag = TMath::Sqrt((TMath::Power(currentVelocity[0],2) + TMath::Power(currentVelocity[1],2) + TMath::Power(currentVelocity[2],2)));
   // Check that velocity is not zero
   assert(velocityMag != 0.); 
   // Update Current Point
   navigator->SetCurrentPoint(currentPoint);
   // Update Current Direction
   for (Int_t i = 0; i < 3; i++) currentDir[i] = currentVelocity[i]/velocityMag;
   navigator->SetCurrentDirection(currentDir);
   // Update stepsize and steptime
   navigator->SetStep(navigator->GetStep());
   this->SetStepTime(this->GetStepTime());
   #ifdef VERBOSE_MODE
      cout << "PBF - Final X: " << currentPoint[0] << "\t" << "Y: " << currentPoint[1] <<  "\t";
      cout << "Z: " << currentPoint[2] << endl;
      cout << "PBF - Final Vx: " << currentVelocity[0] << "\t" << "Vy: " << currentVelocity[1] <<  "\t" << "Vz: " << currentVelocity[2] << endl;
      cout << "PBF - Field Gx: " << currentField[0] << "\t" << "Gy: " << currentField[1] <<  "\t";
      cout << "Gz: " << currentField[2] << endl;
      cout << "PBF - Sqrt(X^2 + Y^2): " << TMath::Sqrt(TMath::Power(currentPoint[0],2) + TMath::Power(currentPoint[1],2)) << endl;
      cout << "PBF - Sqrt(X^2 + Z^2): " << TMath::Sqrt(TMath::Power(currentPoint[0],2) + TMath::Power(currentPoint[2],2)) << endl;
   #endif
   // *********************************************************************
   // -- BRANCH 4
   // -- Final check on results of above
   // *********************************************************************
   if (icrossed == -2) {
      // Nothing crossed within stepMax -> propagate and return same location   
      #ifdef VERBOSE_MODE
         cout << "PBF - Branch 4. Nothing crossed within stepMax. Propagating point and returning same location." << endl;
      #endif
      fUCNIsOnBoundary = kFALSE;
      return navigator->GetCurrentNode();
   }
   fUCNIsOnBoundary = kTRUE;
   if (icrossed == -1) {
      #ifdef VERBOSE_MODE
         cout << "PBF - Branch 4. On Boundary. Crossing boundary and locating." << endl;
         cout << "PBF - Get Level: " << navigator->GetLevel() << endl;
         cout << "PBF - Current Node: " << gGeoManager->GetCurrentNode()->GetName() << endl;
      #endif
      skip = navigator->GetCurrentNode();
      is_assembly = navigator->GetCurrentNode()->GetVolume()->IsAssembly();
      if (!(navigator->GetLevel()) && !is_assembly) {
         fUCNIsOutside = kTRUE;
         return 0;
      }   
      if (navigator->GetCurrentNode()->IsOffset()) {
         throw runtime_error("In PBF Need to call CrossDivisionCell but can't!");
         //return navigator->CrossDivisionCell();
      }
      if (navigator->GetLevel()) {
         navigator->CdUp();
      } else {
         skip = 0;
      }
      TGeoNode* finalNode = navigator->CrossBoundaryAndLocate(kFALSE, skip);
      #ifdef VERBOSE_MODE
         cout << "PBF - Final Node: " << finalNode->GetName() << endl;
         cout << "---------------------------------------" << endl;
      #endif
      return finalNode;
   }
   current = navigator->GetCurrentNode();
   navigator->CdDown(icrossed);
   nextindex = navigator->GetCurrentNode()->GetVolume()->GetNextNodeIndex();
   while (nextindex>=0) {
      current = navigator->GetCurrentNode();
      navigator->CdDown(nextindex);
      nextindex = navigator->GetCurrentNode()->GetVolume()->GetNextNodeIndex();
   }
   #ifdef VERBOSE_MODE
      cout << "ParabolicBoundaryFinder - Branch 4. Crossing boundary and locating." << endl;
   #endif
   return navigator->CrossBoundaryAndLocate(kTRUE, current);
}

//_____________________________________________________________________________
Double_t TUCNParticle::DetermineNextStepTime(const Double_t maxStepTime, const Double_t runTime)
{
   // Placeholder for method to calculate the next step time depending on 
   // electric/magnetic field environment
   // Start with the maximum stepTime 
   this->SetStepTime(maxStepTime);
   
   // Check if we will reach the maximum runtime of the track. If so propagate only until this time.
   if (runTime == 0.0) {
      return this->GetStepTime();
   } else if (this->T() > (runTime - this->GetStepTime())) {
      this->SetStepTime((runTime - this->T()) + TGeoShape::Tolerance());
   }
   return this->GetStepTime();
}

//_____________________________________________________________________________
void TUCNParticle::SetStepTime(Double_t stepTime) 
{
   assert(stepTime > 0.0);
   fStepTime = stepTime;
}

//_____________________________________________________________________________
void TUCNParticle::Update(const TGeoNavigator* navigator, const Double_t timeInterval, const TUCNGravField* gravField)
{
   // -- Take the particle and update its position, momentum, time and energy
   // -- with the current properties stored in the Navigator
   #ifdef VERBOSE_MODE
      cout << "Update -- Initial X: " << this->X() << "\t" << "Y: " << this->Y();
      cout << "\t" <<  "Z: " << this->Z() << "\t" <<  "T: " << this->T() << endl;
      cout << "Update -- Initial PX: " << this->Px() << "\t" << "PY: " << this->Py();
      cout << "\t" <<  "PZ: " << this->Pz() << "\t";
      cout <<  "E: " << this->Energy()/Units::neV << endl;
   #endif
   
   const Double_t* pos = navigator->GetCurrentPoint();
   const Double_t* dir = navigator->GetCurrentDirection(); 
   Double_t heightClimbed = 0.0;
   Double_t gravPotentialEnergy = 0.0;
   
   if (gravField) {
      // Determine the height of our particle in the global coordinate system of TOP.
      // Take the dot product of the point vector with the field direction unit vector
      // to get the height of this point in the gravitational field.
      // This assumes a convention that 'height' is measured along an axis that INCREASES
      // in the opposite direction to the field direction vector (which is usually 'downwards')
      heightClimbed = -1.0*((pos[0] - this->X())*gravField->Nx() + (pos[1] - this->Y())*gravField->Ny() + (pos[2] - this->Z())*gravField->Nz());
      gravPotentialEnergy = this->Mass_eV_c2()*gravField->GravAcceleration()*heightClimbed;
   }
   
   // Determine current Kinetic energy of particle given the height climbed in graviational field
   Double_t kineticEnergy = this->Energy() - gravPotentialEnergy;
   
   // Detemine current momentum
   Double_t momentum = TMath::Sqrt(2.0*this->Mass_eV()*kineticEnergy);
   Double_t mom[3] = {momentum*dir[0], momentum*dir[1], momentum*dir[2]};
   
   // Update particle
   this->SetVertex(pos[0], pos[1], pos[2], this->T() + timeInterval);
   this->SetMomentum(mom[0], mom[1], mom[2], kineticEnergy);
   this->IncreaseDistance(navigator->GetStep()); // Increase the distance travelled
   
   #ifdef VERBOSE_MODE
      cout << "UpdateParticle -- Height climbed: " << heightClimbed << "\t";
      cout << "Kinetic Energy Gained(Lost): " << -gravPotentialEnergy/Units::neV << endl;
      cout << "UpdateParticle -- Final X: " << this->X() << "\t" << "Y: " << this->Y();
      cout << "\t" <<  "Z: " << this->Z() << "\t" <<  "T: " << this->T() << endl;
      cout << "UpdateParticle -- Final PX: " << this->Px() << "\t" << "PY: " << this->Py();
      cout << "\t" <<  "PZ: " << this->Pz() << "\t";
      cout <<  "E: " << this->Energy()/Units::neV << endl;
   #endif
}

//_____________________________________________________________________________
Bool_t TUCNParticle::Bounce(TGeoNavigator* navigator, const Double_t* normal, const TUCNGeoMaterial* wallMaterial)
{
   // -- Make a reflection off of the current boundary
   // -- Direction Vector
   Double_t dir[3] = {navigator->GetCurrentDirection()[0], navigator->GetCurrentDirection()[1], navigator->GetCurrentDirection()[2]};
   
   // -- Normal Vector
   Double_t norm[3] = {normal[0], normal[1], normal[2]};
   
   // Check if the normal vector is actually pointing in the wrong direction  
   // (wrong means pointing along the direction of the track, rather than in the opposite direction)
   // This will actually be the case nearly all (if not all) of the time,
   // because of the way ROOT calculates the normal
   Double_t dotProduct = dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2];
   if (dotProduct > 0.) {
      // If so, reflect the normal to get the correct direction
      norm[0] = -norm[0];
      norm[1] = -norm[1];
      norm[2] = -norm[2];
   }
   
   // -- Calculate Probability of diffuse reflection
   Double_t fermiPotential = wallMaterial->FermiPotential();
   Double_t diffuseCoefficient = wallMaterial->RoughnessCoeff();
   Double_t diffuseProbability = this->DiffuseProbability(diffuseCoefficient, normal, fermiPotential);
   
   // Determine Reflection Type 
   Double_t prob = gRandom->Uniform(0.0,1.0);
   if (prob <= diffuseProbability) {
      // -- Diffuse Bounce
      this->DiffuseBounce(navigator, dir, norm);
      this->MadeDiffuseBounce(); // Update counter
   } else {
      // -- Specular Bounce
      this->SpecularBounce(dir, norm);
      this->MadeSpecularBounce(); // Update counter
   }

   // -- Set New Direction
   navigator->SetCurrentDirection(dir);
   
   // -- Update Particle
   this->Update(navigator);	

   // -- Update Bounce Counter
   this->MadeBounce();
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNParticle::SpecularBounce(Double_t* dir, const Double_t* norm)
{
   #ifdef VERBOSE_MODE
      cout << "----------------------------" << endl;
      cout << "Specular Bounce" << endl;
      cout << "BEFORE - nx: " << dir[0] <<"\t"<< "ny: " << dir[1] <<"\t"<< "nz: " << dir[2] << endl;
      cout <<"normx: "<< norm[0] <<"\t"<<"normy: "<< norm[1] <<"\t"<<"normz: "<< norm[2] << endl;
   #endif
   
   Double_t dotProduct = dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2];	
   // Reflection Law for Specular Reflection
   dir[0] = dir[0] - 2.0*dotProduct*norm[0];
   dir[1] = dir[1] - 2.0*dotProduct*norm[1];
   dir[2] = dir[2] - 2.0*dotProduct*norm[2];
   
   #ifdef VERBOSE_MODE
      cout << "AFTER - nx: " << dir[0] <<"\t"<< "ny: " << dir[1] <<"\t"<< "nz: " << dir[2] << endl;
   #endif
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNParticle::DiffuseBounce(const TGeoNavigator* navigator, Double_t* dir, const Double_t* norm)
{
   #ifdef VERBOSE_MODE	
      cout << "----------------------------" << endl;
      cout << "Diffuse Bounce" << endl;
      cout << "BEFORE - nx: " << dir[0] <<"\t"<< "ny: " << dir[1] <<"\t"<< "nz: " << dir[2] << endl;
      cout << "normx: "<< norm[0] <<"\t"<<"normy: "<< norm[1] <<"\t"<<"normz: "<< norm[2] << endl;
   #endif
   
   // First we need to pick random angles to choose the orientation of our diffuse direction vector. 
   // Correct method for UCN physics though is to weight these angles towards the poles by adding
   // an extra cos(theta). Derivation of how to pick these angles is in notes
   // Overall solid angle used is dOmega = cos(theta)sin(theta) dtheta dphi
   Double_t phi = gRandom->Uniform(0.0, 1.0)*2*TMath::Pi();
   // We do not want the full range of theta from 0 to pi/2 however.
   // An angle of pi/2 would imply moving off the boundary exactly parallel to
   // the current surface.Therefore we should restrict theta to a slightly smaller
   // proportion of angles - letting u be between 0 and 0.499, ensures theta
   // is between 0 and ~89 degrees. 
   Double_t u = gRandom->Uniform(0.0, 0.499);
   // We ignore the negative sqrt term when selecting theta,
   // since we are only interested in theta between 0 and pi/2 
   // (negative root provides the pi/2 to pi branch) 
   Double_t theta = TMath::ACos(TMath::Sqrt(1.0 - 2*u)); 
   
   // Calculate local normal vector	
   Double_t lnorm[3] = {0.,0.,0.};
   navigator->MasterToLocalVect(norm, lnorm);
   TVector3 localNorm(lnorm[0], lnorm[1], lnorm[2]);
   assert(TMath::Abs(localNorm.Mag() - 1.0) < TGeoShape::Tolerance());
   
   // ------------------------------------------------------------------
   // -- FIND A SINGLE ARBITRARY VECTOR PERPENDICULAR TO THE LOCAL NORMAL
   // Define a preferred direction in our coordinate system - usually the z-direction
   // - that we want to be perpendicular to the normal vector of our surface
   TVector3 upAxis(0.,0.,1.);
   
   // Here we check to make sure that the upAxis we chose is not parallel to the normal vector.
   // If it is, we try another one, x. 
   if (TMath::Abs(upAxis.Dot(localNorm)) > TGeoShape::Tolerance()) {
      upAxis.SetXYZ(1.,0.,0.);
      if (TMath::Abs(upAxis.Dot(localNorm)) > TGeoShape::Tolerance()) {
         // In the (hopefully) unlikely case that localNorm has components in the x and z
         // directions. This has been observed when including the bend geometry. This suggests
         // that in the local coordinate system in this instance, the z-axis is not along the
         // vertical as we usually assume. Thus including this check covers our backs. 
         upAxis.SetXYZ(0.,1.,0.);
         if (TMath::Abs(upAxis.Dot(localNorm)) > TGeoShape::Tolerance()) {
            cout << "Axis X: " << upAxis.X() << "\t" <<  "Y: " << upAxis.Y() << "\t";
            cout <<   "Z: " << upAxis.Z() << endl;
            cout << "LocalNorm X: " << localNorm.X() << "\t" <<  "Y: " << localNorm.Y() << "\t";
            cout <<   "Z: " << localNorm.Z() << endl;
            throw runtime_error("In TUCNParticle::DiffuseBounce - Could not find an axis perpendicular to normal. Normal is parallel to z and x axes!!!");
         }
      }
   }
   // Calculate the cross product of the 'up' vector with our normal vector to get a vector
   // guaranteed to be perpendicular to the normal. 
   // This is the vector we will want to rotate around initially. 
   TVector3 perpAxis = localNorm.Cross(upAxis); 
   assert(TMath::Abs(perpAxis.Mag() - 1.0) < TGeoShape::Tolerance());
   // ------------------------------------------------------------------
   
   // Rotate normal vector about perpendicular axis by angle theta. 
   // Using Rodrigues' formula derived in notes. 
   TVector3 rotatedNorm(0.,0.,0.);
   {
      // name term2 and term3 just refer to the terms as they appear in the Rodrigues' formula
      TVector3 term2 = localNorm - (localNorm.Dot(perpAxis))*perpAxis; 
      TVector3 term3 = perpAxis.Cross(localNorm);
      rotatedNorm = localNorm + (TMath::Cos(theta) - 1.0)*term2 + TMath::Sin(theta)*term3;
   }
   assert(TMath::Abs(rotatedNorm.Mag() - 1.0) < TGeoShape::Tolerance());
   
   // Rotate the newly rotated Normal vector, rotatedNorm, by angle phi, this time
   // rotating about the original normal vector, norm.
   TVector3 direction(0.,0.,0.);
   {
      TVector3 term3 = localNorm.Cross(rotatedNorm);
      TVector3 term2 = rotatedNorm - (rotatedNorm.Dot(localNorm))*localNorm;
      direction = rotatedNorm + (TMath::Cos(phi) - 1.0)*term2 + TMath::Sin(phi)*term3;
   }
   assert(TMath::Abs(direction.Mag() - 1.0) < TGeoShape::Tolerance()); 
   
   // Convert the direction vector back to the global frame
   Double_t ldir[3] = {direction.X(), direction.Y(), direction.Z()};
   navigator->LocalToMasterVect(ldir, dir);
   
   assert(TMath::Abs(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2] - 1.0) < TGeoShape::Tolerance());
   // Assert that our final direction is not perpendicular to the normal.
   // This could result in escaping the boundary
   assert(TMath::Abs(dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2]) > TGeoShape::Tolerance()); 
   
   #ifdef VERBOSE_MODE	
      cout << "AFTER - nx: " << dir[0] <<"\t"<< "nx: " << dir[1] <<"\t"<< "nx: " << dir[2] << endl;
   #endif
   
   return kTRUE;
}
