// Polynomial
// Author: Matthew Raso-Barnett  06/07/2009
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cassert>

#include "TMath.h"
#include "TGeoShape.h"
#include "TGeoManager.h"
#include "TVirtualGeoTrack.h"

#include "gsl/gsl_poly.h"

#include "Polynomial.h"
#include "Units.h"

using std::cout;
using std::endl;
using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(Polynomial)

Polynomial *Polynomial::fgPolynomial = NULL;

//_____________________________________________________________________________
Polynomial::Polynomial()
           :TObject()
{
// -- Default constructor.
//   Info("Polynomial", "Constructor");
   fgPolynomial = this;
} 

//_____________________________________________________________________________
Polynomial::Polynomial(const Polynomial& other)
           :TObject(other)
{
// -- Copy constructor.
   Error("Polynomial","Copying not allowed for Polynomial");
} 

//_____________________________________________________________________________
Polynomial::~Polynomial()
{
// -- Destructor.
   Info("Polynomial", "Destructor");
   fgPolynomial = NULL;
}   

//_____________________________________________________________________________
Polynomial &Polynomial::operator=(const Polynomial&)
{
// -- Assignment.
   Error("Polynomial","Assignment not allowed for TGeoBuilder");
   return *this;
}

//_____________________________________________________________________________
Polynomial* Polynomial::Instance() 
{
// -- Return pointer to singleton.  
   if (!fgPolynomial) fgPolynomial = new Polynomial();
   return fgPolynomial;
}   

//_____________________________________________________________________________
Int_t Polynomial::QuadraticRootFinder(const Double_t* params, Double_t* roots)
{
   // -- Solves quadratic equation and returns number of REAL roots if any.
   // -- Now using GNU Scientific Library to do all polynomial root finding
   #ifdef VERBOSE_MODE
      cout << "QuadraticRootFinder - a: " << params[0] << "\t";
      cout << "b: " << params[1] << "\t" << "c: " << params[2] << endl;
   #endif
   Int_t numRoots = gsl_poly_solve_quadratic(params[0],params[1],params[2],&roots[0],&roots[1]);
   #ifdef VERBOSE_MODE
      cout << "QuadraticRootFinder - Num Roots: " << numRoots << endl;
      cout << "Root 1: " << roots[0] << "\t";
      cout << "Root 2: " << roots[1] << endl;
   #endif
   return numRoots;
}

//_____________________________________________________________________________
Int_t Polynomial::CubicRootFinder(const Double_t* params, Double_t* roots)
{
   // -- Solves cubic equation for a polynomial of form ax^3 + bx^2 + cx + d = 0
   Double_t a = params[0], b = params[1], c = params[2], d = params[3];
   // Check that we actually have a cubic equation
   assert(a != 0.0); 
   #ifdef VERBOSE_MODE
      cout << "CubicRootFinder - Input Params - ";
      cout << " a: " << a << "\t" << "b: " << b << "\t" << "c: " << c << "\t" << "d: " << d << endl;
   #endif
   // If d = 0, then one of the solutions must be zero (since we can then factor a 't'
   // out of each term), and so we actually have a quadratic equation. Set the third root
   // of the "roots" storage to zero and pass the remaining parameters to the quadratic root finder. 
   if (d == 0.0) {
      #ifdef VERBOSE_MODE
         cout << "CubicRootFinder - Constant parameter is zero, implying one solution is zero. ";
         cout << "Remaining equation is now a quadratic." << endl;
      #endif
      roots[2] = 0.0;
      Double_t quadraticParams[3] = {a, b, c};
      return this->QuadraticRootFinder(quadraticParams, roots);
   }
   // Get cubic into the standard form: x^3 + a2*x^2 + a1*x + a0 = 0  --  see notes
   Double_t a2 = b/a, a1 = c/a, a0 = d/a;
   // Use GNU Scientific Library polynomial algorithms to solve cubic
   Double_t realroots[3] = {0.,0.,0.};
   Int_t numRoots = gsl_poly_solve_cubic(a2, a1, a0, &realroots[0], &realroots[1], &realroots[2]);
   #ifdef VERBOSE_MODE
      cout << "---------------------------" << endl;
      cout << "GSL:Cubic" << endl;
      cout << "No. of Roots: " << numRoots << endl;
   #endif
   // Copy real roots into ouput
   for (UInt_t i = 0; i < 3; i++) {
      #ifdef VERBOSE_MODE
         cout << i << ": " << realroots[i] << "\t";
      #endif
      roots[i] = realroots[i];
   }
   #ifdef VERBOSE_MODE
      cout << endl << "---------------------------" << endl;
   #endif
   // Return number of real roots
   return numRoots;
}

