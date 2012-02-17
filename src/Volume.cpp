// Volume
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Volume.hpp"

#include "TGeoElement.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include "TGeoNavigator.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"

#include "Particle.hpp"
#include "Material.hpp"
#include "Observer.hpp"
#include "RunConfig.hpp"

#include "Units.hpp"
#include "Constants.hpp"

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
Bool_t Volume::Interact(Particle* /*particle*/, const Double_t* /*normal*/, TGeoNavigator* /*navigator*/, TGeoNode* /*crossedNode*/, const char* /*initialPath*/, const RunConfig& /*runconfig*/)
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
Bool_t TrackingVolume::Interact(Particle* /*particle*/, const Double_t* /*normal*/, TGeoNavigator* /*navigator*/, TGeoNode* /*crossedNode*/, const char* /*initialPath*/, const RunConfig& /*runconfig*/)
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
Bool_t Boundary::Interact(Particle* particle, const Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath, const RunConfig& runConfig)
{
// -- Interaction of particle with the boundary material
   // -- Is Track on the surface of a boundary?
   #ifdef VERBOSE_MODE
      cout << "On Boundary - Deciding what action to take..." << endl;
   #endif
         
   //------------------------------------------------------
   // 1. -- Calculate whether particle is absorbed/upscatteed by wall nuclei
   if (runConfig.WallLossesOn() == kTRUE) {
      // Check if wall losses are turned on
      if (this->AbsorbParticle(particle, normal) == kTRUE) {
         // Particle was absorbed
         #ifdef VERBOSE_MODE
            cout << "Particle absorbed by boundary" << endl;
         #endif
         particle->IsAbsorbed();
         return kFALSE;
      }
   }
   //------------------------------------------------------
   // 2. -- Reflect particle
   if (particle->Reflect(normal, navigator, crossedNode, initialPath) == kFALSE) {
      Error("Interact","Reflect particle failed");
      throw runtime_error("Reflection of particle failed");
   }  
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t Boundary::AbsorbParticle(Particle* particle, const Double_t* normal)
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
      Warning("AbsorbParticle","No Wall Losses for boundary %s. V: %f, W: %f", this->GetName(), this->FermiPotential(), this->WPotential());
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
Bool_t Detector::Interact(Particle* particle, const Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath, const RunConfig& /*runconfig*/)
{   
// -- Was particle detected?
   if (gRandom->Uniform(0.0,1.0) <= fDetectionEfficiency) {
      // -- Particle detected
      particle->IsDetected();
      return kFALSE;
   }
   
   // If not detected, reflect particle.
   if (particle->Reflect(normal, navigator, crossedNode, initialPath) == kFALSE) {
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
Bool_t BlackHole::Interact(Particle* particle, const Double_t* /*normal*/, TGeoNavigator* /*navigator*/, TGeoNode* /*crossedNode*/, const char* /*initialPath*/, const RunConfig& /*runconfig*/)
{
// -- Particle is Lost if it finds itself in BlackHole
   particle->IsLost();
   return kFALSE;
}


