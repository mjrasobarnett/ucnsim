// @(#)root/geom:$Id: TUCNGeoCompositeShape.cxx 21301 2007-12-10 16:21:50Z brun $
// Author: Andrei Gheata   31/01/02

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "Riostream.h"
#include "TRandom3.h"

#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TVirtualGeoPainter.h"

#include "TVirtualPad.h"
#include "TVirtualViewer3D.h"
#include "TBuffer3D.h"
#include "TBuffer3DTypes.h"

#include "TUCNGeoBoolNode.h"
#include "TUCNGeoCompositeShape.h"

ClassImp(TUCNGeoCompositeShape)


//_____________________________________________________________________________
TUCNGeoCompositeShape::TUCNGeoCompositeShape()
                      :TUCNGeoBBox(0, 0, 0)
{
// Default constructor
   SetShapeBit(TGeoShape::kGeoComb);
   fNode  = 0;
}   

//_____________________________________________________________________________
TUCNGeoCompositeShape::TUCNGeoCompositeShape(const char *name, const char *expression)
                      :TUCNGeoBBox(0, 0, 0)
{
// Default constructor
   SetShapeBit(TGeoShape::kGeoComb);
   SetName(name);
   fNode  = 0;
   MakeNode(expression);
   if (!fNode) {
      Error("ctor", "Composite %s: cannot parse expression: %s", name, expression);
      return;
   }
   ComputeBBox();
}  

//_____________________________________________________________________________
TUCNGeoCompositeShape::TUCNGeoCompositeShape(const char *expression)
                      :TUCNGeoBBox(0, 0, 0)
{
// Default constructor
   SetShapeBit(TGeoShape::kGeoComb);
   fNode  = 0;
   MakeNode(expression);
   if (!fNode) {
      char message[256];
      sprintf(message, "Composite (no name) could not parse expression %s", expression);
      Error("ctor", message);
      return;
   }
   ComputeBBox();
}  

//_____________________________________________________________________________
TUCNGeoCompositeShape::TUCNGeoCompositeShape(const char *name, TUCNGeoBoolNode *node)
                      :TUCNGeoBBox(0,0,0)
{
// Constructor with a Boolean node
   SetName(name);
   fNode = node;
   if (!fNode) {
      Error("ctor", "Composite shape %s has null node", name);
      return;
   }
   ComputeBBox();
}

//_____________________________________________________________________________
TUCNGeoCompositeShape::~TUCNGeoCompositeShape()
{
// destructor
   if (fNode) delete fNode;
}

//_____________________________________________________________________________
Double_t TUCNGeoCompositeShape::Capacity() const
{
// Computes capacity of this shape [length^3] by sampling with 1% error.
   Double_t pt[3];
   if (!gRandom) gRandom = new TRandom3();
   Double_t vbox = 8*fDX*fDY*fDZ; // cm3
   Int_t igen=0;
   Int_t iin = 0;
   while (iin<10000) {
      pt[0] = fOrigin[0]-fDX+2*fDX*gRandom->Rndm();
      pt[1] = fOrigin[1]-fDY+2*fDY*gRandom->Rndm();
      pt[2] = fOrigin[2]-fDZ+2*fDZ*gRandom->Rndm();
      igen++;
      if (Contains(pt)) iin++;
   }      
   Double_t capacity = iin*vbox/igen;
   return capacity;
}   

//_____________________________________________________________________________
void TUCNGeoCompositeShape::ComputeBBox()
{
// compute bounding box of the sphere
   if(fNode) fNode->ComputeBBox(fDX, fDY, fDZ, fOrigin);
}   

//_____________________________________________________________________________
void TUCNGeoCompositeShape::ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm)
{
// Computes normal vector in POINT to the composite shape.
   if (fNode) fNode->ComputeNormal(point,dir,norm);
}

//_____________________________________________________________________________
Bool_t TUCNGeoCompositeShape::Contains(Double_t *point) const
{
// Tests if point is inside the shape.
   if (fNode) return fNode->Contains(point);
   return kFALSE;
}

