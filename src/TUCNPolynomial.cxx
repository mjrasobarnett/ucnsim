// TUCNPolynomial
// Author: Matthew Raso-Barnett  06/07/2009
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cassert>

#include "TMath.h"
#include "TGeoShape.h"

#include "TUCNPolynomial.h"
#include "Units.h"

using std::cout;
using std::endl;
using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(TUCNPolynomial)

TUCNPolynomial *TUCNPolynomial::fgPolynomial = NULL;

//_____________________________________________________________________________
TUCNPolynomial::TUCNPolynomial()
				 	:TObject()
{
// -- Default constructor.
   Info("TUCNPolynomial", "Constructor");
	fgPolynomial = this;
} 

//_____________________________________________________________________________
TUCNPolynomial::TUCNPolynomial(const TUCNPolynomial& other)
				 	:TObject(other)
{
// -- Copy constructor.
   Error("TUCNPolynomial","Copying not allowed for TUCNPolynomial");
} 

//_____________________________________________________________________________
TUCNPolynomial::~TUCNPolynomial()
{
// -- Destructor.
   Info("TUCNPolynomial", "Destructor");
	fgPolynomial = NULL;
}   

//_____________________________________________________________________________
TUCNPolynomial &TUCNPolynomial::operator=(const TUCNPolynomial&)
{
// -- Assignment.
   Error("TUCNPolynomial","Assignment not allowed for TGeoBuilder");
   return *this;
}

//_____________________________________________________________________________
TUCNPolynomial* TUCNPolynomial::Instance() 
{
// -- Return pointer to singleton.  
   if (!fgPolynomial) fgPolynomial = new TUCNPolynomial();
	return fgPolynomial;
}   

