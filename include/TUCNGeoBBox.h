// TUCNGeoBBox
// Author: Matthew Raso-Barnett  22/05/2009

#ifndef ROOT_TUCNGeoBBox
#define ROOT_TUCNGeoBBox

#include "TGeoBBox.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNGeoBBox														  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNGeoBBox : public TGeoBBox 
{
public:
   // constructors
   TUCNGeoBBox();
   TUCNGeoBBox(Double_t dx, Double_t dy, Double_t dz, Double_t *origin=0);
   TUCNGeoBBox(const char *name, Double_t dx, Double_t dy, Double_t dz, Double_t *origin=0);
   TUCNGeoBBox(Double_t *param);
   // destructor
   virtual ~TUCNGeoBBox();
   
   // methods
   virtual Double_t     TimeFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, 
                               Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;
   virtual Double_t     TimeFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, 
                               Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;
   static  Double_t     TimeFromInsideAlongParabolaS(Double_t* point, Double_t* velocity, Double_t* field, 
                               Double_t dx, Double_t dy, Double_t dz, const Double_t *origin, Double_t stepmax=TGeoShape::Big());
   static  Double_t     TimeFromOutsideAlongParabolaS(Double_t* point, Double_t* velocity, Double_t* field, 
                               Double_t dx, Double_t dy, Double_t dz, const Double_t *origin, Double_t stepmax=TGeoShape::Big());
   static  Bool_t       IsNextPointOnBox(Double_t* point, Double_t* velocity, Double_t* field, Double_t* boundary, Double_t t);
	
	ClassDef(TUCNGeoBBox, 1) // UCNGeoBBox
};

#endif
