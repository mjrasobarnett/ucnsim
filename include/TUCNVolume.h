// TUCNVolume
// Author: Matthew Raso-Barnett  09/06/2009

#ifndef ROOT_TUCNVolume
#define ROOT_TUCNVolume

#ifndef  ROOT_TGeoVolume
   #include "TGeoVolume.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNVolume                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNParticle;
class TUCNMaterial;
class TGeoNavigator;
class TGeoShape;
class TGeoMedium;

class TUCNVolume : public TGeoVolume
{
protected:
   
public:
   
   // -- constructors
   TUCNVolume();
   TUCNVolume(const char *name, const TGeoShape* shape, const TGeoMedium* med);
   TUCNVolume(const TUCNVolume&);
   TUCNVolume& operator=(const TUCNVolume&);
   
   // -- destructor
   virtual ~TUCNVolume();
   
   // -- methods
   virtual Bool_t  Interact(TUCNParticle* particle, Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath);
   
   virtual Double_t FermiPotential() const;
   virtual Double_t WPotential() const;
   virtual Double_t LossFactor() const;
   
   ClassDef(TUCNVolume, 1)
};


////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNTrackingVolume - These materials are types that allow         //
//
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNTrackingVolume : public TUCNVolume 
{
protected:
   
public:
   
   // -- constructors
   TUCNTrackingVolume();
   TUCNTrackingVolume(const char *name, const TGeoShape* shape, const TGeoMedium* med);
   TUCNTrackingVolume(const TUCNTrackingVolume&);
   TUCNTrackingVolume& operator=(const TUCNTrackingVolume&);
   
   // -- destructor
   virtual ~TUCNTrackingVolume();
   
   virtual Bool_t Interact(TUCNParticle* particle, Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath);
   
   ClassDef(TUCNTrackingVolume, 1)
};

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNBoundary                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNBoundary : public TUCNVolume 
{
protected:
   Double_t fRoughness;
   
   Bool_t AbsorbParticle(TUCNParticle* particle, Double_t* normal);
   Bool_t ReflectParticle(TUCNParticle* particle, TGeoNavigator* navigator, Double_t* normal);
   Bool_t SpecularBounce(TUCNParticle* particle, TGeoNavigator* navigator, const Double_t* norm);
   Bool_t DiffuseBounce(TUCNParticle* particle, TGeoNavigator* navigator, const Double_t* norm);
   Double_t DiffuseProbability(const TUCNParticle* particle, const Double_t* normal) const;
   
   
public:
   
   // -- constructors
   TUCNBoundary();
   TUCNBoundary(const char *name, const TGeoShape* shape, const TGeoMedium* med, Double_t surfaceRoughness);
   TUCNBoundary(const TUCNBoundary&);
   TUCNBoundary& operator=(const TUCNBoundary&);
   
   // -- destructor
   virtual ~TUCNBoundary();
   
   virtual Bool_t Interact(TUCNParticle* particle, Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath);
   
   ClassDef(TUCNBoundary, 1)
};


////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNDetector                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNDetector : public TUCNBoundary 
{
protected:
   Double_t fDetectionEfficiency;
   
public:
   
   // -- constructors
   TUCNDetector();
   TUCNDetector(const char *name, const TGeoShape* shape, const TGeoMedium* med, Double_t detectEff);
   TUCNDetector(const TUCNDetector&);
   TUCNDetector& operator=(const TUCNDetector&);
   
   // -- destructor
   virtual ~TUCNDetector();
   
   virtual Bool_t Interact(TUCNParticle* particle, Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath);
   
   ClassDef(TUCNDetector, 1)
};


////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNBlackHole - BlackHole-ytpe materials are perfect absorbers and  //
//    represent the particle being lost through a hole/gap in the system  //
//    into the outer geometry                                             //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNBlackHole : public TUCNVolume 
{
protected:
   
public:
   
   // -- constructors
   TUCNBlackHole();
   TUCNBlackHole(const char *name, const TGeoShape* shape, const TGeoMedium* med);
   TUCNBlackHole(const TUCNBlackHole&);
   TUCNBlackHole& operator=(const TUCNBlackHole&);
   
   // -- destructor
   virtual ~TUCNBlackHole();
   
   virtual Bool_t Interact(TUCNParticle* particle, Double_t* normal, TGeoNavigator* navigator, TGeoNode* crossedNode, const char* initialPath);
   
   ClassDef(TUCNBlackHole, 1)
};

#endif
