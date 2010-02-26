// TUCNGeoTube.cxx
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <stdexcept>
#include <cassert>

#include "Riostream.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TVirtualGeoPainter.h"
#include "TVirtualPad.h"
#include "TBuffer3D.h"
#include "TBuffer3DTypes.h"
#include "TMath.h"

#include "TUCNGeoTube.h"
#include "TUCNPolynomial.h"
#include "TUCNParabola.h"

using std::cout;
using std::endl;
using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(TUCNGeoTube)

//_____________________________________________________________________________
TUCNGeoTube::TUCNGeoTube()
{
// Default constructor
   Info("TUCNGeoTube", "Default Constructor");
	SetShapeBit(TGeoShape::kGeoTube);
   fRmin = 0.0;
   fRmax = 0.0;
   fDz   = 0.0;
}


//_____________________________________________________________________________
TUCNGeoTube::TUCNGeoTube(Double_t rmin, Double_t rmax, Double_t dz)
         	:TUCNGeoBBox(0, 0, 0)
{
// Default constructor specifying minimum and maximum radius
   Info("TUCNGeoTube", "Constructor");
   SetShapeBit(TGeoShape::kGeoTube);
   SetTubeDimensions(rmin, rmax, dz);
   if ((fDz<0) || (fRmin<0) || (fRmax<0)) {
      SetShapeBit(kGeoRunTimeShape);
//      if (fRmax<=fRmin) SetShapeBit(kGeoInvalidShape);
//      printf("tube : dz=%f rmin=%f rmax=%f\n", dz, rmin, rmax);
   }
   ComputeBBox();
}
//_____________________________________________________________________________
TUCNGeoTube::TUCNGeoTube(const char *name, Double_t rmin, Double_t rmax, Double_t dz)
            :TUCNGeoBBox(name, 0, 0, 0)
{
// Default constructor specifying minimum and maximum radius
   Info("TUCNGeoTube", "Constructor");
   SetShapeBit(TGeoShape::kGeoTube);
   SetTubeDimensions(rmin, rmax, dz);
   if ((fDz<0) || (fRmin<0) || (fRmax<0)) {
      SetShapeBit(kGeoRunTimeShape);
//      if (fRmax<=fRmin) SetShapeBit(kGeoInvalidShape);
//      printf("tube : dz=%f rmin=%f rmax=%f\n", dz, rmin, rmax);
   }
   ComputeBBox();
}

//_____________________________________________________________________________
TUCNGeoTube::TUCNGeoTube(Double_t *param)
         	:TUCNGeoBBox(0, 0, 0)
{
// Default constructor specifying minimum and maximum radius
// param[0] = Rmin
// param[1] = Rmax
// param[2] = dz
   Info("TUCNGeoTube", "Constructor");
   SetShapeBit(TGeoShape::kGeoTube);
   SetDimensions(param);
   if ((fDz<0) || (fRmin<0) || (fRmax<0)) SetShapeBit(kGeoRunTimeShape);
   ComputeBBox();
}

//_____________________________________________________________________________
TUCNGeoTube::~TUCNGeoTube()
{
// destructor
   Info("TUCNGeoTube", "Destructor");
}

//_____________________________________________________________________________
Double_t TUCNGeoTube::Capacity() const
{
// Computes capacity of the shape in [length^3]
   return TUCNGeoTube::Capacity(fRmin,fRmax, fDz);
}   

//_____________________________________________________________________________
Double_t TUCNGeoTube::Capacity(Double_t rmin, Double_t rmax, Double_t dz)
{
// Computes capacity of the shape in [length^3]
   Double_t capacity = 2.*TMath::Pi()*(rmax*rmax-rmin*rmin)*dz;
   return capacity;
}   

//_____________________________________________________________________________
void TUCNGeoTube::ComputeBBox()
{
// compute bounding box of the tube
   fDX = fDY = fRmax;
   fDZ = fDz;
}

//_____________________________________________________________________________
void TUCNGeoTube::ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm)
{
// Compute normal to closest surface from POINT.
   Double_t saf[3];
   Double_t rsq = point[0]*point[0]+point[1]*point[1];
   Double_t r = TMath::Sqrt(rsq);
   saf[0] = TMath::Abs(fDz-TMath::Abs(point[2]));
   saf[1] = (fRmin>1E-10)?TMath::Abs(r-fRmin):TGeoShape::Big();
   saf[2] = TMath::Abs(fRmax-r);
   Int_t i = TMath::LocMin(3,saf);
   if (i==0) {
      norm[0] = norm[1] = 0.;
      norm[2] = TMath::Sign(1.,dir[2]);
      return;
   }
   norm[2] = 0;
   Double_t phi = TMath::ATan2(point[1], point[0]);
   norm[0] = TMath::Cos(phi);
   norm[1] = TMath::Sin(phi);
   if (norm[0]*dir[0]+norm[1]*dir[1]<0) {
      norm[0] = -norm[0];
      norm[1] = -norm[1];
   }
}

//_____________________________________________________________________________
void TUCNGeoTube::ComputeNormalS(Double_t *point, Double_t *dir, Double_t *norm,
                              Double_t /*rmin*/, Double_t /*rmax*/, Double_t /*dz*/)
{
// Compute normal to closest surface from POINT.
   norm[2] = 0;
   Double_t phi = TMath::ATan2(point[1], point[0]);
   norm[0] = TMath::Cos(phi);
   norm[1] = TMath::Sin(phi);
   if (norm[0]*dir[0]+norm[1]*dir[1]<0) {
      norm[0] = -norm[0];
      norm[1] = -norm[1];
   }
}

//_____________________________________________________________________________
Bool_t TUCNGeoTube::Contains(Double_t *point) const
{
// test if point is inside this tube
   if (TMath::Abs(point[2]) > fDz) return kFALSE;
   Double_t r2 = point[0]*point[0]+point[1]*point[1];
   if ((r2<fRmin*fRmin) || (r2>fRmax*fRmax)) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________
Int_t TUCNGeoTube::DistancetoPrimitive(Int_t px, Int_t py)
{
// compute closest distance from point px,py to each corner
   Int_t n = gGeoManager->GetNsegments();
   Int_t numPoints = 4*n;
   if (!HasRmin()) numPoints = 2*(n+1);
   return ShapeDistancetoPrimitive(numPoints, px, py);
}

//_____________________________________________________________________________
Double_t TUCNGeoTube::DistFromInsideS(Double_t *point, Double_t *dir, Double_t rmin, Double_t rmax, Double_t dz)
{
// Compute distance from inside point to surface of the tube (static)
// Boundary safe algorithm.
   // compute distance to surface
   // Do Z
   Double_t sz = TGeoShape::Big();
   if (dir[2]) {
      sz = (TMath::Sign(dz, dir[2])-point[2])/dir[2];
      if (sz<=0) return 0.0;
   }
   // Do R
   Double_t nsq=dir[0]*dir[0]+dir[1]*dir[1];
   if (TMath::Abs(nsq)<TGeoShape::Tolerance()) return sz;
   Double_t rsq=point[0]*point[0]+point[1]*point[1];
   Double_t rdotn=point[0]*dir[0]+point[1]*dir[1];
   Double_t b,d;
   Double_t sr = TGeoShape::Big();
   // inner cylinder
   if (rmin>0) {
      // Protection in case point is actually outside the tube
      if (rsq <= rmin*rmin+TGeoShape::Tolerance()) {
         if (rdotn<0) return 0.0;
      } else {
         if (rdotn<0) {
            DistToTube(rsq,nsq,rdotn,rmin,b,d);
            if (d>0) {
               sr=-b-d;
               if (sr>0) return TMath::Min(sz,sr);
            }
         }
      }
   }
   // outer cylinder
   if (rsq >= rmax*rmax-TGeoShape::Tolerance()) {
      if (rdotn>=0) return 0.0;
   }
   DistToTube(rsq,nsq,rdotn,rmax,b,d);
   if (d>0) {
      sr=-b+d;
      if (sr>0) return TMath::Min(sz,sr);
   }
   return 0.;
}

//_____________________________________________________________________________
Double_t TUCNGeoTube::DistFromInside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// Compute distance from inside point to surface of the tube
// Boundary safe algorithm.
   if (iact<3 && safe) {
      *safe = Safety(point, kTRUE);
      if (iact==0) return TGeoShape::Big();
      if ((iact==1) && (*safe>step)) return TGeoShape::Big();
   }
   // compute distance to surface
   return DistFromInsideS(point, dir, fRmin, fRmax, fDz);
}

