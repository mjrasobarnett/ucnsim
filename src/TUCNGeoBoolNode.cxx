// @(#):$Id: TUCNGeoBoolNode.cxx 27731 2009-03-09 17:40:56Z brun $
// Author: Andrei Gheata   30/05/02
// TUCNGeoBoolNode::Contains and parser implemented by Mihaela Gheata

   
/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "Riostream.h"

#include "TGeoMatrix.h"
#include "TGeoManager.h"

#include "TUCNGeoCompositeShape.h"
#include "TUCNGeoBoolNode.h"

#include "TVirtualPad.h"
#include "TVirtualViewer3D.h"
#include "TBuffer3D.h"
#include "TBuffer3DTypes.h"
#include "TMath.h"

//#define VERBOSE_MODE

//_____________________________________________________________________________
//  TUCNGeoBoolNode - base class for Boolean operations between two shapes.
//===============
// A Boolean node describes a Boolean operation between 'left' and 'right' 
// shapes positioned with respect to an ARBITRARY reference frame. The boolean
// node is referenced by a mother composite shape and its shape components may
// be primitive but also composite shapes. The later situation leads to a binary
// tree hierarchy. When the parent composite shape is used to create a volume,
// the reference frame of the volume is chosen to match the frame in which 
// node shape components were defined.
//
// The positioned shape components may or may not be disjoint. The specific 
// implementations for Boolean nodes are:
//
//    TUCNGeoUnion - representing the Boolean  union of two positioned shapes
//
//    TUCNGeoSubtraction - representing the Boolean subtraction of two positioned 
//                shapes
// 
//    TUCNGeoIntersection - representing the Boolean intersection of two positioned
//                shapes
//_____________________________________________________________________________

ClassImp(TUCNGeoBoolNode)

//_____________________________________________________________________________
TUCNGeoBoolNode::TUCNGeoBoolNode()
{
// Default constructor
	#ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoBoolNode","Default Constructor");
	#endif
	fLeft     = 0;
   fRight    = 0;
   fLeftMat  = 0;
   fRightMat = 0;
   fSelected = 0;
}
//_____________________________________________________________________________
TUCNGeoBoolNode::TUCNGeoBoolNode(const char *expr1, const char *expr2)
{
// Constructor called by TUCNGeoCompositeShape providing 2 subexpressions for the 2 branches.
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoBoolNode","Constructor");
	#endif
	fLeft     = 0;
   fRight    = 0;
   fLeftMat  = 0;
   fRightMat = 0;
   fSelected = 0;
   if (!MakeBranch(expr1, kTRUE)) {
      return;
   }
   if (!MakeBranch(expr2, kFALSE)) {
      return;
   }
}

//_____________________________________________________________________________
TUCNGeoBoolNode::TUCNGeoBoolNode(TGeoShape *left, TGeoShape *right, TGeoMatrix *lmat, TGeoMatrix *rmat)
{
// Constructor providing left and right shapes and matrices (in the Boolean operation).
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoBoolNode","Constructor");
	#endif
	fSelected = 0;
   fLeft = left;
   fRight = right;
   fLeftMat = lmat;
   if (!fLeftMat) fLeftMat = gGeoIdentity;
   else fLeftMat->RegisterYourself();
   fRightMat = rmat;
   if (!fRightMat) fRightMat = gGeoIdentity;
   else fRightMat->RegisterYourself();
   if (!fLeft) {
      Error("ctor", "left shape is NULL");
      return;
   }   
   if (!fRight) {
      Error("ctor", "right shape is NULL");
      return;
   }   
}

