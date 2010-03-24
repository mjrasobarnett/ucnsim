// TUCNMaterial
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <cassert>

#include "TUCNMaterial.h"

#include "TGeoMaterial.h"
#include "TMath.h"

#include "TUCNElement.h"

#include "Units.h"
#include "Constants.h"
#include "Materials.h"

using namespace std;

ClassImp(TUCNMaterial)

//_____________________________________________________________________________
TUCNMaterial::TUCNMaterial()
             :TGeoMaterial()
{
// Default constructor
   Info("TUCNMaterial", "Default Constructor");
   fElements = 0; // List of Elements
   fFermiPotential = 0.; // Units of eV
   fWPotential = 0.; // Units of eV
}

//_____________________________________________________________________________
TUCNMaterial::TUCNMaterial(const char *name, TUCNElement* elem, Double_t density)
             :TGeoMaterial(name)
{
// constructor
   Info("TUCNMaterial", "Constructor");
   // Create a new TObjArray to store Elements
   fElements = new TObjArray(1);
   // Tell ObjArray to 'own' the elements. Ensures that they are destructed by array
   fElements->SetOwner(kTRUE);
   // Add first element to array 
   fElements->Add(elem);
   // Calculate the element's number density
   Double_t numberDensity = 0;
   if (elem->A() > 0.0) numberDensity = density*Constants::avagadro_number/elem->A();
   // Calculate real part of the Fermi potential
   fFermiPotential = ((2.*TMath::Pi()*TMath::Power(Constants::hbar,2.)) /Constants::neutron_mass_kg)
                     * numberDensity * elem->ScatLength() / Units::e_SI; // Units of eV
   
   // Calculate imaginary part of Fermi Potential
   fWPotential = 0.5 * Constants::hbar * numberDensity * elem->LossCrossSec()
                  * Elements::reference_velocity / Units::e_SI; // Units of eV
   
   // Calculate loss factor
   if (fFermiPotential == 0.0) { fLossFactor = 0.0; }
   else { fLossFactor = fWPotential/fFermiPotential; }
}

//_____________________________________________________________________________
TUCNMaterial::TUCNMaterial(const TUCNMaterial& m)
             :TGeoMaterial(m),
              fElements(m.fElements),
              fFermiPotential(m.fFermiPotential),
              fWPotential(m.fWPotential)
{
   //copy constructor
   Info("TUCNMaterial", "Copy Constructor");  
}

//_____________________________________________________________________________
TUCNMaterial& TUCNMaterial::operator=(const TUCNMaterial& m) 
{
   //assignment operator
   Info("TUCNMaterial", "Assignment");
   if(this!=&m) {
      TGeoMaterial::operator=(m);
      fElements = m.fElements;
      fFermiPotential = m.fFermiPotential;
      fWPotential = m.fWPotential;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNMaterial::~TUCNMaterial()
{
// Destructor
   Info("TUCNMaterial", "Destructor");
   if (fElements) delete fElements;
}

//_____________________________________________________________________________
Bool_t TUCNMaterial::AddElement(TUCNElement* elem, Double_t density)
{
// Add Element to list and recalculate the fermi/W-potential
// ****
// This is probably implemented incorrectly, but should serve as an interface for
// handling interstitial gases / material mixtures in future
// ****
   if (!fElements) {return kFALSE;}
   fElements->AddLast(elem);
   // Calculate the element's number density
   Double_t numberDensity = density*Constants::avagadro_number/elem->A();
   // Calculate real part of the Fermi potential and add to what is already there
   fFermiPotential = fFermiPotential + (((2.*TMath::Pi()*TMath::Power(Constants::hbar,2.))/Constants::neutron_mass_kg) * numberDensity * elem->ScatLength() / Units::e_SI); // Units of eV
   // Calculate imaginary part of Fermi Potential
   fWPotential = fWPotential + (0.5*Constants::hbar * numberDensity * elem->LossCrossSec()
                  * Materials::reference_velocity / Units::e_SI); // Units of eV
   return kTRUE;
}

//_____________________________________________________________________________
TGeoElement *TUCNMaterial::GetElement(Int_t i) const
{
// Retreive the pointer to the element corresponding to component I.
   if (!fElements) {return 0;}
   if (i < 0 || i >= fElements->GetEntriesFast()) {
      Error("GetElement", "Mixture %s has only %d elements", GetName(), fElements->GetEntriesFast());
      return 0;
   }   
   return (TGeoElement*)fElements->At(i);
}