//_____________________________________________________________________________
Double_t TUCNGeoCompositeShape::DistFromOutside(Double_t *point, Double_t *dir, Int_t iact,
                                      Double_t step, Double_t *safe) const
{
// Compute distance from outside point to this composite shape.
// Check if the bounding box is crossed within the requested distance
   Double_t sdist = TUCNGeoBBox::DistFromOutside(point,dir, fDX, fDY, fDZ, fOrigin, step);
   if (sdist>=step) return TGeoShape::Big();
   if (fNode) return fNode->DistFromOutside(point, dir, iact, step, safe);
   return TGeoShape::Big();
}   

//_____________________________________________________________________________
Double_t TUCNGeoCompositeShape::DistFromInside(Double_t *point, Double_t *dir, Int_t iact,
                                      Double_t step, Double_t *safe) const
{
// Compute distance from inside point to outside of this composite shape.
   if (fNode) return fNode->DistFromInside(point, dir, iact, step, safe);
   return TGeoShape::Big();
}   

//_____________________________________________________________________________
TGeoVolume *TUCNGeoCompositeShape::Divide(TGeoVolume  * /*voldiv*/, const char * /*divname*/, Int_t /*iaxis*/, 
                                       Int_t /*ndiv*/, Double_t /*start*/, Double_t /*step*/) 
{
// Divide all range of iaxis in range/step cells 
   Error("Divide", "Composite shapes cannot be divided");
   return 0;
}      

//_____________________________________________________________________________
void TUCNGeoCompositeShape::GetMeshNumbers(Int_t &nvert, Int_t &nsegs, Int_t &npols) const
{
// Returns numbers of vertices, segments and polygons composing the shape mesh.
   nvert = 0;
   nsegs = 0;
   npols = 0;
}

//_____________________________________________________________________________
void TUCNGeoCompositeShape::InspectShape() const
{
// print shape parameters
   printf("*** TUCNGeoCompositeShape : %s = %s\n", GetName(), GetTitle());
   printf(" Bounding box:\n");
   TUCNGeoBBox::InspectShape();
}

//_____________________________________________________________________________
void TUCNGeoCompositeShape::MakeNode(const char *expression)
{
// Make a booleann node according to the top level boolean operation of expression.
// Propagates signal to branches until expression is fully decomposed.
//   printf("Making node for : %s\n", expression);
   if (fNode) delete fNode;
   fNode = 0;
   SetTitle(expression);
   TString sleft, sright, smat;
   Int_t boolop;
   boolop = TGeoManager::Parse(expression, sleft, sright, smat);
   if (boolop<0) {
      // fail
      Error("MakeNode", "parser error");
      return;
   }   
   if (smat.Length())
      Warning("MakeNode", "no geometrical transformation allowed at this level");
   switch (boolop) {
      case 0: 
         Error("MakeNode", "Expression has no boolean operation");
         return;    
      case 1:
         fNode = new TUCNGeoUnion(sleft.Data(), sright.Data());
         return;
      case 2:
         fNode = new TUCNGeoSubtraction(sleft.Data(), sright.Data());
         return;
      case 3:
         fNode = new TUCNGeoIntersection(sleft.Data(), sright.Data());
   }
}               

//_____________________________________________________________________________
Bool_t TUCNGeoCompositeShape::PaintComposite(Option_t *option) const
{
   // Paint this composite shape into the current 3D viewer
   // Returns bool flag indicating if the caller should continue to
   // paint child objects

   Bool_t addChildren = kTRUE;

   TVirtualGeoPainter *painter = gGeoManager->GetGeomPainter();
   TVirtualViewer3D * viewer = gPad->GetViewer3D();
   if (!painter || !viewer) return kFALSE;

   if (fNode) {
      // Fill out the buffer for the composite shape - nothing extra
      // over TUCNGeoBBox
      Bool_t preferLocal = viewer->PreferLocalFrame();
      if (TBuffer3D::GetCSLevel()) preferLocal = kFALSE;
      static TBuffer3D buffer(TBuffer3DTypes::kComposite);
      FillBuffer3D(buffer, TBuffer3D::kCore|TBuffer3D::kBoundingBox,
                   preferLocal);

      Bool_t paintComponents = kTRUE;
      
      // Start a composite shape, identified by this buffer
      if (!TBuffer3D::GetCSLevel()) 
         paintComponents = viewer->OpenComposite(buffer, &addChildren);

      TBuffer3D::IncCSLevel();

      // Paint the boolean node - will add more buffers to viewer
      TGeoHMatrix *matrix = (TGeoHMatrix*)TGeoShape::GetTransform();
      TGeoHMatrix backup(*matrix); 
      if (preferLocal) matrix->Clear();
      if (paintComponents) fNode->Paint(option);
      if (preferLocal) *matrix = backup;
      // Close the composite shape
      if (!TBuffer3D::DecCSLevel())
         viewer->CloseComposite();
   }

   return addChildren;
}

