// TUCNGravField
// Author: Matthew Raso-Barnett  22/05/2009

#ifndef ROOT_TUCNGravField
#define ROOT_TUCNGravField

#include "TObject.h"
#include "TVector3.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNGravField - Defined in the							  //
//                   Global Coordinate frame of TOP							  //               
//                                      											  //
////////////////////////////////////////////////////////////////////////////

class TUCNGeoManager;

class TUCNGravField : public TObject
{

protected:
	
	static TUCNGravField    *fgInstance;  
	
	// -- constructors
   TUCNGravField(const TUCNGravField&);
	TUCNGravField& operator=(const TUCNGravField&); 
   
private:
	
	TVector3 						fGlobalFieldVector;
	static const Double_t 		fGravAcceleration = 9.80665; //*m/(s*s);
	
public:
	
   TUCNGravField();

	static TUCNGravField* Instance();
	   
	// -- destructor
   virtual ~TUCNGravField();
	
	// -- methods
	void 					Direction(Double_t nx, Double_t ny, Double_t nz);
	Double_t 			Nx() const { return fGlobalFieldVector.X(); }
	Double_t 			Ny() const { return fGlobalFieldVector.Y(); }
	Double_t 			Nz() const { return fGlobalFieldVector.Z(); }
	Double_t 			Gx() const { return fGravAcceleration*(fGlobalFieldVector.X()); }
	Double_t 			Gy() const { return fGravAcceleration*(fGlobalFieldVector.Y()); }
	Double_t 			Gz() const { return fGravAcceleration*(fGlobalFieldVector.Z()); }
	
	Double_t 			GravAcceleration() const { return fGravAcceleration; }
		
   ClassDef(TUCNGravField, 1)          // Gravitational Field Vector
};

#endif
