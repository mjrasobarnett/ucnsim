// Element
// Author: Matthew Raso-Barnett  19/03/2010

#ifndef ROOT_Element
#define ROOT_Element

#ifndef  ROOT_TGeoElement
   #include "TGeoElement.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    Element                                                         //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class Element : public TGeoElement
{
protected:
   Double_t    fScatLength;
   Double_t    fCohCrossSec; // Coherent Scattering Cross-sec
   Double_t    fIncohCrossSec; // Incoherent Scattering Cross-sec
   Double_t    fAbsCrossSec; // Absorption Cross-sec at R-T
   Double_t    fLossCrossSec; // Total loss cross-section
   
public:
   
   // -- constructors
   Element();
   Element(const char *name, const char *title, Int_t z, Double_t a, Double_t scatLength, Double_t cohCrossSec, Double_t incoCrossSec, Double_t absCrossSec);
   Element(const Element&);
   Element& operator=(const Element&);
   
   // -- destructor
   virtual ~Element();
   
   // -- methods
   Double_t    ScatLength() const {return fScatLength;}
   Double_t    CoherentCrossSec() const {return fCohCrossSec;}
   Double_t    InCoherentCrossSec() const {return fIncohCrossSec;}
   Double_t    AbsorbtionCrossSec() const {return fAbsCrossSec;}
   Double_t    LossCrossSec() const {return fLossCrossSec;}
   
   ClassDef(Element, 1)
};

#endif /*Element*/