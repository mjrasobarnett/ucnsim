// Volume
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Volume.h"

#include "TGeoElement.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include "TGeoNavigator.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"

#include "Particle.h"
#include "Material.h"
#include "Observer.h"

#include "Units.h"
#include "Constants.h"

//#define VERBOSE_MODE

using namespace std;

ClassImp(Volume)

//_____________________________________________________________________________
Volume::Volume()
           :TGeoVolume()
{
// Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Volume", "Default Constructor");
   #endif
}

//_____________________________________________________________________________
Volume::Volume(const char *name, const TGeoShape* shape, const TGeoMedium* med)
           :TGeoVolume(name,shape,med)
{
// constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Volume", "Constructor");
   #endif
}

//_____________________________________________________________________________
Volume::Volume(const Volume& other)
           :TGeoVolume(other)
{
   //copy constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Volume", "Copy Constructor");  
   #endif
}

//_____________________________________________________________________________
Volume& Volume::operator=(const Volume& other) 
{
   //assignment operator
   #ifdef PRINT_CONSTRUCTORS
      Info("Volume", "Assignment");
   #endif
   if(this!=&other) {
      TGeoVolume::operator=(other);
   }
   return *this;
}

//_____________________________________________________________________________
Volume::~Volume()
{
// Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Volume", "Destructor");
   #endif
}

//_____________________________________________________________________________
Double_t Volume::FermiPotential() const 
{
   return dynamic_cast<Material*>(this->GetMaterial())->FermiPotential();
}

//_____________________________________________________________________________
Double_t Volume::WPotential() const 
{
   return dynamic_cast<Material*>(this->GetMaterial())->WPotential();
}

//_____________________________________________________________________________
Double_t Volume::LossFactor() const 
{
   return dynamic_cast<Material*>(this->GetMaterial())->LossFactor();
}

//_____________________________________________________________________________
Bool_t Volume::Interact(Particle* /*particle*/, Double_t* /*normal*/, TGeoNavigator* /*navigator*/, TGeoNode* /*crossedNode*/, const char* /*initialPath*/)
{
   // Default, no interaction
   return kTRUE;
}

//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________

ClassImp(TrackingVolume)

//_____________________________________________________________________________
TrackingVolume::TrackingVolume()
                   :Volume()
{
// Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TrackingVolume", "Default Constructor");
   #endif
}

//_____________________________________________________________________________
TrackingVolume::TrackingVolume(const char *name, const TGeoShape* shape, const TGeoMedium* med)
                   :Volume(name, shape, med)
{
// constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TrackingVolume", "Constructor");
   #endif
}

//_____________________________________________________________________________
TrackingVolume::TrackingVolume(const TrackingVolume& m)
                   :Volume(m)
{ 
   //copy constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TrackingVolume", "Copy Constructor");  
   #endif
}

//_____________________________________________________________________________
TrackingVolume& TrackingVolume::operator=(const TrackingVolume& m) 
{
   //assignment operator
   #ifdef PRINT_CONSTRUCTORS
      Info("TrackingVolume", "Assignment");
   #endif
   if(this!=&m) {
      Volume::operator=(m);
   } 
   return *this;
}

//_____________________________________________________________________________
TrackingVolume::~TrackingVolume()
{
// Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TrackingVolume", "Destructor");
   #endif
}

//_____________________________________________________________________________
Bool_t TrackingVolume::Interact(Particle* /*particle*/, Double_t* /*normal*/, TGeoNavigator* /*navigator*/, TGeoNode* /*crossedNode*/, const char* /*initialPath*/)
{
   // Default, no interaction
   return kTRUE;
}

//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________

ClassImp(Boundary)

//_____________________________________________________________________________
Boundary::Boundary()
             :Volume()
{
// Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Boundary", "Default Constructor");
   #endif
}

//_____________________________________________________________________________
Boundary::Boundary(const char *name, const TGeoShape* shape, const TGeoMedium* med, Double_t surfaceRoughness)
             :Volume(name, shape, med)
{
// constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Boundary", "Constructor");
   #endif
   fRoughness = surfaceRoughness;
}

//_____________________________________________________________________________
Boundary::Boundary(const Boundary& m)
             :Volume(m),
              fRoughness(m.fRoughness)
             
