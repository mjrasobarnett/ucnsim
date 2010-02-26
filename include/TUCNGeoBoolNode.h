// @(#):$Id: TUCNGeoBoolNode.h 25878 2008-10-18 19:12:08Z rdm $
// Author: Andrei Gheata   30/05/02

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TUCNGeoBoolNode
#define ROOT_TUCNGeoBoolNode

#ifndef ROOT_TObject
#include "TObject.h"
#endif

#ifndef ROOT_TGeoMatrix
#include "TGeoMatrix.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// TUCNGeoBoolNode - Base class for boolean nodes. A boolean node has pointers //
//  to two shapes having two transformations with respect to the mother     //
//  composite shape they belong to. It represents the boolean operation     //
//  between the two component shapes.                                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// forward declarations
class TGeoShape;
class TGeoMatrix;
class TGeoHMatrix;

class TUCNGeoBoolNode : public TObject
{
public:
enum EGeoBoolType {
   kGeoUnion,
   kGeoIntersection,
   kGeoSubtraction
};
private:
   TUCNGeoBoolNode(const TUCNGeoBoolNode&); // Not implemented
   TUCNGeoBoolNode& operator=(const TUCNGeoBoolNode&); // Not implemented

protected:
   Int_t             fSelected;       // ! selected branch
   TGeoShape        *fLeft;           // shape on the left branch
   TGeoShape        *fRight;          // shape on the right branch
   TGeoMatrix       *fLeftMat;        // transformation that applies to the left branch
   TGeoMatrix       *fRightMat;       // transformation that applies to the right branch
// methods
   Bool_t            MakeBranch(const char *expr, Bool_t left);
public:
   // constructors
   TUCNGeoBoolNode();
   TUCNGeoBoolNode(const char *expr1, const char *expr2);
   TUCNGeoBoolNode(TGeoShape *left, TGeoShape *right, TGeoMatrix *lmat=0, TGeoMatrix *rmat=0);

   // destructor
   virtual ~TUCNGeoBoolNode();
   // methods
   virtual void      ComputeBBox(Double_t &dx, Double_t &dy, Double_t &dz, Double_t *origin) = 0;
   virtual void      ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm) = 0;
   virtual Bool_t    Contains(Double_t *point) const         = 0;
   virtual Int_t     DistanceToPrimitive(Int_t px, Int_t py) = 0;
   virtual Double_t  DistFromInside(Double_t *point, Double_t *dir, Int_t iact=1,
                               Double_t step=0, Double_t *safe=0) const = 0;
   virtual Double_t  DistFromOutside(Double_t *point, Double_t *dir, Int_t iact=1,
                               Double_t step=0, Double_t *safe=0) const = 0;
   virtual EGeoBoolType GetBooleanOperator() const = 0;
   virtual Int_t     GetNpoints() const = 0;
   TGeoMatrix       *GetLeftMatrix() const {return fLeftMat;}
   TGeoMatrix       *GetRightMatrix() const {return fRightMat;}
   TGeoShape        *GetLeftShape() const {return fLeft;}
   TGeoShape        *GetRightShape() const {return fRight;}
   virtual void      Paint(Option_t *option);
   void              RegisterMatrices();
   virtual Double_t  Safety(Double_t *point, Bool_t in=kTRUE) const = 0;
   virtual void      SavePrimitive(ostream &out, Option_t *option = "");
   virtual void      SetPoints(Double_t *points) const = 0;
   virtual void      SetPoints(Float_t *points)  const = 0;
   void              SetSelected(Int_t sel) {fSelected = sel;}
   virtual void      Sizeof3D() const;
   
   // new methods
   virtual Double_t     TimeFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field,
                                    Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const = 0;
   virtual Double_t     TimeFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field,
                                    Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const = 0;
	
   ClassDef(TUCNGeoBoolNode, 1)              // a boolean node
};

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// TUCNGeoUnion - Boolean node representing a union between two components. //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

class TUCNGeoUnion : public TUCNGeoBoolNode
{
public:
   // constructors
   TUCNGeoUnion();
   TUCNGeoUnion(const char *expr1, const char *expr2);
   TUCNGeoUnion(TGeoShape *left, TGeoShape *right, TGeoMatrix *lmat=0, TGeoMatrix *rmat=0);

