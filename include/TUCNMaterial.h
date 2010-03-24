// TUCNMaterial
// Author: Matthew Raso-Barnett  09/06/2009

#ifndef ROOT_TUCNMaterial
#define ROOT_TUCNMaterial

#ifndef  ROOT_TGeoMaterial
   #include "TGeoMaterial.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNMaterial                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNParticle;
class TUCNElement;
class TGeoNavigator;

class TUCNMaterial : public TGeoMaterial
{
protected:
   TObjArray*  fElements;
   Double_t    fFermiPotential;
   Double_t    fWPotential;
   Double_t    fLossFactor;
   
public:
   
   // -- constructors
   TUCNMaterial();
   TUCNMaterial(const char *name, TUCNElement* elem, Double_t density);
   TUCNMaterial(const TUCNMaterial&);
   TUCNMaterial& operator=(const TUCNMaterial&);
   
   // -- destructor
   virtual ~TUCNMaterial();
   
   // -- methods
   Bool_t       AddElement(TUCNElement* element, Double_t density);
   TGeoElement* GetElement(Int_t i=0) const;
   Double_t     FermiPotential() const {return fFermiPotential;}
   void         FermiPotential(Double_t fermiPotential) {fFermiPotential = fermiPotential;}
   Double_t     WPotential() const {return fWPotential;}
   void         WPotential(Double_t wPotential) {fWPotential = wPotential;}
   Double_t     LossFactor() const {return fLossFactor;}
   void         LossFactor(Double_t lossFactor) {fLossFactor = lossFactor;}
      
   ClassDef(TUCNMaterial, 1)
};

#endif /*TUCNMATERIAL_H*/