//_____________________________________________________________________________
void TUCNGeoCompositeShape::RegisterYourself()
{
// Register the shape and all components to TGeoManager class.
   if (gGeoManager->GetListOfShapes()->FindObject(this)) return;
   gGeoManager->AddShape(this);
   TGeoMatrix *matrix;
   TGeoShape  *shape;
   TUCNGeoCompositeShape *comp;
   if (fNode) {
      matrix = fNode->GetLeftMatrix();
      if (!matrix->IsRegistered()) matrix->RegisterYourself();
      else if (!gGeoManager->GetListOfMatrices()->FindObject(matrix)) {
         gGeoManager->GetListOfMatrices()->Add(matrix);
      }
      matrix = fNode->GetRightMatrix();
      if (!matrix->IsRegistered()) matrix->RegisterYourself();
      else if (!gGeoManager->GetListOfMatrices()->FindObject(matrix)) {
         gGeoManager->GetListOfMatrices()->Add(matrix);
      }
      shape = fNode->GetLeftShape();
      if (!gGeoManager->GetListOfShapes()->FindObject(shape)) {
         if (shape->IsComposite()) {
            comp = (TUCNGeoCompositeShape*)shape;
            comp->RegisterYourself();
         } else {
            gGeoManager->AddShape(shape);
         }
      }   
      shape = fNode->GetRightShape();
      if (!gGeoManager->GetListOfShapes()->FindObject(shape)) {
         if (shape->IsComposite()) {
            comp = (TUCNGeoCompositeShape*)shape;
            comp->RegisterYourself();
         } else {
            gGeoManager->AddShape(shape);
         }
      }   
   }
}

//_____________________________________________________________________________
Double_t TUCNGeoCompositeShape::Safety(Double_t *point, Bool_t in) const
{
// computes the closest distance from given point to this shape, according
// to option. The matching point on the shape is stored in spoint.
   if (fNode) return fNode->Safety(point,in);
   return 0.;
}

//_____________________________________________________________________________
void TUCNGeoCompositeShape::SavePrimitive(ostream &out, Option_t *option /*= ""*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   if (TObject::TestBit(kGeoSavePrimitive)) return;
   if (fNode) fNode->SavePrimitive(out,option);
   out << "   // Shape: " << GetName() << " type: " << ClassName() << endl;   
   out << "   TGeoShape *" << GetPointerName() << " = new TUCNGeoCompositeShape(\"" << GetName() << "\", pBoolNode);" << endl;
   if (strlen(GetTitle())) out << "   " << GetPointerName() << "->SetTitle(\"" << GetTitle() << "\");" << endl;
   TObject::SetBit(TGeoShape::kGeoSavePrimitive);
}

//_____________________________________________________________________________
void TUCNGeoCompositeShape::SetPoints(Double_t *points) const
{
// create points for a composite shape
   TUCNGeoBBox::SetPoints(points);
}

//_____________________________________________________________________________
void TUCNGeoCompositeShape::SetPoints(Float_t *points) const
{
// create points for a composite shape
   TUCNGeoBBox::SetPoints(points);
}

//_____________________________________________________________________________
void TUCNGeoCompositeShape::Sizeof3D() const
{
// compute size of this 3D object
   if (fNode) fNode->Sizeof3D();
}

//_____________________________________________________________________________
Int_t TUCNGeoCompositeShape::GetNmeshVertices() const
{
// Return number of vertices of the mesh representation
   if (!fNode) return 0;
   return 8;
}      