//_____________________________________________________________________________
Int_t TUCNPolynomial::QuadraticRootFinder(const Double_t* params, Double_t* roots)
{
// Solve quadratic equation and return number of REAL roots if any, and return their values.
	Int_t numberOfSolutions = 0;
	
	// Ensure that potential roots are set to zero to begin with.
	roots[0] = 0.0;
	roots[1] = 0.0;
			
	// The constants of quadratic equation
	Double_t a, b, c;
	a = params[0];
	b = params[1]; 
	c = params[2];
			
	// Sometimes a,b or c (usually a) can be extremely small but non-zero, ie: (10^-16), due to computational precision I imagine. This
	// then screws up the calculations below.
	if (TMath::Abs(a) < TGeoShape::Tolerance()) {
		a = 0.;
	}
	if (TMath::Abs(b) < TGeoShape::Tolerance()) {
		b = 0.;
	}
	if (TMath::Abs(c) < TGeoShape::Tolerance()) {
		c = 0.;
	}		
	#ifdef VERBOSE_MODE		
		Info("QuadraticRootFinder","a: %f, b: %f, c: %f", a, b, c);
	#endif
		
	// -- Check if we have a linear, rather than quadratic equation
	if (a == 0.0) {
		// --------------------------------------------------
		// -- CASE 0: Linear Equation - One potential root
		// --------------------------------------------------
		// -- Check that b > 0. Otherwise there is no solution. 
		if (b == 0.0) {
			numberOfSolutions = 0;
		} else {
			Double_t t = -c/b;
			if (t <= 0.) { 
				numberOfSolutions = 0;
				#ifdef VERBOSE_MODE		
					Info("QuadraticRootFinder", "Linear eqn. - t < 0. No Solution."); 
				#endif
			} else {
				numberOfSolutions = 1;
				roots[0] = t;
				#ifdef VERBOSE_MODE		
					Info("QuadraticRootFinder","Linear eqn. t: %f", roots[0]);
				#endif
			}
		}
		return numberOfSolutions;
	} 
	
	// If c = 0, then one of the solutions must be zero (since we can then factor a t out of each term), and so we actually have a linear
	// equation. Set the second root of the "roots" storage to zero and solve the linear equation for the final root. 
	if (c == 0.0) {
		#ifdef VERBOSE_MODE		
			Info("CubicRootFinder", "Constant parameter is zero, implying one solution is zero. Remaining equation is now a quadratic.");
		#endif
		assert(a != 0.0); // Catch in case this code is moved above the previous 'if' block for some reason. 
		roots[1] = 0.0;
		roots[0] = b/a;
		numberOfSolutions = 1;
		return numberOfSolutions;
	}
	
	
	// -- Have quadratic equation. Check the discriminant to determine number of roots 
	Double_t discriminant = b*b - 4*a*c;
	#ifdef VERBOSE_MODE		
		Info("QuadraticRootFinder","Quad. eqn - b*b - 4*a*c: %f", discriminant);
	#endif	
	if (discriminant > 0.0) {
		// --------------------------------------------------
		// -- CASE 1: Two Real Roots
		// --------------------------------------------------
		numberOfSolutions = 2;
		roots[0] = (-b + TMath::Sqrt(b*b - 4*a*c))/(2*a);
		roots[1] = (-b - TMath::Sqrt(b*b - 4*a*c))/(2*a);
		#ifdef VERBOSE_MODE		
			Info("QuadraticRootFinder","2 Solutions - t+: %f, t-: %f", roots[0], roots[1]);
		#endif
	} else if (discriminant == 0.0) {
		// --------------------------------------------------
		// -- CASE 2: One Real Root
		// --------------------------------------------------
		Double_t t = -b/(2*a);
		// Check if root is negative 
		if (t < 0.) { 
			numberOfSolutions = 0;
			#ifdef VERBOSE_MODE		
				Info("QuadraticRootFinder", "1 Solution - solution is negative."); 
			#endif
		} else {
			roots[0] = t;
			numberOfSolutions = 1;
			#ifdef VERBOSE_MODE		
				Info("QuadraticRootFinder","1 Solution - t: %f", roots[0]);
			#endif
		}
	} else {
		// --------------------------------------------------
		// -- CASE 3: No Real Roots
		// --------------------------------------------------
		numberOfSolutions = 0;
		#ifdef VERBOSE_MODE		
			Info("QuadraticRootFinder","0 Solutions");
		#endif
	}
	
//	// -- Ensure solutions are within tolerance
//	for(Int_t i=0; i < numberOfSolutions; i++) {
//		if (TMath::Abs(roots[i]) < TGeoShape::Tolerance()) {
//			roots[i] = 0.0;
//		}
//	}
	return numberOfSolutions; 	
}

//_____________________________________________________________________________
Int_t TUCNPolynomial::CubicRootFinder(const Double_t* params, Double_t* roots)
{
	// Solves cubic equation for a polynomial of form ax^3 + bx^2 + cx + d = 0
	
	// -- 1. First get cubic into the standard form: x^3 + a2*x^2 + a1*x + a0 = 0   --  see notes
	// Define parameters and check they are within tolerance values
	Double_t a = params[0], b = params[1], c = params[2], d = params[3];
	if (TMath::Abs(a) < TGeoShape::Tolerance()) {
		a = 0.;
	}
	if (TMath::Abs(b) < TGeoShape::Tolerance()) {
		b = 0.;
	}
	if (TMath::Abs(c) < TGeoShape::Tolerance()) {
		c = 0.;
	}
	if (TMath::Abs(d) < TGeoShape::Tolerance()) {
		d = 0.;
	}
	assert(a != 0.0); // Check that we actually have a cubic equation
	#ifdef VERBOSE_MODE		
		Info("CubicRootFinder", "Input Params -  a: %f,  b: %f,  c: %f,  d: %f", a, b, c, d);
	#endif
	
	// If d = 0, then one of the solutions must be zero (since we can then factor a t out of each term), and so we actually have a quadratic
	// equation. Set the third root of the "roots" storage to zero and pass the remaining parameters to the quadratic root finder. 
	if (d == 0.0) {
		#ifdef VERBOSE_MODE		
			Info("CubicRootFinder", "Constant parameter is zero, implying one solution is zero. Remaining equation is now a quadratic.");
		#endif
		roots[2] = 0.0;
		Double_t quadraticParams[3] = {a, b, c};
		return this->QuadraticRootFinder(quadraticParams, roots);
	}
	
	// Define the constants of the standard form
	Double_t a2 = b/a, a1 = c/a, a0 = d/a; 
	#ifdef VERBOSE_MODE		
		Info("CubicRootFinder", "Eliminate leading coefficient -  a2: %f,  a1: %f,  a0: %f", a2, a1, a0);
	#endif
	
	// -- 2. Get rid of the quadratic term through the substitution -- x = u - p/3
	// -- u^3 + pu - q = 0
	Double_t p = (3.0*a1 - (a2*a2))/3.0;
	Double_t q = ((9.0*a1*a2) - (27.0*a0) - (2.0*a2*a2*a2))/27.0 ;
	
	#ifdef VERBOSE_MODE		
		Info("CubicRootFinder", "Vieta's Form -  p: %f,  q: %f", p, q);
	#endif	
	// -- Choose Algorithm to use
	return this->AnalyticCubicAlgorithm(p, q, a2, &roots[0]);
}

