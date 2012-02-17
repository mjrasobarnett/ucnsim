// Element
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <cassert>

#include "Element.hpp"

#include "Units.hpp"
#include "Constants.hpp"

using namespace std;

ClassImp(Element)

//_____________________________________________________________________________
Element::Element()
        :TNamed()
{
// Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Element", "Default Constructor");
   #endif
   fZ = 0;
   fA = 0.;
   fScatLength = 0.; // Bound Coherent Scattering Length
   fCohCrossSec = 0.; // Coherent Scattering Cross-sec
   fIncohCrossSec = 0.; // Incoherent Scattering Cross-sec
   fAbsCrossSec = 0.; // Absorption Cross-sec at R-T
   fLossCrossSec = 0.; // Total loss cross-sec
}

//_____________________________________________________________________________
Element::Element(const char *name, Int_t z, Double_t a, Double_t scatLength, Double_t cohCrossSec, Double_t incoCrossSec, Double_t absCrossSec)
        :TNamed(name,name)
{
// constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Element", "Constructor");
   #endif
   fZ = z;
   fA = a;
   fScatLength = scatLength; // Bound Coherent Scattering Length
   fCohCrossSec = cohCrossSec;
   fIncohCrossSec = incoCrossSec; // Incoherent Scattering Cross-sec
   fAbsCrossSec = absCrossSec; // Absorption Cross-sec at R-T
   fLossCrossSec = incoCrossSec + absCrossSec; // Total loss cross-sec
}

//_____________________________________________________________________________
Element::Element(const Element& m)
            :TNamed(m),
             fZ(m.fZ),
             fA(m.fA),
             fScatLength(m.fScatLength),
             fCohCrossSec(m.fCohCrossSec),
             fIncohCrossSec(m.fIncohCrossSec),
             fAbsCrossSec(m.fAbsCrossSec),
             fLossCrossSec(m.fLossCrossSec)
{
   //copy constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Element", "Copy Constructor");  
   #endif
}

//_____________________________________________________________________________
Element::~Element()
{
// Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Element", "Destructor");
   #endif
}
