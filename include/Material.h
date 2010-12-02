// Material
// Author: Matthew Raso-Barnett  09/06/2009

#ifndef ROOT_Material
#define ROOT_Material

#ifndef  ROOT_TGeoMaterial
   #include "TGeoMaterial.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    Material                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Particle;
class Element;
class TGeoNavigator;

class Material : public TGeoMaterial
{
protected:
   TObjArray*  fElements;
   Double_t    fFermiPotential;
   Double_t    fWPotential;
   Double_t    fLossFactor;
   
public:
   
   // -- constructors
   Material();
   Material(const char *name, Element* elem, Double_t density);
   Material(const Material&);
   Material& operator=(const Material&);
   
   // -- destructor
   virtual ~Material();
   
   // -- methods
   Bool_t       AddElement(Element* element, Double_t density);
   TGeoElement* GetElement(Int_t i=0) const;
   Double_t     FermiPotential() const {return fFermiPotential;}
   void         FermiPotential(Double_t fermiPotential) {fFermiPotential = fermiPotential;}
   Double_t     WPotential() const {return fWPotential;}
   void         WPotential(Double_t wPotential) {fWPotential = wPotential;}
   Double_t     LossFactor() const {return fLossFactor;}
   void         LossFactor(Double_t lossFactor) {fLossFactor = lossFactor;}
      
   ClassDef(Material, 1)
};

#endif /*MATERIAL_H*/