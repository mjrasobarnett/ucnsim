// TUCNGeoNavigator
// Author: Matthew Raso-Barnett  27/05/2009

#ifndef ROOT_TUCNGeoNavigator
#define ROOT_TUCNGeoNavigator

#ifndef 	ROOT_TGeoNavigator
#include "TGeoNavigator.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNGeoNavigator												  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNParticle;
class TUCNGravField;
class TUCNGeoManager;
class TGeoManager;
class TGeoNode;
class TGeoVolume;
class TGeoMatrix;
class TGeoHMatrix;


class TUCNGeoNavigator : public TGeoNavigator 
{

protected:
	TUCNGeoNavigator(const TUCNGeoNavigator&); 
 	TUCNGeoNavigator& operator=(const TUCNGeoNavigator&);
	static const Double_t 		fgTolerance = 1.E-10;
	static const Int_t 			fgMaxSteps = 100000000;
		
	TGeoNode*						fUCNNextNode;
	Double_t							fUCNNormal[3];
	
	Double_t					 		fStepTime;			  
	
	Bool_t 							fUCNIsStepEntering;
	Bool_t 							fUCNIsStepExiting;
	Bool_t                		fUCNIsOutside;        //! flag that current point is outside geometry
   Bool_t                		fUCNIsOnBoundary;     //! flag that current point is on some boundary	
	
	Int_t 					 		fLostCounter;
	Int_t 					 		fDetectedCounter;
	Int_t						 		fDecayedCounter;
	
public:
	// -- constructors
   TUCNGeoNavigator();
   TUCNGeoNavigator(TUCNGeoManager* geom);
   
	// -- destructor
	virtual ~TUCNGeoNavigator();

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
		
	Double_t                DetermineNextStepTime(TUCNParticle* particle, const Double_t maxStepTime, const Double_t runTime=0.);
	Double_t 					GetStepTime() const {return fStepTime;}
	void							SetStepTime(Double_t stepTime); 
		
	Bool_t						PropagateTrack(TVirtualGeoTrack* track, const Double_t runTime, const Double_t maxStepTime);
	Bool_t						PropagateTrack(TVirtualGeoTrack* track, const Int_t steps, const Double_t maxStepTime);
	
//	Bool_t						MakeStep(TVirtualGeoTrack* track, TUCNGravField* field);
	
	Bool_t 						Bounce(TVirtualGeoTrack* track);
	Bool_t 						SpecularBounce(Double_t* dir, const Double_t* norm);
	Bool_t 						DiffuseBounce(Double_t* dir, const Double_t* norm);
	void							UpdateTrack(TVirtualGeoTrack* track, Double_t timeInterval=0.);
	
//	Int_t 						GetNumberLost() const {return fLostCounter;}
//	Int_t 						GetNumberDetected() const {return fDetectedCounter;}
//	Int_t 						GetNumberDecayed() const {return fDecayedCounter;}
		
   ClassDef(TUCNGeoNavigator, 1)          // UCN geometry manager
};

#endif
