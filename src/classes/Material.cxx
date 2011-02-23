// Material
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <cassert>

#include "Material.h"

#include "TGeoMaterial.h"
#include "TMath.h"

#include "Element.h"

#include "Units.h"
#include "Constants.h"
#include "Materials.h"

using namespace std;

ClassImp(Material)

//_____________________________________________________________________________
Material::Material()
             :TGeoMaterial()
{
// Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Material", "Default Constructor");
   #endif
   fElements = 0; // List of Elements
   fFermiPotential = 0.; // Units of eV
   fWPotential = 0.; // Units of eV
}

//_____________________________________________________________________________
Material::Material(const char *name, Element* elem, Double_t density)
             :TGeoMaterial(name)
{
// constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Material", "Constructor");
   #endif
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
   fFermiPotential = ((2.*TMath::Pi()*TMath::Power(Constants::hbar,2.)) /Neutron::mass_Kg)
                     * numberDensity * elem->ScatLength() / Units::e_SI; // Units of eV
   
   // Calculate imaginary part of Fermi Potential
   fWPotential = 0.5 * Constants::hbar * numberDensity * elem->LossCrossSec()
                  * Elements::reference_velocity / Units::e_SI; // Units of eV
   
   // Calculate loss factor
   if (fFermiPotential == 0.0) { fLossFactor = 0.0; }
   else { fLossFactor = fWPotential/fFermiPotential; }
}

//_____________________________________________________________________________
Material::Material(const Material& m)
             :TGeoMaterial(m),
              fElements(m.fElements),
              fFermiPotential(m.fFermiPotential),
              fWPotential(m.fWPotential)
{
   //copy constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Material", "Copy Constructor");  
   #endif
}

//_____________________________________________________________________________
Material& Material::operator=(const Material& m) 
{
   //assignment operator
   #ifdef PRINT_CONSTRUCTORS
      Info("Material", "Assignment");
   #endif
   if(this!=&m) {
      TGeoMaterial::operator=(m);
      fElements = m.fElements;
      fFermiPotential = m.fFermiPotential;
      fWPotential = m.fWPotential;
   }
   return *this;
}

//_____________________________________________________________________________
Material::~Material()
{
// Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Material", "Destructor");
   #endif
   if (fElements) delete fElements;
}

//_____________________________________________________________________________
Bool_t Material::AddElement(Element* elem, Double_t density)
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
   fFermiPotential = fFermiPotential + (((2.*TMath::Pi()*TMath::Power(Constants::hbar,2.))/Neutron::mass_Kg) * numberDensity * elem->ScatLength() / Units::e_SI); // Units of eV
   // Calculate imaginary part of Fermi Potential
   fWPotential = fWPotential + (0.5*Constants::hbar * numberDensity * elem->LossCrossSec()
                  * Materials::reference_velocity / Units::e_SI); // Units of eV
   return kTRUE;
}

//_____________________________________________________________________________
TGeoElement *Material::GetElement(Int_t i) const
{
// Retreive the pointer to the element corresponding to component I.
   if (!fElements) {return 0;}
   if (i < 0 || i >= fElements->GetEntriesFast()) {
      Error("GetElement", "Mixture %s has only %d elements", GetName(), fElements->GetEntriesFast());
      return 0;
   }   
   return (TGeoElement*)fElements->At(i);
}

