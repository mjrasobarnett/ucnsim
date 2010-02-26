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
class TGeoTrack;

class TCanvas;
class TUCNConfigFile;
class TUCNFieldManager;
class TUCNData;
class TUCNParticle;

class TUCNRun : public TNamed 
{
	protected:
		TUCNData*				fData;
		
		Int_t						fNeutrons;
		Double_t					fTotalEnergy;
		Double_t					fRunTime;
		Double_t					fMaxStepTime;
		Double_t					fDiffuseCoeff;
		Bool_t					fSampleMagField;
		
		Int_t 					fBoundaryLossCounter;
		Int_t 					fDetectedCounter;
		Int_t						fDecayedCounter;
		
		TUCNData*				GetData() {return fData;}
		
	public:
		// -- constructors
		TUCNRun();
		TUCNRun(const TUCNRun&); 
	 	TUCNRun& operator=(const TUCNRun&);
		TUCNRun(const char *name, const char *title);
		// -- destructor
		virtual ~TUCNRun();
		
		Bool_t					Initialise(TUCNConfigFile* configFile);
		Bool_t					Export(TString& outputFile);
			
		Int_t						Neutrons() const					{return fNeutrons;}
		Double_t					TotalEnergy() const				{return fTotalEnergy;}
		Double_t					RunTime() const					{return fRunTime;}
		Double_t					MaxStepTime()						{return fMaxStepTime;}
		Int_t 					Detected() const					{return fDetectedCounter;}
		Int_t 					LostToBoundary() const			{return fBoundaryLossCounter;}
		Int_t 					Decayed() const					{return fDecayedCounter;}
		
		void						DrawParticles(TCanvas* canvas, TPolyMarker3D* points);
		void						DrawTrack(TCanvas* canvas, Int_t trackID);
		
		Bool_t					AddTrack(TGeoTrack* track);
		Bool_t					AddInitialParticle(TUCNParticle* particle);
		Bool_t					AddParticle(TUCNParticle* particle);
		TGeoTrack*				GetTrack(Int_t trackID);
		TUCNParticle*			GetInitialParticle(Int_t particleID);
		TUCNParticle*			GetParticle(Int_t particleID);
		
		Bool_t					Propagate(TGeoManager* geoManager, TUCNFieldManager* fieldManager);
		Bool_t 					PropagateTrack(TGeoManager* geoManager, TUCNFieldManager* fieldManager);
		
	
		

		

	ClassDef(TUCNRun, 1)      
};

#endif
