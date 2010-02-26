// @(#)root/geom:$Id: TUCNGeoCompositeShape.h 24879 2008-07-18 08:04:40Z brun $
// Author: Andrei Gheata   31/01/02

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TUCNGeoCompositeShape
#define ROOT_TUCNGeoCompositeShape

#ifndef ROOT_TUCNGeoBBox
#include "TUCNGeoBBox.h"
#endif
    
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// TUCNGeoCompositeShape - composite shape class. A composite shape contains //
//   a list of primitive shapes, the list of coresponding transformations    //
//   and a boolean finder handling boolean operations among components.      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

class TUCNGeoBoolNode;

class TUCNGeoCompositeShape : public TUCNGeoBBox
{
private :
// data members
   TUCNGeoBoolNode         *fNode;             // top boolean node

protected:
   TUCNGeoCompositeShape(const TUCNGeoCompositeShape& gcs)
     : TUCNGeoBBox(gcs), fNode(gcs.fNode) { }
   TUCNGeoCompositeShape& operator=(const TUCNGeoCompositeShape& gcs)
     {if(this!=&gcs) {TUCNGeoBBox::operator=(gcs); fNode=gcs.fNode;} return *this;}

public:
   // constructors
   TUCNGeoCompositeShape();
   TUCNGeoCompositeShape(const char *name, const char *expression);
   TUCNGeoCompositeShape(const char *expression);
   TUCNGeoCompositeShape(const char *name, TUCNGeoBoolNode *node);
   // destructor
   virtual ~TUCNGeoCompositeShape();
   // methods
   virtual Double_t      Capacity() const;
   virtual void          ComputeBBox();
   virtual void          ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm);
   virtual Bool_t        Contains(Double_t *point) const;
   virtual Double_t      DistFromInside(Double_t *point, Double_t *dir, Int_t iact=1, 
                                   Double_t step=TGeoShape::Big(), Double_t *safe=0) const;
   virtual Double_t      DistFromOutside(Double_t *point, Double_t *dir, Int_t iact=1, 
                                   Double_t step=TGeoShape::Big(), Double_t *safe=0) const;
   virtual TGeoVolume   *Divide(TGeoVolume *voldiv, const char *divname, Int_t iaxis, Int_t ndiv, 
                                Double_t start, Double_t step);
   TUCNGeoBoolNode      *GetBoolNode() const {return fNode;}
   virtual void          GetBoundingCylinder(Double_t * /*param*/) const {;}
   virtual TGeoShape    *GetMakeRuntimeShape(TGeoShape * /*mother*/, TGeoMatrix * /*mat*/) const {return 0;}
   virtual void          GetMeshNumbers(Int_t &nvert, Int_t &nsegs, Int_t &npols) const;
   virtual Int_t         GetNmeshVertices() const;
   virtual Bool_t        GetPointsOnSegments(Int_t /*npoints*/, Double_t * /*array*/) const {return kFALSE;}
   virtual void          InspectShape() const;
   virtual Bool_t        IsComposite() const {return kTRUE;}
   virtual Bool_t        IsCylType() const {return kFALSE;}
   void                  MakeNode(const char *expression);
   virtual Bool_t        PaintComposite(Option_t *option = "") const;
   void                  RegisterYourself();
   virtual Double_t      Safety(Double_t *point, Bool_t in=kTRUE) const;
   virtual void          SavePrimitive(ostream &out, Option_t *option = "");
   virtual void          SetDimensions(Double_t * /*param*/) {;}
   virtual void          SetPoints(Double_t *points) const;
   virtual void          SetPoints(Float_t *points) const;
   virtual void          Sizeof3D() const;

	// methods
//	virtual Double_t		TimeFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, 
//																			Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;
//	virtual Double_t		TimeFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, 
//																			Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;
		
   ClassDef(TUCNGeoCompositeShape, 1)         // boolean composite shape
};



#endif