//_____________________________________________________________________________
Double_t TUCNGeoTube::DistFromOutsideS(Double_t *point, Double_t *dir, Double_t rmin, Double_t rmax, Double_t dz)
{
// Static method to compute distance from outside point to a tube with given parameters
// Boundary safe algorithm.
   // check Z planes
   Double_t xi,yi,zi;
   Double_t rmaxsq = rmax*rmax;
   Double_t rminsq = rmin*rmin;
   zi = dz - TMath::Abs(point[2]);
   Double_t s = TGeoShape::Big();
   Bool_t in = kFALSE;
   Bool_t inz = (zi<0)?kFALSE:kTRUE;
   if (!inz) {
      if (point[2]*dir[2]>=0) return TGeoShape::Big();
      s  = -zi/TMath::Abs(dir[2]);
      xi = point[0]+s*dir[0];
      yi = point[1]+s*dir[1];
      Double_t r2=xi*xi+yi*yi;
      if ((rminsq<=r2) && (r2<=rmaxsq)) return s;
   }

   Double_t rsq = point[0]*point[0]+point[1]*point[1];
   // check outer cyl. surface
   Double_t nsq=dir[0]*dir[0]+dir[1]*dir[1];
   Double_t rdotn=point[0]*dir[0]+point[1]*dir[1];
   Double_t b,d;
   Bool_t inrmax = kFALSE;
   Bool_t inrmin = kFALSE;
   if (rsq<=rmaxsq+TGeoShape::Tolerance()) inrmax = kTRUE;
   if (rsq>=rminsq-TGeoShape::Tolerance()) inrmin = kTRUE;
   in = inz & inrmin & inrmax;
   // If inside, we are most likely on a boundary within machine precision.
   if (in) {
      Bool_t checkout = kFALSE;
      Double_t r = TMath::Sqrt(rsq);
      if (zi<rmax-r) {
         if ((TGeoShape::IsSameWithinTolerance(rmin,0)) || (zi<r-rmin)) {
            if (point[2]*dir[2]<0) return 0.0;
            return TGeoShape::Big();
         }
      }
      if ((rmaxsq-rsq) < (rsq-rminsq)) checkout = kTRUE;
      if (checkout) {
         if (rdotn>=0) return TGeoShape::Big();
         return 0.0;
      }
      if (TGeoShape::IsSameWithinTolerance(rmin,0)) return 0.0;
      if (rdotn>=0) return 0.0;
      // Ray exiting rmin -> check (+) solution for inner tube
      if (TMath::Abs(nsq)<TGeoShape::Tolerance()) return TGeoShape::Big();
      DistToTube(rsq, nsq, rdotn, rmin, b, d);
      if (d>0) {
         s=-b+d;
         if (s>0) {
            zi=point[2]+s*dir[2];
            if (TMath::Abs(zi)<=dz) return s;
         }
      }
      return TGeoShape::Big();
   }
   // Check outer cylinder (only r>rmax has to be considered)
   if (TMath::Abs(nsq)<TGeoShape::Tolerance()) return TGeoShape::Big();
   if (!inrmax) {
      DistToTube(rsq, nsq, rdotn, rmax, b, d);
      if (d>0) {
         s=-b-d;
         if (s>0) {
            zi=point[2]+s*dir[2];
            if (TMath::Abs(zi)<=dz) return s;
         }
      }
   }
   // check inner cylinder
   if (rmin>0) {
      DistToTube(rsq, nsq, rdotn, rmin, b, d);
      if (d>0) {
         s=-b+d;
         if (s>0) {
            zi=point[2]+s*dir[2];
            if (TMath::Abs(zi)<=dz) return s;
         }
      }
   }
   return TGeoShape::Big();
}

//_____________________________________________________________________________
Double_t TUCNGeoTube::DistFromOutside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// Compute distance from outside point to surface of the tube and safe distance
// Boundary safe algorithm.
   // fist localize point w.r.t tube
   if (iact<3 && safe) {
      *safe = Safety(point, kFALSE);
      if (iact==0) return TGeoShape::Big();
      if ((iact==1) && (step<=*safe)) return TGeoShape::Big();
   }
// Check if the bounding box is crossed within the requested distance
   Double_t sdist = TUCNGeoBBox::DistFromOutside(point,dir, fDX, fDY, fDZ, fOrigin, step);
   if (sdist>=step) return TGeoShape::Big();
   // find distance to shape
   return DistFromOutsideS(point, dir, fRmin, fRmax, fDz);
}