//_____________________________________________________________________________
Int_t TUCNPolynomial::QuarticRootFinder(const Double_t* params, Double_t* roots)
{
	// -- Solves quartic equation for all REAL solutions to a polynomial of form ax^4 + bx^3 + cx^2 + dx + e = 0
	// -- Takes 5 parameters and 4 roots as arguments.
	
	// -- Step 1. First get quartic into the standard form: x^4 + a3*x^3 + a2*x^2 + a1*x + a0 = 0   --  see notes
	
	// Define parameters and check they are within tolerance values
	Double_t a = params[0], b = params[1], c = params[2], d = params[3], e = params[4];
	if (TMath::Abs(a) < TGeoShape::Tolerance()) {
		a = 0.;
	}
	if (TMath::Abs(b) < TGeoShape::Tolerance()) {
		b = 0.;
	}
	if (TMath::Abs(c) < TGeoShape::Tolerance()) {
		c = 0.;
	}
	if (TMath::Abs(d) < TGeoShape::Tolerance()) {
		d = 0.;
	}
	if (TMath::Abs(e) < TGeoShape::Tolerance()) {
		e = 0.;
	}
	assert(a != 0.0); // Check that we actually have a quartic equation
	#ifdef VERBOSE_MODE		
		Info("QuarticRootFinder","a: %f, b: %f, c: %f, d: %f, e: %f", a, b, c, d, e);
	#endif
	
//	if (e > 0.0) cout << "\t" << "e: " << e << endl;
	
	// If e = 0, then one of the solutions must be zero (since we can then factor a t out of each term), and so we actually have a cubic
	// equation. Set the final root of the "roots" storage to zero and pass the remaining parameters to the cubic root finder. 
	if (e == 0.0) {
		#ifdef VERBOSE_MODE		
			Info("QuarticRootFinder", "Constant parameter is zero, implying one solution is zero. Remaining equation is now a cubic.");
		#endif
		roots[3] = 0.0;
		Double_t cubicParams[4] = {a, b, c, d};
		return this->CubicRootFinder(cubicParams, roots);
	}
	
	// Define the constants of the standard form
	const Double_t a3 = b/a;
	const Double_t a2 = c/a;
	const Double_t a1 = d/a, a0 = e/a;
	
	// -- Step 2. (Following the notes - jumped a bit here) - Now we need to solve the resolvent cubic to find alpha
	// -- z^3 + c1*z^2 + c2*z + c3 = 0
	const Double_t c1 = -a2;
	const Double_t c2 = ((a1*a3) - (4.0*a0));
	const Double_t c3 = ((4.0*a2*a0) - (a1*a1) - (a3*a3*a0));
	
	#ifdef VERBOSE_MODE		
		Info("QuarticRootFinder","a3: %f, a2: %f, a1: %f, a0: %f",a3,a2,a1,a0);
		Info("QuarticRootFinder","c1: %f, c2: %f, c3: %f",c1,c2,c3);
	#endif
	
	Double_t resolventCubicParams[4] = {1.0, c1, c2, c3};
	Double_t resolventCubicRoots[3] = {0.,0.,0.};
		
	this->CubicRootFinder(resolventCubicParams, resolventCubicRoots);
		
	#ifdef VERBOSE_MODE		
		Info("QuarticRootFinder","Cubic Roots -- Root1: %f, Root2: %f, Root3: %f",resolventCubicRoots[0], resolventCubicRoots[1], resolventCubicRoots[2]);
	#endif
	
	// Take ANY one REAL solution of the cubic (even if it is zero)
	const Double_t cubicSolution = resolventCubicRoots[0];

	// -- Step 3. Use solution of the cubic to solve quartic for all Real roots
	Int_t quarticSolutions = 4; // Counter of Real solutions to quartic
	
	// Calculate W 
	Double_t WSquared = ((0.25*a3*a3) - a2 + cubicSolution);
	assert(WSquared >= 0.0);	
	if (TMath::Abs(WSquared) < TGeoShape::Tolerance()) {
		WSquared = 0.0;
	}
	
	Double_t W = TMath::Sqrt(WSquared);
	
	// Calculate U, V
	Double_t U = 0.0;
	Double_t V = 0.0;
	Double_t USquared = 0.;
	Double_t VSquared = 0.;
	
	if (W == 0.0) {
		if ((cubicSolution*cubicSolution - 4.0*a0) < 0.0) {
			USquared = 0.0;
			VSquared = 0.0;
		} else {
			assert((cubicSolution*cubicSolution - 4.0*a0) > 0.0);
			
			USquared = (0.75*a3*a3) - 2.0*a2 + 2.0*TMath::Sqrt(cubicSolution*cubicSolution - 4.0*a0);
			VSquared = (0.75*a3*a3) - 2.0*a2 - 2.0*TMath::Sqrt(cubicSolution*cubicSolution - 4.0*a0);	
		}
	} else {
		USquared = (0.75*a3*a3) - W*W - 2.0*a2 + ((0.25*(4.0*a3*a2 - 8.0*a1 - a3*a3*a3))/W);
		VSquared = (0.75*a3*a3) - W*W - 2.0*a2 - ((0.25*(4.0*a3*a2 - 8.0*a1 - a3*a3*a3))/W);
	}
	
	// Calculate the 4 possible roots -- if imaginary, set to zero
	Double_t solutions[4] = {0.,0.,0.,0.};
	if (USquared >= 0.0 && VSquared >= 0.0) {
		U = TMath::Sqrt(USquared);	
		V = TMath::Sqrt(VSquared);
		solutions[0] = -0.25*a3 + 0.5*W + 0.5*U;
		solutions[1] = -0.25*a3 + 0.5*W - 0.5*U;
		solutions[2] = -0.25*a3 - 0.5*W + 0.5*V;
		solutions[3] = -0.25*a3 - 0.5*W - 0.5*V;
	} else if (USquared >= 0.0) {
		// Two Roots are real, two roots are imaginary
		U = TMath::Sqrt(USquared);	
		solutions[0] = -0.25*a3 + 0.5*W + 0.5*U;
		solutions[1] = -0.25*a3 + 0.5*W - 0.5*U;
		// Update the root counter to reflect this
		quarticSolutions -= 2;
	} else if (VSquared >= 0.0) {
		// Two Roots are real, two roots are imaginary
		V = TMath::Sqrt(VSquared);
		solutions[0] = -0.25*a3 - 0.5*W + 0.5*V;
		solutions[1] = -0.25*a3 - 0.5*W - 0.5*V;
		// Update the root counter to reflect this
		quarticSolutions -= 2;
	} else {
		// All roots are imaginary
		quarticSolutions -= 4;
	}
	
//	assert(quarticSolutions > 0);
	// Update roots storage with the solutions found
	for (Int_t i = 0; i < quarticSolutions; i++) {		
		roots[i] = solutions[i];
		
		// Ensure solution is within tolerance
		// WARNING:: This is a bit of a fudge here! - During testing the value of this tolerance was important as occasionally a root was returned with
		// a value of ~2.E-10. This caused the neutron to escape, and therefore I fudged it by reducing the tolerance to 1.E-9.This is a sign that 
		// numerically I have overlooked something, but I have yet to spot where.
		// UPDATE:: This doesn't work, as if the number of steps, or time of propagation is increased, eventually the particle will still escape. There seems
		// to be plenty of occasions where we have 1.E-9, -8, -7.... solutions. 
//		if (TMath::Abs(roots[i]) < TGeoShape::Tolerance()) {
//			cout << "Quartic solution < tolerance --- Root: " << i << "\t" << roots[i] << "\t" << a3 << "\t" << W << "\t" << U << endl;
//			roots[i] = 0.0; 
//		}
	}
	#ifdef VERBOSE_MODE		
		Info("QuarticRootFinder","W: %e, U: %e, V: %e",W,U,V);
		Info("QuarticRootFinder","No. of Real Solutions: %i",quarticSolutions);
		Info("QuarticRootFinder","Root 1: %e, Root 2: %e, Root 3: %e, Root 4: %e", roots[0], roots[1], roots[2], roots[3]);
	#endif
		
	return quarticSolutions;
}

