// TUCNPolynomial
// Author: Matthew Raso-Barnett  06/07/2009

#ifndef ROOT_TUCNPolynomial
#define ROOT_TUCNPolynomial

#ifndef ROOT_TObject
#include "TObject.h"
#endif

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  TUCNPolynomial                                                       //
//                                                                       //
//  Class to hold some Polynomial root-finding methods                   //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class TUCNPolynomial : public TObject
{
protected:
	static TUCNPolynomial    *fgPolynomial;  
	
	TUCNPolynomial();
	TUCNPolynomial(const TUCNPolynomial&); 
   TUCNPolynomial& operator=(const TUCNPolynomial&);

private:
	
public:
	virtual ~TUCNPolynomial();
   
	static TUCNPolynomial* 	Instance();
	
	virtual Int_t 			QuadraticRootFinder(const Double_t* params, Double_t* roots);
	virtual Int_t 			QuarticRootFinder(const Double_t* params, Double_t* roots);
	virtual Int_t 			CubicRootFinder(const Double_t* params, Double_t* roots);
	
	virtual Int_t			AnalyticCubicAlgorithm(const Double_t p, const Double_t q, const Double_t a2, Double_t* roots);
	 
   ClassDef(TUCNPolynomial, 1)          // Polynomial singleton
};

#endif
