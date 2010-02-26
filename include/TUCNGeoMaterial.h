// TUCNGeoMaterial
// Author: Matthew Raso-Barnett  09/06/2009

#ifndef ROOT_TUCNGeoMaterial
#define ROOT_TUCNGeoMaterial

#ifndef 	ROOT_TGeoMaterial
#include "TGeoMaterial.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNGeoMaterial												  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNGeoMaterial : public TGeoMaterial 
{
private:
	
protected:
	Double_t					fFermiPotential;
	Double_t					fWPotential;
	Double_t 				fScatteringLength;
	
	Double_t 				fTotalLossCrossSection;
	
	Bool_t					fIsBlackHole;
	Bool_t					fIsTrackingMaterial;
	Bool_t					fIsDetectorMaterial;
	Double_t 				fDetectionEfficiency;
	
	TUCNGeoMaterial(const TUCNGeoMaterial&);
   TUCNGeoMaterial& operator=(const TUCNGeoMaterial&);
		
public:
	
	// -- constructors
   TUCNGeoMaterial();
   TUCNGeoMaterial(const char *name, Double_t fermiPotential, Double_t wPotential);
   
	// -- destructor
   virtual ~TUCNGeoMaterial();
	
	// -- methods
	void						ScatteringLength(Double_t scatteringLength) 	{fScatteringLength = scatteringLength;}
	Double_t 				ScatteringLength() const 							{return fScatteringLength;}
	void						TotalLossCrossSection(Double_t crossSection) {fTotalLossCrossSection = crossSection;}
	Double_t 				TotalLossCrossSection() const 					{return fTotalLossCrossSection;}
	void						FermiPotential(Double_t fermiPotential)		{fFermiPotential = fermiPotential;}
	Double_t 				FermiPotential() const 								{return fFermiPotential;}
	
	Double_t 				Eta() const ;
	
	void						IsBlackHole(Bool_t isBlackHole) 					{fIsBlackHole = isBlackHole;}
	Bool_t					IsBlackHole() const 									{return fIsBlackHole;}
	void						IsTrackingMaterial(Bool_t isTracking) 			{fIsTrackingMaterial = isTracking;}
	Bool_t					IsTrackingMaterial() const 						{return fIsTrackingMaterial;}
	void						IsDetectorMaterial(Bool_t isDetecting) 		{fIsDetectorMaterial = isDetecting;}
	Bool_t					IsDetectorMaterial() const 						{return fIsDetectorMaterial;}
	
	void						DetectionEfficiency(Double_t efficiency)		{fDetectionEfficiency = efficiency;}
	Double_t					DetectionEfficiency() const 						{return fDetectionEfficiency;}
	
   ClassDef(TUCNGeoMaterial, 1)          // 
};

#endif
