// UCN Particle class
// Author: Matthew Raso-Barnett  19/01/2009


#ifndef TUCNPARTICLE_H
#define TUCNPARTICLE_H

#include <TParticle.h>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNParticle 													  //
//                   																	  //
////////////////////////////////////////////////////////////////////////////

class TUCNParticle :  public TParticle 
{

private:
	static const Int_t 	fgNeutronPDGCode = 2112;
	static const Double_t fgDiffuseProb = 0.1;
	
	Bool_t fDecayed;
	Bool_t fLost;		
	Bool_t fDetected;

	Int_t 							fBounces;
	Int_t 							fSpecularBounces;
	Int_t 							fDiffuseBounces;
	
		
public:
	// -- constructors
   TUCNParticle();
	TUCNParticle(Double_t* pos, Double_t* dir, Double_t kinetic_energy, Double_t startTime);
   TUCNParticle(const TUCNParticle &part);
   TUCNParticle& operator=(const TUCNParticle&);

	// -- destructor
	virtual ~TUCNParticle();

	// -- methods
//	inline Double_t		PathLength() const {return fPathLength;}
//	inline void 			PathLength(Double_t pathlength) {fPathLength = pathlength;}
//	inline void 			IncreasePathLength(Double_t distance) {fPathLength += distance;}		
	
	void 						MadeBounce() 									{fBounces++;}
	void 						MadeSpecularBounce() 						{fSpecularBounces++;}
	void 						MadeDiffuseBounce() 							{fDiffuseBounces++;}
	Int_t						Bounces()										{return fBounces;}
	Int_t						SpecularBounces()								{return fSpecularBounces;}
	Int_t						DiffuseBounces()								{return fDiffuseBounces;}
	
//	void						DiffuseProbability(Double_t prob) 		{fDiffuseProb = prob;}
	Double_t 				DiffuseProbability() const 				{return fgDiffuseProb;}
	
	Bool_t 					WillDecay(Double_t timeInterval);
	
	Bool_t 					Lost() const 									{return fLost;}
	Bool_t 					Decayed() const 								{return fDecayed;}
	Bool_t 					Detected() const 								{return fDetected;}
	void 						Lost(Bool_t lost)  							{fLost = lost;}
	void 						Decayed(Bool_t decayed)  					{fDecayed = decayed;}
	void 						Detected(Bool_t detected)  				{fDetected = detected;}
	
	Double_t          	Dir()  			const;	
	Double_t          	DirX()  			const;	
	Double_t          	DirY()  			const;	
	Double_t          	DirZ()  			const;
	Double_t          	Velocity()  	const;	
	Double_t          	VelocityX()  	const;	
	Double_t          	VelocityY()  	const;	
	Double_t          	VelocityZ()  	const;	
	Double_t					Mass_GeV() 		const;
	Double_t					Mass_GeV_c() 	const;
	Double_t					Mass_GeV_c2() 	const;
	
	
   ClassDef(TUCNParticle,1)   // Ultra-Cold Neutron
};

#endif
