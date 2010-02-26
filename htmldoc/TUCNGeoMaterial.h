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
	Double_t 	fScatteringLength;
	Double_t 	fTotalLossCrossSection;
	
	Bool_t 		fIsBlackhole;
	Bool_t 		fIsTrackingMaterial;
	Bool_t   	fIsDetectorMaterial;
	Double_t 	fDetectionEfficiency;
	
	TUCNGeoMaterial(const TUCNGeoMaterial&);
   TUCNGeoMaterial& operator=(const TUCNGeoMaterial&);
		
public:
	
	// -- constructors
   TUCNGeoMaterial();
   TUCNGeoMaterial(const char *name);
   TUCNGeoMaterial(const char *name, Double_t a, Double_t z, 
                Double_t rho, Double_t radlen=0, Double_t intlen=0);
   TUCNGeoMaterial(const char *name, TGeoElement *elem, Double_t rho);
   
	// -- destructor
   virtual ~TUCNGeoMaterial();
	
	// -- methods
	void					ScatteringLength(Double_t scatteringLength) 	{fScatteringLength = scatteringLength;}
	Double_t 			ScatteringLength() const 							{return fScatteringLength;}
	void					TotalLossCrossSection(Double_t crossSection) {fTotalLossCrossSection = crossSection;}
	Double_t 			TotalLossCrossSection() const 					{return fTotalLossCrossSection;}
	
	void 					IsBlackhole(Bool_t isblackhole) 					{fIsBlackhole = isblackhole;}
	Bool_t 				IsBlackhole() const 									{return fIsBlackhole;}
	void 					IsTrackingMaterial(Bool_t istracking) 			{fIsTrackingMaterial = istracking;}
	Bool_t 				IsTrackingMaterial() const 						{return fIsTrackingMaterial;}
	void 					IsDetectorMaterial(Bool_t isdetecting) 		{fIsDetectorMaterial = isdetecting;}
	Bool_t 				IsDetectorMaterial() const 						{return fIsDetectorMaterial;}
	
	void					DetectionEfficiency(Double_t efficiency)		{fDetectionEfficiency = efficiency;}
	Double_t 			DetectionEfficiency() const 						{return fDetectionEfficiency;}
	
   ClassDef(TUCNGeoMaterial, 1)          // 
};

#endif