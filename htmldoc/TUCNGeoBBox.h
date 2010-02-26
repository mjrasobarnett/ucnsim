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
	protected :
	// data members

	// methods
		
	private:
		
	
	public:
	   // constructors
	   TUCNGeoBBox();
	   TUCNGeoBBox(Double_t dx, Double_t dy, Double_t dz, Double_t *origin=0);
	   TUCNGeoBBox(const char *name, Double_t dx, Double_t dy, Double_t dz, Double_t *origin=0);
	   TUCNGeoBBox(Double_t *param);
	   // destructor
	   virtual ~TUCNGeoBBox();
	   
		// methods
		virtual Double_t      	DistFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, Double_t boundarytime, 
																				Double_t stepsize=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;
		static  Double_t      	DistFromInsideAlongParabola(const Double_t *point,const Double_t *dir, 
		                                   							Double_t dx, Double_t dy, Double_t dz, const Double_t *origin, Double_t stepmax=TGeoShape::Big());
		virtual Double_t      	DistFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, Double_t boundarytime, 
																				Double_t stepsize=TGeoShape::Big(), Int_t iact=1, Double_t *safe=0) const;
		static  Double_t      	DistFromOutsideAlongParabola(const Double_t *point,const Double_t *dir, 
		                                   							Double_t dx, Double_t dy, Double_t dz, const Double_t *origin, Double_t stepmax=TGeoShape::Big());
		virtual Double_t 			TimeFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field) const;
		virtual Double_t 			TimeFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field) const;
		virtual Double_t			DistanceAlongParabola(Double_t* velocity, Double_t* field, Double_t time)	const;
		virtual Bool_t		    	IsNextPointOnSurface(Double_t* point, Double_t* velocity, Double_t* field, Double_t t) const;
																			
		ClassDef(TUCNGeoBBox, 1)          // UCNGeoBBox
};

#endif