//_____________________________________________________________________________
TUCNGeoBoolNode::~TUCNGeoBoolNode()
{
// Destructor.
// --- deletion of components handled by TGeoManager class.
	#ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoBoolNode","Destructor");
   #endif
}
//_____________________________________________________________________________
Bool_t TUCNGeoBoolNode::MakeBranch(const char *expr, Bool_t left)
{
// Expands the boolean expression either on left or right branch, creating
// component elements (composite shapes and boolean nodes). Returns true on success.
   TString sleft, sright, stransf;
   Int_t boolop = TGeoManager::Parse(expr, sleft, sright, stransf);
   if (boolop<0) {
      Error("MakeBranch", "invalid expresion");
      return kFALSE;
   }
   TGeoShape *shape = 0;
   TGeoMatrix *mat;
   TString newshape;

   if (stransf.Length() == 0) {
      mat = gGeoIdentity;
   } else {   
      mat = (TGeoMatrix*)gGeoManager->GetListOfMatrices()->FindObject(stransf.Data());    
   }
   if (!mat) {
      Error("MakeBranch", "transformation %s not found", stransf.Data());
      return kFALSE;
   }
   switch (boolop) {
      case 0:
         // elementary shape
         shape = (TGeoShape*)gGeoManager->GetListOfShapes()->FindObject(sleft.Data()); 
         if (!shape) {
            Error("MakeBranch", "shape %s not found", sleft.Data());
            return kFALSE;
         }
         break;
      case 1:
         // composite shape - union
         newshape = sleft;
         newshape += "+";
         newshape += sright;
         shape = new TUCNGeoCompositeShape(newshape.Data());
         break;
      case 2:
         // composite shape - difference
         newshape = sleft;
         newshape += "-";
         newshape += sright;
         shape = new TUCNGeoCompositeShape(newshape.Data());
         break;
      case 3:
         // composite shape - intersection
         newshape = sleft;
         newshape += "*";
         newshape += sright;
         shape = new TUCNGeoCompositeShape(newshape.Data());
         break;
   }      
   if (boolop && !shape->IsValid()) {
      Error("MakeBranch", "Shape %s not valid", newshape.Data());
      delete shape;
      return kFALSE;
   }      
   if (left) {
      fLeft = shape;
      fLeftMat = mat;
   } else {
      fRight = shape;
      fRightMat = mat;
   }
   return kTRUE;                  
}
//_____________________________________________________________________________
void TUCNGeoBoolNode::Paint(Option_t * option)
{
// Special schema for feeding the 3D buffers to the painter client.
   TVirtualViewer3D * viewer = gPad->GetViewer3D();
   if (!viewer) return;

   // Components of composite shape hierarchies for local frame viewers are painted 
   // in coordinate frame of the top level composite shape. So we force 
   // conversion to this.  See TGeoPainter::PaintNode for loading of GLMatrix.
   Bool_t localFrame = kFALSE; //viewer->PreferLocalFrame();

   TGeoHMatrix *glmat = (TGeoHMatrix*)TGeoShape::GetTransform();
   TGeoHMatrix mat;
   mat = glmat; // keep a copy

   // Now perform fetch and add of the two components buffers.
   // Note we assume that composite shapes are always completely added
   // so don't bother to get addDaughters flag from viewer->AddObject()

   // Setup matrix and fetch/add the left component buffer
   glmat->Multiply(fLeftMat);
   //fLeft->Paint(option);
   if (TUCNGeoCompositeShape *left = dynamic_cast<TUCNGeoCompositeShape *>(fLeft)) left->PaintComposite(option);
   else {
      const TBuffer3D & leftBuffer = fLeft->GetBuffer3D(TBuffer3D::kAll, localFrame);
      viewer->AddObject(leftBuffer);
   }

   // Setup matrix and fetch/add the right component buffer
   *glmat = &mat;
   glmat->Multiply(fRightMat);
   //fRight->Paint(option);
   if (TUCNGeoCompositeShape *right = dynamic_cast<TUCNGeoCompositeShape *>(fRight)) right->PaintComposite(option);
   else {
      const TBuffer3D & rightBuffer = fRight->GetBuffer3D(TBuffer3D::kAll, localFrame);
      viewer->AddObject(rightBuffer);
   }

   *glmat = &mat;   
}
//_____________________________________________________________________________
void TUCNGeoBoolNode::RegisterMatrices()
{
// Register all matrices of the boolean node and descendents.
   if (!fLeftMat->IsIdentity()) fLeftMat->RegisterYourself();   
   if (!fRightMat->IsIdentity()) fRightMat->RegisterYourself();   
   if (fLeft->IsComposite()) ((TUCNGeoCompositeShape*)fLeft)->GetBoolNode()->RegisterMatrices();
   if (fRight->IsComposite()) ((TUCNGeoCompositeShape*)fRight)->GetBoolNode()->RegisterMatrices();
}
//_____________________________________________________________________________
void TUCNGeoBoolNode::SavePrimitive(ostream &out, Option_t *option /*= ""*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   fLeft->SavePrimitive(out,option);
   fRight->SavePrimitive(out,option);
   if (!fLeftMat->IsIdentity()) {
      fLeftMat->RegisterYourself();
      fLeftMat->SavePrimitive(out,option);
   }      
   if (!fRightMat->IsIdentity()) {
      fRightMat->RegisterYourself();
      fRightMat->SavePrimitive(out,option);
   }      
}
//_____________________________________________________________________________
void TUCNGeoBoolNode::Sizeof3D() const
{
// Register size of this 3D object
   fLeft->Sizeof3D();
   fRight->Sizeof3D();
}


//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________

ClassImp(TUCNGeoUnion)

//_____________________________________________________________________________
void TUCNGeoUnion::Paint(Option_t *option)
{
// Paint method.
   TVirtualViewer3D *viewer = gPad->GetViewer3D();

   if (!viewer) {
      Error("Paint", "gPad->GetViewer3D() returned 0, cannot work with composite!\n");
      return;
   }

   viewer->AddCompositeOp(TBuffer3D::kCSUnion);

   TUCNGeoBoolNode::Paint(option);
}

//_____________________________________________________________________________
TUCNGeoUnion::TUCNGeoUnion()
{
// Default constructor
	#ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoUnion","Default Constructor");
   #endif
}
//_____________________________________________________________________________
TUCNGeoUnion::TUCNGeoUnion(const char *expr1, const char *expr2)
          :TUCNGeoBoolNode(expr1, expr2)
{
// Constructor
	#ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoUnion","Constructor");
   #endif
}

//_____________________________________________________________________________
TUCNGeoUnion::TUCNGeoUnion(TGeoShape *left, TGeoShape *right, TGeoMatrix *lmat, TGeoMatrix *rmat)
          :TUCNGeoBoolNode(left,right,lmat,rmat)
{
// Constructor providing pointers to components
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoUnion","Constructor");
	#endif
	if (left->TestShapeBit(TGeoShape::kGeoHalfSpace) || right->TestShapeBit(TGeoShape::kGeoHalfSpace)) {
      Fatal("TUCNGeoUnion", "Unions with a half-space (%s + %s) not allowed", left->GetName(), right->GetName());
   }
}

//_____________________________________________________________________________
TUCNGeoUnion::~TUCNGeoUnion()
{
// Destructor
// --- deletion of components handled by TGeoManager class.
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoUnion","Destructor");
   #endif
}
//_____________________________________________________________________________
void TUCNGeoUnion::ComputeBBox(Double_t &dx, Double_t &dy, Double_t &dz, Double_t *origin)
{
// Compute bounding box corresponding to a union of two shapes.
   if (((TGeoBBox*)fLeft)->IsNullBox()) fLeft->ComputeBBox();
   if (((TGeoBBox*)fRight)->IsNullBox()) fRight->ComputeBBox();
   Double_t vert[48];
   Double_t pt[3];
   Int_t i;
   Double_t xmin, xmax, ymin, ymax, zmin, zmax;
   xmin = ymin = zmin = TGeoShape::Big();
   xmax = ymax = zmax = -TGeoShape::Big();
   ((TGeoBBox*)fLeft)->SetBoxPoints(&vert[0]);
   ((TGeoBBox*)fRight)->SetBoxPoints(&vert[24]);
   for (i=0; i<8; i++) {
      fLeftMat->LocalToMaster(&vert[3*i], &pt[0]);
      if (pt[0]<xmin) xmin=pt[0];
      if (pt[0]>xmax) xmax=pt[0];
      if (pt[1]<ymin) ymin=pt[1];
      if (pt[1]>ymax) ymax=pt[1];
      if (pt[2]<zmin) zmin=pt[2];
      if (pt[2]>zmax) zmax=pt[2];
   }   
   for (i=8; i<16; i++) {
      fRightMat->LocalToMaster(&vert[3*i], &pt[0]);
      if (pt[0]<xmin) xmin=pt[0];
      if (pt[0]>xmax) xmax=pt[0];
      if (pt[1]<ymin) ymin=pt[1];
      if (pt[1]>ymax) ymax=pt[1];
      if (pt[2]<zmin) zmin=pt[2];
      if (pt[2]>zmax) zmax=pt[2];
   }   
   dx = 0.5*(xmax-xmin);
   origin[0] = 0.5*(xmin+xmax);
   dy = 0.5*(ymax-ymin);
   origin[1] = 0.5*(ymin+ymax);
   dz = 0.5*(zmax-zmin);
   origin[2] = 0.5*(zmin+zmax);
}   
//_____________________________________________________________________________
Bool_t TUCNGeoUnion::Contains(Double_t *point) const
{
// Find if a union of two shapes contains a given point
   Double_t local[3];
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   fLeftMat->MasterToLocal(point, &local[0]);
   Bool_t inside = fLeft->Contains(&local[0]);
   if (inside) {
      node->SetSelected(1);
      return kTRUE;
   }   
   fRightMat->MasterToLocal(point, &local[0]);
   inside = fRight->Contains(&local[0]);
   if (inside) node->SetSelected(2);
   return inside;
}

//_____________________________________________________________________________
void TUCNGeoUnion::ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm)
{
// Normal computation in POINT. The orientation is chosen so that DIR.dot.NORM>0.
   norm[0] = norm[1] = 0.;
   norm[2] = 1.;
   Double_t local[3];
   Double_t ldir[3], lnorm[3];
   if (fSelected == 1) {
      fLeftMat->MasterToLocal(point, local);
      fLeftMat->MasterToLocalVect(dir, ldir);
      fLeft->ComputeNormal(local,ldir,lnorm);
      fLeftMat->LocalToMasterVect(lnorm, norm);
      return;
   }
   if (fSelected == 2) {
      fRightMat->MasterToLocal(point, local);
      fRightMat->MasterToLocalVect(dir, ldir);
      fRight->ComputeNormal(local,ldir,lnorm);
      fRightMat->LocalToMasterVect(lnorm, norm);
      return;
   }            
   fLeftMat->MasterToLocal(point, local);
   if (fLeft->Contains(local)) {
      fLeftMat->MasterToLocalVect(dir, ldir);
      fLeft->ComputeNormal(local,ldir,lnorm);
      fLeftMat->LocalToMasterVect(lnorm, norm);
      return;
   }   
   fRightMat->MasterToLocal(point, local);
   if (fRight->Contains(local)) {
      fRightMat->MasterToLocalVect(dir, ldir);
      fRight->ComputeNormal(local,ldir,lnorm);
      fRightMat->LocalToMasterVect(lnorm, norm);
      return;
   }   
   // Propagate forward/backward to see which of the components is intersected first
   local[0] = point[0] + 1E-5*dir[0];
   local[1] = point[1] + 1E-5*dir[1];
   local[2] = point[2] + 1E-5*dir[2];

   if (!Contains(local)) {
      local[0] = point[0] - 1E-5*dir[0];
      local[1] = point[1] - 1E-5*dir[1];
      local[2] = point[2] - 1E-5*dir[2];
      if (!Contains(local)) return;
   }
   ComputeNormal(local,dir,norm);   
}

//_____________________________________________________________________________
Int_t TUCNGeoUnion::DistanceToPrimitive(Int_t /*px*/, Int_t /*py*/)
{
// Compute minimum distance to shape vertices.
   return 9999;
}
//_____________________________________________________________________________
Double_t TUCNGeoUnion::DistFromInside(Double_t *point, Double_t *dir, Int_t iact,
                              Double_t step, Double_t *safe) const
{
// Computes distance from a given point inside the shape to its boundary.
   if (iact<3 && safe) {
      // compute safe distance
      *safe = Safety(point,kTRUE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }

   Double_t local[3], master[3], ldir[3], rdir[3], pushed[3];
   memcpy(master, point, 3*sizeof(Double_t));
   Int_t i;
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Double_t d1=0., d2=0., snxt=0.;
   fLeftMat->MasterToLocalVect(dir, ldir);
   fRightMat->MasterToLocalVect(dir, rdir);
   fLeftMat->MasterToLocal(point, local);
   Bool_t inside1 = fLeft->Contains(local);
   if (inside1) d1 = fLeft->DistFromInside(local, ldir, 3);
   fRightMat->MasterToLocal(point, local);
   Bool_t inside2 = fRight->Contains(local);
   if (inside2) d2 = fRight->DistFromInside(local, rdir, 3);

   while (inside1 || inside2) {
      if (inside1 && inside2) {
         if (d1<d2) {      
            snxt += d1;
            node->SetSelected(1);
            // propagate to exit of left shape
            inside1 = kFALSE;
            for (i=0; i<3; i++) master[i] += d1*dir[i];
            // check if propagated point is in right shape        
            fRightMat->MasterToLocal(master, local);
            inside2 = fRight->Contains(local);
            if (!inside2) return snxt;
            d2 = fRight->DistFromInside(local, rdir, 3);
            if (d2 < TGeoShape::Tolerance()) return snxt;
         } else {
            snxt += d2;
            node->SetSelected(2);
            // propagate to exit of right shape
            inside2 = kFALSE;
            for (i=0; i<3; i++) master[i] += d2*dir[i];
            // check if propagated point is in left shape        
            fLeftMat->MasterToLocal(master, local);
            inside1 = fLeft->Contains(local);
            if (!inside1) return snxt;
            d1 = fLeft->DistFromInside(local, ldir, 3);
            if (d1 < TGeoShape::Tolerance()) return snxt;
         }
      } 
      if (inside1) {
         snxt += d1;
         node->SetSelected(1);
         // propagate to exit of left shape
         inside1 = kFALSE;
         for (i=0; i<3; i++) {
            master[i] += d1*dir[i];
            pushed[i] = master[i]+(1.+d1)*TGeoShape::Tolerance()*dir[i];
         }   
         // check if propagated point is in right shape        
         fRightMat->MasterToLocal(pushed, local);
         inside2 = fRight->Contains(local);
         if (!inside2) return snxt;
         d2 = fRight->DistFromInside(local, rdir, 3);
         if (d2 < TGeoShape::Tolerance()) return snxt;
         d2 += (1.+d1)*TGeoShape::Tolerance();
      }   
      if (inside2) {
         snxt += d2;
         node->SetSelected(2);
         // propagate to exit of right shape
         inside2 = kFALSE;
         for (i=0; i<3; i++) {
            master[i] += d2*dir[i];
            pushed[i] = master[i]+(1.+d2)*TGeoShape::Tolerance()*dir[i];
         }   
         // check if propagated point is in left shape        
         fLeftMat->MasterToLocal(pushed, local);
         inside1 = fLeft->Contains(local);
         if (!inside1) return snxt;
         d1 = fLeft->DistFromInside(local, ldir, 3);
         if (d1 < TGeoShape::Tolerance()) return snxt;
         d1 += (1.+d2)*TGeoShape::Tolerance();
      }
   }      
   return snxt;
}

//_____________________________________________________________________________
Double_t TUCNGeoUnion::DistFromOutside(Double_t *point, Double_t *dir, Int_t iact,
                              Double_t step, Double_t *safe) const
{
// Compute distance from a given outside point to the shape.
   if (iact<3 && safe) {
      // compute safe distance
      *safe = Safety(point,kFALSE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Double_t local[3], ldir[3], rdir[3];
   Double_t d1, d2, snxt;
   fLeftMat->MasterToLocal(point, &local[0]);
   fLeftMat->MasterToLocalVect(dir, &ldir[0]);
   fRightMat->MasterToLocalVect(dir, &rdir[0]);
   d1 = fLeft->DistFromOutside(&local[0], &ldir[0], iact, step, safe);
   fRightMat->MasterToLocal(point, &local[0]);
   d2 = fRight->DistFromOutside(&local[0], &rdir[0], iact, step, safe);
   if (d1<d2) {
      snxt = d1;
      node->SetSelected(1);
   } else {
      snxt = d2;
      node->SetSelected(2);
   }      
   return snxt;
}

//_____________________________________________________________________________
Int_t TUCNGeoUnion::GetNpoints() const
{
// Returns number of vertices for the composite shape described by this union.
   return 0;
}

//_____________________________________________________________________________
void TUCNGeoUnion::SetPoints(Double_t * /*points*/) const
{
// Fill buffer with shape vertices.
}

//_____________________________________________________________________________
Double_t TUCNGeoUnion::Safety(Double_t *point, Bool_t in) const
{
// Compute safety distance for a union node;
   Double_t local1[3], local2[3];
   fLeftMat->MasterToLocal(point,local1);
   Bool_t in1 = fLeft->Contains(local1);
   fRightMat->MasterToLocal(point,local2);
   Bool_t in2 = fRight->Contains(local2);
   Bool_t intrue = in1 | in2;
   if (intrue^in) return 0.0;
   Double_t saf1 = fLeft->Safety(local1, in1);
   Double_t saf2 = fRight->Safety(local2, in2);
   if (in1 && in2) return TMath::Min(saf1, saf2);
   if (in1)        return saf1;
   if (in2)        return saf2;
   return TMath::Min(saf1,saf2);
}   

//_____________________________________________________________________________
void TUCNGeoUnion::SavePrimitive(ostream &out, Option_t *option /*= ""*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   TUCNGeoBoolNode::SavePrimitive(out,option);
   out << "   pBoolNode = new TUCNGeoUnion(";
   out << fLeft->GetPointerName() << ",";
   out << fRight->GetPointerName() << ",";
   if (!fLeftMat->IsIdentity()) out << fLeftMat->GetPointerName() << ",";
   else                         out << "0,";
   if (!fRightMat->IsIdentity()) out << fRightMat->GetPointerName() << ");" << endl;
   else                         out << "0);" << endl;
}   

//_____________________________________________________________________________
void TUCNGeoUnion::SetPoints(Float_t * /*points*/) const
{
// Fill buffer with shape vertices.
}

//_____________________________________________________________________________
void TUCNGeoUnion::Sizeof3D() const
{
// Register 3D size of this shape.
   TUCNGeoBoolNode::Sizeof3D();
}

//_____________________________________________________________________________
Double_t TUCNGeoUnion::TimeFromInsideAlongParabola(const Double_t* point, const Double_t* velocity, const Double_t* field,
                                 const Double_t stepTime, const Bool_t onBoundary) const
{
// Compute time from inside point to outside of this composite shape along parabola.
// Refering to 'left' shape and 'right' shape, I am refering to the two terms in the equation for
// this particular BoolNode: left + right. 
   #ifdef VERBOSE_MODE
      Info("TimeFromInsideAlongParabola","Start");
      cout << "Initial X: " << point[0] << "\t" << "V: " << point[1] << "\t" << "Z: " << point[2] << endl;
      cout << "Initial Vx: " << velocity[0] << "\t" << "Vy: " << velocity[1] << "\t" << "Vz: " << velocity[2] << endl;
      cout << "Field Gx: " << field[0] << "\t" << "Gy: " << field[1] << "\t" << "Gz: " << field[2] << "\t" << endl;
   #endif
   
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Int_t i;
   Double_t leftTime=0., rightTime=0., finalTime=0.;
   // Create some temporary storage for the master point and velocity
   Double_t masterPoint[3], masterVel[3], masterField[3], pushedPoint[3], pushedVel[3];
   memcpy(masterPoint, point, 3*sizeof(Double_t));
   memcpy(masterVel, velocity, 3*sizeof(Double_t));
   memcpy(masterField, field, 3*sizeof(Double_t));
   // Perform coordinate transformations from Global to the local coords of ('+') & ('-')
   Double_t leftPoint[3], leftVel[3], leftField[3], rightPoint[3], rightVel[3], rightField[3];
   fLeftMat->MasterToLocal(point, &leftPoint[0]);
   fLeftMat->MasterToLocalVect(velocity, &leftVel[0]);
   fLeftMat->MasterToLocalVect(field, &leftField[0]);
   fRightMat->MasterToLocal(point, &rightPoint[0]);
   fRightMat->MasterToLocalVect(velocity, &rightVel[0]);
   fRightMat->MasterToLocalVect(field, &rightField[0]);
   // Calculate Time to each Node
   Bool_t insideLeft = fLeft->Contains(leftPoint);
   if (insideLeft) leftTime = dynamic_cast<TUCNGeoBBox*>(fLeft)->TimeFromInsideAlongParabola(leftPoint, leftVel, \
                                                                                 leftField, stepTime, onBoundary);
   Bool_t insideRight = fRight->Contains(rightPoint);
   if (insideRight) rightTime = dynamic_cast<TUCNGeoBBox*>(fRight)->TimeFromInsideAlongParabola(rightPoint, rightVel, \
                                                                                    rightField, stepTime, onBoundary);
   #ifdef VERBOSE_MODE
      Info("TimeFromInsideAlongParabola","Are we Inside Left? %i. Are we inside Right? %i",insideLeft,insideRight);
      Info("TimeFromInsideAlongParabola","LeftTime: %f. RightTime: %f",leftTime,rightTime);
   #endif
   while (insideLeft || insideRight) {
      if (insideLeft && insideRight) {
         #ifdef VERBOSE_MODE
            Info("TimeFromInsideAlongParabola","We are inside both Left and Right. Determining which we exit first.");
         #endif
         if (leftTime < rightTime) {      
            finalTime += leftTime;
            node->SetSelected(1);
            // propagate to exit of left shape
            #ifdef VERBOSE_MODE
               Info("TimeFromInsideAlongParabola","Exit Left First. Propagating outside Left.");
            #endif
            insideLeft = kFALSE;
            for (i=0; i<3; i++) {
               masterPoint[i] += masterVel[i]*leftTime + 0.5*masterField[i]*leftTime*leftTime;
               masterVel[i] += masterField[i]*leftTime;
            }
            // check if propagated point is in right shape        
            fRightMat->MasterToLocal(masterPoint, rightPoint);
            fRightMat->MasterToLocalVect(masterVel, rightVel);
            insideRight = fRight->Contains(rightPoint);
            #ifdef VERBOSE_MODE
               Info("TimeFromInsideAlongParabola","Propagated Point inside Right? %i",insideRight);
            #endif
            if (!insideRight) {
               #ifdef VERBOSE_MODE
                  Info("TimeFromInsideAlongParabola","Returning finalTime: %f",finalTime);
               #endif
               return finalTime;
            }
            rightTime = dynamic_cast<TUCNGeoBBox*>(fRight)->TimeFromInsideAlongParabola(rightPoint, rightVel, \
                                                                           rightField, stepTime, onBoundary);
            #ifdef VERBOSE_MODE
               Info("TimeFromInsideAlongParabola","Time to Exit Right: %f",rightTime);
            #endif
            if (rightTime < TGeoShape::Tolerance()) return finalTime;
         } else {
            finalTime += rightTime;
            node->SetSelected(2);
            // propagate to exit of right shape
            #ifdef VERBOSE_MODE
               Info("TimeFromInsideAlongParabola","Exit Right First. Propagating outside Right.");
            #endif
            insideRight = kFALSE;
            for (i=0; i<3; i++) {
               masterPoint[i] += masterVel[i]*rightTime + 0.5*masterField[i]*rightTime*rightTime;
               masterVel[i] += masterField[i]*rightTime;
            }
            // check if propagated point is in left shape        
            fLeftMat->MasterToLocal(masterPoint, leftPoint);
            fLeftMat->MasterToLocalVect(masterVel, leftVel);
            insideLeft = fLeft->Contains(leftPoint);
            #ifdef VERBOSE_MODE
               Info("TimeFromInsideAlongParabola","Propagated Point inside Left? %i",insideLeft);
            #endif
            if (!insideLeft) {
               #ifdef VERBOSE_MODE
                  Info("TimeFromInsideAlongParabola","Returning finalTime: %f",finalTime);
               #endif
               return finalTime;
            }
            leftTime = dynamic_cast<TUCNGeoBBox*>(fLeft)->TimeFromInsideAlongParabola(leftPoint, leftVel, \
                                                                           leftField, stepTime, onBoundary);
            #ifdef VERBOSE_MODE
               Info("TimeFromInsideAlongParabola","Time to Exit Left? %f",leftTime);
            #endif
            if (leftTime < TGeoShape::Tolerance()) return finalTime;
         }
      } 
      if (insideLeft) {
         finalTime += leftTime;
         node->SetSelected(1);
         // propagate to exit of left shape
         #ifdef VERBOSE_MODE
            Info("TimeFromInsideAlongParabola","Currently Only Inside Left. Propagating to Exit Left.");
            cout << "Final: " << finalTime << endl;
         #endif
         insideLeft = kFALSE;
         for (i=0; i<3; i++) {
            masterPoint[i] += masterVel[i]*leftTime + 0.5*masterField[i]*leftTime*leftTime;
            masterVel[i] += masterField[i]*leftTime;
            pushedPoint[i] = masterPoint[i] + (masterVel[i]*(1.+leftTime)*TGeoShape::Tolerance()) + (0.5*masterField[i]*(1.+leftTime)*TGeoShape::Tolerance()*(1.+leftTime)*TGeoShape::Tolerance());
            pushedVel[i] = masterVel[i] + masterField[i]*(1.+leftTime)*TGeoShape::Tolerance();
         }   
         // check if propagated point is in right shape
         fRightMat->MasterToLocal(&pushedPoint[0], &rightPoint[0]);
         fRightMat->MasterToLocalVect(&pushedVel[0], &rightVel[0]);
         insideRight = fRight->Contains(rightPoint);
         #ifdef VERBOSE_MODE
            cout << "Master : Propagated by RightTime: " << leftTime << endl;
            cout << "Master X: " << masterPoint[0] << "\t" << "Y: " << masterPoint[1] << "\t" << "Z: " << masterPoint[2] << endl;
            cout << "Master Vx: " << masterVel[0] << "\t" << "Vy: " << masterVel[1] << "\t" << "Vz: " << masterVel[2] << endl;
            cout << "Field Gx: " << masterField[0] << "\t" << "Gy: " << masterField[1] << "\t" << "Gz: " << masterField[2] << "\t" << endl;
            cout << "Master Sqrt(X^2 + Z^2): " << TMath::Sqrt(TMath::Power(masterPoint[0],2) + TMath::Power(masterPoint[2],2)) << endl;
            cout << "Pushed : Propagated by (1.+leftTime)*TGeoShape::Tolerance(): " << (1.+leftTime)*TGeoShape::Tolerance() << endl;
            cout << "Pushed X: " << pushedPoint[0] << "\t" << "Y: " << pushedPoint[1] << "\t" << "Z: " << pushedPoint[2] << endl;
            cout << "Pushed Vx: " << pushedVel[0] << "\t" << "Vy: " << pushedVel[1] << "\t" << "Vz: " << pushedVel[2] << endl;
            cout << "Pushed Sqrt(X^2 + Z^2): " << TMath::Sqrt(TMath::Power(pushedPoint[0],2) + TMath::Power(pushedPoint[2],2)) << endl;
            Info("TimeFromInsideAlongParabola","Propagated Point inside Right? %i",insideRight);
         #endif
         if (!insideRight) {
            #ifdef VERBOSE_MODE
               cout << "Final: " << finalTime << endl;
               Info("TimeFromInsideAlongParabola","Returning finalTime: %f",finalTime);
            #endif
            return finalTime;
         }
         #ifdef VERBOSE_MODE
            Info("TimeFromInsideAlongParabola","Still Inside Right therefore find time to exit right");
         #endif
         rightTime = dynamic_cast<TUCNGeoBBox*>(fRight)->TimeFromInsideAlongParabola(rightPoint, rightVel, \
                                                                           rightField, stepTime, onBoundary);
         if (rightTime < TGeoShape::Tolerance()) {
            #ifdef VERBOSE_MODE
               Info("TimeFromInsideAlongParabola","Time to right is very small. Returning finalTime: %f",finalTime);
            #endif
            return finalTime;
         }
         rightTime += (1.+leftTime)*TGeoShape::Tolerance();
         #ifdef VERBOSE_MODE
            cout << "Incrementing Right Time by (1.+leftTime)*TGeoShape::Tolerance(): " << rightTime << endl;
            Info("TimeFromInsideAlongParabola","Time to Exit Right? %f",rightTime);
         #endif
      }   
      if (insideRight) {
         finalTime += rightTime;
         node->SetSelected(2);
         // propagate to exit of right shape
         #ifdef VERBOSE_MODE
            Info("TimeFromInsideAlongParabola","Currently Only Inside Right. Propagating to Exit Right.");
            cout << "Final: " << finalTime << endl;
         #endif
         insideRight = kFALSE;
         for (i=0; i<3; i++) {
            masterPoint[i] += masterVel[i]*rightTime + 0.5*masterField[i]*rightTime*rightTime;
            masterVel[i] += masterField[i]*rightTime;
            pushedPoint[i] = masterPoint[i] + (masterVel[i]*(1.+rightTime)*TGeoShape::Tolerance()) + (0.5*masterField[i]*(1.+rightTime)*TGeoShape::Tolerance()*(1.+rightTime)*TGeoShape::Tolerance());
            pushedVel[i] = masterVel[i] + masterField[i]*(1.+leftTime)*TGeoShape::Tolerance();         
         }   
         // check if propagated point is in left shape        
         fLeftMat->MasterToLocal(&pushedPoint[0], &leftPoint[0]);
         fLeftMat->MasterToLocalVect(&pushedVel[0], &leftVel[0]);
         insideLeft = fLeft->Contains(leftPoint);
         #ifdef VERBOSE_MODE
            cout << "Master : Propagated by RightTime: " << rightTime << endl;
            cout << "Master X: " << masterPoint[0] << "\t" << "Y: " << masterPoint[1] << "\t" << "Z: " << masterPoint[2] << endl;
            cout << "Master Vx: " << masterVel[0] << "\t" << "Vy: " << masterVel[1] << "\t" << "Vz: " << masterVel[2] << endl;
            cout << "Field Gx: " << masterField[0] << "\t" << "Gy: " << masterField[1] << "\t" << "Gz: " << masterField[2] << "\t" << endl;
            cout << "Master Sqrt(X^2 + Z^2): " << TMath::Sqrt(TMath::Power(masterPoint[0],2) + TMath::Power(masterPoint[2],2)) << endl;
            cout << "Pushed : Propagated by (1.+rightTime)*TGeoShape::Tolerance(): " << (1.+rightTime)*TGeoShape::Tolerance() << endl;
            cout << "Pushed X: " << pushedPoint[0] << "\t" << "Y: " << pushedPoint[1] << "\t" << "Z: " << pushedPoint[2] << endl;
            cout << "Pushed Vx: " << pushedVel[0] << "\t" << "Vy: " << pushedVel[1] << "\t" << "Vz: " << pushedVel[2] << endl;
            cout << "Pushed Sqrt(X^2 + Z^2): " << TMath::Sqrt(TMath::Power(pushedPoint[0],2) + TMath::Power(pushedPoint[2],2)) << endl;
            Info("TimeFromInsideAlongParabola","Propagated Point inside Left? %i",insideLeft);
         #endif
         if (!insideLeft) {
            #ifdef VERBOSE_MODE
               cout << "Final: " << finalTime << endl;
               Info("TimeFromInsideAlongParabola","Returning finalTime: %f",finalTime);
            #endif
            return finalTime;
         }
         #ifdef VERBOSE_MODE
            Info("TimeFromInsideAlongParabola","Still Inside Left therefore find time to exit left");
         #endif
         leftTime = dynamic_cast<TUCNGeoBBox*>(fLeft)->TimeFromInsideAlongParabola(leftPoint, leftVel, \
                                                                        leftField, stepTime, onBoundary);
         if (leftTime < TGeoShape::Tolerance()) {
            #ifdef VERBOSE_MODE
               cout << "Final: " << finalTime << endl;
               Info("TimeFromInsideAlongParabola","Returning finalTime: %f",finalTime);
            #endif
            return finalTime;
         }
         leftTime += (1.+rightTime)*TGeoShape::Tolerance();
         #ifdef VERBOSE_MODE
            cout << "Incrementing Left Time by (1.+rightTime)*TGeoShape::Tolerance(): " << leftTime << endl;
            Info("TimeFromInsideAlongParabola","Time to Exit Left? %f",leftTime);
         #endif
      }
   }
   #ifdef VERBOSE_MODE
      Info("TimeFromInsideAlongParabola","FinalTime: %f",finalTime);
   #endif      
   return finalTime;
}

//_____________________________________________________________________________
Double_t TUCNGeoUnion::TimeFromOutsideAlongParabola(const Double_t* point, const Double_t* velocity, const Double_t* field,
                                 const Double_t stepTime, const Bool_t onBoundary) const
{
// Compute the time from outside point to this composite shape along parabola.
   #ifdef VERBOSE_MODE
      Info("TimeFromOutsideAlongParabola","Start");
   #endif
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Double_t leftTime, rightTime, finalTime;
   // Perform coordinate transformations from Global to the local coords of ('+') & ('-')
   Double_t leftPoint[3], leftVel[3], leftField[3], rightPoint[3], rightVel[3], rightField[3];
   fLeftMat->MasterToLocal(point, &leftPoint[0]);
   fLeftMat->MasterToLocalVect(velocity, &leftVel[0]);
   fLeftMat->MasterToLocalVect(field, &leftField[0]);
   fRightMat->MasterToLocal(point, &rightPoint[0]);
   fRightMat->MasterToLocalVect(velocity, &rightVel[0]);
   fRightMat->MasterToLocalVect(field, &rightField[0]);
   // Calculate time from outside to both shapes
   leftTime = dynamic_cast<TUCNGeoBBox*>(fLeft)->TimeFromOutsideAlongParabola(leftPoint, leftVel, \
                                                               leftField, stepTime, onBoundary);
   rightTime = dynamic_cast<TUCNGeoBBox*>(fRight)->TimeFromOutsideAlongParabola(rightPoint, rightVel, \
                                                               rightField, stepTime, onBoundary);
   #ifdef VERBOSE_MODE
      Info("TimeFromOutsideAlongParabola","LeftTime: %f. RightTime: %f",leftTime,rightTime);
   #endif
   // Simply choose the shape that is hit first
   if (leftTime < rightTime) {
      finalTime = leftTime;
      node->SetSelected(1);
   } else {
      finalTime = rightTime;
      node->SetSelected(2);
   }
   #ifdef VERBOSE_MODE
      Info("TimeFromOutsideAlongParabola","FinalTime: %f",finalTime);
   #endif      
   return finalTime;
}

//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________

ClassImp(TUCNGeoSubtraction)

//_____________________________________________________________________________
void TUCNGeoSubtraction::Paint(Option_t *option)
{
// Paint method.
   TVirtualViewer3D *viewer = gPad->GetViewer3D();

   if (!viewer) {
      Error("Paint", "gPad->GetViewer3D() returned 0, cannot work with composite!\n");
      return;
   }

   viewer->AddCompositeOp(TBuffer3D::kCSDifference);

   TUCNGeoBoolNode::Paint(option);
}

//_____________________________________________________________________________
TUCNGeoSubtraction::TUCNGeoSubtraction()
{
// Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoSubtraction","Default Constructor");
   #endif
}

//_____________________________________________________________________________
TUCNGeoSubtraction::TUCNGeoSubtraction(const char *expr1, const char *expr2)
          :TUCNGeoBoolNode(expr1, expr2)
{
// Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoSubtraction","Constructor");
   #endif
}

//_____________________________________________________________________________
TUCNGeoSubtraction::TUCNGeoSubtraction(TGeoShape *left, TGeoShape *right, TGeoMatrix *lmat, TGeoMatrix *rmat)
                :TUCNGeoBoolNode(left,right,lmat,rmat)
{
// Constructor providing pointers to components
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoSubtraction","Constructor");
   #endif
   if (left->TestShapeBit(TGeoShape::kGeoHalfSpace)) {
      Fatal("TGeoSubstraction", "Substractions from a half-space (%s) not allowed", left->GetName());
   }
}

//_____________________________________________________________________________
TUCNGeoSubtraction::~TUCNGeoSubtraction()
{
// Destructor
// --- deletion of components handled by TGeoManager class.
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoSubtraction","Destructor");
   #endif
}

//_____________________________________________________________________________
void TUCNGeoSubtraction::ComputeBBox(Double_t &dx, Double_t &dy, Double_t &dz, Double_t *origin)
{
// Compute bounding box corresponding to a subtraction of two shapes.
   TGeoBBox *box = (TGeoBBox*)fLeft;
   if (box->IsNullBox()) fLeft->ComputeBBox();
   Double_t vert[24];
   Double_t pt[3];
   Int_t i;
   Double_t xmin, xmax, ymin, ymax, zmin, zmax;
   xmin = ymin = zmin = TGeoShape::Big();
   xmax = ymax = zmax = -TGeoShape::Big();
   box->SetBoxPoints(&vert[0]);
   for (i=0; i<8; i++) {
      fLeftMat->LocalToMaster(&vert[3*i], &pt[0]);
      if (pt[0]<xmin) xmin=pt[0];
      if (pt[0]>xmax) xmax=pt[0];
      if (pt[1]<ymin) ymin=pt[1];
      if (pt[1]>ymax) ymax=pt[1];
      if (pt[2]<zmin) zmin=pt[2];
      if (pt[2]>zmax) zmax=pt[2];
   }   
   dx = 0.5*(xmax-xmin);
   origin[0] = 0.5*(xmin+xmax);
   dy = 0.5*(ymax-ymin);
   origin[1] = 0.5*(ymin+ymax);
   dz = 0.5*(zmax-zmin);
   origin[2] = 0.5*(zmin+zmax);
}   

//_____________________________________________________________________________
void TUCNGeoSubtraction::ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm)
{
// Normal computation in POINT. The orientation is chosen so that DIR.dot.NORM>0.
   norm[0] = norm[1] = 0.;
   norm[2] = 1.;
   Double_t local[3], ldir[3], lnorm[3];
   if (fSelected == 1) {
      fLeftMat->MasterToLocal(point, local);
      fLeftMat->MasterToLocalVect(dir, ldir);
      fLeft->ComputeNormal(local,ldir,lnorm);
      fLeftMat->LocalToMasterVect(lnorm, norm);
      return;
   }
   if (fSelected == 2) {
      fRightMat->MasterToLocal(point, local);
      fRightMat->MasterToLocalVect(dir, ldir);
      fRight->ComputeNormal(local,ldir,lnorm);
      fRightMat->LocalToMasterVect(lnorm, norm);
      return;
   }
   fRightMat->MasterToLocal(point,local);
   if (fRight->Contains(local)) {
      fRightMat->MasterToLocalVect(dir,ldir);
      fRight->ComputeNormal(local,ldir, lnorm);
      fRightMat->LocalToMasterVect(lnorm,norm);
      return;
   }   
   fLeftMat->MasterToLocal(point,local);
   if (!fLeft->Contains(local)) {
      fLeftMat->MasterToLocalVect(dir,ldir);
      fLeft->ComputeNormal(local,ldir, lnorm);
      fLeftMat->LocalToMasterVect(lnorm,norm);
      return;
   }
   // point is inside left shape, but not inside the right
   local[0] = point[0]+1E-5*dir[0];
   local[1] = point[1]+1E-5*dir[1];
   local[2] = point[2]+1E-5*dir[2];
   if (Contains(local)) {
      local[0] = point[0]-1E-5*dir[0];
      local[1] = point[1]-1E-5*dir[1];
      local[2] = point[2]-1E-5*dir[2];
      if (Contains(local)) return;
   }  
   ComputeNormal(local,dir,norm);
}

//_____________________________________________________________________________
Bool_t TUCNGeoSubtraction::Contains(Double_t *point) const
{
// Find if a subtraction of two shapes contains a given point
   Double_t local[3];
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   fLeftMat->MasterToLocal(point, &local[0]);
   Bool_t inside = fLeft->Contains(&local[0]);
   if (inside) node->SetSelected(1);
   else return kFALSE;
   fRightMat->MasterToLocal(point, &local[0]);
   inside = !fRight->Contains(&local[0]);
   if (!inside) node->SetSelected(2);
   return inside;
}
//_____________________________________________________________________________
Int_t TUCNGeoSubtraction::DistanceToPrimitive(Int_t /*px*/, Int_t /*py*/)
{
// Compute minimum distance to shape vertices
   return 9999;
}
//_____________________________________________________________________________
Double_t TUCNGeoSubtraction::DistFromInside(Double_t *point, Double_t *dir, Int_t iact,
                              Double_t step, Double_t *safe) const
{
// Compute distance from a given point inside to the shape boundary.
   if (iact<3 && safe) {
      // compute safe distance
      *safe = Safety(point,kTRUE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Double_t local[3], ldir[3], rdir[3];
   Double_t d1, d2, snxt=0.;
   fLeftMat->MasterToLocal(point, &local[0]);
   fLeftMat->MasterToLocalVect(dir, &ldir[0]);
   fRightMat->MasterToLocalVect(dir, &rdir[0]);
   d1 = fLeft->DistFromInside(&local[0], &ldir[0], iact, step, safe);
   fRightMat->MasterToLocal(point, &local[0]);
   d2 = fRight->DistFromOutside(&local[0], &rdir[0], iact, step, safe);
   if (d1<d2) {
      snxt = d1;
      node->SetSelected(1);
   } else {
      snxt = d2;
      node->SetSelected(2);
   }      
   return snxt;
}   
//_____________________________________________________________________________
Double_t TUCNGeoSubtraction::DistFromOutside(Double_t *point, Double_t *dir, Int_t iact,
                              Double_t step, Double_t *safe) const
{
// Compute distance from a given point outside to the shape.
   if (iact<3 && safe) {
      // compute safe distance
      *safe = Safety(point,kFALSE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Double_t local[3], master[3], ldir[3], rdir[3];
   memcpy(&master[0], point, 3*sizeof(Double_t));
   Int_t i;
   Double_t d1, d2, snxt=0.;
   fRightMat->MasterToLocal(point, &local[0]);
   fLeftMat->MasterToLocalVect(dir, &ldir[0]);
   fRightMat->MasterToLocalVect(dir, &rdir[0]);
   // check if inside '-'
   Bool_t inside = fRight->Contains(&local[0]);
   Double_t epsil = 0.;
   while (1) {
      if (inside) {
         // propagate to outside of '-'
         node->SetSelected(2);
         d1 = fRight->DistFromInside(&local[0], &rdir[0], iact, step, safe);
         snxt += d1+epsil;
         for (i=0; i<3; i++) master[i] += (d1+1E-8)*dir[i];
         epsil = 1.E-8;
         // now master outside '-'; check if inside '+'
         fLeftMat->MasterToLocal(&master[0], &local[0]);
         if (fLeft->Contains(&local[0])) return snxt;
      } 
      // master outside '-' and outside '+' ;  find distances to both
      node->SetSelected(1);
      fLeftMat->MasterToLocal(&master[0], &local[0]);
      d2 = fLeft->DistFromOutside(&local[0], &ldir[0], iact, step, safe);
      if (d2>1E20) return TGeoShape::Big();
      
      fRightMat->MasterToLocal(&master[0], &local[0]);
      d1 = fRight->DistFromOutside(&local[0], &rdir[0], iact, step, safe);
      if (d2<d1-TGeoShape::Tolerance()) {
         snxt += d2+epsil;
         return snxt;
      }   
      // propagate to '-'
      snxt += d1+epsil;
      for (i=0; i<3; i++) master[i] += (d1+1E-8)*dir[i];
      epsil = 1.E-8;
      // now inside '-' and not inside '+'
      fRightMat->MasterToLocal(&master[0], &local[0]);
      inside = kTRUE;
   }
}
//_____________________________________________________________________________
Int_t TUCNGeoSubtraction::GetNpoints() const
{
// Returns number of vertices for the composite shape described by this subtraction.
   return 0;
}
//_____________________________________________________________________________
Double_t TUCNGeoSubtraction::Safety(Double_t *point, Bool_t in) const
{
// Compute safety distance for a union node;
   Double_t local1[3], local2[3];
   fLeftMat->MasterToLocal(point,local1);
   Bool_t in1 = fLeft->Contains(local1);
   fRightMat->MasterToLocal(point,local2);
   Bool_t in2 = fRight->Contains(local2);
   Bool_t intrue = in1 && (!in2);
   if (in^intrue) return 0.0;
   Double_t saf1 = fLeft->Safety(local1, in1);
   Double_t saf2 = fRight->Safety(local2, in2);
   if (in1 && in2) return saf2;
   if (in1)        return TMath::Min(saf1,saf2);
   if (in2)        return TMath::Max(saf1,saf2);
   return saf1;
}   
//_____________________________________________________________________________
void TUCNGeoSubtraction::SavePrimitive(ostream &out, Option_t *option /*= ""*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   TUCNGeoBoolNode::SavePrimitive(out,option);
   out << "   pBoolNode = new TUCNGeoSubtraction(";
   out << fLeft->GetPointerName() << ",";
   out << fRight->GetPointerName() << ",";
   if (!fLeftMat->IsIdentity()) out << fLeftMat->GetPointerName() << ",";
   else                         out << "0,";
   if (!fRightMat->IsIdentity()) out << fRightMat->GetPointerName() << ");" << endl;
   else                         out << "0);" << endl;
}   
//_____________________________________________________________________________
void TUCNGeoSubtraction::SetPoints(Double_t * /*points*/) const
{
// Fill buffer with shape vertices.
}
//_____________________________________________________________________________
void TUCNGeoSubtraction::SetPoints(Float_t * /*points*/) const
{
// Fill buffer with shape vertices.
}
//_____________________________________________________________________________
void TUCNGeoSubtraction::Sizeof3D() const
{
// Register 3D size of this shape.
   TUCNGeoBoolNode::Sizeof3D();
}

//_____________________________________________________________________________
Double_t TUCNGeoSubtraction::TimeFromInsideAlongParabola(const Double_t* point, const Double_t* velocity, const Double_t*
                                       field, const Double_t stepTime, const Bool_t onBoundary) const
{
// Compute time from inside point to outside of this composite shape along parabola.
   #ifdef VERBOSE_MODE
      Info("TimeFromInsideAlongParabola","Start");
   #endif
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Double_t leftTime, rightTime, finalTime=0.;
   // Perform coordinate transformations from Global to the local coords of ('+') & ('-')
   Double_t leftPoint[3], leftVel[3], leftField[3], rightPoint[3], rightVel[3], rightField[3];
   fLeftMat->MasterToLocal(point, &leftPoint[0]);
   fLeftMat->MasterToLocalVect(velocity, &leftVel[0]);
   fLeftMat->MasterToLocalVect(field, &leftField[0]);
   fRightMat->MasterToLocal(point, &rightPoint[0]);
   fRightMat->MasterToLocalVect(velocity, &rightVel[0]);
   fRightMat->MasterToLocalVect(field, &rightField[0]);
   // We are inside ('+') and therefore should'nt be inside ('-') since this is a subtraction volume
   // Therefore first we want to find the time to exit ('+') from Inside
   leftTime = dynamic_cast<TUCNGeoBBox*>(fLeft)->TimeFromInsideAlongParabola(&leftPoint[0], &leftVel[0], &leftField[0], \
                                                                              stepTime, onBoundary);
   // Now we want to find the time to the boundary of the ('-') from Outside
   rightTime= dynamic_cast<TUCNGeoBBox*>(fRight)->TimeFromOutsideAlongParabola(&rightPoint[0], &rightVel[0], &rightField[0], \
                                                                                 stepTime, onBoundary);
   #ifdef VERBOSE_MODE
      Info("TimeFromInsideAlongParabola","LeftTime: %f, RightTime: %f",leftTime,rightTime);
   #endif
   // Compare the two times for which boundary will be intersected first
   if (leftTime < rightTime) {
      finalTime = leftTime;
      node->SetSelected(1);
   } else {
      finalTime = rightTime;
      node->SetSelected(2);
   }
   #ifdef VERBOSE_MODE
      Info("TimeFromInsideAlongParabola","FinalTime: %f", finalTime);
   #endif
   return finalTime;
}

//_____________________________________________________________________________
Double_t TUCNGeoSubtraction::TimeFromOutsideAlongParabola(const Double_t* point, const Double_t* velocity, const Double_t*
                                       field, const Double_t stepTime, const Bool_t onBoundary) const
{
// Compute the time from outside point to this composite shape along parabola.
   #ifdef VERBOSE_MODE
		Info("TimeFromOutsideAlongParabola","Start");
   #endif
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Int_t i;
   Double_t leftTime, rightTime, finalTime=0.;
   // Create some temporary storage for the master point and velocity
   Double_t masterPoint[3], masterVel[3], masterField[3]; 
   memcpy(&masterPoint[0], point, 3*sizeof(Double_t));
   memcpy(&masterVel[0], velocity, 3*sizeof(Double_t));
   memcpy(&masterField[0], field, 3*sizeof(Double_t));
   // Perform coordinate transformations from Global to the local coords of '+' & '-'
   Double_t leftPoint[3], leftVel[3], leftField[3], rightPoint[3], rightVel[3], rightField[3];
   fLeftMat->MasterToLocal(point, &leftPoint[0]);
   fLeftMat->MasterToLocalVect(velocity, &leftVel[0]);
   fLeftMat->MasterToLocalVect(field, &leftField[0]);
   fRightMat->MasterToLocal(point, &rightPoint[0]);
   fRightMat->MasterToLocalVect(velocity, &rightVel[0]);
   fRightMat->MasterToLocalVect(field, &rightField[0]);
   // If we are coming from outside the subtraction then we should definitely be outside '+'
   // However we could be inside ('-') so check for this
   Bool_t inside = fRight->Contains(&rightPoint[0]);
   Double_t epsil = 0.;
   while (1) {
      #ifdef VERBOSE_MODE
         Info("TimeFromOutsideAlongParabola","Currently Outside Left. Are we Inside Right? %i",inside);
      #endif
      if (inside) {
         // We are inside '-' so propagate outside of this
         node->SetSelected(2);
         rightTime = dynamic_cast<TUCNGeoBBox*>(fRight)->TimeFromInsideAlongParabola(&rightPoint[0], &rightVel[0], \
                                                                              &rightField[0], stepTime, onBoundary);
         #ifdef VERBOSE_MODE
            Info("TimeFromOutsideAlongParabola","Currently Inside Right. Propagating outside Right. RightTime: %f",rightTime);
         #endif
         finalTime += rightTime+epsil;
         for (i=0; i<3; i++) {
            masterPoint[i] += masterVel[i]*(rightTime+1E-8) + 0.5*masterField[i]*(rightTime+1E-8)*(rightTime+1E-8);
            masterVel[i] += masterField[i]*(rightTime+1E-8);
         }
         epsil = 1.E-8;
         // now masterPoint outside '-'; check if inside '+'
         fLeftMat->MasterToLocal(&masterPoint[0], &leftPoint[0]);
         fLeftMat->MasterToLocalVect(&masterVel[0], &leftVel[0]);
         if (fLeft->Contains(&leftPoint[0])) {
            #ifdef VERBOSE_MODE
               Info("TimeFromOutsideAlongParabola","We are inside Left. Returning finalTime: %f",finalTime);
            #endif
            return finalTime;
         }
      }
      // masterPoint is outside '-' and outside '+' ;  find times to both
      #ifdef VERBOSE_MODE
         Info("TimeFromOutsideAlongParabola","Currently outside both Left and Right.");
      #endif
      node->SetSelected(1);
      fLeftMat->MasterToLocal(&masterPoint[0], &leftPoint[0]);
      fLeftMat->MasterToLocalVect(&masterVel[0], &leftVel[0]);
      leftTime = dynamic_cast<TUCNGeoBBox*>(fLeft)->TimeFromOutsideAlongParabola(&leftPoint[0], &leftVel[0], \
                                                                           &leftField[0], stepTime, onBoundary);
      if (leftTime > 1E20) return TGeoShape::Big();
      
      fRightMat->MasterToLocal(&masterPoint[0], &rightPoint[0]);
      fRightMat->MasterToLocalVect(&masterVel[0], &rightVel[0]);
      rightTime = dynamic_cast<TUCNGeoBBox*>(fRight)->TimeFromOutsideAlongParabola(&rightPoint[0], &rightVel[0], \
                                                                              &rightField[0], stepTime, onBoundary);
      #ifdef VERBOSE_MODE
         Info("TimeFromOutsideAlongParabola","LeftTime: %f. RightTime: %f",leftTime, rightTime);
      #endif
      // If we reach '+' first then return this time
      if (leftTime < rightTime-TGeoShape::Tolerance()) {
         finalTime += leftTime+epsil;
         #ifdef VERBOSE_MODE
            Info("TimeFromOutsideAlongParabola","Propagating to Left. Returning finalTime: %f",finalTime);
         #endif
         return finalTime;
      }
      // Otherwise propagate to '-' and start the loop again
      finalTime += rightTime+epsil;
      for (i=0; i<3; i++) {
         masterPoint[i] += masterVel[i]*(rightTime+1E-8) + 0.5*masterField[i]*(rightTime+1E-8)*(rightTime+1E-8);
         masterVel[i] += masterField[i]*(rightTime+1E-8);
      }
      epsil = 1.E-8;
      // now inside '-' and not inside '+'
      fRightMat->MasterToLocal(&masterPoint[0], &rightPoint[0]);
      fRightMat->MasterToLocalVect(&masterVel[0], &rightVel[0]);
      inside = kTRUE;
      #ifdef VERBOSE_MODE
         Info("TimeFromOutsideAlongParabola","Propagated to Right. finalTime: %f", finalTime);
      #endif
   }
}

//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________

ClassImp(TUCNGeoIntersection)

//_____________________________________________________________________________
void TUCNGeoIntersection::Paint(Option_t *option)
{
// Paint method.
   TVirtualViewer3D *viewer = gPad->GetViewer3D();

   if (!viewer) {
      Error("Paint", "gPad->GetViewer3D() returned 0, cannot work with composite!\n");
      return;
   }

   viewer->AddCompositeOp(TBuffer3D::kCSIntersection);

   TUCNGeoBoolNode::Paint(option);
}

//_____________________________________________________________________________
TUCNGeoIntersection::TUCNGeoIntersection()
{
// Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoIntersection","Default Constructor");
   #endif
}

//_____________________________________________________________________________
TUCNGeoIntersection::TUCNGeoIntersection(const char *expr1, const char *expr2)
          :TUCNGeoBoolNode(expr1, expr2)
{
// Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoIntersection","Constructor");
   #endif
}

//_____________________________________________________________________________
TUCNGeoIntersection::TUCNGeoIntersection(TGeoShape *left, TGeoShape *right, TGeoMatrix *lmat, TGeoMatrix *rmat)
                 :TUCNGeoBoolNode(left,right,lmat,rmat)
{
// Constructor providing pointers to components
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoIntersection","Constructor");
   #endif
   Bool_t hs1 = (fLeft->TestShapeBit(TGeoShape::kGeoHalfSpace))?kTRUE:kFALSE;
   Bool_t hs2 = (fRight->TestShapeBit(TGeoShape::kGeoHalfSpace))?kTRUE:kFALSE;
   if (hs1 && hs2) Fatal("ctor", "cannot intersect two half-spaces: %s * %s", left->GetName(), right->GetName());
}

//_____________________________________________________________________________
TUCNGeoIntersection::~TUCNGeoIntersection()
{
// Destructor
// --- deletion of components handled by TGeoManager class.
   #ifdef PRINT_CONSTRUCTORS
      Info("TUCNGeoIntersection","Destructor");
   #endif
}

//_____________________________________________________________________________
void TUCNGeoIntersection::ComputeBBox(Double_t &dx, Double_t &dy, Double_t &dz, Double_t *origin)
{
// Compute bounding box corresponding to a intersection of two shapes.
   Bool_t hs1 = (fLeft->TestShapeBit(TGeoShape::kGeoHalfSpace))?kTRUE:kFALSE;
   Bool_t hs2 = (fRight->TestShapeBit(TGeoShape::kGeoHalfSpace))?kTRUE:kFALSE;
   Double_t vert[48];
   Double_t pt[3];
   Int_t i;
   Double_t xmin1, xmax1, ymin1, ymax1, zmin1, zmax1;
   Double_t xmin2, xmax2, ymin2, ymax2, zmin2, zmax2;
   xmin1 = ymin1 = zmin1 = xmin2 = ymin2 = zmin2 = TGeoShape::Big();
   xmax1 = ymax1 = zmax1 = xmax2 = ymax2 = zmax2 =  -TGeoShape::Big();
   if (!hs1) {
      if (((TGeoBBox*)fLeft)->IsNullBox()) fLeft->ComputeBBox();
      ((TGeoBBox*)fLeft)->SetBoxPoints(&vert[0]);
      for (i=0; i<8; i++) {
         fLeftMat->LocalToMaster(&vert[3*i], &pt[0]);
         if (pt[0]<xmin1) xmin1=pt[0];
         if (pt[0]>xmax1) xmax1=pt[0];
         if (pt[1]<ymin1) ymin1=pt[1];
         if (pt[1]>ymax1) ymax1=pt[1];
         if (pt[2]<zmin1) zmin1=pt[2];
         if (pt[2]>zmax1) zmax1=pt[2];
      }   
   }   
   if (!hs2) {
      if (((TGeoBBox*)fRight)->IsNullBox()) fRight->ComputeBBox();
      ((TGeoBBox*)fRight)->SetBoxPoints(&vert[24]);
      for (i=8; i<16; i++) {
         fRightMat->LocalToMaster(&vert[3*i], &pt[0]);
         if (pt[0]<xmin2) xmin2=pt[0];
         if (pt[0]>xmax2) xmax2=pt[0];
         if (pt[1]<ymin2) ymin2=pt[1];
         if (pt[1]>ymax2) ymax2=pt[1];
         if (pt[2]<zmin2) zmin2=pt[2];
         if (pt[2]>zmax2) zmax2=pt[2];
      }
   }      
   if (hs1) {
      dx = 0.5*(xmax2-xmin2);
      origin[0] = 0.5*(xmax2+xmin2);   
      dy = 0.5*(ymax2-ymin2);
      origin[1] = 0.5*(ymax2+ymin2);   
      dz = 0.5*(zmax2-zmin2);
      origin[2] = 0.5*(zmax2+zmin2);   
      return;
   }            
   if (hs2) {
      dx = 0.5*(xmax1-xmin1);
      origin[0] = 0.5*(xmax1+xmin1);   
      dy = 0.5*(ymax1-ymin1);
      origin[1] = 0.5*(ymax1+ymin1);   
      dz = 0.5*(zmax1-zmin1);
      origin[2] = 0.5*(zmax1+zmin1);   
      return;
   }   
   Double_t sort[4];
   Int_t isort[4];
   sort[0] = xmin1;
   sort[1] = xmax1;
   sort[2] = xmin2;
   sort[3] = xmax2;
   TMath::Sort(4, &sort[0], &isort[0], kFALSE);
   if (isort[1]%2) {
      Warning("ComputeBBox", "shapes %s and %s do not intersect", fLeft->GetName(), fRight->GetName());
      dx = dy = dz = 0;
      memset(origin, 0, 3*sizeof(Double_t));
      return;
   }
   dx = 0.5*(sort[isort[2]]-sort[isort[1]]);
   origin[0] = 0.5*(sort[isort[1]]+sort[isort[2]]);   
   sort[0] = ymin1;
   sort[1] = ymax1;
   sort[2] = ymin2;
   sort[3] = ymax2;
   TMath::Sort(4, &sort[0], &isort[0], kFALSE);
   if (isort[1]%2) {
      Warning("ComputeBBox", "shapes %s and %s do not intersect", fLeft->GetName(), fRight->GetName());
      dx = dy = dz = 0;
      memset(origin, 0, 3*sizeof(Double_t));
      return;
   }
   dy = 0.5*(sort[isort[2]]-sort[isort[1]]);
   origin[1] = 0.5*(sort[isort[1]]+sort[isort[2]]);   
   sort[0] = zmin1;
   sort[1] = zmax1;
   sort[2] = zmin2;
   sort[3] = zmax2;
   TMath::Sort(4, &sort[0], &isort[0], kFALSE);
   if (isort[1]%2) {
      Warning("ComputeBBox", "shapes %s and %s do not intersect", fLeft->GetName(), fRight->GetName());
      dx = dy = dz = 0;
      memset(origin, 0, 3*sizeof(Double_t));
      return;
   }
   dz = 0.5*(sort[isort[2]]-sort[isort[1]]);
   origin[2] = 0.5*(sort[isort[1]]+sort[isort[2]]);   
}   

//_____________________________________________________________________________
void TUCNGeoIntersection::ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm)
{
// Normal computation in POINT. The orientation is chosen so that DIR.dot.NORM>0.
   Double_t local[3], ldir[3], lnorm[3];
   norm[0] = norm[1] = 0.;
   norm[2] = 1.;
   if (fSelected == 1) {
      fLeftMat->MasterToLocal(point, local);
      fLeftMat->MasterToLocalVect(dir, ldir);
      fLeft->ComputeNormal(local,ldir,lnorm);
      fLeftMat->LocalToMasterVect(lnorm, norm);
      return;
   }
   if (fSelected == 2) {
      fRightMat->MasterToLocal(point, local);
      fRightMat->MasterToLocalVect(dir, ldir);
      fRight->ComputeNormal(local,ldir,lnorm);
      fRightMat->LocalToMasterVect(lnorm, norm);
      return;
   }            
   fLeftMat->MasterToLocal(point,local);
   if (!fLeft->Contains(local)) {
      fLeftMat->MasterToLocalVect(dir,ldir);
      fLeft->ComputeNormal(local,ldir,lnorm);
      fLeftMat->LocalToMasterVect(lnorm,norm);
      return;
   }
   fRightMat->MasterToLocal(point,local);
   if (!fRight->Contains(local)) {
      fRightMat->MasterToLocalVect(dir,ldir);
      fRight->ComputeNormal(local,ldir,lnorm);
      fRightMat->LocalToMasterVect(lnorm,norm);
      return;
   }
   // point is inside intersection.
   local[0] = point[0] + 1E-5*dir[0];   
   local[1] = point[1] + 1E-5*dir[1];   
   local[2] = point[2] + 1E-5*dir[2];
   if (Contains(local)) {
      local[0] = point[0] - 1E-5*dir[0];   
      local[1] = point[1] - 1E-5*dir[1];   
      local[2] = point[2] - 1E-5*dir[2];
      if (Contains(local)) return;
   }
   ComputeNormal(local,dir,norm);   
}

//_____________________________________________________________________________
Bool_t TUCNGeoIntersection::Contains(Double_t *point) const
{
// Find if a intersection of two shapes contains a given point
   Double_t local[3];
   fLeftMat->MasterToLocal(point, &local[0]);
   Bool_t inside = fLeft->Contains(&local[0]);
   if (!inside) return kFALSE;
   fRightMat->MasterToLocal(point, &local[0]);
   inside = fRight->Contains(&local[0]);
   return inside;
}

//_____________________________________________________________________________
Int_t TUCNGeoIntersection::DistanceToPrimitive(Int_t /*px*/, Int_t /*py*/)
{
// Compute minimum distance to shape vertices
   return 9999;
}

//_____________________________________________________________________________
Double_t TUCNGeoIntersection::DistFromInside(Double_t *point, Double_t *dir, Int_t iact,
                              Double_t step, Double_t *safe) const
{
// Compute distance from a given point inside to the shape boundary.
   if (iact<3 && safe) {
      // compute safe distance
      *safe = Safety(point,kTRUE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Double_t local[3], ldir[3], rdir[3];
   Double_t d1, d2, snxt=0.;
   fLeftMat->MasterToLocal(point, &local[0]);
   fLeftMat->MasterToLocalVect(dir, &ldir[0]);
   fRightMat->MasterToLocalVect(dir, &rdir[0]);
   d1 = fLeft->DistFromInside(&local[0], &ldir[0], iact, step, safe);
   fRightMat->MasterToLocal(point, &local[0]);
   d2 = fRight->DistFromInside(&local[0], &rdir[0], iact, step, safe);
   if (d1<d2) {
      snxt = d1;
      node->SetSelected(1);
   } else {
      snxt = d2;
      node->SetSelected(2);
   }      
   return snxt;
}   

//_____________________________________________________________________________
Double_t TUCNGeoIntersection::DistFromOutside(Double_t *point, Double_t *dir, Int_t iact,
                              Double_t step, Double_t *safe) const
{
// Compute distance from a given point outside to the shape.
   if (iact<3 && safe) {
      // compute safe distance
      *safe = Safety(point,kFALSE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Double_t lpt[3], rpt[3], master[3], ldir[3], rdir[3];
   memcpy(master, point, 3*sizeof(Double_t));
   Int_t i;
   Double_t d1 = 0.;
   Double_t d2 = 0.;
   fLeftMat->MasterToLocal(point, lpt);
   fRightMat->MasterToLocal(point, rpt);
   fLeftMat->MasterToLocalVect(dir, ldir);
   fRightMat->MasterToLocalVect(dir, rdir);
   Bool_t inleft = fLeft->Contains(lpt);
   Bool_t inright = fRight->Contains(rpt);
   node->SetSelected(0);
   Double_t snext = 0.0;
   if (inleft && inright) return snext;

   while (1) {
      d1 = d2 = 0.0;
      if (!inleft)  {
         d1 = fLeft->DistFromOutside(lpt,ldir,3);
         if (d1>1E20) return TGeoShape::Big();
      }
      if (!inright) {  
         d2 = fRight->DistFromOutside(rpt,rdir,3);
         if (d2>1E20) return TGeoShape::Big();
      }
   
      if (d1>d2) {
         // propagate to left shape
         snext += d1;
         node->SetSelected(1);
         inleft = kTRUE;
         for (i=0; i<3; i++) master[i] += d1*dir[i];
         fRightMat->MasterToLocal(master,rpt);
         // check if propagated point is inside right shape
         inright = fRight->Contains(rpt);
         if (inright) return snext;
         // here inleft=true, inright=false         
      } else {
         // propagate to right shape
         snext += d2;
         node->SetSelected(2);
         inright = kTRUE;
         for (i=0; i<3; i++) master[i] += d2*dir[i];
         fLeftMat->MasterToLocal(master,lpt);
         // check if propagated point is inside left shape
         inleft = fLeft->Contains(lpt);
         if (inleft) return snext;
         // here inleft=false, inright=true
      }            
   }   
   return snext;
}      

//_____________________________________________________________________________
Int_t TUCNGeoIntersection::GetNpoints() const
{
// Returns number of vertices for the composite shape described by this intersection.
   return 0;
}

//_____________________________________________________________________________
Double_t TUCNGeoIntersection::Safety(Double_t *point, Bool_t in) const
{
// Compute safety distance for a union node;
   Double_t local1[3], local2[3];
   fLeftMat->MasterToLocal(point,local1);
   Bool_t in1 = fLeft->Contains(local1);
   fRightMat->MasterToLocal(point,local2);
   Bool_t in2 = fRight->Contains(local2);
   Bool_t intrue = in1 & in2;
   if (in^intrue) return 0.0;
   Double_t saf1 = fLeft->Safety(local1, in1);
   Double_t saf2 = fRight->Safety(local2, in2);
   if (in1 && in2) return TMath::Min(saf1, saf2);
   if (in1)        return saf2;
   if (in2)        return saf1;
   return TMath::Max(saf1,saf2);
}   

//_____________________________________________________________________________
void TUCNGeoIntersection::SavePrimitive(ostream &out, Option_t *option /*= ""*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   TUCNGeoBoolNode::SavePrimitive(out,option);
   out << "   pBoolNode = new TUCNGeoIntersection(";
   out << fLeft->GetPointerName() << ",";
   out << fRight->GetPointerName() << ",";
   if (!fLeftMat->IsIdentity()) out << fLeftMat->GetPointerName() << ",";
   else                         out << "0,";
   if (!fRightMat->IsIdentity()) out << fRightMat->GetPointerName() << ");" << endl;
   else                         out << "0);" << endl;
}   

//_____________________________________________________________________________
void TUCNGeoIntersection::SetPoints(Double_t * /*points*/) const
{
// Fill buffer with shape vertices.
}

//_____________________________________________________________________________
void TUCNGeoIntersection::SetPoints(Float_t * /*points*/) const
{
// Fill buffer with shape vertices.
}

//_____________________________________________________________________________
void TUCNGeoIntersection::Sizeof3D() const
{
// Register 3D size of this shape.
   TUCNGeoBoolNode::Sizeof3D();
}

//_____________________________________________________________________________
Double_t TUCNGeoIntersection::TimeFromOutsideAlongParabola(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t stepTime, const Bool_t onBoundary) const
{
// Compute the time from outside point to this composite shape along parabola.
// Check if the bounding box is crossed within the requested distance
	#ifdef VERBOSE_MODE
		Info("TimeFromOutsideAlongParabola","Start");
   #endif
	TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Double_t leftPoint[3], rightPoint[3], leftVelocity[3], rightVelocity[3], leftField[3], rightField[3];
   // Store the initial position in masterPoint for later use
   Double_t masterPoint[3], masterVel[3], masterField[3];
	memcpy(masterPoint, point, 3*sizeof(Double_t));
   memcpy(masterVel, velocity, 3*sizeof(Double_t));
	memcpy(masterField, field, 3*sizeof(Double_t));
	Int_t i;
   // Define the local coordinates for the left and right shapes
	Double_t leftTime = 0.;
   Double_t rightTime = 0.;
   fLeftMat->MasterToLocal(point, leftPoint);
   fRightMat->MasterToLocal(point, rightPoint);
   fLeftMat->MasterToLocalVect(velocity, leftVelocity);
   fRightMat->MasterToLocalVect(velocity, rightVelocity);
   fLeftMat->MasterToLocalVect(field, leftField);
   fRightMat->MasterToLocalVect(field, rightField);
	// Determine which shape (if either) contains the point
	Bool_t inleft = fLeft->Contains(leftPoint);
   Bool_t inright = fRight->Contains(rightPoint);
   #ifdef VERBOSE_MODE
		Info("TimeFromOutsideAlongParabola","Is Point in %s Node? : ",fLeft->GetName(),inleft);
   	Info("TimeFromOutsideAlongParabola","Is Point in %s Node? : ",fRight->GetName(),inright);	
	#endif
	node->SetSelected(0);
   Double_t timeFromInside = 0.0;
   if (inleft && inright) {
	   #ifdef VERBOSE_MODE
			Info("TimeFromOutsideAlongParabola","Particle in Both. Returning: %f",timeFromInside);
		#endif
		return timeFromInside;
	}
   while (1) {
      leftTime = rightTime = 0.0;
      if (!inleft)  {
		  	#ifdef VERBOSE_MODE
				Info("TimeFromOutsideAlongParabola","Point is not in Left node: %s. Finding Time FromOutside.",fLeft->GetName());
			#endif
			leftTime = static_cast<TUCNGeoBBox*>(fLeft)->TimeFromOutsideAlongParabola(leftPoint, leftVelocity, leftField, \
			                                                                           stepTime, onBoundary);
         if (leftTime > 1E20) return TGeoShape::Big();
      }
      if (!inright) {  
         #ifdef VERBOSE_MODE
				Info("TimeFromOutsideAlongParabola","Point is not in Right node: %s. Finding Time FromOutside.",fRight->GetName());
			#endif
			rightTime = static_cast<TUCNGeoBBox*>(fRight)->TimeFromOutsideAlongParabola(rightPoint, rightVelocity, rightField, \
			                                                                              stepTime, onBoundary);
         if (rightTime > 1E20) return TGeoShape::Big();
      }

      if (leftTime > rightTime) {
         // propagate to left shape
         timeFromInside += leftTime;
         #ifdef VERBOSE_MODE
				Info("TimeFromOutsideAlongParabola","LeftTime: %f > RightTime: %f.",leftTime, rightTime);
	         Info("TimeFromOutsideAlongParabola","TimeFromInside: %f.",timeFromInside);
			#endif
			node->SetSelected(1);
         inleft = kTRUE;
         for (i=0; i<3; i++) {
            // moving under gravity
            masterPoint[i] += masterVel[i]*leftTime + 0.5*masterField[i]*leftTime*leftTime;
            masterVel[i] += masterField[i]*leftTime;
         }
         fRightMat->MasterToLocal(&masterPoint[0],&rightPoint[0]);
         fRightMat->MasterToLocalVect(&masterVel[0],&rightVelocity[0]);
         // check if propagated point is inside right shape
         inright = fRight->Contains(rightPoint);
         #ifdef VERBOSE_MODE
				Info("TimeFromOutsideAlongParabola","Propagate point by LeftTime: X: %f, Y: %f, Z: %f", masterPoint[0], masterPoint[1], masterPoint[2]);
				Info("TimeFromOutsideAlongParabola","Is new point in Right? : %i",inright);
			#endif
			if (inright) {
				#ifdef VERBOSE_MODE
					Info("TimeFromOutsideAlongParabola","Returning TimeFromInside: %f.",timeFromInside);
				#endif
				return timeFromInside;
			}// here inleft=true, inright=false         
      } else {
         // propagate to right shape
         timeFromInside += rightTime;
         #ifdef VERBOSE_MODE
				Info("TimeFromOutsideAlongParabola","RightTime: %f > LeftTime: %f.",rightTime, leftTime);
				Info("TimeFromOutsideAlongParabola","TimeFromInside: %f.",timeFromInside);
         #endif
			node->SetSelected(2);
         inright = kTRUE;
         for (i=0; i<3; i++) {
            // moving under gravity
            masterPoint[i] += masterVel[i]*rightTime + 0.5*masterField[i]*rightTime*rightTime;
            masterVel[i] += masterField[i]*rightTime;
         }
		  	fLeftMat->MasterToLocal(&masterPoint[0],&leftPoint[0]);
         fLeftMat->MasterToLocalVect(&masterVel[0],&leftVelocity[0]);
         // check if propagated point is inside left shape
         inleft = fLeft->Contains(leftPoint);
         #ifdef VERBOSE_MODE
				Info("TimeFromOutsideAlongParabola","Propagate point by LeftTime: X: %f, Y: %f, Z: %f", masterPoint[0], masterPoint[1], masterPoint[2]);
         	Info("TimeFromOutsideAlongParabola","Is new point in Left? : %i",inleft);
         #endif
			if (inleft) {
				#ifdef VERBOSE_MODE
					Info("TimeFromOutsideAlongParabola","Returning TimeFromInside: %f.",timeFromInside);
	         #endif
				return timeFromInside;
         }
			// here inleft=false, inright=true
      }            
   }   
   return timeFromInside;
}   

//_____________________________________________________________________________
Double_t TUCNGeoIntersection::TimeFromInsideAlongParabola(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t stepTime, const Bool_t onBoundary) const
{
// -- Compute time from inside point to outside of this composite shape along parabola.
	#ifdef VERBOSE_MODE
		Info("TimeFromInsideAlongParabola","Start");
	#endif
	// Compute the time to each of the shapes involved in the intersection
   TUCNGeoBoolNode *node = (TUCNGeoBoolNode*)this;
   Double_t leftPoint[3], rightPoint[3], leftVelocity[3], rightVelocity[3], leftField[3], rightField[3];
   Double_t leftTime, rightTime, timeFromInside=0.;
   // Calculate the time to the boundary of the shape on the left branch
   fLeftMat->MasterToLocal(point, &leftPoint[0]);
   fLeftMat->MasterToLocalVect(velocity, &leftVelocity[0]);
   fLeftMat->MasterToLocalVect(field, &leftField[0]);
	leftTime = static_cast<TUCNGeoBBox*>(fLeft)->TimeFromInsideAlongParabola(&leftPoint[0], &leftVelocity[0], &leftField[0], stepTime, onBoundary);
   #ifdef VERBOSE_MODE
		Info("TimeFromInsideAlongParabola","Finding Time from Inside to Left Node, %s.",fLeft->GetName());
   	Info("TimeFromInsideAlongParabola","Time from Inside to Left Node, %s: %f",fLeft->GetName(),leftTime);
	#endif
	if (leftTime <= 0.0) {
		Error("TimeFromInsideAlongParabola", "No boundary of left Node was hit from inside");
		return 0.0;
	}
	// Calculate the time to the boundary of the shape on the right branch
   fRightMat->MasterToLocal(point, &rightPoint[0]);
   fRightMat->MasterToLocalVect(velocity, &rightVelocity[0]);
   fRightMat->MasterToLocalVect(field, &rightField[0]);
   rightTime = static_cast<TUCNGeoBBox*>(fRight)->TimeFromInsideAlongParabola(&rightPoint[0], &rightVelocity[0], &rightField[0], stepTime, onBoundary);
   #ifdef VERBOSE_MODE
		Info("TimeFromInsideAlongParabola","Finding Time from Inside to Right Node, %s.",fRight->GetName());
	   Info("TimeFromInsideAlongParabola","Time from Inside to Right Node, %s: %f",fRight->GetName(),rightTime);
	#endif
	if (rightTime <= 0.0) {
		Error("TimeFromInsideAlongParabola", "No boundary of right Node was hit from inside");
		return 0.0;
	}
	// Work out which time is the shortest
   if (leftTime < rightTime) {
      #ifdef VERBOSE_MODE
			Info("TimeFromInsideAlongParabola","LeftTime < RightTime");
	   #endif
		timeFromInside = leftTime;
      node->SetSelected(1);
   } else {
      #ifdef VERBOSE_MODE
			Info("TimeFromInsideAlongParabola","RightTime < LeftTime");
      #endif
		timeFromInside = rightTime;
      node->SetSelected(2);
   }      
   #ifdef VERBOSE_MODE
		Info("TimeFromInsideAlongParabola","TimeFromInside: %f", timeFromInside);
	#endif
	return timeFromInside;
}
