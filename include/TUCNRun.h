// TUCNRun
// Author: Matthew Raso-Barnett  08/10/2009

#ifndef ROOT_TUCNRun
#define ROOT_TUCNRun

#include "TNamed.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TPolyMarker3D.h"

#include "TUCNData.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNRun															  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNRun : public TNamed 
{
	protected:
		TUCNRun(const TUCNRun&); 
	 	TUCNRun& operator=(const TUCNRun&);

		TUCNData*				fData;
		
	public:
		// -- constructors
		TUCNRun();
		TUCNRun(const char *name, const char *title);
		// -- destructor
		virtual ~TUCNRun();
		
		void						Initialise(Int_t particles, Double_t totalEnergy);
		
		void 						DrawParticles(TCanvas* canvas, TPolyMarker3D* points);
		void						DrawTrack(TCanvas* canvas, Int_t trackID);
		Bool_t 					GenerateMonoEnergeticParticles(Int_t totalParticles, Double_t totalEnergy);
		TGeoTrack*				GetTrack(Int_t trackID);
		TUCNParticle*			GetParticle(Int_t particleID);
		Bool_t 					PropagateAllTracks(Double_t runTime, Double_t maxStepTime);
		Bool_t 					PropagateAllTracks(Int_t steps, Double_t maxStepTime);
		Bool_t 					PropagateTrack(Double_t runTime, Double_t maxStepTime, Int_t trackIndex = 0);
		Bool_t 					PropagateTrack(Int_t steps, Double_t maxStepTime, Int_t trackIndex = 0);
		
		void						WriteOutData(TFile* file);
		

	ClassDef(TUCNRun, 1)      
};

#endif
