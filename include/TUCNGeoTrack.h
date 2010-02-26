// @(#)root/geom:$Id: $
// Author: Matthew Raso-Barnett  February 2009

#ifndef TUCNGEOTRACK_H
#define TUCNGEOTRACK_H

//#define STORE_TRACKS

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TUCNGeoTrack -         																  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////


class TUCNGeoTrack : public TGeoTrack
{
	private: 
		// -- fields
		Int_t fTrackID;
		
		Double_t fStepTime;
		
		Double_t fTolerance;
		
		Bool_t fDetected;
		Bool_t fThrownAway;
		
		Double_t fDiffuseProb;
		
		Int_t fBounceCounter;
		Int_t fSpecBounceCounter;
		Int_t fDiffBounceCounter;
		
		// -- methods
		
	public: 
		// -- Constructors and Destructor
		TUCNGeoTrack();
		TUCNGeoTrack(Int_t trackid, Double_t steptime);		
		virtual ~TUCNGeoTrack();
			
		// -- Methods
		void InitialiseTrack(TParticle* neutron);
		
/*		inline Bool_t Detected() const { return fDetected; }
		inline Bool_t ThrownAway() const { return fThrownAway; }
		inline void SetStepSize(Double_t steptime) { fStepTime = steptime; }
		inline void SetDiffuseProbDecimal(Double_t diffprob) { fDiffuseProb = diffprob; }
		
		inline Int_t Bounces() const { return fBounceCounter; }
		inline Int_t DiffuseBounces() const { return fDiffBounceCounter; }
		inline Int_t SpecularBounces() const { return fSpecBounceCounter; }
		
		Bool_t Step(Double_t steptime, TUCNParticle* neutron);
		Bool_t Propagate(Int_t maxsteps);
		Bool_t PropagateForSetTime(Double_t seconds);
*/	
		ClassDef(TUCNGeoTrack,1)
		
};

#endif
