// Volume
// Author: Matthew Raso-Barnett  09/06/2009

#ifndef ROOT_Volume
#define ROOT_Volume

#include "TGeoVolume.h"
#include "MagField.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    Volume                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Particle;
class Material;
class TGeoNavigator;
class TGeoShape;
class TGeoMedium;
class RunConfig;

class Volume : public TGeoVolume
{   
public:
   
   // -- constructors
   Volume();
   Volume(const char *name, const TGeoShape* shape, const TGeoMedium* med);
   Volume(const Volume&);
   Volume& operator=(const Volume&);
   
   // -- destructor
   virtual ~Volume();
   
   // -- methods
   virtual Bool_t  Interact(Particle* particle, const Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath, const RunConfig& runconfig);
   
   virtual Double_t FermiPotential() const;
   virtual Double_t WPotential() const;
   virtual Double_t LossFactor() const;
   virtual Bool_t   IsTrackingVolume() const {return kFALSE;}
   
   ClassDef(Volume, 1)
};


////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TrackingVolume - These materials are types that allow         //
//
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TrackingVolume : public Volume 
{
protected:
   
public:
   
   // -- constructors
   TrackingVolume();
   TrackingVolume(const char *name, const TGeoShape* shape, const TGeoMedium* med);
   TrackingVolume(const TrackingVolume&);
   TrackingVolume& operator=(const TrackingVolume&);
   
   // -- destructor
   virtual ~TrackingVolume();
   
   virtual Bool_t Interact(Particle* particle, const Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath, const RunConfig& runconfig);
   virtual Bool_t IsTrackingVolume() const {return kTRUE;}
   
   ClassDef(TrackingVolume, 1)
};

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    Boundary                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class Boundary : public Volume 
{
protected:
   Double_t fRoughness;
   
   Bool_t AbsorbParticle(Particle* particle, const Double_t* normal);
   
public:
   
   // -- constructors
   Boundary();
   Boundary(const char *name, const TGeoShape* shape, const TGeoMedium* med, Double_t surfaceRoughness);
   Boundary(const Boundary&);
   Boundary& operator=(const Boundary&);
   
   // -- destructor
   virtual ~Boundary();
   
   virtual Bool_t Interact(Particle* particle, const Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath, const RunConfig& runconfig);
   
   Double_t GetRoughness() const {return fRoughness;}
   
   ClassDef(Boundary, 1)
};


////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    Detector                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class Detector : public Boundary 
{
protected:
   Double_t fDetectionEfficiency;
   
public:
   
   // -- constructors
   Detector();
   Detector(const char *name, const TGeoShape* shape, const TGeoMedium* med, Double_t detectEff);
   Detector(const Detector&);
   Detector& operator=(const Detector&);
   
   // -- destructor
   virtual ~Detector();
   
   virtual Bool_t Interact(Particle* particle, const Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath, const RunConfig& runconfig);
   
   ClassDef(Detector, 1)
};


////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    BlackHole - BlackHole-ytpe materials are perfect absorbers and  //
//    represent the particle being lost through a hole/gap in the system  //
//    into the outer geometry                                             //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class BlackHole : public Volume 
{
protected:
   
public:
   
   // -- constructors
   BlackHole();
   BlackHole(const char *name, const TGeoShape* shape, const TGeoMedium* med);
   BlackHole(const BlackHole&);
   BlackHole& operator=(const BlackHole&);
   
   // -- destructor
   virtual ~BlackHole();
   
   virtual Bool_t Interact(Particle* particle, const Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath, const RunConfig& runconfig);
   
   ClassDef(BlackHole, 1)
};

#endif