//_____________________________________________________________________________
Int_t Polynomial::QuarticRootFinder(const Double_t* params, Double_t* roots)
{
   // -- Solves quartic equation for all REAL solutions to a polynomial of form
   // -- ax^4 + bx^3 + cx^2 + dx + e = 0 
   // -- Takes 5 parameters and 4 roots as arguments.	
   Double_t a = params[0], b = params[1], c = params[2], d = params[3], e = params[4];
   // Check that we actually have a quartic equation
   assert(a != 0.0); 
   #ifdef VERBOSE_MODE
      cout << "QuarticRootFinder - a: " << a << "\t" << "b: " << b << "\t" << "c: " << c << "\t" << "d: " << d << "\t" << "e: " << e << endl;
   #endif
   // If e = 0, then one of the solutions must be zero (since we can then factor a 't'
   // out of each term), and so we actually have a cubic equation. Set the 4th root
   // to zero and pass the remaining parameters to the cubic root finder. 
   if (e == 0.0) {
      #ifdef VERBOSE_MODE
         cout << "QuarticRootFinder - Constant parameter is zero, implying one solution is zero. Remaining equation is now a cubic." << endl;
      #endif
      roots[3] = 0.0;
      Double_t cubicParams[4] = {a, b, c, d};
      return this->CubicRootFinder(cubicParams, roots);
   }
   // Get quartic into the standard form: x^4 + a3*x^3 + a2*x^2 + a1*x + a0 = 0   --  see notes
   const Double_t a3 = b/a, a2 = c/a, a1 = d/a, a0 = e/a;
   // Solve Quartic using GNU Scientific Library's general polynomial complex solver
   double std_params[5] = { a0, a1, a2, a3, 1};
   double complexRoots[8];
   std::vector<Double_t> realroots;
   gsl_poly_complex_workspace* workspace = gsl_poly_complex_workspace_alloc(5);
   gsl_poly_complex_solve(std_params, 5, workspace, complexRoots);
   gsl_poly_complex_workspace_free(workspace);
   // Find only the REAL roots (by checking for Im = 0 to within defined tolerance)
   for (int i = 0; i < 4; i++) {
      if (TMath::Abs(complexRoots[2*i+1]) < Units::tolerance) {
         realroots.push_back(complexRoots[2*i]);
      }
   }
   #ifdef VERBOSE_MODE
      cout << "---------------------------" << endl;
      cout << "GSL::Complex Root Finder" << endl;
      cout << "No. of Roots: " << realroots.size() << endl;
   #endif
   // Copy real roots into ouput
   for (UInt_t i = 0; i < realroots.size(); i++) {
      #ifdef VERBOSE_MODE
         cout << i << ": " << realroots[i] << "\t";
      #endif
      roots[i] = realroots[i];
   }
   #ifdef VERBOSE_MODE
      cout << endl << "---------------------------" << endl;
   #endif
   return realroots.size();
}

