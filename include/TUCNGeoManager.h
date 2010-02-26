// TUCNGeoManager
// Author: Matthew Raso-Barnett  22/05/2009

#ifndef ROOT_TUCNGeoManager
#define ROOT_TUCNGeoManager

#ifndef 	ROOT_TGeoManager
#include "TGeoManager.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNGeoManager													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNParticle;
class TUCNParticleCloud;
class TVirtualGeoTrack;
class TUCNGeoNavigator;
class TUCNGravField;
class TVector3;

class TUCNGeoManager : public TGeoManager 
{

protected:
	
private:
	TUCNGeoNavigator* 	fUCNNavigator;					// The current navigator
	TUCNGravField*			fGravField;						// The grav field direction vector
	
	TUCNParticleCloud*   fParticleCloud;
		
	Int_t 					fNavigatorIndex; 				// Index of Current Navigator held in TGeoManager
	
public:
	
	// -- constructors
   TUCNGeoManager();
   TUCNGeoManager(const char *name, const char *title);
   
	// -- destructor
   virtual ~TUCNGeoManager();
	
	// -- methods
	
	// Make UCNVolumes using TUCNGeoBuilder
	TGeoVolume* 	 		   MakeBox(const char *name, TGeoMedium *medium, Double_t dx, Double_t dy, Double_t dz);
 	TGeoVolume*				   MakeTube(const char *name, TGeoMedium *medium, Double_t rmin, Double_t rmax, Double_t dz);
	
	TGeoVolume*             MakeArb8(const char *name, TGeoMedium *medium,
                                     Double_t dz, Double_t *vertices=0);
   TGeoVolume*             MakeCone(const char *name, TGeoMedium *medium,
                                      Double_t dz, Double_t rmin1, Double_t rmax1,
                                      Double_t rmin2, Double_t rmax2);
   TGeoVolume*             MakeCons(const char *name, TGeoMedium *medium,
                                      Double_t dz, Double_t rmin1, Double_t rmax1,
                                      Double_t rmin2, Double_t rmax2,
                                      Double_t phi1, Double_t phi2);
   TGeoVolume*             MakeCtub(const char *name, TGeoMedium *medium,
                                      Double_t rmin, Double_t rmax, Double_t dz, Double_t phi1, Double_t phi2,
                                      Double_t lx, Double_t ly, Double_t lz, Double_t tx, Double_t ty, Double_t tz);
   TGeoVolume*             MakeEltu(const char *name, TGeoMedium *medium,
                                      Double_t a, Double_t b, Double_t dz);
   TGeoVolume*             MakeGtra(const char *name, TGeoMedium *medium,
                                   Double_t dz, Double_t theta, Double_t phi, Double_t twist, Double_t h1,
                                   Double_t bl1, Double_t tl1, Double_t alpha1, Double_t h2, Double_t bl2,
                                   Double_t tl2, Double_t alpha2);
   TGeoVolume*             MakePara(const char *name, TGeoMedium *medium,
                                     Double_t dx, Double_t dy, Double_t dz,
                                     Double_t alpha, Double_t theta, Double_t phi);
   TGeoVolume*             MakePcon(const char *name, TGeoMedium *medium,
                                      Double_t phi, Double_t dphi, Int_t nz);
   TGeoVolume*             MakeParaboloid(const char *name, TGeoMedium *medium,
                                      Double_t rlo, Double_t rhi, Double_t dz);
   TGeoVolume*             MakeHype(const char *name, TGeoMedium *medium,
                                      Double_t rin, Double_t stin, Double_t rout, Double_t stout, Double_t dz);
   TGeoVolume*             MakePgon(const char *name, TGeoMedium *medium,
                                      Double_t phi, Double_t dphi, Int_t nedges, Int_t nz);
   TGeoVolume*             MakeSphere(const char *name, TGeoMedium *medium,
                                     Double_t rmin, Double_t rmax,
                                     Double_t themin=0, Double_t themax=180,
                                     Double_t phimin=0, Double_t phimax=360);
   TGeoVolume*             MakeTorus(const char *name, TGeoMedium *medium, Double_t r,
                                    Double_t rmin, Double_t rmax, Double_t phi1=0, Double_t dphi=360);
   TGeoVolume*             MakeTrap(const char *name, TGeoMedium *medium,
                                   Double_t dz, Double_t theta, Double_t phi, Double_t h1,
                                   Double_t bl1, Double_t tl1, Double_t alpha1, Double_t h2, Double_t bl2,
                                   Double_t tl2, Double_t alpha2);
   TGeoVolume*             MakeTrd1(const char *name, TGeoMedium *medium,
                                      Double_t dx1, Double_t dx2, Double_t dy, Double_t dz);
   TGeoVolume*             MakeTrd2(const char *name, TGeoMedium *medium,
                                      Double_t dx1, Double_t dx2, Double_t dy1, Double_t dy2,
                                      Double_t dz);
   TGeoVolume*             MakeTubs(const char *name, TGeoMedium *medium,
                                      Double_t rmin, Double_t rmax, Double_t dz,
                                      Double_t phi1, Double_t phi2);
   TGeoVolume*             MakeXtru(const char *name, TGeoMedium *medium,
                                   Int_t nz);
	
	// Make TUCNGeoNavigator
	Int_t 						AddUCNNavigator(TUCNGeoNavigator *navigator);
   TUCNGeoNavigator*			GetUCNNavigator() const 	{return fUCNNavigator;} 
	Int_t							GetNavigatorIndex() const	{return fNavigatorIndex;} 		
	
	// Navigational Methods
	TGeoNode*					FindNextBoundaryAndStepAlongParabola(TVirtualGeoTrack* track, TUCNGravField* field, Double_t stepTime, Bool_t compsafe=kFALSE); 	
	
	// Make Gravitational Field
	TUCNGravField*				AddGravField(Double_t nx, Double_t ny, Double_t nz);
	TUCNGravField*				GetGravField() const {return fGravField;}
		
	// Make Particle Cloud
	TUCNParticleCloud*	   AddParticleCloud();
	TUCNParticleCloud*		GetParticleCloud() const {return fParticleCloud;}
	
	// Make Tracks
	void							MakeTracks(TUCNParticleCloud* cloud);
	void							PropagateTracks(Double_t runTime, Double_t maxStepTime, Bool_t storeData=kFALSE, Bool_t storeTrack=kFALSE); 
	void							PropagateTracks(Int_t steps, Double_t maxStepTime, Bool_t storeTrack=kFALSE); 
	
   ClassDef(TUCNGeoManager, 1)          // UCN geometry manager
};

#endif
