// TUCNRunManager
// Author: Matthew Raso-Barnett  01/08/2009

#ifndef ROOT_TUCNRunManager
#define ROOT_TUCNRunManager

#include "TNamed.h"

#include "TUCNGeoManager.h"
//#include "TUCNData.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TPolyMarker3D.h"


////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNRunManager													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNRunManager : public TNamed 
{

protected:
	TUCNRunManager(const TUCNRunManager&); 
 	TUCNRunManager& operator=(const TUCNRunManager&);
	
	TUCNGeoManager*				fManager;
//	TUCNData*						fData;
			
public:
	// -- constructors
   TUCNRunManager();
   
	// -- destructor
	virtual ~TUCNRunManager();

	// -- methods
	TUCNGeoManager* 		GetGeoManager() { return fManager; } 
	
	TGeoNode*				CreateGeometry();
	
	void						TurnGravityOn() { fManager->SetGravity(kTRUE); }
			
	Bool_t 					GenerateMonoEnergeticParticles(TGeoVolume* sourceVolume, TGeoMatrix* matrix, Int_t totalParticles, Double_t totalEnergy);
	
	Bool_t 					PropagateAllTracks(Double_t runTime, Double_t maxStepTime);
	Bool_t 					PropagateAllTracks(Int_t steps, Double_t maxStepTime);
	Bool_t 					PropagateTrack(Double_t runTime, Double_t maxStepTime, Int_t trackIndex = 0);
	Bool_t 					PropagateTrack(Int_t steps, Double_t maxStepTime, Int_t trackIndex = 0);
//	void 						DrawParticles(TCanvas* canvas, TPolyMarker3D* points);
//	void						DrawTrack(TCanvas* canvas, TVirtualGeoTrack* track);
	
//	void						WriteOutData(TFile* file);
//	TGeoTrack* 				GetTrack(Int_t trackID);
	
	
	
   ClassDef(TUCNRunManager, 1)      
};

#endif