{ 
   //copy constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Boundary", "Copy Constructor");  
   #endif
}

//_____________________________________________________________________________
Boundary& Boundary::operator=(const Boundary& m) 
{
   //assignment operator
   #ifdef PRINT_CONSTRUCTORS
      Info("Boundary", "Assignment");
   #endif
   if(this!=&m) {
      Volume::operator=(m);
      fRoughness = m.fRoughness;
   } 
   return *this;
}

//_____________________________________________________________________________
Boundary::~Boundary()
{
// Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Boundary", "Destructor");
   #endif
}

//_____________________________________________________________________________
Bool_t Boundary::Interact(Particle* particle, Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath)
{
// -- Interaction of particle with the boundary material
   // -- Is Track on the surface of a boundary?
   #ifdef VERBOSE_MODE
      cout << "On Boundary - Deciding what action to take..." << endl;
   #endif
         
   //------------------------------------------------------
   // 1. -- Calculate whether particle is absorbed/upscatteed by wall nuclei
   if (this->AbsorbParticle(particle, normal) == kTRUE) {
      // Particle was absorbed
      #ifdef VERBOSE_MODE
         cout << "Particle absorbed by boundary" << endl;
      #endif
      particle->IsAbsorbed();
      return kFALSE;
   }
   
   //------------------------------------------------------
   // 2. -- Reflect particle
   #ifdef VERBOSE_MODE
      cout << "------------------- BOUNCE ----------------------" << endl;
      cout << "Boundary Node: " << navigator->GetCurrentNode()->GetName() << endl;
   #endif
   TGeoNode* boundaryNode = navigator->GetCurrentNode();
   TGeoMatrix* boundaryMatrix = navigator->GetCurrentMatrix();
   // Reflect
   if (this->ReflectParticle(particle, navigator, normal) == kFALSE) {
      Error("Interact","Reflect particle failed");
      throw runtime_error("Reflection of particle failed");
   }
   // Change navigator state back to the initial node after bounce
   if (navigator->cd(initialPath) == kFALSE) {
      Error("MakeStep","Unable to cd to initial node after bounce!");
      throw runtime_error("Unable to cd to initial node");
   }
   // Attempt to locate particle within the current node
   Bool_t locatedParticle = particle->LocateInGeometry(particle, navigator, boundaryNode, boundaryMatrix, crossedNode);
   if (locatedParticle == kFALSE) {
      #ifdef VERBOSE_MODE
         cout << "Error - After Bounce - Unable to locate particle correctly in Geometry"<< endl;
      #endif
      // Change state
      particle->IsAnomalous();
      throw runtime_error("Unable to locate particle uniquely in correct volume");
   }
   #ifdef VERBOSE_MODE
      cout << "-------------------------------------------------" << endl;
      cout << "Final Node: " << navigator->GetCurrentNode()->GetName() << endl;
      cout << "-------------------------------------------------" << endl << endl;
   #endif
   // End of Bounce. We should have returned to the original node, 
   // and guarenteed that the current point is located within it.
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t Boundary::AbsorbParticle(Particle* particle, Double_t* normal)
{
// -- Calculate whether particle will be absorbed/upscattered/lost to the wall
   // 1. Check the perpendicular energy
   // Take dot product of two unit vectors - normal and direction - to give the angle between them
   Double_t cosTheta = TMath::Abs(particle->Nx()*normal[0] + particle->Ny()*normal[1] + particle->Nz()*normal[2]);
   Double_t energyPerp = particle->Energy()*cosTheta*cosTheta;
   if (energyPerp >= this->FermiPotential()) {
      return kTRUE;
   }
   // 2. Perpendicular energy is less than the fermi-potential. Calculate the probability of loss
   Double_t eta = this->LossFactor();
   if (eta == 0.0) {
      // No wall losses implemented
      Warning("AbsorbParticle","No Wall Losses Implemented. V: %f, W: %f", this->FermiPotential(), this->WPotential());
      return kFALSE;
   }
   Double_t lossProb = 2.*eta*(TMath::Sqrt(energyPerp/(this->FermiPotential() - energyPerp)));
   if (gRandom->Uniform(0.0, 1.0) <= lossProb) {
      // Particle absorbed/upscattered by nuclei
      return kTRUE;
   }
   // Particle not absorbed or lost
   return kFALSE;
}

//_____________________________________________________________________________
Bool_t Boundary::ReflectParticle(Particle* particle, TGeoNavigator* navigator, Double_t* normal)
{
// -- Particle reflects from boundary
   // -- Normal Vector
   // Check if the normal vector is actually pointing in the wrong direction  
   // (wrong means pointing along the direction of the track, rather than in the opposite direction)
   // This will actually be the case nearly all (if not all) of the time,
   // because of the way ROOT calculates the normal
   Double_t norm[3] = {normal[0], normal[1], normal[2]};
   Double_t dotProduct = particle->Nx()*norm[0] + particle->Ny()*norm[1] + particle->Nz()*norm[2];
   if (dotProduct > 0.) {
      // If so, reflect the normal to get the correct direction
      for (Int_t i=0; i<3; i++) {norm[i] = -norm[i];}
   }
   
   // -- Calculate Probability of diffuse reflection
   Double_t diffuseProbability = this->DiffuseProbability(particle, norm);
   
   // Determine Reflection Type 
   if (gRandom->Uniform(0.0,1.0) <= diffuseProbability) {
      // -- Diffuse Bounce
      if (this->DiffuseBounce(particle, navigator, norm) == kFALSE) {return kFALSE;}
      particle->NotifyObservers(Context::DiffBounce);
   } else {
      // -- Specular Bounce
      if (this->SpecularBounce(particle, navigator, norm) == kFALSE) {return kFALSE;}
      particle->NotifyObservers(Context::SpecBounce);
   
   }
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t Boundary::SpecularBounce(Particle* particle, TGeoNavigator* navigator, const Double_t* norm)
{
   #ifdef VERBOSE_MODE
      cout << "----------------------------" << endl;
      cout << "Specular Bounce" << endl;
      cout << "BEFORE - nx: " << particle->Nx() <<"\t"<< "ny: " << particle->Ny();
      cout << "\t" << "nz: " << particle->Nz() << endl;
      cout << "normx: " << norm[0] <<"\t"<<"normy: "<< norm[1] <<"\t"<<"normz: "<< norm[2] << endl;
   #endif
   Double_t dir[3] = {particle->Nx(), particle->Ny(), particle->Nz()};
   Double_t dotProduct = dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2];	
   // Reflection Law for Specular Reflection
   for (Int_t i=0; i<3; i++) {dir[i] = dir[i] - 2.0*dotProduct*norm[i];}
   #ifdef VERBOSE_MODE
      cout << "AFTER - nx: " << dir[0] <<"\t"<< "ny: " << dir[1] <<"\t"<< "nz: " << dir[2] << endl;
   #endif
   // Update Particle direction   
   particle->SetMomentum(particle->P()*dir[0], particle->P()*dir[1], particle->P()*dir[2], particle->Energy());
   // Update navigator
   navigator->SetCurrentDirection(dir[0], dir[1], dir[2]);
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t Boundary::DiffuseBounce(Particle* particle, TGeoNavigator* navigator, const Double_t* norm)
{
   #ifdef VERBOSE_MODE
      cout << "----------------------------" << endl;
      cout << "Diffuse Bounce" << endl;
      cout << "BEFORE - nx: " << particle->Nx() <<"\t"<< "ny: " << particle->Ny();
      cout << "\t" << "nz: " << particle->Ny() << endl;
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
            Error("DiffuseBounce","Could not find an axis perpendicular to normal.");
            return kFALSE;
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
   Double_t dir[3];
   navigator->LocalToMasterVect(ldir, dir);
   
   assert(TMath::Abs(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2] - 1.0) < TGeoShape::Tolerance());
   // Assert that our final direction is not perpendicular to the normal.
   // This could result in escaping the boundary
   assert(TMath::Abs(dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2]) > TGeoShape::Tolerance()); 
   
   #ifdef VERBOSE_MODE	
      cout << "AFTER - nx: " << dir[0] <<"\t"<< "nx: " << dir[1] <<"\t"<< "nx: " << dir[2] << endl;
   #endif
   
   // Update Particle Direction
   particle->SetMomentum(particle->P()*dir[0], particle->P()*dir[1], particle->P()*dir[2], particle->Energy());
   // Update navigator
   navigator->SetCurrentDirection(dir[0], dir[1], dir[2]);
   return kTRUE;
}

