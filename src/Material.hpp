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
   Double_t    fLossFactor;
   
public:
   
   // -- constructors
   Material();
   Material(const char *name, Element* elem, Double_t density);
   Material(const Material&);
   
   // -- destructor
   virtual ~Material();
   
   // -- methods
   Double_t     FermiPotential() const {return fFermiPotential;}
   void         FermiPotential(Double_t fermiPotential) {fFermiPotential = fermiPotential;}
   Double_t     WPotential() const {return fLossFactor*fFermiPotential;}
   void         WPotential(Double_t wPotential) {fLossFactor = wPotential/fFermiPotential;}
   Double_t     LossFactor() const {return fLossFactor;}
   void         LossFactor(Double_t lossFactor) {fLossFactor = lossFactor;}
      
   ClassDef(Material, 1)
};

#endif /*MATERIAL_H*/