//_____________________________________________________________________________
Int_t Polynomial::AnalyticQuadraticAlgorithm(const Double_t a, const Double_t b, const Double_t c, Double_t* roots)
{
   // Solve quadratic equation and return number of REAL roots if any, and return their values.
   Int_t numberOfSolutions = 0;
   // Ensure that potential roots are set to zero to begin with.
   roots[0] = 0.0;
   roots[1] = 0.0;
   #ifdef VERBOSE_MODE		
   	cout << "QuadraticRootFinder - a: " << a << "\t" << "b: " << b << "\t" << "c: " << c << endl;
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
   				cout << "QuadraticRootFinder - Linear eqn. - t < 0. No Solution." << endl; 
   			#endif
   		} else {
   			numberOfSolutions = 1;
   			roots[0] = t;
   			#ifdef VERBOSE_MODE		
   				cout << "QuadraticRootFinder - Linear eqn. t: " << roots[0] << endl;
   			#endif
   		}
   	}
   	return numberOfSolutions;
   } 

   // If c = 0, then one of the solutions must be zero (since we can then factor a t out of each term), and so we actually have a linear
   // equation. Set the second root of the "roots" storage to zero and solve the linear equation for the final root. 
   if (c == 0.0) {
   	#ifdef VERBOSE_MODE		
   		cout << "CubicRootFinder - Constant parameter is zero, implying one solution is zero. Remaining equation is now a quadratic." << endl;
   	#endif
   	assert(a != 0.0); // Catch in case this code is moved above the previous 'if' block for some reason. 
   	roots[1] = 0.0;
   	roots[0] = -b/a;
   	numberOfSolutions = 1;
   	return numberOfSolutions;
   }


   // -- We therefore have a quadratic equation. Check the discriminant to determine number of roots 
   Double_t discriminant = b*b - 4*a*c;
   #ifdef VERBOSE_MODE		
   	cout << "QuadraticRootFinder - Quad. eqn - b*b - 4*a*c: " << discriminant << endl;
   #endif	
   if (discriminant > 0.0) {
   	// --------------------------------------------------
   	// -- CASE 1: Two Real Roots
   	// --------------------------------------------------
   	numberOfSolutions = 2;
   	roots[0] = (-b + TMath::Sqrt(b*b - 4*a*c))/(2*a);
   	roots[1] = (-b - TMath::Sqrt(b*b - 4*a*c))/(2*a);
   	#ifdef VERBOSE_MODE		
   		cout << "QuadraticRootFinder - 2 Solutions - t+: " << roots[0] << "\t" << "t-: " << roots[1] << endl;
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
   			cout << "QuadraticRootFinder - 1 Solution - solution is negative." << endl; 
   		#endif
   	} else {
   		roots[0] = t;
   		numberOfSolutions = 1;
   		#ifdef VERBOSE_MODE		
   			cout << "QuadraticRootFinder - 1 Solution - t: " << roots[0] << endl;
   		#endif
   	}
   } else {
   	// --------------------------------------------------
   	// -- CASE 3: No Real Roots
   	// --------------------------------------------------
   	numberOfSolutions = 0;
   	#ifdef VERBOSE_MODE		
   		cout << "QuadraticRootFinder - 0 Solutions" << endl;
   	#endif
   }

   return numberOfSolutions; 	

   /*	ROOT::Math::Polynomial quadratic(a,b,c);
   std::vector<Double_t> realroots = quadratic.FindRealRoots();
   #ifdef VERBOSE_MODE
   	cout << "---------------------------" << endl;
   	cout << "ROOT::MATHMORE::Solver -- Quadratic" << endl;
   	cout << "No. of Roots: " << realroots.size() << endl;
   #endif
   for (UInt_t i = 0; i < realroots.size(); i++) {
   	#ifdef VERBOSE_MODE	
   		cout << i << "\t" << realroots[i] << "\t";
   	#endif
   	roots[i] = realroots[i];
   }
   #ifdef VERBOSE_MODE
   	cout << endl << "---------------------------" << endl;
   #endif
   return realroots.size();
   */
}