   // destructor
   virtual ~TUCNGeoUnion();
   // methods
   virtual void      ComputeBBox(Double_t &dx, Double_t &dy, Double_t &dz, Double_t *origin);
   virtual void      ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm);
   virtual Bool_t    Contains(Double_t *point) const;
   virtual Int_t     DistanceToPrimitive(Int_t px, Int_t py);
   virtual Double_t  DistFromInside(Double_t *point, Double_t *dir, Int_t iact=1,
                               Double_t step=0, Double_t *safe=0) const;
   virtual Double_t  DistFromOutside(Double_t *point, Double_t *dir, Int_t iact=1,
                               Double_t step=0, Double_t *safe=0) const;
   virtual EGeoBoolType GetBooleanOperator() const {return kGeoUnion;}
   virtual Int_t     GetNpoints() const;
   virtual Double_t  Safety(Double_t *point, Bool_t in=kTRUE) const;
   virtual void      SavePrimitive(ostream &out, Option_t *option = "");
   virtual void      SetPoints(Double_t *points) const;
   virtual void      SetPoints(Float_t *points)  const;
   virtual void      Sizeof3D() const;

   //CS specific
   virtual void      Paint(Option_t *option);

   // new methods
   virtual Double_t     TimeFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field,
                                    Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;
   virtual Double_t     TimeFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field,
                                    Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;

   ClassDef(TUCNGeoUnion, 1)              // union node
};

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// TUCNGeoIntersection - Boolean node representing an intersection between two //
// components.                                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

class TUCNGeoIntersection : public TUCNGeoBoolNode
{
public:
   // constructors
   TUCNGeoIntersection();
   TUCNGeoIntersection(const char *expr1, const char *expr2);
   TUCNGeoIntersection(TGeoShape *left, TGeoShape *right, TGeoMatrix *lmat=0, TGeoMatrix *rmat=0);

   // destructor
   virtual ~TUCNGeoIntersection();
   // methods
   virtual void      ComputeBBox(Double_t &dx, Double_t &dy, Double_t &dz, Double_t *origin);
   virtual void      ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm);
   virtual Bool_t    Contains(Double_t *point) const;
   virtual Int_t     DistanceToPrimitive(Int_t px, Int_t py);
   virtual Double_t  DistFromInside(Double_t *point, Double_t *dir, Int_t iact=1,
                               Double_t step=0, Double_t *safe=0) const;
   virtual Double_t  DistFromOutside(Double_t *point, Double_t *dir, Int_t iact=1,
                               Double_t step=0, Double_t *safe=0) const;
   virtual EGeoBoolType GetBooleanOperator() const {return kGeoIntersection;}
   virtual Int_t     GetNpoints() const;
   virtual Double_t  Safety(Double_t *point, Bool_t in=kTRUE) const;
   virtual void      SavePrimitive(ostream &out, Option_t *option = "");
   virtual void      SetPoints(Double_t *points) const;
   virtual void      SetPoints(Float_t *points)  const;
   virtual void      Sizeof3D() const;

   //CS specific
   virtual void      Paint(Option_t *option);

   // new methods
   virtual Double_t     TimeFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field,
                                    Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;
   virtual Double_t     TimeFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field,
                                    Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;

   ClassDef(TUCNGeoIntersection, 1)              // intersection node
};

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// TUCNGeoSubtraction - Boolean node representing a subtraction.               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

class TUCNGeoSubtraction : public TUCNGeoBoolNode
{
public:
   // constructors
   TUCNGeoSubtraction();
   TUCNGeoSubtraction(const char *expr1, const char *expr2);
   TUCNGeoSubtraction(TGeoShape *left, TGeoShape *right, TGeoMatrix *lmat=0, TGeoMatrix *rmat=0);

   // destructor
   virtual ~TUCNGeoSubtraction();
   // methods
   virtual void      ComputeBBox(Double_t &dx, Double_t &dy, Double_t &dz, Double_t *origin);
   virtual void      ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm);
   virtual Bool_t    Contains(Double_t *point) const;
   virtual Int_t     DistanceToPrimitive(Int_t px, Int_t py);
   virtual Double_t  DistFromInside(Double_t *point, Double_t *dir, Int_t iact=1,
                               Double_t step=0, Double_t *safe=0) const;
   virtual Double_t  DistFromOutside(Double_t *point, Double_t *dir, Int_t iact=1,
                               Double_t step=0, Double_t *safe=0) const;
   virtual EGeoBoolType GetBooleanOperator() const {return kGeoSubtraction;}
   virtual Int_t     GetNpoints() const;
   virtual Double_t  Safety(Double_t *point, Bool_t in=kTRUE) const;
   virtual void      SavePrimitive(ostream &out, Option_t *option = "");
   virtual void      SetPoints(Double_t *points) const;
   virtual void      SetPoints(Float_t *points)  const;
   virtual void      Sizeof3D() const;

   //CS specific
   virtual void      Paint(Option_t *option);

   // new methods
   virtual Double_t     TimeFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field,
                                    Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;
   virtual Double_t     TimeFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field,
                                    Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;

   ClassDef(TUCNGeoSubtraction, 1)              // subtraction node
};
#endif

