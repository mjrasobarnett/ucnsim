// TUCNGeoTube
// Author: Matthew Raso-Barnett  22/05/2009

#ifndef ROOT_TUCNGeoTube
#define ROOT_TUCNGeoTube

#include "TUCNGeoBBox.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNGeoTube														  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNGeoTube : public TUCNGeoBBox 
{
	protected :
	// data members
	   Double_t              fRmin; // inner radius
	   Double_t              fRmax; // outer radius
	   Double_t              fDz;   // half length
	// methods
	
	public:
	   // constructors
	   TUCNGeoTube();
	   TUCNGeoTube(Double_t rmin, Double_t rmax, Double_t dz);
	   TUCNGeoTube(const char * name, Double_t rmin, Double_t rmax, Double_t dz);
	   TUCNGeoTube(Double_t *params);
	   // destructor
	   virtual ~TUCNGeoTube();
	   
		// methods
	   virtual Double_t      Capacity() const;
	   static  Double_t      Capacity(Double_t rmin, Double_t rmax, Double_t dz);
	   virtual void          ComputeBBox();
	   virtual void          ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm);
	   static  void          ComputeNormalS(Double_t *point, Double_t *dir, Double_t *norm,
	                                        Double_t rmin, Double_t rmax, Double_t dz);
	   virtual Bool_t        Contains(Double_t *point) const;
	   static  Double_t      DistFromInsideS(Double_t *point, Double_t *dir, Double_t rmin, Double_t rmax, Double_t dz);
	   virtual Double_t      DistFromInside(Double_t *point, Double_t *dir, Int_t iact=1, 
	                                   Double_t step=TGeoShape::Big(), Double_t *safe=0) const;
	   static  Double_t      DistFromOutsideS(Double_t *point, Double_t *dir, Double_t rmin, Double_t rmax, Double_t dz);
	   virtual Double_t      DistFromOutside(Double_t *point, Double_t *dir, Int_t iact=1, 
	                                   Double_t step=TGeoShape::Big(), Double_t *safe=0) const;
	   static  void          DistToTube(Double_t rsq, Double_t nsq, Double_t rdotn, Double_t radius, Double_t &b, Double_t &delta);
	   virtual Int_t         DistancetoPrimitive(Int_t px, Int_t py);
	   virtual TGeoVolume   *Divide(TGeoVolume *voldiv, const char *divname, Int_t iaxis, Int_t ndiv, 
	                                Double_t start, Double_t step);
	   virtual const char   *GetAxisName(Int_t iaxis) const;
	   virtual Double_t      GetAxisRange(Int_t iaxis, Double_t &xlo, Double_t &xhi) const;
	   virtual void          GetBoundingCylinder(Double_t *param) const;
	   virtual const TBuffer3D &GetBuffer3D(Int_t reqSections, Bool_t localFrame) const;
	   virtual Int_t         GetByteCount() const {return 48;}
	   virtual Bool_t        GetPointsOnSegments(Int_t npoints, Double_t *array) const;
	   virtual TGeoShape    *GetMakeRuntimeShape(TGeoShape *mother, TGeoMatrix *mat) const;
	   virtual void          GetMeshNumbers(Int_t &nvert, Int_t &nsegs, Int_t &npols) const;
	   virtual Int_t         GetNmeshVertices() const;
	   virtual Double_t      GetRmin() const {return fRmin;}
	   virtual Double_t      GetRmax() const {return fRmax;}
	   virtual Double_t      GetDz() const   {return fDz;}
	   Bool_t                HasRmin() const {return (fRmin>0)?kTRUE:kFALSE;}
	   virtual void          InspectShape() const;
	   virtual Bool_t        IsCylType() const {return kTRUE;}
	   virtual TBuffer3D    *MakeBuffer3D() const;
	   virtual Double_t      Safety(Double_t *point, Bool_t in=kTRUE) const;
	   static  Double_t      SafetyS(Double_t *point, Bool_t in, Double_t rmin, Double_t rmax, Double_t dz, Int_t skipz=0);
	   virtual void          SavePrimitive(ostream &out, Option_t *option = "");
	   void                  SetTubeDimensions(Double_t rmin, Double_t rmax, Double_t dz);
	   virtual void          SetDimensions(Double_t *param);
	   virtual void          SetPoints(Double_t *points) const;
	   virtual void          SetPoints(Float_t *points) const;
	   virtual void          SetSegsAndPols(TBuffer3D &buff) const;
	   virtual void          Sizeof3D() const;
		
		// -- new methods
		
		virtual Double_t 		TimeFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, 
																				Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;
		static  Double_t 		TimeFromInsideAlongParabolaS(Double_t* point, Double_t* velocity, Double_t* field, Double_t rmin, Double_t rmax, Double_t dz);
		virtual Double_t 		TimeFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, 
																				Double_t stepmax=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;
		static  Double_t 		TimeFromOutsideAlongParabolaS(Double_t* point, Double_t* velocity, Double_t* field, Double_t rmin, Double_t rmax, Double_t dz);

		static  Bool_t			IsNextPointOnTube(Double_t* point, Double_t* velocity, Double_t* field, const Double_t radius, const Double_t dz, const Double_t t);
		
   ClassDef(TUCNGeoTube, 1)     // UCNGeoTube
};

#endif
