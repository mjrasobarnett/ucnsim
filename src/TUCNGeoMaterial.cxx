// TUCNGeoMaterial
// Author: Matthew Raso-Barnett  22/05/2009

#include "TGeoMaterial.h"

#include "TUCNGeoMaterial.h"


ClassImp(TUCNGeoMaterial)

//_____________________________________________________________________________
TUCNGeoMaterial::TUCNGeoMaterial()
             	 :TGeoMaterial()
{
// Default constructor
	Info("TUCNGeoMaterial", "Default Constructor");
	this->ScatteringLength(0.);
	this->TotalLossCrossSection(0.);
	this->IsBlackhole(kFALSE);
	this->IsTrackingMaterial(kFALSE);
	this->IsDetectorMaterial(kFALSE);
	this->DetectionEfficiency(1.);
}

//_____________________________________________________________________________
TUCNGeoMaterial::TUCNGeoMaterial(const char *name)
             	 :TGeoMaterial(name)         
{
// constructor
	Info("TUCNGeoMaterial", "Constructor");
	this->ScatteringLength(0.);
	this->TotalLossCrossSection(0.);
	this->IsBlackhole(kFALSE);
	this->IsTrackingMaterial(kFALSE);
	this->IsDetectorMaterial(kFALSE);
	this->DetectionEfficiency(1.);	
}

//_____________________________________________________________________________
TUCNGeoMaterial::TUCNGeoMaterial(const char *name, Double_t a, Double_t z, Double_t rho, Double_t radlen, Double_t intlen)
             	 :TGeoMaterial(name, a, z, rho, radlen, intlen)
{
// constructor
 	Info("TUCNGeoMaterial", "Constructor");
	this->ScatteringLength(0.);
	this->TotalLossCrossSection(0.);
	this->IsBlackhole(kFALSE);
	this->IsTrackingMaterial(kFALSE);
	this->IsDetectorMaterial(kFALSE);
	this->DetectionEfficiency(1.);  
}

//_____________________________________________________________________________
TUCNGeoMaterial::TUCNGeoMaterial(const char *name, TGeoElement *elem, Double_t rho)
             	 :TGeoMaterial(name, elem, rho)
{
// constructor
 	Info("TUCNGeoMaterial", "Constructor");
	this->ScatteringLength(0.);
	this->TotalLossCrossSection(0.);
	this->IsBlackhole(kFALSE);
	this->IsTrackingMaterial(kFALSE);
	this->IsDetectorMaterial(kFALSE); 
	this->DetectionEfficiency(1.); 
}

//_____________________________________________________________________________
TUCNGeoMaterial::TUCNGeoMaterial(const TUCNGeoMaterial& gm)
             	 :TGeoMaterial(gm),
					  fScatteringLength(gm.fScatteringLength),
					  fTotalLossCrossSection(gm.fTotalLossCrossSection),
					  fIsBlackhole(gm.fIsBlackhole),
					  fIsTrackingMaterial(gm.fIsTrackingMaterial),
					  fIsDetectorMaterial(gm.fIsDetectorMaterial),
					  fDetectionEfficiency(gm.fDetectionEfficiency)
{ 
   //copy constructor
 	Info("TUCNGeoMaterial", "Copy Constructor");  
}

//_____________________________________________________________________________
TUCNGeoMaterial& TUCNGeoMaterial::operator=(const TUCNGeoMaterial& gm) 
{
   //assignment operator
 	Info("TUCNGeoMaterial", "Assignment");  

   if(this!=&gm) {
      TGeoMaterial::operator=(gm);
		fScatteringLength = gm.fScatteringLength;
		fTotalLossCrossSection = gm.fTotalLossCrossSection;
		fIsBlackhole = gm.fIsBlackhole;
		fIsTrackingMaterial = gm.fIsTrackingMaterial;
		fIsDetectorMaterial = gm.fIsDetectorMaterial;
		fDetectionEfficiency = gm.fDetectionEfficiency;
   } 
   return *this;
}

//_____________________________________________________________________________
TUCNGeoMaterial::~TUCNGeoMaterial()
{
// Destructor
 	Info("TUCNGeoMaterial", "Destructor");  
}
