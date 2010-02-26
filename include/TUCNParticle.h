// UCN Particle class
// Author: Matthew Raso-Barnett  19/01/2009


#ifndef TUCNPARTICLE_H
#define TUCNPARTICLE_H

#include "TParticle.h"

#include <vector>

using std::vector;

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNParticle 													  //
//                   																	  //
////////////////////////////////////////////////////////////////////////////
class TUCNGeoMaterial;

class TUCNParticle :  public TParticle 
{

private:
	static const Int_t 		fgNeutronPDGCode = 2112;
	
	Bool_t 						fDecayed;
	Bool_t 						fLostToBoundary;
	Bool_t 						fDetected;
	Double_t 					fDistance;
	Int_t 						fBounces;
	Int_t 						fSpecularBounces;
	Int_t 						fDiffuseBounces;
	
	Double_t						fAvgMagField;
	Int_t							fFieldPointsSampled;
	
	Int_t							fInitialSeed;
	
public:
	// -- constructors
   TUCNParticle();
	TUCNParticle(Double_t* pos, Double_t* dir, Double_t kinetic_energy, Double_t startTime);
   TUCNParticle(const TUCNParticle &part);
   TUCNParticle& operator=(const TUCNParticle&);

	// -- destructor
	virtual ~TUCNParticle();

	// -- methods
	inline Double_t		Distance() const 								{return fDistance;}
	inline void				IncreaseDistance(Double_t stepsize) 	{fDistance += stepsize;}
	
	void						MadeBounce() 									{fBounces++;}
	void						MadeSpecularBounce() 						{fSpecularBounces++;}
	void						MadeDiffuseBounce() 							{fDiffuseBounces++;}
	Int_t						Bounces()										{return fBounces;}
	Int_t						SpecularBounces()								{return fSpecularBounces;}
	Int_t						DiffuseBounces()								{return fDiffuseBounces;}
	
	Bool_t					IsLostToWall(TUCNGeoMaterial* wall, const Double_t* normal) const;
	Double_t					DiffuseProbability(const Double_t diffuseCoeff, const Double_t* normal, const Double_t fermiPotential) const;
	Bool_t					WillDecay(Double_t timeInterval);
	
	Bool_t					LostToBoundary() const						{return fLostToBoundary;}
	void						LostToBoundary(Bool_t lost)				{fLostToBoundary = lost;}
	Bool_t					Decayed() const								{return fDecayed;}
	void						Decayed(Bool_t decayed)						{fDecayed = decayed;}
	Bool_t					Detected() const								{return fDetected;}
	void						Detected(Bool_t detected)  				{fDetected = detected;}
	
	Double_t					Dir()  			const;
	Double_t					DirX()  			const;
	Double_t					DirY()  			const;
	Double_t					DirZ()  			const;
	Double_t					Velocity()  	const;
	Double_t					VelocityX()  	const;
	Double_t					VelocityY()  	const;
	Double_t					VelocityZ()  	const;
	Double_t					Mass_GeV() 		const;
	Double_t					Mass_GeV_c() 	const;
	Double_t					Mass_GeV_c2() 	const;
	
	void						SampleMagField(const Double_t integratedField, const Double_t stepTime);
	Double_t					AvgMagField() const {return fAvgMagField;}
	
	void						SetInitialSeed(const Int_t seed) {fInitialSeed = seed;}
	Int_t						GetInitialSeed() const {return fInitialSeed;}
	
   ClassDef(TUCNParticle,1)   // Ultra-Cold Neutron
};

#endif
