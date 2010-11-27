// TUCNElement
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <cassert>

#include "TUCNElement.h"

#include "TGeoElement.h"

#include "Units.h"
#include "Constants.h"

using namespace std;

ClassImp(TUCNElement)

//_____________________________________________________________________________
TUCNElement::TUCNElement()
            :TGeoElement()
{
// Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNElement", "Default Constructor");
   #endif
   fScatLength = 0.; // Bound Coherent Scattering Length
   fCohCrossSec = 0.; // Coherent Scattering Cross-sec
   fIncohCrossSec = 0.; // Incoherent Scattering Cross-sec
   fAbsCrossSec = 0.; // Absorption Cross-sec at R-T
   fLossCrossSec = 0.; // Total loss cross-sec
}

//_____________________________________________________________________________
TUCNElement::TUCNElement(const char *name, const char *title, Int_t z, Double_t a, Double_t scatLength, Double_t cohCrossSec, Double_t incoCrossSec, Double_t absCrossSec)
            :TGeoElement(name,title,z,a)
{
// constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNElement", "Constructor");
   #endif
   fScatLength = scatLength; // Bound Coherent Scattering Length
   fCohCrossSec = cohCrossSec;
   fIncohCrossSec = incoCrossSec; // Incoherent Scattering Cross-sec
   fAbsCrossSec = absCrossSec; // Absorption Cross-sec at R-T
   fLossCrossSec = incoCrossSec + absCrossSec; // Total loss cross-sec
}

//_____________________________________________________________________________
TUCNElement::TUCNElement(const TUCNElement& m)
            :TGeoElement(m),
             fScatLength(m.fScatLength),
             fCohCrossSec(m.fCohCrossSec),
             fIncohCrossSec(m.fIncohCrossSec),
             fAbsCrossSec(m.fAbsCrossSec),
             fLossCrossSec(m.fLossCrossSec)
{
   //copy constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNElement", "Copy Constructor");  
   #endif
}

//_____________________________________________________________________________
TUCNElement& TUCNElement::operator=(const TUCNElement& m) 
{
   //assignment operator
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNElement", "Assignment");
   #endif
   if(this!=&m) {
      TGeoElement::operator=(m);
      fScatLength = m.fScatLength;
      fCohCrossSec = m.fCohCrossSec;
      fIncohCrossSec = m.fIncohCrossSec;
      fAbsCrossSec = m.fAbsCrossSec;
      fLossCrossSec = m.fLossCrossSec;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNElement::~TUCNElement()
{
// Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNElement", "Destructor");
   #endif
}
