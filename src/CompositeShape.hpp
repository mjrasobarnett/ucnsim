// @(#)root/geom:$Id: CompositeShape.h 24879 2008-07-18 08:04:40Z brun $
// Author: Andrei Gheata   31/01/02

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_CompositeShape
#define ROOT_CompositeShape

#ifndef ROOT_Box
#include "Box.hpp"
#endif
    
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// CompositeShape - composite shape class. A composite shape contains //
//   a list of primitive shapes, the list of coresponding transformations    //
//   and a boolean finder handling boolean operations among components.      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

class BoolNode;

class CompositeShape : public Box
{
private :
// data members
   BoolNode         *fNode;             // top boolean node

protected:
   CompositeShape(const CompositeShape& gcs);
   CompositeShape& operator=(const CompositeShape& gcs);

public:
   // constructors
   CompositeShape();
   CompositeShape(const char *name, const char *expression);
   CompositeShape(const char *expression);

   // destructor
   virtual ~CompositeShape();

   // methods
   virtual Double_t TimeFromInside(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t stepmax, const Bool_t onBoundary) const;
   virtual Double_t TimeFromOutside(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t stepmax, const Bool_t onBoundary) const;

   // -- The TGeoCompositeShape class interface is reproduced in full here
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
   BoolNode             *GetBoolNode() const {return fNode;}
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
   
   ClassDef(CompositeShape, 1)         // boolean composite shape
};

#endif

