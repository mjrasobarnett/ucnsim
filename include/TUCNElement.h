// TUCNElement
// Author: Matthew Raso-Barnett  19/03/2010

#ifndef ROOT_TUCNElement
#define ROOT_TUCNElement

#ifndef  ROOT_TGeoElement
   #include "TGeoElement.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNElement                                                         //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNElement : public TGeoElement
{
protected:
   Double_t    fScatLength;
   Double_t    fCohCrossSec; // Coherent Scattering Cross-sec
   Double_t    fIncohCrossSec; // Incoherent Scattering Cross-sec
   Double_t    fAbsCrossSec; // Absorption Cross-sec at R-T
   Double_t    fLossCrossSec; // Total loss cross-section
   
public:
   
   // -- constructors
   TUCNElement();
   TUCNElement(const char *name, const char *title, Int_t z, Double_t a, Double_t scatLength, Double_t cohCrossSec, Double_t incoCrossSec, Double_t absCrossSec);
   TUCNElement(const TUCNElement&);
   TUCNElement& operator=(const TUCNElement&);
   
   // -- destructor
   virtual ~TUCNElement();
   
   // -- methods
   Double_t    ScatLength() const {return fScatLength;}
   Double_t    CoherentCrossSec() const {return fCohCrossSec;}
   Double_t    InCoherentCrossSec() const {return fIncohCrossSec;}
   Double_t    AbsorbtionCrossSec() const {return fAbsCrossSec;}
   Double_t    LossCrossSec() const {return fLossCrossSec;}
   
   ClassDef(TUCNElement, 1)
};

#endif /*TUCNElement*/