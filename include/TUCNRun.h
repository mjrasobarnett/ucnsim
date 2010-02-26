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
class TGeoNode;
class TGeoVolume;
class TGeoMatrix;
class TGeoHMatrix;
class TCanvas;
class TVirtualGeoTrack;

class TUCNConfigFile;
class TUCNFieldManager;
class TUCNData;
class TUCNParticle;
class TUCNGravField;
class TUCNMagField;
class TUCNGeoManager;
class TUCNGeoMaterial;
class TUCNFieldManager;


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
		Bool_t					fWallLosses;
		
		Int_t 					fBoundaryLossCounter;
		Int_t 					fDetectedCounter;
		Int_t						fDecayedCounter;
		
		
		static const Double_t 		fgTolerance = 1.E-10;

		TGeoNode*						fUCNNextNode;
		Double_t							fUCNNormal[3];
		Double_t							fStepTime;
		Bool_t 							fUCNIsStepEntering;
		Bool_t 							fUCNIsStepExiting;
		Bool_t							fUCNIsOutside;        //! flag that current point is outside geometry
		Bool_t							fUCNIsOnBoundary;     //! flag that current point is on some boundary
		
		
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
		
		Bool_t					AddTrack(TVirtualGeoTrack* track);
		Bool_t					AddInitialParticle(TUCNParticle* particle);
		Bool_t					AddParticle(TUCNParticle* particle);
		TGeoTrack*				GetTrack(Int_t trackID);
		TUCNParticle*			GetInitialParticle(Int_t particleID);
		TUCNParticle*			GetParticle(Int_t particleID);
		
		Bool_t					Propagate(TGeoManager* geoManager, TUCNFieldManager* fieldManager);
		Bool_t 					PropagateTrack(TGeoManager* geoManager, TUCNFieldManager* fieldManager);
		
	
		// -- methods
		Bool_t                  IsUCNStepEntering() const       {return fUCNIsStepEntering;}
	   Bool_t                  IsUCNStepExiting() const        {return fUCNIsStepExiting;}
	   Bool_t                  IsUCNOutside() const            {return fUCNIsOutside;}
	   Bool_t                  IsUCNOnBoundary() const         {return fUCNIsOnBoundary;}

		// New methods to find the normal vector of fUCNNextNode, instead of fNextNode
		Double_t*					FindUCNNormal();				
		const Double_t*			GetNormal() const        			{return fUCNNormal;}

		// Track Propagation methods
		TGeoNode*					FindNextDaughterBoundaryAlongParabola(Double_t* point, Double_t* velocity, 
																								Double_t* field, Int_t &idaughter, Bool_t compmatrix=kFALSE);
		TGeoNode*					FindNextBoundaryAndStepAlongParabola(TVirtualGeoTrack* track, TUCNGravField* field,
																								Double_t stepTime, Bool_t compsafe=kFALSE);
		TGeoNode*					FindNextBoundaryAlongParabola(TVirtualGeoTrack* track, TUCNGravField* field, Double_t stepTime, Bool_t onBoundary);
		
		Double_t                DetermineNextStepTime(TUCNParticle* particle, const Double_t maxStepTime, const Double_t runTime=0.);
		Double_t 					GetStepTime() const {return fStepTime;}
		void							SetStepTime(Double_t stepTime); 

		Bool_t						MakeStep(TVirtualGeoTrack* track, TUCNGravField* gravField=0, TUCNMagField* magField=0);

		Bool_t 						Bounce(TUCNParticle* particle, const Double_t* normal, const TUCNGeoMaterial* wallMaterial);
		Bool_t 						SpecularBounce(Double_t* dir, const Double_t* norm);
		Bool_t 						DiffuseBounce(Double_t* dir, const Double_t* norm);
		void							UpdateParticle(TUCNParticle* particle, const Double_t timeInterval=0., const TUCNGravField* gravField=0);
		

	ClassDef(TUCNRun, 1)      
};

#endif