//_____________________________________________________________________________
void TUCNGeoTube::DistToTube(Double_t rsq, Double_t nsq, Double_t rdotn, Double_t radius, Double_t &b, Double_t &delta)
{
// Static method computing the distance to a tube with given radius, starting from
// POINT along DIR director cosines. The distance is computed as :
//    RSQ   = point[0]*point[0]+point[1]*point[1]
//    NSQ   = dir[0]*dir[0]+dir[1]*dir[1]  ---> should NOT be 0 !!!
//    RDOTN = point[0]*dir[0]+point[1]*dir[1]
// The distance can be computed as :
//    D = -B +/- DELTA
// where DELTA.GT.0 and D.GT.0

   Double_t t1 = 1./nsq;
   Double_t t3=rsq-(radius*radius);
   b          = t1*rdotn;
   Double_t c =t1*t3;
   delta = b*b-c;
   if (delta>0) {
      delta=TMath::Sqrt(delta);
   } else {
      delta = -1;
   }
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoTube::Divide(TGeoVolume* /*voldiv*/, const char* /*divname*/, Int_t /*iaxis*/, Int_t /*ndiv*/,
                             Double_t /*start*/, Double_t /*step*/)
{
/*//--- Divide this tube shape belonging to volume "voldiv" into ndiv volumes
// called divname, from start position with the given step. Returns pointer
// to created division cell volume in case of Z divisions. For radial division
// creates all volumes with different shapes and returns pointer to volume that
// was divided. In case a wrong division axis is supplied, returns pointer to
// volume that was divided.
   TGeoShape *shape;           //--- shape to be created
   TGeoVolume *vol;            //--- division volume to be created
   TGeoVolumeMulti *vmulti;    //--- generic divided volume
   TGeoPatternFinder *finder;  //--- finder to be attached
   TString opt = "";           //--- option to be attached
   Int_t id;
   Double_t end = start+ndiv*step;
   switch (iaxis) {
      case 1:  //---                R division
         finder = new TGeoPatternCylR(voldiv, ndiv, start, end);
         vmulti = gGeoManager->MakeVolumeMulti(divname, voldiv->GetMedium());
         voldiv->SetFinder(finder);
         finder->SetDivIndex(voldiv->GetNdaughters());
         for (id=0; id<ndiv; id++) {
            shape = new TUCNGeoTube(start+id*step, start+(id+1)*step, fDz);
            vol = new TGeoVolume(divname, shape, voldiv->GetMedium());
            vmulti->AddVolume(vol);
            opt = "R";
            voldiv->AddNodeOffset(vol, id, 0, opt.Data());
            ((TGeoNodeOffset*)voldiv->GetNodes()->At(voldiv->GetNdaughters()-1))->SetFinder(finder);
         }
         return vmulti;
      case 2:  //---                Phi division
         finder = new TGeoPatternCylPhi(voldiv, ndiv, start, end);
         voldiv->SetFinder(finder);
         finder->SetDivIndex(voldiv->GetNdaughters());
         shape = new TUCNGeoTubeSeg(fRmin, fRmax, fDz, -step/2, step/2);
         vol = new TGeoVolume(divname, shape, voldiv->GetMedium());
         vmulti = gGeoManager->MakeVolumeMulti(divname, voldiv->GetMedium());
         vmulti->AddVolume(vol);
         opt = "Phi";
         for (id=0; id<ndiv; id++) {
            voldiv->AddNodeOffset(vol, id, start+id*step+step/2, opt.Data());
            ((TGeoNodeOffset*)voldiv->GetNodes()->At(voldiv->GetNdaughters()-1))->SetFinder(finder);
         }
         return vmulti;
      case 3: //---                  Z division
         finder = new TGeoPatternZ(voldiv, ndiv, start, start+ndiv*step);
         voldiv->SetFinder(finder);
         finder->SetDivIndex(voldiv->GetNdaughters());
         shape = new TUCNGeoTube(fRmin, fRmax, step/2);
         vol = new TGeoVolume(divname, shape, voldiv->GetMedium());
         vmulti = gGeoManager->MakeVolumeMulti(divname, voldiv->GetMedium());
         vmulti->AddVolume(vol);
         opt = "Z";
         for (id=0; id<ndiv; id++) {
            voldiv->AddNodeOffset(vol, id, start+step/2+id*step, opt.Data());
            ((TGeoNodeOffset*)voldiv->GetNodes()->At(voldiv->GetNdaughters()-1))->SetFinder(finder);
         }
         return vmulti;
      default:
         Error("Divide", "In shape %s wrong axis type for division", GetName());
         return 0;
   }
*/
	Error("Divide","Divide is not implemented for UCNTube. Sorry");
	return 0;
}

//_____________________________________________________________________________
const char *TUCNGeoTube::GetAxisName(Int_t iaxis) const
{
// Returns name of axis IAXIS.
   switch (iaxis) {
      case 1:
         return "R";
      case 2:
         return "PHI";
      case 3:
         return "Z";
      default:
         return "UNDEFINED";
   }
}

//_____________________________________________________________________________
Double_t TUCNGeoTube::GetAxisRange(Int_t iaxis, Double_t &xlo, Double_t &xhi) const
{
// Get range of shape for a given axis.
   xlo = 0;
   xhi = 0;
   Double_t dx = 0;
   switch (iaxis) {
      case 1:
         xlo = fRmin;
         xhi = fRmax;
         dx = xhi-xlo;
         return dx;
      case 2:
         xlo = 0;
         xhi = 360;
         dx = 360;
         return dx;
      case 3:
         xlo = -fDz;
         xhi = fDz;
         dx = xhi-xlo;
         return dx;
   }
   return dx;
}

//_____________________________________________________________________________
void TUCNGeoTube::GetBoundingCylinder(Double_t *param) const
{
//--- Fill vector param[4] with the bounding cylinder parameters. The order
// is the following : Rmin, Rmax, Phi1, Phi2, dZ
   param[0] = fRmin; // Rmin
   param[0] *= param[0];
   param[1] = fRmax; // Rmax
   param[1] *= param[1];
   param[2] = 0.;    // Phi1
   param[3] = 360.;  // Phi1
}

//_____________________________________________________________________________
TGeoShape *TUCNGeoTube::GetMakeRuntimeShape(TGeoShape *mother, TGeoMatrix * /*mat*/) const
{
// in case shape has some negative parameters, these has to be computed
// in order to fit the mother
   if (!TestShapeBit(kGeoRunTimeShape)) return 0;
   Double_t rmin, rmax, dz;
   Double_t xmin,xmax;
   rmin = fRmin;
   rmax = fRmax;
   dz = fDz;
   if (fDz<0) {
      mother->GetAxisRange(3,xmin,xmax);
      if (xmax<0) return 0;
      dz=xmax;
   }
   mother->GetAxisRange(1,xmin,xmax);
   if (fRmin<0) {
      if (xmin<0) return 0;
      rmin = xmin;
   }
   if (fRmax<0) {
      if (xmax<=0) return 0;
      rmax = xmax;
   }

   return (new TUCNGeoTube(GetName(), rmin, rmax, dz));
}

//_____________________________________________________________________________
void TUCNGeoTube::InspectShape() const
{
// print shape parameters
   printf("*** Shape %s: TUCNGeoTube ***\n", GetName());
   printf("    Rmin = %11.5f\n", fRmin);
   printf("    Rmax = %11.5f\n", fRmax);
   printf("    dz   = %11.5f\n", fDz);
   printf(" Bounding box:\n");
   TUCNGeoBBox::InspectShape();
}

//_____________________________________________________________________________
TBuffer3D *TUCNGeoTube::MakeBuffer3D() const
{
   // Creates a TBuffer3D describing *this* shape.
   // Coordinates are in local reference frame.

   Int_t n = gGeoManager->GetNsegments();
   Int_t nbPnts = 4*n;
   Int_t nbSegs = 8*n;
   Int_t nbPols = 4*n;
   if (!HasRmin()) {
      nbPnts = 2*(n+1);
      nbSegs = 5*n;
      nbPols = 3*n;
   }   
   TBuffer3D* buff = new TBuffer3D(TBuffer3DTypes::kGeneric,
                                   nbPnts, 3*nbPnts, nbSegs, 3*nbSegs, nbPols, 6*nbPols);
   if (buff)
   {
      SetPoints(buff->fPnts);
      SetSegsAndPols(*buff);
   }

   return buff;
}

//_____________________________________________________________________________
void TUCNGeoTube::SetSegsAndPols(TBuffer3D &buffer) const
{
// Fill TBuffer3D structure for segments and polygons.
   Int_t i, j,indx;
   Int_t n = gGeoManager->GetNsegments();
   Int_t c = (((buffer.fColor) %8) -1) * 4;
   if (c < 0) c = 0;

   if (HasRmin()) {
      // circle segments:
      // lower rmin circle: i=0, (0, n-1)
      // lower rmax circle: i=1, (n, 2n-1)
      // upper rmin circle: i=2, (2n, 3n-1)
      // upper rmax circle: i=1, (3n, 4n-1)
      for (i = 0; i < 4; i++) {
         for (j = 0; j < n; j++) {
            indx = 3*(i*n+j);
            buffer.fSegs[indx  ] = c;
            buffer.fSegs[indx+1] = i*n+j;
            buffer.fSegs[indx+2] = i*n+(j+1)%n;
         }
      }
      // Z-parallel segments
      // inner: i=4, (4n, 5n-1)
      // outer: i=5, (5n, 6n-1)
      for (i = 4; i < 6; i++) {
         for (j = 0; j < n; j++) {
            indx = 3*(i*n+j);
            buffer.fSegs[indx  ] = c+1;
            buffer.fSegs[indx+1] = (i-4)*n+j;
            buffer.fSegs[indx+2] = (i-2)*n+j;
         }
      }
      // Radial segments
      // lower: i=6, (6n, 7n-1)
      // upper: i=7, (7n, 8n-1)
      for (i = 6; i < 8; i++) {
         for (j = 0; j < n; j++) {
            indx = 3*(i*n+j);
            buffer.fSegs[indx  ] = c;
            buffer.fSegs[indx+1] = 2*(i-6)*n+j;
            buffer.fSegs[indx+2] = (2*(i-6)+1)*n+j;
         }
      }
      // Polygons
      i=0;
      // Inner lateral (0, n-1)
      for (j = 0; j < n; j++) {
         indx = 6*(i*n+j);
         buffer.fPols[indx  ] = c;
         buffer.fPols[indx+1] = 4;
         buffer.fPols[indx+2] = j;
         buffer.fPols[indx+3] = 4*n+(j+1)%n;
         buffer.fPols[indx+4] = 2*n+j;
         buffer.fPols[indx+5] = 4*n+j;
      }
      i=1;
      // Outer lateral (n,2n-1)
      for (j = 0; j < n; j++) {
         indx = 6*(i*n+j);
         buffer.fPols[indx  ] = c+1;
         buffer.fPols[indx+1] = 4;
         buffer.fPols[indx+2] = n+j;
         buffer.fPols[indx+3] = 5*n+j;
         buffer.fPols[indx+4] = 3*n+j;
         buffer.fPols[indx+5] = 5*n+(j+1)%n;
      }
      i=2;
      // lower disc (2n, 3n-1)
      for (j = 0; j < n; j++) {
         indx = 6*(i*n+j);
         buffer.fPols[indx  ] = c;
         buffer.fPols[indx+1] = 4;
         buffer.fPols[indx+2] = j;
         buffer.fPols[indx+3] = 6*n+j;
         buffer.fPols[indx+4] = n+j;
         buffer.fPols[indx+5] = 6*n+(j+1)%n;
      }
      i=3;
      // upper disc (3n, 4n-1)
      for (j = 0; j < n; j++) {
         indx = 6*(i*n+j);
         buffer.fPols[indx  ] = c;
         buffer.fPols[indx+1] = 4;
         buffer.fPols[indx+2] = 2*n+j;
         buffer.fPols[indx+3] = 7*n+(j+1)%n;
         buffer.fPols[indx+4] = 3*n+j;
         buffer.fPols[indx+5] = 7*n+j;
      }
      return;
   }
   // Rmin=0 tubes
   // circle segments
   // lower rmax circle: i=0, (0, n-1)
   // upper rmax circle: i=1, (n, 2n-1)
   for (i = 0; i < 2; i++) {
      for (j = 0; j < n; j++) {
         indx = 3*(i*n+j);
         buffer.fSegs[indx  ] = c;
         buffer.fSegs[indx+1] = 2+i*n+j;
         buffer.fSegs[indx+2] = 2+i*n+(j+1)%n;
      }
   }
   // Z-parallel segments (2n,3n-1)
   for (j = 0; j < n; j++) {
      indx = 3*(2*n+j);
      buffer.fSegs[indx  ] = c+1;
      buffer.fSegs[indx+1] = 2+j;
      buffer.fSegs[indx+2] = 2+n+j;
   }
   // Radial segments
   // Lower circle: i=3, (3n,4n-1)
   // Upper circle: i=4, (4n,5n-1)
   for (i = 3; i < 5; i++) {
      for (j = 0; j < n; j++) {
         indx = 3*(i*n+j);
         buffer.fSegs[indx  ] = c;
         buffer.fSegs[indx+1] = i-3;
         buffer.fSegs[indx+2] = 2+(i-3)*n+j;
      }
   }
   // Polygons
   // lateral (0,n-1)
   for (j = 0; j < n; j++) {
      indx = 6*j;
      buffer.fPols[indx  ] = c+1;
      buffer.fPols[indx+1] = 4;
      buffer.fPols[indx+2] = j;
      buffer.fPols[indx+3] = 2*n+j;
      buffer.fPols[indx+4] = n+j;
      buffer.fPols[indx+5] = 2*n+(j+1)%n;
   }
   // bottom triangles (n,2n-1)
   for (j = 0; j < n; j++) {
      indx = 6*n + 5*j;
      buffer.fPols[indx  ] = c;
      buffer.fPols[indx+1] = 3;
      buffer.fPols[indx+2] = j;
      buffer.fPols[indx+3] = 3*n+(j+1)%n;
      buffer.fPols[indx+4] = 3*n+j;
   }
   // top triangles (2n,3n-1)  
   for (j = 0; j < n; j++) {
      indx = 6*n + 5*n + 5*j;
      buffer.fPols[indx  ] = c;
      buffer.fPols[indx+1] = 3;
      buffer.fPols[indx+2] = n+j;
      buffer.fPols[indx+3] = 4*n+j;
      buffer.fPols[indx+4] = 4*n+(j+1)%n;
   }
}

//_____________________________________________________________________________
Double_t TUCNGeoTube::Safety(Double_t *point, Bool_t in) const
{
// computes the closest distance from given point to this shape, according
// to option. The matching point on the shape is stored in spoint.
#ifndef NEVER
   Double_t r = TMath::Sqrt(point[0]*point[0]+point[1]*point[1]);
   Double_t safe, safrmin, safrmax;
   if (in) {
      safe    = fDz-TMath::Abs(point[2]); // positive if inside
      if (fRmin>1E-10) {
         safrmin = r-fRmin;
         if (safrmin < safe) safe = safrmin;
      }
      safrmax = fRmax-r;
      if (safrmax < safe) safe = safrmax;
   } else {
      safe    = -fDz+TMath::Abs(point[2]);
      if (fRmin>1E-10) {
         safrmin = -r+fRmin;
         if (safrmin > safe) safe = safrmin;
      }
      safrmax = -fRmax+r;
      if (safrmax > safe) safe = safrmax;
   }
   return safe;
#else
   Double_t saf[3];
   Double_t rsq = point[0]*point[0]+point[1]*point[1];
   Double_t r = TMath::Sqrt(rsq);
   saf[0] = fDz-TMath::Abs(point[2]); // positive if inside
   saf[1] = (fRmin>1E-10)?(r-fRmin):TGeoShape::Big();
   saf[2] = fRmax-r;
   if (in) return saf[TMath::LocMin(3,saf)];
   for (Int_t i=0; i<3; i++) saf[i]=-saf[i];
   return saf[TMath::LocMax(3,saf)];
#endif
}

//_____________________________________________________________________________
Double_t TUCNGeoTube::SafetyS(Double_t *point, Bool_t in, Double_t rmin, Double_t rmax, Double_t dz, Int_t skipz)
{
// computes the closest distance from given point to this shape, according
// to option. The matching point on the shape is stored in spoint.
   Double_t saf[3];
   Double_t rsq = point[0]*point[0]+point[1]*point[1];
   Double_t r = TMath::Sqrt(rsq);
   switch (skipz) {
      case 1: // skip lower Z plane
         saf[0] = dz - point[2];
         break;
      case 2: // skip upper Z plane
         saf[0] = dz + point[2];
         break;
      case 3: // skip both
         saf[0] = TGeoShape::Big();
         break;
      default:
         saf[0] = dz-TMath::Abs(point[2]);
   }
   saf[1] = (rmin>1E-10)?(r-rmin):TGeoShape::Big();
   saf[2] = rmax-r;
//   printf("saf0=%g saf1=%g saf2=%g in=%d skipz=%d\n", saf[0],saf[1],saf[2],in,skipz);
   if (in) return saf[TMath::LocMin(3,saf)];
   for (Int_t i=0; i<3; i++) saf[i]=-saf[i];
   return saf[TMath::LocMax(3,saf)];
}

//_____________________________________________________________________________
void TUCNGeoTube::SavePrimitive(ostream &out, Option_t * /*option*/ /*= ""*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   if (TObject::TestBit(kGeoSavePrimitive)) return;
   out << "   // Shape: " << GetName() << " type: " << ClassName() << endl;
   out << "   rmin = " << fRmin << ";" << endl;
   out << "   rmax = " << fRmax << ";" << endl;
   out << "   dz   = " << fDz << ";" << endl;
   out << "   TGeoShape *" << GetPointerName() << " = new TUCNGeoTube(\"" << GetName() << "\",rmin,rmax,dz);" << endl;
   TObject::SetBit(TGeoShape::kGeoSavePrimitive);
}

//_____________________________________________________________________________
void TUCNGeoTube::SetTubeDimensions(Double_t rmin, Double_t rmax, Double_t dz)
{
// Set tube dimensions.
   fRmin = rmin;
   fRmax = rmax;
   fDz   = dz;
   if (fRmin>0 && fRmax>0 && fRmin>=fRmax)
      Error("SetTubeDimensions", "In shape %s wrong rmin=%g rmax=%g", GetName(), rmin,rmax);
}

//_____________________________________________________________________________
void TUCNGeoTube::SetDimensions(Double_t *param)
{
// Set tube dimensions starting from a list.
   Double_t rmin = param[0];
   Double_t rmax = param[1];
   Double_t dz   = param[2];
   SetTubeDimensions(rmin, rmax, dz);
}

//_____________________________________________________________________________
Bool_t TUCNGeoTube::GetPointsOnSegments(Int_t npoints, Double_t *array) const
{
// Fills array with n random points located on the line segments of the shape mesh.
// The output array must be provided with a length of minimum 3*npoints. Returns
// true if operation is implemented.
   if (npoints > (npoints/2)*2) {
      Error("GetPointsOnSegments","Npoints must be even number");
      return kFALSE;
   }   
   Int_t nc = 0;
   if (HasRmin()) nc = (Int_t)TMath::Sqrt(0.5*npoints);
   else           nc = (Int_t)TMath::Sqrt(1.*npoints);
   Double_t dphi = TMath::TwoPi()/nc;
   Double_t phi = 0;
   Int_t ntop = 0;
   if (HasRmin()) ntop = npoints/2 - nc*(nc-1);
   else           ntop = npoints - nc*(nc-1);
   Double_t dz = 2*fDz/(nc-1);
   Double_t z = 0;
   Int_t icrt = 0;
   Int_t nphi = nc;
   // loop z sections
   for (Int_t i=0; i<nc; i++) {
      if (i == (nc-1)) nphi = ntop;
      z = -fDz + i*dz;
      // loop points on circle sections
      for (Int_t j=0; j<nphi; j++) {
         phi = j*dphi;
         if (HasRmin()) {
            array[icrt++] = fRmin * TMath::Cos(phi);
            array[icrt++] = fRmin * TMath::Sin(phi);
            array[icrt++] = z;
         }
         array[icrt++] = fRmax * TMath::Cos(phi);
         array[icrt++] = fRmax * TMath::Sin(phi);
         array[icrt++] = z;
      }
   }
   return kTRUE;
}                    

//_____________________________________________________________________________
void TUCNGeoTube::SetPoints(Double_t *points) const
{
// create tube mesh points
   Double_t dz;
   Int_t j, n;
   n = gGeoManager->GetNsegments();
   Double_t dphi = 360./n;
   Double_t phi = 0;
   dz = fDz;
   Int_t indx = 0;
   if (points) {
      if (HasRmin()) {
         // 4*n points
         // (0,n-1) lower rmin circle
         // (2n, 3n-1) upper rmin circle
         for (j = 0; j < n; j++) {
            phi = j*dphi*TMath::DegToRad();
            points[indx+6*n] = points[indx] = fRmin * TMath::Cos(phi);
            indx++;
            points[indx+6*n] = points[indx] = fRmin * TMath::Sin(phi);
            indx++;
            points[indx+6*n] = dz;
            points[indx]     =-dz;
            indx++;
         }   
         // (n, 2n-1) lower rmax circle
         // (3n, 4n-1) upper rmax circle
         for (j = 0; j < n; j++) {
            phi = j*dphi*TMath::DegToRad();
            points[indx+6*n] = points[indx] = fRmax * TMath::Cos(phi);
            indx++;
            points[indx+6*n] = points[indx] = fRmax * TMath::Sin(phi);
            indx++;
            points[indx+6*n]= dz;
            points[indx]    =-dz;
            indx++;
         }
      } else {
         // centers of lower/upper circles (0,1)
         points[indx++] = 0.;
         points[indx++] = 0.;
         points[indx++] = -dz;
         points[indx++] = 0.;
         points[indx++] = 0.;
         points[indx++] = dz;
         // lower rmax circle (2, 2+n-1)
         // upper rmax circle (2+n, 2+2n-1)
         for (j = 0; j < n; j++) {
            phi = j*dphi*TMath::DegToRad();
            points[indx+3*n] = points[indx] = fRmax * TMath::Cos(phi);
            indx++;
            points[indx+3*n] = points[indx] = fRmax * TMath::Sin(phi);
            indx++;
            points[indx+3*n]= dz;
            points[indx]    =-dz;
            indx++;
         }
      }   
   }
}

//_____________________________________________________________________________
void TUCNGeoTube::SetPoints(Float_t *points) const
{
// create tube mesh points
   Double_t dz;
   Int_t j, n;
   n = gGeoManager->GetNsegments();
   Double_t dphi = 360./n;
   Double_t phi = 0;
   dz = fDz;
   Int_t indx = 0;
   if (points) {
      if (HasRmin()) {
         // 4*n points
         // (0,n-1) lower rmin circle
         // (2n, 3n-1) upper rmin circle
         for (j = 0; j < n; j++) {
            phi = j*dphi*TMath::DegToRad();
            points[indx+6*n] = points[indx] = fRmin * TMath::Cos(phi);
            indx++;
            points[indx+6*n] = points[indx] = fRmin * TMath::Sin(phi);
            indx++;
            points[indx+6*n] = dz;
            points[indx]     =-dz;
            indx++;
         }   
         // (n, 2n-1) lower rmax circle
         // (3n, 4n-1) upper rmax circle
         for (j = 0; j < n; j++) {
            phi = j*dphi*TMath::DegToRad();
            points[indx+6*n] = points[indx] = fRmax * TMath::Cos(phi);
            indx++;
            points[indx+6*n] = points[indx] = fRmax * TMath::Sin(phi);
            indx++;
            points[indx+6*n]= dz;
            points[indx]    =-dz;
            indx++;
         }
      } else {
         // centers of lower/upper circles (0,1)
         points[indx++] = 0.;
         points[indx++] = 0.;
         points[indx++] = -dz;
         points[indx++] = 0.;
         points[indx++] = 0.;
         points[indx++] = dz;
         // lower rmax circle (2, 2+n-1)
         // upper rmax circle (2+n, 2+2n-1)
         for (j = 0; j < n; j++) {
            phi = j*dphi*TMath::DegToRad();
            points[indx+3*n] = points[indx] = fRmax * TMath::Cos(phi);
            indx++;
            points[indx+3*n] = points[indx] = fRmax * TMath::Sin(phi);
            indx++;
            points[indx+3*n]= dz;
            points[indx]    =-dz;
            indx++;
         }
      }   
   }
}

//_____________________________________________________________________________
Int_t TUCNGeoTube::GetNmeshVertices() const
{
// Return number of vertices of the mesh representation
   Int_t n = gGeoManager->GetNsegments();
   Int_t numPoints = n*4;
   if (!HasRmin()) numPoints = 2*(n+1);
   return numPoints;
}

//_____________________________________________________________________________
void TUCNGeoTube::GetMeshNumbers(Int_t &nvert, Int_t &nsegs, Int_t &npols) const
{
// Returns numbers of vertices, segments and polygons composing the shape mesh.
   Int_t n = gGeoManager->GetNsegments();
   nvert = n*4;
   nsegs = n*8;
   npols = n*4;
   if (!HasRmin()) {
      nvert = 2*(n+1);
      nsegs = 5*n;
      npols = 3*n;
   } else {
      nvert = n*4;
      nsegs = n*8;
      npols = n*4;
   }   
}

//_____________________________________________________________________________
void TUCNGeoTube::Sizeof3D() const
{
///// fill size of this 3-D object
///    TVirtualGeoPainter *painter = gGeoManager->GetGeomPainter();
///    if (!painter) return;
///    Int_t n = gGeoManager->GetNsegments();
///    Int_t numPoints = n*4;
///    Int_t numSegs   = n*8;
///    Int_t numPolys  = n*4;
///    painter->AddSize3D(numPoints, numSegs, numPolys);
}

//_____________________________________________________________________________
const TBuffer3D & TUCNGeoTube::GetBuffer3D(Int_t reqSections, Bool_t localFrame) const
{
// Fills a static 3D buffer and returns a reference.
   static TBuffer3DTube buffer;
   TUCNGeoBBox::FillBuffer3D(buffer, reqSections, localFrame);

   if (reqSections & TBuffer3D::kShapeSpecific) {
      buffer.fRadiusInner  = fRmin;
      buffer.fRadiusOuter  = fRmax;
      buffer.fHalfLength   = fDz;
      buffer.SetSectionsValid(TBuffer3D::kShapeSpecific);
   }
   if (reqSections & TBuffer3D::kRawSizes) {
      Int_t n = gGeoManager->GetNsegments();
      Int_t nbPnts = 4*n;
      Int_t nbSegs = 8*n;
      Int_t nbPols = 4*n;
      if (!HasRmin()) {
         nbPnts = 2*(n+1);
         nbSegs = 5*n;
         nbPols = 3*n;
      }   
      if (buffer.SetRawSizes(nbPnts, 3*nbPnts, nbSegs, 3*nbSegs, nbPols, 6*nbPols)) {
         buffer.SetSectionsValid(TBuffer3D::kRawSizes);
      }
   }
   if ((reqSections & TBuffer3D::kRaw) && buffer.SectionsValid(TBuffer3D::kRawSizes)) {
      SetPoints(buffer.fPnts);
      if (!buffer.fLocalFrame) {
         TransformPoints(buffer.fPnts, buffer.NbPnts());
      }
      SetSegsAndPols(buffer);
      buffer.SetSectionsValid(TBuffer3D::kRaw);
   }

   return buffer;
}


//_____________________________________________________________________________
Double_t TUCNGeoTube::TimeFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, Double_t stepmax, Int_t iact, Double_t *safe) const
{
	// Compute time from inside point to surface of the tube
	#ifdef VERBOSE_MODE				
		cout << "TimeFromInsideAlongParabola - Start" << endl;
	#endif
	if (iact<3 && safe) {
	   *safe = Safety(point, kTRUE);
	   if (iact==0) return TGeoShape::Big();
	   if ((iact==1) && (*safe>stepmax)) return TGeoShape::Big();
	}
	#ifdef VERBOSE_MODE				
		cout << "TimeFromInsideAlongParabola - Calling TimeFromInsideAlongParabolaS" << endl;
	#endif
	// compute time to surface
	return TimeFromInsideAlongParabolaS(point, velocity, field, fRmin, fRmax, fDz);
}

//_____________________________________________________________________________
Double_t TUCNGeoTube::TimeFromInsideAlongParabolaS(Double_t* point, Double_t* velocity, Double_t* field, Double_t rmin, Double_t rmax, Double_t dz)
{
	// Compute time from inside point to surface of the tube	
	Double_t tfinal = 0.; // Storage for the overall smallest, non-zero time to the nearest boundary
	#ifdef VERBOSE_MODE				
		cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "Local Field Coords - X: " <<  field[0] << "  Y: " << field[1] << "  Z: " << field[2] << endl;
		cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "Local Point - X: " <<  point[0] << "  Y: " << point[1] << "  Z: " << point[2] << endl;
		cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "Local Velocity - X: " <<  velocity[0] << "  Y: " << velocity[1] << "  Z: " << velocity[2] << endl;
	#endif
	// --------------------------------------------------------------------------------------
   // -- First calculate time to intersect the +/- Z planes.
	Double_t boundary = dz;
	
	// -- Loop over +/- Z boundaries
	#ifdef VERBOSE_MODE				
		cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "Solve for +/- Z Boundaries" << endl;
	#endif
	for (Int_t j = 0; j < 2; j++) {
		// -- Define storage for the smallest non-zero root to the CURRENT boundary
		Double_t tmin = 0.0;
		// -- Define the constants in the quadratic equation, a, b, c - see note book for details
		Double_t params[3] = {0., 0., 0.};
		params[0] = 0.5*field[2];
		params[1] = velocity[2]; 
		params[2] = point[2] - (TMath::Power(-1,j)*boundary);

		// -- Solve equation for potential roots
		Double_t roots[2] = {0., 0.};
		Int_t solutions = TUCNPolynomial::Instance()->QuadraticRootFinder(&params[0], &roots[0]);

		#ifdef VERBOSE_MODE		
			cout << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
			cout << "solutions: " << solutions << "\t" << "root 1: " << roots[0] << "\t" << "root 2: " << roots[1] << endl;
		#endif

		// -- Determing number of roots, and select the smallest, real, non-zero value. 
		if (solutions == 2) {
			// -- Two roots
			if (roots[0] > 0. && roots[1] > 0.) {
				// If both are positive - determine which of two positive roots is the smaller
				if (roots[0] < roots[1]) {
					tmin = roots[0];
				} else {
					tmin = roots[1];
				}
			} else if (roots[0] > 0.) {
				tmin = roots[0];
			} else if (roots[1] > 0.) {
				tmin = roots[1];
			} else {
				#ifdef VERBOSE_MODE		
					cout << "TimeFromInsideAlongParabola" << "\t" <<  "Both roots are negative or zero." << endl; 
				#endif
			}
		} else if (solutions == 1) {
			// -- One Root
			if (roots[0] > 0.) {
				tmin = roots[0];
			} else {
				//-- Only Root is negative or zero
			}
		} else if (solutions == 0) {
			// -- No Real Roots
		} else {
			throw runtime_error("In TUCNGeoBBox, TimeFromInsideAlongParabola - number of quadratic eqn roots is not between 0 and 2");
		}
		
		if (tmin > 0.0 && tfinal == 0.0) {
			// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
			tfinal = tmin;
		} else if (tmin > 0.0 && tmin < tfinal) {
			// -- Check if this time, the smallest, non-zero time to the current boundary, is smaller than the current overall smallest time to any boundary
			tfinal = tmin;
		}
		#ifdef VERBOSE_MODE				
			cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "Z BOUNDARY --- tfinal: " <<  tfinal << endl;
		#endif
	}
	
	
	// --------------------------------------------------------------------------------------
   // -- Now Calculate the time to reach the circular boundaries of the tube.
	// -- The general case require us to solve a quartic polynomial. 
	Double_t a, b, c, d; 	// Four of the constants of the quartic polynomial. see notes for the equations.
	a = 0.25*(field[0]*field[0] + field[1]*field[1]);
	b = velocity[0]*field[0] + velocity[1]*field[1];
	c = point[0]*field[0] + point[1]*field[1] + velocity[0]*velocity[0] + velocity[1]*velocity[1];
	d = 2.0*(point[0]*velocity[0] + point[1]*velocity[1]);
	
	// -- It is possible we have a lower class of equation however. Check for this. 
	Int_t order = 4; // Order of the polynomial
	if (a == 0.0) {
		// We have a cubic or less
		order = 3;
		if (b == 0.0) {
			order = 2;
			if (c == 0.0) {
				order = 1;
				if (d == 0.0) {
					// No solution - no intersection is possible (i.e: particle has no horizontal momentum and field is aligned along local z-axis?)
					order = 0;
				}
			}
		} 
	}
	
	
	
	#ifdef VERBOSE_MODE				
		cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "Polynomial Order: " <<  order << endl;
	#endif
	
	Double_t params[5] = {0.,0.,0.,0.,0.};
	// --------------------------------------------------------------------------------------
	// -- CASE 1 - Inner Cylinder
   if (rmin > 0.) {
		// -- Solve the quartic equation to find the intersections of the parabola with the inner cylinder
		Double_t e = point[0]*point[0] + point[1]*point[1] - rmin*rmin; // the fifth constant of the quartic polynomial. see notes for equation
		params[0] = a;
		params[1] = b;
		params[2] = c;
		params[3] = d;
		params[4] = e;
		
		#ifdef VERBOSE_MODE				
			cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "Quartic Eqn - a: " <<  a << "  b: " << b << "  c: " << c << "  d: " << d << "  e: " << e << endl;
		#endif
		
		Int_t solutions = 0; // Counts the number of possible solutions
		Double_t roots[4] = {0.,0.,0.,0.};
		
		if (order < 4) {
			// We have a lower class of problem. 
			if (order == 3) {
				// We have a cubic. Organise parameters and solve. 
				Double_t cubicParams[4] = {params[1], params[2], params[3], params[4]};
				solutions = TUCNPolynomial::Instance()->CubicRootFinder(cubicParams, roots);
				
			} else if (order == 2) {
				Double_t quadraticParams[3] = {params[2], params[3], params[4]};
				solutions = TUCNPolynomial::Instance()->QuadraticRootFinder(quadraticParams, roots);

			} else if (order == 1) {
				// We have a linear equation. 
				assert(d != 0.0);
				roots[0] = -e/d;
				solutions = 1;
				
			} else if (order == 0) {
				// No solutions - no intersection
				solutions = 0;
				
			} else {
				throw runtime_error("TimeFromInsideAlongParabolaS - Order of polynomial is not between 0 and 4!");
			}	
		} else {
			// We have a quartic equation
			solutions = TUCNPolynomial::Instance()->QuarticRootFinder(params, roots);
		}
		
		#ifdef VERBOSE_MODE				
			cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "Solutions: " <<  solutions << endl;
		#endif
		
		// -- Find the smallest, positive, non-zero, real root of the polynomial
		for (Int_t i = 0; i < solutions; i++) {
			// -- Check if the current root positive and non-zero
			if (roots[i] > 0.0) {
				if (tfinal == 0.0) {
					// -- If a particular root is positive and tfinal has not yet been set, then intialise tfinal with that value
					tfinal = roots[i];
				} else if (roots[i] < tfinal) {
					// -- If tfinal has been set to a certain solution, check if current solution is smaller and if so set that to be the current smallest tfinal
					tfinal = roots[i];
				}
			} else {
				// -- Root is negative or zero, nothing to see here...
				continue;
			}
		}
		#ifdef VERBOSE_MODE				
			cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "RMIN BOUNDARY --- tfinal: " <<  tfinal << endl;
		#endif
	}	

   // --------------------------------------------------------------------------------------
	// -- CASE 2 - Outer Cylinder
	if (rmax > 0.) {
		// -- Solve the quartic equation to find the intersections of the parabola with the outer cylinder
		Double_t e = point[0]*point[0] + point[1]*point[1] - rmax*rmax;
		params[0] = a;
		params[1] = b;
		params[2] = c;
		params[3] = d;
		params[4] = e;	

		#ifdef VERBOSE_MODE				
			cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "Quartic Eqn - a: " <<  a << "  b: " << b << "  c: " << c << "  d: " << d << "  e: " << e << endl;
		#endif
		
//		Double_t pointRadius = TMath::Sqrt(point[0]*point[0] + point[1]*point[1]);
//		if (pointRadius - rmax > 0.0) {
//			cout << "Point is outside the tube" << "\t" << "e: " << e << endl;
//		}
		
		Int_t solutions = 0; // Counts the number of possible solutions
		Double_t roots[4] = {0.,0.,0.,0.};
		if (order < 4) {
			// We have a lower class of problem. 
			if (order == 3) {
				// We have a cubic. Organise parameters and solve. 
				Double_t cubicParams[4] = {b, c, d, e};
				solutions = TUCNPolynomial::Instance()->CubicRootFinder(cubicParams, roots);
				
			} else if (order == 2) {
				Double_t quadraticParams[3] = {c, d, e};
				solutions = TUCNPolynomial::Instance()->QuadraticRootFinder(quadraticParams, roots);

			} else if (order == 1) {
				// We have a linear equation. 
				assert(d != 0.0);
				roots[0] = -e/d;
				solutions = 1;
				
			} else if (order == 0) {
				// No solutions - no intersection
				solutions = 0;
				
			} else {
				throw runtime_error("TimeFromInsideAlongParabolaS - Order of polynomial less than 0!");
			}	
		} else {
			// We have a quartic equation
			solutions = TUCNPolynomial::Instance()->QuarticRootFinder(params, roots);
		}
		
		#ifdef VERBOSE_MODE				
			cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "Solutions: " <<  solutions << endl;
		#endif
		
		// -- Find the smallest, positive, non-zero, real root of the polynomial
		for (Int_t i = 0; i < solutions; i++) {
			// -- Check if the current root positive and non-zero
			if (roots[i] > 0.0) {
				if (tfinal == 0.0) {
					// -- If a particular root is positive and tfinal has not yet been set, then intialise tfinal with that value
					tfinal = roots[i];
				} else if (roots[i] < tfinal) {
					// -- If tfinal has been set to a certain solution, check if current solution is smaller and if so set that to be the current smallest tfinal
					tfinal = roots[i];
				}
			} else {
				// -- Root is negative or zero, nothing to see here...
				continue;
			}
		}
		#ifdef VERBOSE_MODE				
			cout << "TimeFromInsideAlongParabolaS" << "\t" <<  "RMAX BOUNDARY --- tfinal: " <<  tfinal << endl;
		#endif
		
	} else {
		throw runtime_error("TimeFromInsideAlongParabolaS - rmax <= 0");
	}
	
	// Find the smallest root of the polynomial
	if (tfinal > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TimeFromInsideAlongParabolaS - Time to nearest boundary: " <<  tfinal << endl; 
		#endif
		return tfinal;
	} else {
		throw runtime_error("TimeFromInsideAlongParabolaS - No boundary reached!");
		return 0;
	}	
}

// -----------------------------------------------------------------------------------------
Double_t TUCNGeoTube::TimeFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, Double_t stepmax, Int_t iact, Double_t *safe) const
{
	// Compute time from outside point to surface of the tube and safe distance
	// Boundary safe algorithm.
	// first localize point w.r.t tube
	#ifdef VERBOSE_MODE				
		cout << "TimeFromOutsideAlongParabola - Start" << endl;
	#endif
	if (iact<3 && safe) {
      *safe = Safety(point, kFALSE);
      if (iact==0) return TGeoShape::Big();
      if ((iact==1) && (stepmax<=*safe)) return TGeoShape::Big();
   }
	#ifdef VERBOSE_MODE				
		cout << "TimeFromOutsideAlongParabola - Check if Bounding box is within maximum step distance" << endl;
	#endif
	// Check if the bounding box is crossed within the requested distance
   Double_t tBox = TUCNGeoBBox::TimeFromOutsideAlongParabolaS(point, velocity, field, fDX, fDY, fDZ, fOrigin, stepmax);
	Double_t sBox = TUCNParabola::Instance()->ArcLength(velocity, field, tBox);
   #ifdef VERBOSE_MODE				
		cout << "TimeFromOutsideAlongParabola - Distance to Box: " << sBox << "\t" << "Max Step Distance: " << stepmax << endl;
	#endif
	if (sBox>=stepmax) return TGeoShape::Big();
   // find time to shape
	return TimeFromOutsideAlongParabolaS(point, velocity, field, fRmin, fRmax, fDz);
}

// -----------------------------------------------------------------------------------------
Double_t TUCNGeoTube::TimeFromOutsideAlongParabolaS(Double_t* point, Double_t* velocity, Double_t* field, Double_t rmin, Double_t rmax, Double_t dz)
{
	// Compute time from outside point to the surface of the tube
	Double_t tfinal = 0.; // Storage for the overall smallest, non-zero time to the nearest boundary
	#ifdef VERBOSE_MODE				
		cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "Local Field Coords - X: " <<  field[0] << "  Y: " << field[1] << "  Z: " << field[2] << endl;
		cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "Local Point - X: " <<  point[0] << "  Y: " << point[1] << "  Z: " << point[2] << endl;
		cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "Local Velocity - X: " <<  velocity[0] << "  Y: " << velocity[1] << "  Z: " << velocity[2] << endl;
	#endif
	// --------------------------------------------------------------------------------------
   // -- First calculate time to intersect the +/- Z planes.
	Double_t boundary = dz;
	
	// -- Loop over +/- Z boundaries
	#ifdef VERBOSE_MODE				
		cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "Solve for +/- Z Boundaries" << endl;
	#endif
	for (Int_t j = 0; j < 2; j++) {
		// -- Define storage for the smallest non-zero root to the CURRENT boundary
		Double_t tmin = 0.0;
		// -- Define the constants in the quadratic equation, a, b, c - see note book for details
		Double_t params[3] = {0., 0., 0.};
		params[0] = 0.5*field[2];
		params[1] = velocity[2]; 
		params[2] = point[2] - (TMath::Power(-1,j)*boundary);

		// -- Solve equation for potential roots
		Double_t roots[2] = {0., 0.};
		Int_t solutions = TUCNPolynomial::Instance()->QuadraticRootFinder(&params[0], &roots[0]);

		#ifdef VERBOSE_MODE		
			cout << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
			cout << "solutions: " << solutions << "\t" << "root 1: " << roots[0] << "\t" << "root 2: " << roots[1] << endl;
		#endif

		// -- Determing number of roots, and select the smallest, real, non-zero value. 
		if (solutions == 2) {
			// -- Two roots
			// -- Check first root to see if it is positive and corresponds to a point actually on the surface of the tube
			if (roots[0] > 0. && IsNextPointOnTube(point, velocity, field, rmax, boundary, roots[0]) == kTRUE) {
				tmin = roots[0];
			}
			// -- Check the second root for the same criteria.
			if (roots[1] > 0. && IsNextPointOnTube(point, velocity, field, rmax, boundary, roots[1]) == kTRUE) {
				if (tmin == 0.) {
					// -- If the first root wasn't a valid solution, then just set tmin to the second root. 
					tmin = roots[1];
				} else if (tmin > 0. && roots[1] < tmin) {
					// -- If both roots are valid, then compare the two roots and pick the smallest value. 
					tmin = roots[1];
				} else {
					#ifdef VERBOSE_MODE		
						cout << "TimeFromOutsideAlongParabolaS - Both roots are negative, zero or invalid" << endl; 
					#endif
				}
			}
		} else if (solutions == 1) {
			// -- One Root
			if (roots[0] > 0. && IsNextPointOnTube(point, velocity, field, rmax, boundary, roots[0]) == kTRUE) {
				tmin = roots[0];
			} else {
				//-- Only Root is negative or zero
			}
		} else if (solutions == 0) {
			// -- No Real Roots
		} else {
			throw runtime_error("TimeFromOutsideAlongParabolaS - number of quadratic eqn roots is not between 0 and 2");
		}
		
		if (tmin > 0.0 && tfinal == 0.0) {
			// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
			tfinal = tmin;
		} else if (tmin > 0.0 && tmin < tfinal) {
			// -- Check if this time, the smallest, non-zero time to the current boundary, is smaller than the current overall smallest time to any boundary
			tfinal = tmin;
		}
		#ifdef VERBOSE_MODE				
			cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "Z BOUNDARY --- tfinal: " <<  tfinal << endl;
		#endif
	}
	
	
	// --------------------------------------------------------------------------------------
   // -- Now Calculate the time to reach the circular boundaries of the tube.
	// -- The general case require us to solve a quartic polynomial. 
	Double_t a, b, c, d; 	// Four of the constants of the quartic polynomial. see notes for the equations.
	a = 0.25*(field[0]*field[0] + field[1]*field[1]);
	b = velocity[0]*field[0] + velocity[1]*field[1];
	c = point[0]*field[0] + point[1]*field[1] + velocity[0]*velocity[0] + velocity[1]*velocity[1];
	d = 2.0*(point[0]*velocity[0] + point[1]*velocity[1]);
	
	// -- It is possible we have a lower class of equation however. Check for this. 
	Int_t order = 4; // Order of the polynomial
	if (a == 0.0) {
		// We have a cubic or less
		order = 3;
		if (b == 0.0) {
			order = 2;
			if (c == 0.0) {
				order = 1;
				if (d == 0.0) {
					// No solution - no intersection is possible 
					// (i.e: particle has no horizontal momentum and field is aligned along local z-axis? So will only hit top or bottom)
					order = 0;
				}
			}
		} 
	}
	
	#ifdef VERBOSE_MODE				
		cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "Polynomial Order: " <<  order << endl;
	#endif
	
	Double_t params[5] = {0.,0.,0.,0.,0.};
	// --------------------------------------------------------------------------------------
	// -- CASE 1 - Inner Cylinder
   if (rmin > 0.) {
		// -- Solve the quartic equation to find the intersections of the parabola with the inner cylinder
		Double_t e = point[0]*point[0] + point[1]*point[1] - rmin*rmin; // the fifth constant of the quartic polynomial. see notes for equation
		params[0] = a;
		params[1] = b;
		params[2] = c;
		params[3] = d;
		params[4] = e;
		
		#ifdef VERBOSE_MODE				
			cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "Quartic Eqn - a: " <<  a << "  b: " << b << "  c: " << c << "  d: " << d << "  e: " << e << endl;
		#endif
		
		Int_t solutions = 0; // Counts the number of possible solutions
		Double_t roots[4] = {0.,0.,0.,0.};
		
		if (order < 4) {
			// We have a lower class of problem. 
			if (order == 3) {
				// We have a cubic. Organise parameters and solve. 
				Double_t cubicParams[4] = {b, c, d, e};
				solutions = TUCNPolynomial::Instance()->CubicRootFinder(cubicParams, roots);
				
			} else if (order == 2) {
				Double_t quadraticParams[3] = {c, d, e};
				solutions = TUCNPolynomial::Instance()->QuadraticRootFinder(quadraticParams, roots);

			} else if (order == 1) {
				// We have a linear equation. 
				assert(d != 0.0);
				roots[0] = -e/d;
				solutions = 1;
				
			} else if (order == 0) {
				// No solutions - no intersection
				solutions = 0;
				
			} else {
				throw runtime_error("TimeFromOutsideAlongParabolaS - Order of polynomial is not between 0 and 4!");
			}	
		} else {
			// We have a quartic equation
			solutions = TUCNPolynomial::Instance()->QuarticRootFinder(params, roots);
		}
		
		#ifdef VERBOSE_MODE				
			cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "Solutions: " <<  solutions << endl;
			for (Int_t i = 0; i < solutions; i++) {
				cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "i: " << i << "  Root: " << roots[i] << endl;
			}
		#endif
		
		
		// -- Find the smallest, positive, non-zero, real root of the polynomial
		for (Int_t i = 0; i < solutions; i++) {
			#ifdef VERBOSE_MODE				
				cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "i: " << i << "  Root: " << roots[i] << endl;
			#endif
			// -- Check if the current root positive and non-zero
			if (roots[i] > 0.0 && IsNextPointOnTube(point, velocity, field, rmin, boundary, roots[i]) == kTRUE) {
				if (tfinal == 0.0) {
					// -- If a particular root is positive and tfinal has not yet been set, then intialise tfinal with that value
					tfinal = roots[i];
				} else if (roots[i] < tfinal) {
					// -- If tfinal has been set to a certain solution, check if current solution is smaller and if so set that to be the current smallest tfinal
					tfinal = roots[i];
				}
			} else {
				// -- Root is negative or zero, nothing to see here...
				continue;
			}
		}
		#ifdef VERBOSE_MODE				
			cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "RMIN BOUNDARY --- tfinal: " <<  tfinal << endl;
		#endif
	}	

   // --------------------------------------------------------------------------------------
	// -- CASE 2 - Outer Cylinder
	if (rmax > 0.) {
		// -- Solve the quartic equation to find the intersections of the parabola with the outer cylinder
		Double_t e = point[0]*point[0] + point[1]*point[1] - rmax*rmax;
		params[0] = a;
		params[1] = b;
		params[2] = c;
		params[3] = d;
		params[4] = e;	

		#ifdef VERBOSE_MODE				
			cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "Quartic Eqn - a: " <<  a << "  b: " << b << "  c: " << c << "  d: " << d << "  e: " << e << endl;
		#endif
		
		Int_t solutions = 0; // Counts the number of possible solutions
		Double_t roots[4] = {0.,0.,0.,0.};
		if (order < 4) {
			// We have a lower class of problem. 
			if (order == 3) {
				// We have a cubic. Organise parameters and solve. 
				Double_t cubicParams[4] = {b, c, d, e};
				solutions = TUCNPolynomial::Instance()->CubicRootFinder(cubicParams, roots);
				
			} else if (order == 2) {
				Double_t quadraticParams[3] = {c, d, e};
				solutions = TUCNPolynomial::Instance()->QuadraticRootFinder(quadraticParams, roots);

			} else if (order == 1) {
				// We have a linear equation. 
				assert(d != 0.0);
				roots[0] = -e/d;
				solutions = 1;
				
			} else if (order == 0) {
				// No solutions - no intersection
				solutions = 0;
				
			} else {
				throw runtime_error("TimeOutsideInsideAlongParabolaS - Order of polynomial less than 0!");
			}	
		} else {
			// We have a quartic equation
			solutions = TUCNPolynomial::Instance()->QuarticRootFinder(params, roots);
		}
		
		#ifdef VERBOSE_MODE				
			cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "Solutions: " <<  solutions << endl;
			for (Int_t i = 0; i < solutions; i++) {
				cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "i: " << i << "  Root: " << roots[i] << endl;
			}
		#endif
		
		// -- Find the smallest, positive, non-zero, real root of the polynomial
		for (Int_t i = 0; i < solutions; i++) {
			// -- Check if the current root positive and non-zero
			if (roots[i] > 0.0 && IsNextPointOnTube(point, velocity, field, rmax, boundary, roots[i]) == kTRUE) {
				if (tfinal == 0.0) {
					// -- If a particular root is positive and tfinal has not yet been set, then intialise tfinal with that value
					tfinal = roots[i];
				} else if (roots[i] < tfinal) {
					// -- If tfinal has been set to a certain solution, check if current solution is smaller and if so set that to be the current smallest tfinal
					tfinal = roots[i];
				}
			} else {
				// -- Root is negative or zero, nothing to see here...
				continue;
			}
		}
		#ifdef VERBOSE_MODE				
			cout << "TimeFromOutsideAlongParabolaS" << "\t" <<  "RMAX BOUNDARY --- tfinal: " <<  tfinal << endl;
		#endif
		
	} else {
		throw runtime_error("TimeFromOutsideAlongParabolaS - rmax <= 0");
	}
	
	// Find the smallest root of the polynomial
	if (tfinal > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TimeFromOutsideAlongParabolaS - Time to nearest boundary: " <<  tfinal << endl; 
		#endif
		return tfinal;
	} 	else if (tfinal == 0.) {
		#ifdef VERBOSE_MODE		
			cout << "TimeFromOutsideAlongParabolaS - No Boundary hit" << endl;
		#endif
		return TGeoShape::Big();
	} else {
		throw runtime_error("In TUCNGeoBBox, TimeFromOutsideAlongParabola - Calculation error - time to boundary is negative");
		return 0.;
	}
}

// -----------------------------------------------------------------------------------------
Bool_t TUCNGeoTube::IsNextPointOnTube(Double_t* point, Double_t* velocity, Double_t* field, Double_t radius, const Double_t dz, const Double_t t)
{
	// Calculate the proposed intersection point given a time, starting point, direction and local field. 
	// Check that this intersection is actually located on the surface of the tube. 
	#ifdef VERBOSE_MODE		
		cout << "IsNextPointOnTube - Calling method. t: " <<  t << endl;
	#endif
	
	Double_t rBoundary = radius + 1.e-10; // Including a tolerance factor
	Double_t zBoundary = dz + 1.e-10;
	
	// Calculate the intersection point. If point coords are within limits of the boundary return TRUE, else point is outside; return FALSE.
	Double_t crossedpoint[3] = {0.,0.,0.};
	for (Int_t i = 0; i < 3; ++i) {
		crossedpoint[i] = point[i] + velocity[i]*t + 0.5*field[i]*t*t;
		#ifdef VERBOSE_MODE		
			cout << "IsNextPointOnTube - Crossing Point: i: " << i << "  -  " << crossedpoint[i] << endl;
		#endif
	}
	
	// Check point is within +/- dz, the endcaps
	if (TMath::Abs(crossedpoint[2]) > zBoundary) {
		#ifdef VERBOSE_MODE		
			cout << "IsNextPointOnTube - Point not within tube endcaps. Returning false" << endl;
		#endif
		return kFALSE;
	}
	// Check point is within the radius of the tube
	if (TMath::Sqrt(crossedpoint[0]*crossedpoint[0] + crossedpoint[1]*crossedpoint[1]) > rBoundary) {
		#ifdef VERBOSE_MODE		
			cout << "IsNextPointOnTube - Point not within tube radius. Returning false" << endl;
		#endif
		return kFALSE;
	}
	
	#ifdef VERBOSE_MODE		
		cout << "IsNextPointOnTube - Point on surface of tube. Return true" << endl;
	#endif
	return kTRUE;
}