//_____________________________________________________________________________
Int_t TUCNPolynomial::AnalyticCubicAlgorithm(const Double_t p, const Double_t q, const Double_t a2, Double_t* roots)
{
	// -- Calculate all the Real roots of the cubic equation using the analytic result from Cardano
	
	// Calculate the polynomial discriminant to tell us how many solutions to expect
	Double_t discriminant = ((p*p*p)/27.0) + ((q*q)/4.0);
	Int_t expectedSolutions = 0;
	#ifdef VERBOSE_MODE				
		Info("AnalyticCubicAlgorithm", "Input Params -  p: %f,  q: %f, a2: %f", p, q, a2);
		Info("AnalyticCubicAlgorithm", "Polynomial Discriminant -  D: %f", discriminant);
	#endif
	if (discriminant > 0.0) {
		#ifdef VERBOSE_MODE				
			Info("AnalyticCubicAlgorithm", "One Real Roots - Two Imaginary Roots");
		#endif
		expectedSolutions = 1;
	} else if (discriminant == 0.0) {
		#ifdef VERBOSE_MODE				
			Info("AnalyticCubicAlgorithm", "Three Real Roots - At least two are equal");
		#endif
		expectedSolutions = 3;
	} else {
		#ifdef VERBOSE_MODE				
			Info("AnalyticCubicAlgorithm", "Three Distinct Real Roots");
		#endif
		expectedSolutions = 3;
	}
	
	// -- Calculate all Real Roots 
	Int_t realSolutions = 0;
	
	// -- Step 0 -- Check that equation is not trivial to solve
	if (p == 0.0) {
		// Then equation is of form x^3 = q
		if (q > 0.0) {
			roots[0] = TMath::Power(q, (1.0/3.0)); // All real numbers have exactly ONE real cube root
			realSolutions = 1;
			
		} else if (q < 0.0) {
			// The cube root of a negative number is just -1 * cube root of the positive number  
			roots[0] = -TMath::Power(TMath::Abs(q),(1.0/3.0));
			realSolutions = 1;
	
		} else {
			roots[0] = 0.0;
			roots[1] = 0.0;
			roots[2] = 0.0;
			realSolutions = 3;
		}
		#ifdef VERBOSE_MODE				
			Info("AnalyticCubicAlgorithm", "CASE 0 - Equation is trivial to solve - 1 Real (or repeated) Root: %f", roots[0]);
		#endif
	} else {
		
	// -- Equation is not trivial -- Solve using analytical algorithm
		// -- Step 1 -- Define C
		Double_t C = 0.5*q*(TMath::Power((3./TMath::Abs(p)), 1.5));
		#ifdef VERBOSE_MODE				
			Info("AnalyticCubicAlgorithm", "C: %f", C);
		#endif
		// -- Step 2 -- Calculate y 
		Double_t y[3] = {0.0, 0.0, 0.0};
	
		// -- CASE 1 ------------  p > 0  ----  One Real Solution - 2 Imaginary Solutions
		if (p > 0.0) {
			y[0] = TMath::SinH(TMath::ASinH(C)/3.0);
			roots[0] = 2.0*TMath::Sqrt((TMath::Abs(p)/3.0))*y[0] - (a2/3.0);
			#ifdef VERBOSE_MODE
				Info("AnalyticCubicAlgorithm", "CASE 1 -  y[0]: %f", y[0]);
				Info("AnalyticCubicAlgorithm", "1 Real Root: %f", roots[0]);
			#endif
			realSolutions = 1;
		
		} else {
		// -- CASE 2 ------------  p < 0
			// -- CASE 2a: C >= 1.0  		----  One Real Solution - 2 Imaginary Solutions
			if (C >= 1.0) {
				y[0] = TMath::CosH(TMath::ACosH(C)/3.0);
				roots[0] = 2.0*TMath::Sqrt((TMath::Abs(p)/3.0))*y[0] - (a2/3.0);
				#ifdef VERBOSE_MODE
					Info("AnalyticCubicAlgorithm", "CASE 2a -  y[0]: %f", y[0]);
					Info("AnalyticCubicAlgorithm", "1 Real Root: %f", roots[0]);
				#endif
				realSolutions = 1;
			
			// -- CASE 3: C <= -1.0  		----  One Real Solution - 2 Imaginary Solutions	
			} else if (C <= -1.0) {
				y[0] = -TMath::CosH(TMath::ACosH(TMath::Abs(C))/3.0);
				roots[0] = 2.0*TMath::Sqrt((TMath::Abs(p)/3.0))*y[0] - (a2/3.0);
				#ifdef VERBOSE_MODE
					Info("AnalyticCubicAlgorithm", "CASE 2b -  y[0]: %f", y[0]);
					Info("AnalyticCubicAlgorithm", "1 Real Root: %f", roots[0]);
				#endif
				realSolutions = 1;
		
			// -- CASE 4: -1.0 < C < 1.0  ----  Three Real Solutions (may not be distinct roots)	
			} else {
				y[0] = TMath::Cos(TMath::ACos(C)/3.0);
				y[1] = TMath::Cos((TMath::Pi()*2.0 + TMath::ACos(C))/3.0);
				y[2] = TMath::Cos((TMath::Pi()*2.0 - TMath::ACos(C))/3.0);
				for(Int_t i=0; i < 3; i++) {
					roots[i] = 2.0*TMath::Sqrt((TMath::Abs(p)/3.0))*y[i] - (a2/3.0);
				}
				#ifdef VERBOSE_MODE
					Info("AnalyticCubicAlgorithm", "CASE 2c -  y[0]: %f, y[1]: %f, y[2]: %f", y[0], y[1], y[2]);
					Info("AnalyticCubicAlgorithm", "3 Real Roots -- Root 1: %f, Root 2: %f, Root 3: %f", roots[0], roots[1], roots[2]);
				#endif
				realSolutions = 3;
			}
		}
	}
	// -- Ensure that the number of solutions we got matches that predicted by polynomial discriminant. Errors here mean a faulty algorithm
	assert(expectedSolutions == realSolutions);
	// -- Ensure solutions are within tolerance
	for(Int_t i=0; i < realSolutions; i++) {
		if (TMath::Abs(roots[i]) < TGeoShape::Tolerance()) {
			cout << "Cubic Solution < Tolerance --- Root/1.E-10: " << roots[i]/TGeoShape::Tolerance() << endl;
			roots[i] = 0.0;
		}
	}
	return realSolutions;
}



