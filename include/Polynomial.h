// Polynomial
// Author: Matthew Raso-Barnett  06/07/2009

#ifndef ROOT_Polynomial
#define ROOT_Polynomial

#ifndef ROOT_TObject
#include "TObject.h"
#endif

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Polynomial                                                       //
//                                                                       //
//  Class to hold some Polynomial root-finding methods                   //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class Polynomial : public TObject
{
protected:
	static Polynomial    *fgPolynomial;  
	
	Polynomial();
	Polynomial(const Polynomial&); 
   Polynomial& operator=(const Polynomial&);

private:
	
public:
	virtual ~Polynomial();
   
	static Polynomial* 	Instance();
	
	virtual Int_t		QuadraticRootFinder(const Double_t* params, Double_t* roots);
	virtual Int_t		QuarticRootFinder(const Double_t* params, Double_t* roots);
	virtual Int_t		CubicRootFinder(const Double_t* params, Double_t* roots);
	
	virtual Int_t		AnalyticCubicAlgorithm(const Double_t p, const Double_t q,
	 																const Double_t a2, Double_t* roots);
	virtual Int_t		AnalyticQuarticAlgorithm(const Double_t a3, const Double_t a2,
	 																const Double_t a1, const Double_t a0, Double_t* roots);

   ClassDef(Polynomial, 1)          // Polynomial singleton
};

#endif