//______________________________________________________________________________
Double_t Boundary::DiffuseProbability(const Particle* particle, const Double_t* normal) const
{
   // Calculate the probability of making a diffuse bounce
   Double_t cosTheta = TMath::Abs(particle->Nx()*normal[0] + particle->Ny()*normal[1] + particle->Nz()*normal[2]);
   Double_t energyPerp = particle->Energy()*cosTheta*cosTheta;
   Double_t diffProb = fRoughness*energyPerp/this->FermiPotential();
   assert(diffProb <= 1.0 && diffProb >= 0.0);
   //return diffProb;
   return diffProb;
}


//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________

ClassImp(Detector)

//_____________________________________________________________________________
Detector::Detector()
             :Boundary()
{
// Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Detector", "Default Constructor");
   #endif
}

//_____________________________________________________________________________
Detector::Detector(const char *name, const TGeoShape* shape, const TGeoMedium* med, Double_t detEff)
             :Boundary(name, shape, med, 0.)
{
// constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Detector", "Constructor");
   #endif
   fDetectionEfficiency = detEff;
}

//_____________________________________________________________________________
Detector::Detector(const Detector& m)
             :Boundary(m),
              fDetectionEfficiency(m.fDetectionEfficiency)
{ 
   //copy constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Detector", "Copy Constructor");  
   #endif
}

