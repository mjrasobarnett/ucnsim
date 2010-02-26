// TUCNRunManager
// Author: Matthew Raso-Barnett  01/08/2009

#ifndef ROOT_TUCNRunManager
#define ROOT_TUCNRunManager

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNRunManager													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNRunManager  
{

protected:
	TUCNRunManager(const TUCNRunManager&); 
 	TUCNRunManager& operator=(const TUCNRunManager&);
	
private:
		
public:
	// -- constructors
   TUCNRunManager();
   TUCNRunManager(TUCNRunManager* geom);
   
	// -- destructor
	virtual ~TUCNRunManager();

	// -- methods
	Double_t                DetermineNextStepTime(TUCNParticle* particle, const Double_t runTime);
	Double_t 					GetStepTime() const {return fStepTime;}
	void							SetStepTime(Double_t stepTime); 
		
	Bool_t						PropagateTrack(TVirtualGeoTrack* track, const Double_t runTime, const Double_t maxStepTime);
	Bool_t						PropagateTrack(TVirtualGeoTrack* track, const Int_t steps, const Double_t maxStepTime);
	
	Bool_t 						Bounce(TVirtualGeoTrack* track);
	Bool_t 						SpecularBounce(Double_t* dir, const Double_t* norm);
	Bool_t 						DiffuseBounce(Double_t* dir, const Double_t* norm);
	void							UpdateTrack(TVirtualGeoTrack* track, Double_t timeInterval=0.);

   ClassDef(TUCNRunManager, 1)      
};

#endif