//_____________________________________________________________________________
Int_t Polynomial::AnalyticCubicAlgorithm(const Double_t p, const Double_t q, const Double_t a2, Double_t* roots)
{
	// -- Calculate all the Real roots of the cubic equation using the analytic result from Cardano
	
	// Calculate the polynomial discriminant to tell us how many solutions to expect
	Double_t discriminant = ((p*p*p)/27.0) + ((q*q)/4.0);
	Int_t expectedSolutions = 0;
	#ifdef VERBOSE_MODE
		cout << "AnalyticCubicAlgorithm - Input Params -  p: " << p << "\t" << "q: " << q << "\t" << "a2: " << a2 << endl;
		cout << "AnalyticCubicAlgorithm - Polynomial Discriminant -  D: " << discriminant << endl;
	#endif
	if (discriminant > 0.0) {
		#ifdef VERBOSE_MODE
			cout << "AnalyticCubicAlgorithm - One Real Roots - Two Imaginary Roots" << endl;
		#endif
		expectedSolutions = 1;
	} else if (discriminant == 0.0) {
		#ifdef VERBOSE_MODE
			cout << "AnalyticCubicAlgorithm - Three Real Roots - At least two are equal" << endl;
		#endif
		expectedSolutions = 3;
	} else {
		#ifdef VERBOSE_MODE
			cout << "AnalyticCubicAlgorithm - Three Distinct Real Roots" << endl;
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
			cout << "AnalyticCubicAlgorithm - CASE 0 - Equation is trivial to solve - 1 Real (or repeated) Root: " << roots[0] << endl;
		#endif
	} else {
		
	// -- Equation is not trivial -- Solve using analytical algorithm
		// -- Step 1 -- Define C
		Double_t C = 0.5*q*(TMath::Power((3./TMath::Abs(p)), 1.5));
		#ifdef VERBOSE_MODE
			cout << "AnalyticCubicAlgorithm - C: " << C << endl;
		#endif
		// -- Step 2 -- Calculate y 
		Double_t y[3] = {0.0, 0.0, 0.0};
	
		// -- CASE 1 ------------  p > 0  ----  One Real Solution - 2 Imaginary Solutions
		if (p > 0.0) {
			y[0] = TMath::SinH(TMath::ASinH(C)/3.0);
			roots[0] = 2.0*TMath::Sqrt((TMath::Abs(p)/3.0))*y[0] - (a2/3.0);
			#ifdef VERBOSE_MODE
				cout << "AnalyticCubicAlgorithm - CASE 1 -  y[0]: " << y[0] << endl;
				cout << "AnalyticCubicAlgorithm - 1 Real Root: " << roots[0] << endl;
			#endif
			realSolutions = 1;
		
		} else {
		// -- CASE 2 ------------  p < 0
			// -- CASE 2a: C >= 1.0  		----  One Real Solution - 2 Imaginary Solutions
			if (C >= 1.0) {
				y[0] = TMath::CosH(TMath::ACosH(C)/3.0);
				roots[0] = 2.0*TMath::Sqrt((TMath::Abs(p)/3.0))*y[0] - (a2/3.0);
				#ifdef VERBOSE_MODE
					cout << "AnalyticCubicAlgorithm - CASE 2a -  y[0]: " << y[0] << endl;
					cout << "AnalyticCubicAlgorithm - 1 Real Root: " << roots[0] << endl;
				#endif
				realSolutions = 1;
			
			// -- CASE 3: C <= -1.0  		----  One Real Solution - 2 Imaginary Solutions	
			} else if (C <= -1.0) {
				y[0] = -TMath::CosH(TMath::ACosH(TMath::Abs(C))/3.0);
				roots[0] = 2.0*TMath::Sqrt((TMath::Abs(p)/3.0))*y[0] - (a2/3.0);
				#ifdef VERBOSE_MODE
					cout << "AnalyticCubicAlgorithm - CASE 2b -  y[0]: " << y[0] << endl;
					cout << "AnalyticCubicAlgorithm - 1 Real Root: " << roots[0] << endl;
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
					cout << "AnalyticCubicAlgorithm - CASE 2c -  y[0]: " << y[0] << "\t" << "y[1]: " << y[1] << "\t" << "y[2]: " << y[2] << endl;
					cout << "AnalyticCubicAlgorithm - 3 Real Roots - Root 1: " << roots[0] << "\t" << "Root 2: " << roots[1] << "\t" << "Root 3: " << roots[2] << endl;
				#endif
				realSolutions = 3;
			}
		}
	}
	// -- Ensure that the number of solutions we got matches that predicted by polynomial discriminant. Errors here mean a faulty algorithm
	assert(expectedSolutions == realSolutions);
	return realSolutions;
}

