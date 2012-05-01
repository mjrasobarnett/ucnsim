// GravField
// Author: Matthew Raso-Barnett  22/05/2009

#ifndef ROOT_GravField
#define ROOT_GravField

#include "TNamed.h"
#include "TVector3.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						GravField - Defined in the							  //
//                   Global Coordinate frame of TOP							  //               
//                                      											  //
////////////////////////////////////////////////////////////////////////////

class GravField : public TNamed
{

private:
	static const double fGravAcceleration; //*m/(s*s);
	TVector3 						fGlobalFieldVector;
	
public:
	
	// -- constructors
	GravField();
  GravField(double nx, double ny, double nz);
  GravField(const GravField&);
	GravField& operator=(const GravField&);
	   
	// -- destructor
   virtual ~GravField();
	
	// -- methods
	void 					Direction(Double_t nx, Double_t ny, Double_t nz);
	Double_t 			Nx() const { return fGlobalFieldVector.X(); }
	Double_t 			Ny() const { return fGlobalFieldVector.Y(); }
	Double_t 			Nz() const { return fGlobalFieldVector.Z(); }
	Double_t 			Gx() const { return fGravAcceleration*(fGlobalFieldVector.X()); }
	Double_t 			Gy() const { return fGravAcceleration*(fGlobalFieldVector.Y()); }
	Double_t 			Gz() const { return fGravAcceleration*(fGlobalFieldVector.Z()); }
	
	Double_t 			GravAcceleration() const { return fGravAcceleration; }
		
   ClassDef(GravField, 1)          // Gravitational Field Vector
};

#endif
