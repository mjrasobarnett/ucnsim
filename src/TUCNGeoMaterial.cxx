// TUCNGeoMaterial
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <cassert>

#include "TGeoMaterial.h"

#include "TUCNGeoMaterial.h"
#include "Units.h"

using namespace std;

ClassImp(TUCNGeoMaterial)

//_____________________________________________________________________________
TUCNGeoMaterial::TUCNGeoMaterial()
             	 :TGeoMaterial()
{
// Default constructor
	Info("TUCNGeoMaterial", "Dummy Constructor");
	fFermiPotential = 0.;
	fWPotential = 0.;
	fRoughnessCoeff = 0.;
	this->IsBlackHole(kFALSE);
	this->IsTrackingMaterial(kFALSE);
	this->IsDetectorMaterial(kFALSE);
	this->DetectionEfficiency(1.);
}

//_____________________________________________________________________________
TUCNGeoMaterial::TUCNGeoMaterial(const char *name, Double_t fermiPotential, Double_t wPotential)
             	 :TGeoMaterial(name)         
{
// constructor
	Info("TUCNGeoMaterial", "Constructor");
	fFermiPotential = fermiPotential;
	fWPotential = wPotential;
	fRoughnessCoeff = 0.;
	this->IsBlackHole(kFALSE);
	this->IsTrackingMaterial(kFALSE);
	this->IsDetectorMaterial(kFALSE);
	this->DetectionEfficiency(1.);	
}

//_____________________________________________________________________________
TUCNGeoMaterial::TUCNGeoMaterial(const TUCNGeoMaterial& gm)
             	 :TGeoMaterial(gm),
					  fFermiPotential(gm.fFermiPotential),
					  fWPotential(gm.fWPotential),
					  fRoughnessCoeff(gm.fRoughnessCoeff),
					  fIsBlackHole(gm.fIsBlackHole),
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
		fFermiPotential = gm.fFermiPotential;
		fWPotential = gm.fWPotential;
		fRoughnessCoeff = gm.fRoughnessCoeff;
		fIsBlackHole = gm.fIsBlackHole;
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

//_____________________________________________________________________________
Double_t TUCNGeoMaterial::Eta() const
{
	if (fFermiPotential == 0.0) {
		cerr << this->GetName() << "\t" << "FermiPotential, V(neV): " << fFermiPotential/Units::neV << "\t" << "LossPotential, W(neV): " << fWPotential/Units::neV << endl;
		assert(fFermiPotential != 0.0);
	}
	return (fWPotential/fFermiPotential);
}

