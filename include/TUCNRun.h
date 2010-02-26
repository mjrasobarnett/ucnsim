// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#ifndef ROOT_TUCNRun
#define ROOT_TUCNRun

#include "TNamed.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNRun															  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TGeoManager;
class TGeoVolume;
class TGeoMatrix;
class TPolyMarker3D;
class TFile;
class TVirtualGeoTrack;

class TCanvas;
class TUCNConfigFile;
class TUCNFieldManager;
class TUCNData;
class TUCNParticle;

class TUCNRun : public TNamed 
{
	protected:
		TUCNRun(const TUCNRun&); 
	 	TUCNRun& operator=(const TUCNRun&);

		TUCNData*				fData;
		
		Int_t						fNeutrons;
		Double_t					fTotalEnergy;
		Double_t					fRunTime;
		Double_t					fMaxStepTime;
		
		Int_t 					fBoundaryLossCounter;
		Int_t 					fDetectedCounter;
		Int_t						fDecayedCounter;
	public:
		// -- constructors
		TUCNRun();
		TUCNRun(const char *name, const char *title);
		// -- destructor
		virtual ~TUCNRun();
		
		Bool_t					Initialise(TUCNConfigFile* configFile);
		
		Int_t						Neutrons() {return fNeutrons;}
		Double_t					TotalEnergy() {return fTotalEnergy;}
		Double_t					RunTime() {return fRunTime;}
		Double_t					MaxStepTime() {return fMaxStepTime;}
		
		void						DrawParticles(TCanvas* canvas, TPolyMarker3D* points);
		void						DrawTrack(TCanvas* canvas, Int_t trackID);
		TVirtualGeoTrack*		GetTrack(Int_t trackID);
		TUCNParticle*			GetParticle(Int_t particleID);
		
		Bool_t					PropagateTracks(TUCNFieldManager* fieldManager);
		Bool_t 					Propagate(TVirtualGeoTrack* track, TUCNFieldManager* fieldManager);
		
		void						WriteOutData(TFile* file);
		
		Int_t 					Detected() const {return fDetectedCounter;}
		Int_t 					LostToBoundary() const {return fBoundaryLossCounter;}
		Int_t 					Decayed() const {return fDecayedCounter;}
		

	ClassDef(TUCNRun, 1)      
};

#endif
