#ifndef TUCNDATA_H
#define TUCNDATA_H

#include "TNamed.h"
#include "TUCNGeoManager.h"
#include "TUCNParticle.h"
#include "TTree.h"
#include "TGeoTrack.h"

#include <string>
#include <vector>


using namespace std;

class TUCNData : public TNamed {
		protected:
		Bool_t         fFullTrack; // Store full track or only end and beginning
		TTree*         fTracks;
		//TUCNGeoManager fManager;
		
		private:
		TGeoTrack    * fCurrentTrack;     
		TUCNParticle * fCurrentParticle;
		
	public:
		TUCNData(void); 
		TUCNData(const char * name, const char * title);
		virtual ~TUCNData(void);
		
		// Geometry
		Bool_t	LoadGeometry(TString filename);
		//TUCNGeoManager * GetGeoManager(void) { return &fManager; }
		
		// Fields
		
		// Tracking
		void		StoreFullTracks(Bool_t value = kTRUE) { fFullTrack = value; }
		void		AddTrack(TVirtualGeoTrack* track);
		void		AddParticle(TUCNParticle* particle);
		TTree*	GetTracks(void) {return fTracks;}
//		TGeoTrack* GetTrack(void);
		
   ClassDef(TUCNData, 1) // UCN Data Object
};
#endif