//_____________________________________________________________________________
Detector& Detector::operator=(const Detector& m) 
{
   //assignment operator
   #ifdef PRINT_CONSTRUCTORS
      Info("Detector", "Assignment");
   #endif
   if(this!=&m) {
      Boundary::operator=(m);
      fDetectionEfficiency = m.fDetectionEfficiency;
   } 
   return *this;
}

//_____________________________________________________________________________
Detector::~Detector()
{
// Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Detector", "Destructor");
   #endif
}

//_____________________________________________________________________________
Bool_t Detector::Interact(Particle* particle, Double_t* normal, TGeoNavigator* navigator, TGeoNode* /*crossedNode*/, const char* /*initialPath*/)
{   
// -- Was particle detected?
   if (gRandom->Uniform(0.0,1.0) <= fDetectionEfficiency) {
      // -- Particle detected
      particle->IsDetected();
      return kFALSE;
   }
   
   // If not detected, reflect particle.
   if (this->ReflectParticle(particle, navigator, normal) == kFALSE) {
      Error("Interact","Reflect particle failed");
      throw runtime_error("Reflect particle failed");
   }
   return kTRUE;
}

//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________

ClassImp(BlackHole)

//_____________________________________________________________________________
BlackHole::BlackHole()
              :Volume()
{
// Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("BlackHole", "Default Constructor");
   #endif
}

//_____________________________________________________________________________
BlackHole::BlackHole(const char *name, const TGeoShape* shape, const TGeoMedium* med)
              :Volume(name,shape,med)
{
// constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("BlackHole", "Constructor");
   #endif
}

//_____________________________________________________________________________
BlackHole::BlackHole(const BlackHole& m)
              :Volume(m)
{ 
   //copy constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("BlackHole", "Copy Constructor");  
   #endif
}

//_____________________________________________________________________________
BlackHole& BlackHole::operator=(const BlackHole& m) 
{
   //assignment operator
   #ifdef PRINT_CONSTRUCTORS
      Info("BlackHole", "Assignment");
   #endif
   if(this!=&m) {
      Volume::operator=(m);
   } 
   return *this;
}

//_____________________________________________________________________________
BlackHole::~BlackHole()
{
// Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("BlackHole", "Destructor");
   #endif
}

//_____________________________________________________________________________
Bool_t BlackHole::Interact(Particle* particle, Double_t* /*normal*/, TGeoNavigator* /*navigator*/, TGeoNode* /*crossedNode*/, const char* /*initialPath*/)
{
// -- Particle is Lost if it finds itself in BlackHole
   particle->IsLost();
   return kFALSE;
}