//_____________________________________________________________________________
Int_t Polynomial::AnalyticQuarticAlgorithm(const Double_t a3, const Double_t a2, const Double_t a1, const Double_t a0, Double_t* roots)
{
	// -- Step 2. (Following the notes - jumped a bit here) - Now we need to solve the resolvent cubic to find alpha
	// -- z^3 + c1*z^2 + c2*z + c3 = 0
	const Double_t c1 = -a2;
	const Double_t c2 = ((a1*a3) - (4.0*a0));
	const Double_t c3 = ((4.0*a2*a0) - (a1*a1) - (a3*a3*a0));
	
	#ifdef VERBOSE_MODE
		cout << "QuarticRootFinder - a3: " << a3 << "\t" << "a2: " << a2 << "\t" << "a1: " << a1 << "\t" << "a0: " << a0 << endl;
		cout << "QuarticRootFinder - c1: " << c1 << "\t" << "c2: " << c2 << "\t" << "c3: " << c3 << endl;
	#endif
	
	Double_t resolventCubicParams[4] = {1.0, c1, c2, c3};
	Double_t resolventCubicRoots[3] = {0.,0.,0.};
		
	this->CubicRootFinder(resolventCubicParams, resolventCubicRoots);
		
	#ifdef VERBOSE_MODE
		cout << "QuarticRootFinder - Cubic Roots -- 1: " << resolventCubicRoots[0] << "\t" << "2: " << resolventCubicRoots[1] << "\t" << "3: " <<  resolventCubicRoots[2] << endl;
	#endif
	
	// Take ANY one REAL solution of the cubic (even if it is zero)
	const Double_t cubicSolution = resolventCubicRoots[0];
	#ifdef VERBOSE_MODE
		cout << "cubicSolution: " << cubicSolution << endl;
	#endif
	// -- Step 3. Use solution of the cubic to solve quartic for all Real roots
	Int_t quarticSolutions = 4; // Counter of Real solutions to quartic
	
	// Calculate W 
	Double_t WSquared = ((0.25*a3*a3) - a2) + cubicSolution;
	#ifdef VERBOSE_MODE
		cout << "a3: " << a3 << "\t" << "0.25*a3*a3: " << (0.25*a3*a3) << "\t" << "a2: " << a2 << "\t" << "cubicSolution: " << cubicSolution << endl;
		cout << "(0.25*a3*a3) - a2: " << (0.25*a3*a3) - a2 << endl;
		cout << "(0.25*a3*a3) - a2 + cubicSolution: " << (0.25*a3*a3) - a2 + cubicSolution << endl;
	#endif
	
	if (WSquared < 0.0) {
		cout << "TrackId: " << gGeoManager->GetCurrentTrack()->GetId() << "\t" << "WSquared: " << WSquared << endl;
		assert(WSquared > 0.0);
	}	
	
	Double_t W = TMath::Sqrt(WSquared);
	#ifdef VERBOSE_MODE
		cout << "WSquared: " << WSquared << "\t" << "W: " << W << endl;
	#endif
	
	
	// Calculate U, V
	Double_t U = 0.0;
	Double_t V = 0.0;
	Double_t USquared = 0.;
	Double_t VSquared = 0.;
	
	if (W == 0.0) {
		#ifdef VERBOSE_MODE		
			cout << "W = 0.0" << endl;
		#endif
		if ((cubicSolution*cubicSolution - 4.0*a0) < 0.0) {
			USquared = 0.0;
			VSquared = 0.0;
			#ifdef VERBOSE_MODE
				cout << "(cubicSolution*cubicSolution - 4.0*a0) < 0.0" << "\t" << "USquared: " << USquared << "\t" << "VSquared: " << VSquared << endl;
			#endif
		} else {
			assert((cubicSolution*cubicSolution - 4.0*a0) > 0.0);
			USquared = (0.75*a3*a3) - 2.0*a2 + 2.0*TMath::Sqrt(cubicSolution*cubicSolution - 4.0*a0);
			VSquared = (0.75*a3*a3) - 2.0*a2 - 2.0*TMath::Sqrt(cubicSolution*cubicSolution - 4.0*a0);	
			#ifdef VERBOSE_MODE
				cout << "(cubicSolution*cubicSolution - 4.0*a0) > 0.0" << "\t" << "USquared: " << USquared << "\t" << "VSquared: " << VSquared << endl;
			#endif
		}
	} else {
		USquared = (0.75*a3*a3) - W*W - 2.0*a2 + ((0.25*(4.0*a3*a2 - 8.0*a1 - a3*a3*a3))/W);
		VSquared = (0.75*a3*a3) - W*W - 2.0*a2 - ((0.25*(4.0*a3*a2 - 8.0*a1 - a3*a3*a3))/W);
		#ifdef VERBOSE_MODE
			cout << "W != 0.0" << "\t" << "USquared: " << USquared << "\t" << "VSquared: " << VSquared << endl;
		#endif
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
		#ifdef VERBOSE_MODE
			cout << "4 solutions: " << "\t" << "solution[0]: " << solutions[0] << "\t" << "solution[1]: " << solutions[1] << "\t" << "solution[2]: " << solutions[2] << "\t" << "solution[3]: " << solutions[3] << endl;
		#endif
	} else if (USquared >= 0.0) {
		// Two Roots are real, two roots are imaginary
		U = TMath::Sqrt(USquared);	
		solutions[0] = -0.25*a3 + 0.5*W + 0.5*U;
		solutions[1] = -0.25*a3 + 0.5*W - 0.5*U;
		// Update the root counter to reflect this
		quarticSolutions -= 2;
		#ifdef VERBOSE_MODE
			cout << "2 solutions: " << "\t" << "solution[0]: " << solutions[0] << "\t" << "solution[1]: " << solutions[1] << "\t" << "solution[2]: " << solutions[2] << "\t" << "solution[3]: " << solutions[3] << endl;
		#endif
	} else if (VSquared >= 0.0) {
		// Two Roots are real, two roots are imaginary
		V = TMath::Sqrt(VSquared);
		solutions[0] = -0.25*a3 - 0.5*W + 0.5*V;
		solutions[1] = -0.25*a3 - 0.5*W - 0.5*V;
		// Update the root counter to reflect this
		quarticSolutions -= 2;
		#ifdef VERBOSE_MODE
			cout << "2 solutions: " << "\t" << "solution[0]: " << solutions[0] << "\t" << "solution[1]: " << solutions[1] << "\t" << "solution[2]: " << solutions[2] << "\t" << "solution[3]: " << solutions[3] << endl;
		#endif
	} else {
		// All roots are imaginary
		quarticSolutions -= 4;
	}
	
//	assert(quarticSolutions > 0);
	
	// Update roots storage with the solutions found
	for (Int_t i = 0; i < quarticSolutions; i++) {		
		roots[i] = solutions[i];		
	}
	
	#ifdef VERBOSE_MODE
		cout << "QuarticRootFinder - W: " << W << "\t" << "U: " << U << "\t" << "V: " << V << endl;
		cout << "QuarticRootFinder - No. of Real Solutions: " << quarticSolutions << endl;
		cout << "QuarticRootFinder - Root 1: " << roots[0] << "\t" << "2: " << roots[1] << "\t" << "3: " << roots[2] << "\t" << "4: " << roots[3] << endl;
	#endif
	
	return quarticSolutions;
}
