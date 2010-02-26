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
   virtual Double_t     TimeFromInsideAlongParabola(const Double_t* point, const Double_t* velocity, const Double_t* field,
                               const Double_t stepmax=TGeoShape::Big()) const;
   static  Double_t     TimeFromInsideAlongParabolaS(const Double_t* point, const Double_t* velocity, const Double_t* field,
                               const Double_t dx, const Double_t dy, const Double_t dz, const Double_t *origin, const Double_t stepmax=TGeoShape::Big());
   virtual Double_t     TimeFromOutsideAlongParabola(const Double_t* point, const Double_t* velocity, const Double_t* field,
                               const Double_t stepmax=TGeoShape::Big()) const;
   static  Double_t     TimeFromOutsideAlongParabolaS(const Double_t* point, const Double_t* velocity, const Double_t* field,
                               const Double_t dx, const Double_t dy, const Double_t dz, const Double_t *origin, const Double_t stepmax=TGeoShape::Big());
   static  Bool_t       IsNextPointOnBox(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t* boundary, const Double_t t);
	
	ClassDef(TUCNGeoBBox, 1) // UCNGeoBBox
};

#endif
