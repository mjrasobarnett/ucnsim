// Material
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <cassert>

#include "Material.hpp"

#include "TGeoMaterial.h"
#include "TMath.h"

#include "Element.hpp"

#include "Units.hpp"
#include "Constants.hpp"
#include "Materials.hpp"

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
   Double_t wPotential = 0.5*Constants::hbar*numberDensity*elem->LossCrossSec()*Elements::reference_velocity/Units::e_SI; // Units of eV
   
   // Calculate loss factor
   if (fFermiPotential == 0.0) { fLossFactor = 0.0; }
   else { fLossFactor = wPotential/fFermiPotential; }
}

//_____________________________________________________________________________
Material::Material(const Material& m)
             :TGeoMaterial(m),
              fElements(m.fElements),
              fFermiPotential(m.fFermiPotential),
              fLossFactor(m.fLossFactor)
              
{
   //copy constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Material", "Copy Constructor");  
   #endif
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
