#ifndef TUCNDATA_H
#define TUCNDATA_H

#include "TNamed.h"
#include "TTree.h"
#include "TGeoTrack.h"

class TUCNParticle;

class TUCNData : public TNamed {
	protected:
		TTree					*fTracks;
		TTree					*fInitialParticleStates;
		TTree					*fFinalParticleStates;
		
	private:
		TGeoTrack			*fCurrentTrack;     
		TUCNParticle		*fInitialParticle;
		TUCNParticle		*fCurrentParticle;
		
		TTree*				GetTracksTree()					{return fTracks;}
		TTree*				GetInitialParticlesTree()		{return fInitialParticleStates;}
		TTree*				GetFinalParticlesTree()			{return fFinalParticleStates;}
		
	public:
		TUCNData(void); 
		TUCNData(const char *name, const char *title);
		virtual ~TUCNData(void);
		
		Bool_t					ChecksOut();
		
		// Storage of Tracks and Particles
		Bool_t					AddTrack(TGeoTrack* track);
		Bool_t					AddInitialParticleState(TUCNParticle* particle);
		Bool_t					AddFinalParticleState(TUCNParticle* particle);
		
		TGeoTrack*				GetTrack(Int_t trackID);
		TUCNParticle*			GetInitialParticleState(Int_t particleID);
		TUCNParticle*			GetFinalParticleState(Int_t particleID);
		
   ClassDef(TUCNData, 1) // UCN Data Object
};
#endif
