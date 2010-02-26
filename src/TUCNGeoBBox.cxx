// TUCNGeoBBox
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <stdexcept>

#include "TMath.h"
#include "TParticle.h"
#include "TGeoManager.h"
#include "TVirtualGeoTrack.h"
#include "TGeoBBox.h"

#include "TUCNGeoManager.h"
#include "TUCNParticle.h"
#include "TUCNParabola.h"
#include "TUCNPolynomial.h"

#include "TUCNGeoBBox.h"

using std::cout;
using std::endl;
using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(TUCNGeoBBox)
   
//_____________________________________________________________________________
TUCNGeoBBox::TUCNGeoBBox()
{
// Default constructor
	Info("TUCNGeoBBox", "Default Constructor");
}   

//_____________________________________________________________________________
TUCNGeoBBox::TUCNGeoBBox(Double_t dx, Double_t dy, Double_t dz, Double_t *origin)
         	:TGeoBBox(dx, dy, dz, origin)
{
// Constructor where half-lengths are provided.
	Info("TUCNGeoBBox", "Constructor");
}

//_____________________________________________________________________________
TUCNGeoBBox::TUCNGeoBBox(const char *name, Double_t dx, Double_t dy, Double_t dz, Double_t *origin)
         	:TGeoBBox(name, dx, dy, dz, origin)
{
// Constructor with shape name.
   Info("TUCNGeoBBox", "Constructor");
}

//_____________________________________________________________________________
TUCNGeoBBox::TUCNGeoBBox(Double_t *param)
         	:TGeoBBox(param)
{
// Constructor based on the array of parameters
   Info("TUCNGeoBBox", "Constructor");

}   

//_____________________________________________________________________________
TUCNGeoBBox::~TUCNGeoBBox()
{
// Destructor
	Info("TUCNGeoBBox", "Destructor");
}

//_____________________________________________________________________________
Double_t TUCNGeoBBox::TimeFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, Double_t stepmax, Int_t iact, Double_t *safe) const
{
	// This method calculates the time of all possible intersections of the particle's future path with all possible boundaries of the current shape.
	// Method then compares the times found and returns the smallest, non-zero value. 
	
	#ifdef VERBOSE_MODE		
		cout << "TimeFromInsideAlongParabola - iact: " << iact << "\t" << "stepmax: " << stepmax << endl;
	#endif
	// ----------------------------------------------------------------------
	// -- For efficiency calculate the safety distance along straight line to surface. If this distance is greater than the proposed stepmax, then just return Big()
	// -- This code is from root's DistanceFrom... methods. 
	Double_t smin,saf[6];
   Double_t newpt[3];

	// -- Determining minimum distances to all boundaries from point
   for (Int_t i=0; i<3; i++) newpt[i] = point[i] - fOrigin[i];
   saf[0] = fDX+newpt[0];
   saf[1] = fDX-newpt[0];
   saf[2] = fDY+newpt[1];
   saf[3] = fDY-newpt[1];
   saf[4] = fDZ+newpt[2];
   saf[5] = fDZ-newpt[2];

   if (iact<3 && safe) {
      smin = saf[0];
      // compute safe distance
      for (Int_t i=1;i<6;i++) if (saf[i] < smin) smin = saf[i];
		*safe = smin;
      if (smin<0) *safe = 0.0;
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && stepmax<*safe) return TGeoShape::Big();
   }
	// ----------------------------------------------------------------------
	
	// -- Calculate actual time to boundary
	Double_t t = 0.; // The smallest time to reach ANY boundary;
	Double_t boundary[3] = {fDX, fDY, fDZ}; // Store the coordinates of the box boundaries
		
	#ifdef VERBOSE_MODE		
		cout << "TimeFromInsideAlongParabola - (Local) +Boundaries - +X: " << boundary[0] << "\t" << "+Y: " << boundary[1] << "\t" <<  "+Z: " << boundary[2] << endl;
		cout << "TimeFromInsideAlongParabola - (Local) point - +X: " << newpt[0] << "\t" << "+Y: " << newpt[1] << "\t" <<  "+Z: " << newpt[2] << endl;
		cout << "TimeFromInsideAlongParabola - (Local) velocity - +X: " << velocity[0] << "\t" << "+Y: " << velocity[1] << "\t" <<  "+Z: " << velocity[2] << endl;
		cout << "TimeFromInsideAlongParabola - (Local) field direction - +X: " << field[0] << "\t" << "+Y: " << field[1] << "\t" <<  "+Z: " << field[2] << endl;
	#endif
			
	// ----------------------------------------------------------------------	
	// -- New root-finding / selecting process implementing TUCNPolynomial
	// -- Algorithm for Selecting ROOTs for Approaching boundary from INSIDE
	// ----------------------------------------------------------------------	
	// -- Loop over x, y, z boundaries/projections of parabola
	for (Int_t i=0; i<3; i++) {
		// -- Loop over +/- fDX,fDY,fDZ boundaries
		for (Int_t j=0; j<2; j++) {
						
			// -- Define storage for the smallest non-zero root to the CURRENT boundary
			Double_t tmin = 0.0;

			// -- Define the constants in the quadratic equation, a, b, c - see note book for details
			Double_t params[3] = {0., 0., 0.};
			params[0] = 0.5*field[i];
			params[1] = velocity[i]; 
			params[2] = newpt[i] - (TMath::Power(-1,j)*boundary[i]);

			// -- Solve equation for potential roots
			Double_t roots[2] = {0., 0.};
			Int_t solutions = TUCNPolynomial::Instance()->QuadraticRootFinder(&params[0], &roots[0]);

			#ifdef VERBOSE_MODE		
				cout << i << "\t" << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
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
						cout << "TimeFromInsideAlongParabola - Both roots are negative or zero." << endl; 
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
			
			if (tmin > 0.0 && t == 0.0) {
				// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
				t = tmin;
			} else if (tmin > 0.0 && tmin < t) {
				// -- Check if this time, the smallest, non-zero time to the current boundary, is smaller than the current overall smallest time to any boundary
				t = tmin;
			}
		}
	}	
	
	// -- Analyse the final value for the time to nearest boundary from inside
	if (t > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TimeFromInsideAlongParabola - time to nearest boundary: " << t << endl; 
		#endif
		return t;
	} else if (t == 0.) {
		throw runtime_error("In TUCNGeoBBox, TimeFromInsideAlongParabola - Calculation error - no time to boundary found"); 
		return 0;
	} else {
		throw runtime_error("In TUCNGeoBBox, TimeFromInsideAlongParabola - Calculation error - time to boundary is negative");
		return 0;
	}
}

//_____________________________________________________________________________
Double_t TUCNGeoBBox::TimeFromInsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, 
																		Double_t dx, Double_t dy, Double_t dz, const Double_t *origin, Double_t /*stepmax*/)
{
	// This method calculates the time of all possible intersections of the particle's future path with all possible boundaries of the current shape.
	// Method then compares the times found and returns the smallest, non-zero value. 
		
	// ----------------------------------------------------------------------
	// -- For efficiency COULD calculate the distance along straight line to surface. If this distance is greater than the proposed stepmax, then just return Big()
	// -- This is yet to be implemented successfully
	Double_t newpt[3];
	
   // -- Determining minimum distances to all boundaries from point
	for (Int_t i=0; i<3; i++) newpt[i] = point[i] - origin[i];

	// ----------------------------------------------------------------------
	
	// -- Calculate actual time to boundary
	Double_t t = 0.; // The smallest time to reach ANY boundary;
	Double_t boundary[3] = {dx, dy, dz}; // Store the coordinates of the box boundaries
		
	#ifdef VERBOSE_MODE		
		cout << "TimeFromInsideAlongParabola - Positive (Local) Boundaries - +X: " << boundary[0] << "  +Y: " << boundary[1] << "  +Z: " << boundary[2] << endl;
		cout << "TimeFromInsideAlongParabola - (Local) point - +X: " << newpt[0] << "  +Y: " << newpt[1] << "  +Z: " << newpt[2] << endl;
		cout << "TimeFromInsideAlongParabola - (Local) velocity - +X: " << velocity[0] << "  +Y: " << velocity[1] << "  +Z: " << velocity[2] << endl;
		cout << "TimeFromInsideAlongParabola - (Local) field direction - +X: " << field[0] << "  +Y: " << field[1] << "  +Z: " << field[2] << endl;
	#endif
			
	// ----------------------------------------------------------------------	
	// -- New root-finding / selecting process implementing TUCNPolynomial
	// -- Algorithm for Selecting ROOTs for Approaching boundary from INSIDE
	// ----------------------------------------------------------------------	
	// -- Loop over x, y, z boundaries/projections of parabola
	for (Int_t i=0; i<3; i++) {
		// -- Loop over +/- fDX,fDY,fDZ boundaries
		for (Int_t j=0; j<2; j++) {
			// -- Define storage for the smallest non-zero root to the CURRENT boundary
			Double_t tmin = 0.0;
			
			// -- Define the constants in the quadratic equation, a, b, c - see note book for details
			Double_t params[3] = {0., 0., 0.};
			params[0] = 0.5*field[i];
			params[1] = velocity[i]; 
			params[2] = newpt[i] - (TMath::Power(-1,j)*boundary[i]);
			
			// -- Solve equation for potential roots
			Double_t roots[2] = {0., 0.};
			Int_t solutions = TUCNPolynomial::Instance()->QuadraticRootFinder(&params[0], &roots[0]);
			
			#ifdef VERBOSE_MODE		
				cout << i << "\t" << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
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
						cout << "TimeFromInsideAlongParabola - Both roots are negative or zero." << endl; 
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
			
			if (tmin > 0.0 && t == 0.0) {
				// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
				t = tmin;
			} else if (tmin > 0.0 && tmin < t) {
				// -- Check if this time, the smallest, non-zero time to the current boundary, is smaller than the current overall smallest time to any boundary
				t = tmin;
			}
		}
	}	

	// -- Analyse the final value for the time to nearest boundary from inside
	if (t > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TimeFromInsideAlongParabola - Time to nearest boundary: " <<  t << endl; 
		#endif
		return t;
	} else if (t == 0.) {
		throw runtime_error("In TUCNGeoBBox, TimeFromInsideAlongParabola - Calculation error - no time to boundary found"); 
		return 0;
	} else {
		throw runtime_error("In TUCNGeoBBox, TimeFromInsideAlongParabola - Calculation error - time to boundary is negative");
		return 0;
	}
}

//_____________________________________________________________________________
Double_t TUCNGeoBBox::TimeFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, Double_t stepmax, Int_t iact, Double_t *safe) const
{
	// This method calculates the time of all possible intersections of the particle's future path with all possible boundaries of the current shape.
	// Method then compares the times found and checks that the corresponding point of intersection with each boundary plane actually corresponds to a point
	// on the box. From these valid solutions, the method	returns the smallest, non-zero value.
	
	#ifdef VERBOSE_MODE		
		cout << "TimeFromOutsideAlongParabola - iact: " << iact << "\t" << "stepmax: " << stepmax << endl;
	#endif
	// ----------------------------------------------------------------------
	// -- For efficiency calculate the safety distance along straight line to surface. If this distance is greater than the proposed stepmax, then just return Big()
	// -- This code is from root's DistanceFrom... methods.
	Bool_t in = kTRUE;
   Double_t saf[3], par[3], newpt[3];

	// compute distance between point and boundaries. compare with stepsize
	for (Int_t i=0; i<3; i++) newpt[i] = point[i] - fOrigin[i];
   par[0] = fDX;
   par[1] = fDY;
   par[2] = fDZ;
   for (Int_t i=0; i<3; i++) {
		saf[i] = TMath::Abs(newpt[i]) - par[i];       // Get min. distance safety to boundary
      if (saf[i] >= stepmax) {
			#ifdef VERBOSE_MODE		
				cout << "TimeFromOutsideAlongParabola - Safety > Step. Returning TGeoShape::Big()" << endl;
			#endif
			return TGeoShape::Big();  // If this safety is greater than the stepsize then just return Big()
      }
		if (in && saf[i] > 0) in = kFALSE;				  // 
   }

   if (iact<3 && safe) {  // iact > 3 - compute only the distance to exiting, ignoring anything else
      // compute safe distance
      if (in) {
         *safe = 0.0;
      } else {   
         *safe = saf[0]; 
         if (saf[1] > *safe) *safe = saf[1];
         if (saf[2] > *safe) *safe = saf[2];     // Store the largest safety?
      }   
      if (iact==0) return TGeoShape::Big();      // iact = 0 - compute only the safety distance and fill it at the location given by SAFE
      if (iact==1 && stepmax<*safe) return TGeoShape::Big(); 
		// iact = 1  - a proposed STEP is supplied. The safe distance is computed first. 
		// If this is bigger than STEP then the proposed step is approved and returned by the method 
		// since it does not cross the shape boundaries. Otherwise, the distance to exiting the shape is 
		// computed and returned.
   }
	
	// protection in case point is actually inside box -- from ROOT's code
//	Double_t norm = TMath::Sqrt(velocity[0]*velocity[0] + velocity[1]*velocity[1] + velocity[2]*velocity[2]);
//	Double_t dir[3] = {velocity[0]/norm, velocity[1]/norm, velocity[2]/norm};
//	Int_t j;
//   if (in) {
//      j = 0;
//      Double_t ss = saf[0];
//      if (saf[1]>ss) {
//         ss = saf[1];
//         j = 1;
//      }
//      if (saf[2]>ss) j = 2;
//      if (newpt[j]*dir[j]>0) return TGeoShape::Big(); // in fact exiting
//		return 0.0;   
//   }
	// ----------------------------------------------------------------------
	
	// -- Compute actual time to reach boundary
	Double_t tfinal = 0.; // The smallest time to reach any boundary;
	Double_t boundary[3] = {fDX, fDY, fDZ}; // Store the coordinates of the box boundaries
	
	#ifdef VERBOSE_MODE		
		cout << "TimeFromOutsideAlongParabola - Positive (Local) Boundaries - +X: " << boundary[0] << "  +Y: " << boundary[1] << "  +Z: " << boundary[2] << endl;
		cout << "TimeFromOutsideAlongParabola - (Local) point - +X: " << newpt[0] << "  +Y: " << newpt[1] << "  +Z: " << newpt[2] << endl;
		cout << "TimeFromOutsideAlongParabola - (Local) velocity - +X: " << velocity[0] << "  +Y: " << velocity[1] << "  +Z: " << velocity[2] << endl;
		cout << "TimeFromOutsideAlongParabola - (Local) field direction - +X: " << field[0] << "  +Y: " << field[1] << "  +Z: " << field[2] << endl;
	#endif
		
	// ----------------------------------------------------------------------	
	// -- New root-finding / selecting process implementing TUCNPolynomial
	// -- Algorithm for Selecting ROOTs for Approaching boundary from OUTSIDE
	// ----------------------------------------------------------------------	
	// -- Loop over x, y, z boundaries/projections of parabola
	for (Int_t i=0; i<3; i++) {
		// -- Loop over +/- fDX,fDY,fDZ boundaries
		for (Int_t j=0; j<2; j++) {
			
			// -- Define storage for the smallest non-zero root to the CURRENT boundary
			Double_t tmin = 0.0;

			// -- Define the constants in the quadratic equation, a, b, c - see note book for details
			Double_t params[3] = {0., 0., 0.};
			params[0] = 0.5*field[i];
			params[1] = velocity[i]; 
			params[2] = newpt[i] - (TMath::Power(-1,j)*boundary[i]);

			// -- Solve equation for potential roots
			Double_t roots[2] = {0., 0.};
			Int_t solutions = TUCNPolynomial::Instance()->QuadraticRootFinder(&params[0], &roots[0]);

			#ifdef VERBOSE_MODE		
				cout << i << "\t" << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
				cout << "solutions: " << solutions << "\t" << "root 1: " << roots[0] << "\t" << "root 2: " << roots[1] << endl;
			#endif

			// -- Determing number of roots, and select the smallest, real, non-zero value. 
			if (solutions == 2) {
				// -- Two roots
				// -- Check first root to see if it is positive and corresponds to a point actually on the surface of the box
				if (roots[0] > 0. && IsNextPointOnBox(point, velocity, field, boundary, roots[0]) == kTRUE) {
					tmin = roots[0];
				}
				// -- Check the second root for the same criteria.
				if (roots[1] > 0. && IsNextPointOnBox(point, velocity, field, boundary, roots[1]) == kTRUE) {
					if (tmin == 0.) {
						// -- If the first root wasn't a valid solution, then just set tmin to the second root. 
						tmin = roots[1];
					} else if (tmin > 0. && roots[1] < tmin) {
						// -- If both roots are valid, then compare the two roots and pick the smallest value. 
						tmin = roots[1];
					} else {
						#ifdef VERBOSE_MODE		
							cout << "TimeFromOutsideAlongParabola - Both roots are negative, zero or invalid" << endl; 
						#endif
					}
				}
			} else if (solutions == 1) {
				// -- One Root
				if (roots[0] > 0. && IsNextPointOnBox(point, velocity, field, boundary, roots[0]) == kTRUE) {
					tmin = roots[0];
				} else {
					//-- Only Root is negative or zero
				}
			} else if (solutions == 0) {
				// -- No Real Roots
			} else {
				throw runtime_error("In TUCNGeoBBox, TimeFromOutsideAlongParabola - number of quadratic eqn roots is not between 0 and 2");
			}
		
			if (tmin > 0.0 && tfinal == 0.0) {
				// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
				tfinal = tmin;
			} else if (tmin > 0.0 && tmin < tfinal) {
				// -- Check if this time, the smallest, non-zero time to the current boundary, is smaller than the current overall smallest time to any boundary
				tfinal = tmin;
			}
		}
	}	

	// -- Analyse the final value of the shortest time to hit the boundary from outside
	if (tfinal > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TimeFromOutsideAlongParabola - Time to nearest boundary: " << tfinal << endl; 
		#endif
		return tfinal;
	} else if (tfinal == 0.) {
		#ifdef VERBOSE_MODE		
			cout << "TimeFromOutsideAlongParabola - No Boundary hit" << endl;
		#endif
		return TGeoShape::Big();
	} else {
		throw runtime_error("In TUCNGeoBBox, TimeFromOutsideAlongParabola - Calculation error - time to boundary is negative");
	}
	return 0.;
}

//_____________________________________________________________________________
Double_t TUCNGeoBBox::TimeFromOutsideAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, 
																		Double_t dx, Double_t dy, Double_t dz, const Double_t *origin, Double_t stepmax) 
{
	// This method calculates the time of all possible intersections of the particle's future path with all possible boundaries of the current shape.
	// Method then compares the times found and checks that the corresponding point of intersection with each boundary plane actually corresponds to a point
	// on the box. From these valid solutions, the method	returns the smallest, non-zero value.
		
	// ----------------------------------------------------------------------
	// -- For efficiency calculate the safety distance along straight line to surface. If this distance is greater than the proposed stepmax, then just return Big()
	// -- This code is from root's DistanceFrom... methods.
	Bool_t in = kTRUE;
   Double_t saf[3], par[3], newpt[3];

	// compute distance between point and boundaries. compare with stepsize
	for (Int_t i=0; i<3; i++) newpt[i] = point[i] - origin[i];
   par[0] = dx;
   par[1] = dy;
   par[2] = dz;
   for (Int_t i=0; i<3; i++) {
		saf[i] = TMath::Abs(newpt[i]) - par[i];       // Get min. distance safety to boundary
      if (saf[i] >= stepmax) {
			#ifdef VERBOSE_MODE		
				cout << "TimeFromOutsideAlongParabola - Safety > Step. Returning TGeoShape::Big()" << endl;
			#endif
			return TGeoShape::Big();  // If this safety is greater than the stepsize then just return Big()
      }
		if (in && saf[i] > 0) in = kFALSE;				  // 
   }

	if (in) {
		#ifdef VERBOSE_MODE
			cout << "In TUCNGeoBBox::TimeFromOutsideAlongParabola - particle actually inside bounding box" << endl;
		#endif
		return 0.0;
	}	
	

	// ----------------------------------------------------------------------
	
	// -- Calculate the actual time to the boundary
	Double_t tfinal = 0.; // The smallest time to reach any boundary;
	Double_t boundary[3] = {dx, dy, dz}; // Store the coordinates of the box boundaries
	
	#ifdef VERBOSE_MODE		
		cout << "TimeFromOutsideAlongParabola - Positive (Local) Boundaries - +X: " << boundary[0] << "  +Y: " << boundary[1] << "  +Z: " << boundary[2] << endl;
		cout << "TimeFromOutsideAlongParabola - (Local) point - +X: " << newpt[0] << "  +Y: " << newpt[1] << "  +Z: " << newpt[2] << endl;
		cout << "TimeFromOutsideAlongParabola - (Local) velocity - +X: " << velocity[0] << "  +Y: " << velocity[1] << "  +Z: " << velocity[2] << endl;
		cout << "TimeFromOutsideAlongParabola - (Local) field direction - +X: " << field[0] << "  +Y: " << field[1] << "  +Z: " << field[2] << endl;
	#endif
	
	// ----------------------------------------------------------------------	
	// -- New root-finding / selecting process implementing TUCNPolynomial
	// -- Algorithm for Selecting ROOTs for Approaching boundary from OUTSIDE
	// ----------------------------------------------------------------------	
	// -- Loop over x, y, z boundaries/projections of parabola
	for (Int_t i=0; i<3; i++) {
		// -- Loop over +/- fDX,fDY,fDZ boundaries
		for (Int_t j=0; j<2; j++) {
			// -- Define storage for the smallest non-zero root to the CURRENT boundary
			Double_t tmin = 0.0;
			
			// -- Define the constants in the quadratic equation, a, b, c - see note book for details
			Double_t params[3] = {0., 0., 0.};
			params[0] = 0.5*field[i];
			params[1] = velocity[i]; 
			params[2] = newpt[i] - (TMath::Power(-1,j)*boundary[i]);
			
			// -- Solve equation for potential roots
			Double_t roots[2] = {0., 0.};
			Int_t solutions = TUCNPolynomial::Instance()->QuadraticRootFinder(&params[0], &roots[0]);
			
			#ifdef VERBOSE_MODE		
				cout << i << "\t" << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
				cout << "solutions: " << solutions << "\t" << "root 1: " << roots[0] << "\t" << "root 2: " << roots[1] << endl;
			#endif
			
			// -- Determing number of roots, and select the smallest, real, non-zero value. 
			if (solutions == 2) {
				// -- Two roots
				// -- Check first root to see if it is positive and corresponds to a point actually on the surface of the box
				if (roots[0] > 0. && IsNextPointOnBox(point, velocity, field, boundary, roots[0]) == kTRUE) {
					tmin = roots[0];
				}
				// -- Check the second root for the same criteria.
				if (roots[1] > 0. && IsNextPointOnBox(point, velocity, field, boundary, roots[1]) == kTRUE) {
					if (tmin == 0.) {
						// -- If the first root wasn't a valid solution, then just set tmin to the second root. 
						tmin = roots[1];
					} else if (tmin > 0. && roots[1] < tmin) {
						// -- If both roots are valid, then compare the two roots and pick the smallest value. 
						tmin = roots[1];
					} else {
						#ifdef VERBOSE_MODE		
							cout << "TimeFromOutsideAlongParabola - Both roots are negative, zero or invalid" << endl; 
						#endif
					}
				}
			} else if (solutions == 1) {
				// -- One Root
				if (roots[0] > 0. && IsNextPointOnBox(point, velocity, field, boundary, roots[0]) == kTRUE) {
					tmin = roots[0];
				} else {
					//-- Only Root is negative or zero
				}
			} else if (solutions == 0) {
				// -- No Real Roots
			} else {
				throw runtime_error("In TUCNGeoBBox, TimeFromOutsideAlongParabola - number of quadratic eqn roots is not between 0 and 2");
			}
			
			if (tmin > 0.0 && tfinal == 0.0) {
				// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
				tfinal = tmin;
			} else if (tmin > 0.0 && tmin < tfinal) {
				// -- Check if this time, the smallest, non-zero time to the current boundary, is smaller than the current overall smallest time to any boundary
				tfinal = tmin;
			}
		}
	}	

	// -- Analyse the final value of the shortest time to hit the boundary from outside
	if (tfinal > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TGeoBBox: TimeFromOutsideAlongParabola - Final Time to nearest boundary: " <<  tfinal << endl; 
		#endif
		return tfinal;
	} else if (tfinal == 0.) {
		#ifdef VERBOSE_MODE		
			cout << "TimeFromOutsideAlongParabola - No Boundary hit" << endl;
		#endif
		return TGeoShape::Big();
	} else {
		throw runtime_error("In TUCNGeoBBox, TimeFromOutsideAlongParabola - Calculation error - time to boundary is negative");
	}
	return 0.0;
}

//_____________________________________________________________________________
Bool_t TUCNGeoBBox::IsNextPointOnBox(Double_t* point, Double_t* velocity, Double_t* field, Double_t* boundary, Double_t t) 
{
	// Calculate the proposed intersection point given a time, starting point, direction and local field. 
	// Check that this intersection is actually located on the surface of the box. 
	#ifdef VERBOSE_MODE		
		cout << "IsNextPointOnBox - Calling method. t: " <<  t << endl;
	#endif
	
	Double_t crossedpoint[3], surface[3];
	
	surface[0] = boundary[0] + 1.e-10; // Including a tolerance factor
	surface[1] = boundary[1] + 1.e-10;
	surface[2] = boundary[2] + 1.e-10;	
	
	// Calculate the intersection point. If point coords are within limits of the boundary return TRUE, else point is outside; return FALSE.
	for (Int_t i = 0; i < 3; ++i) {
		crossedpoint[i] = point[i] + velocity[i]*t + 0.5*field[i]*t*t;
		#ifdef VERBOSE_MODE		
			cout << "IsNextPointOnBox - Crossing Point: i: " << i << "  -  " << crossedpoint[i] << ",   Boundary: " << surface[i] << endl;
		#endif
		// Check point is on surface
		if (TMath::Abs(crossedpoint[i]) > surface[i]) {
			#ifdef VERBOSE_MODE		
				cout << "IsNextPointOnBox - Point not on surface of box. Returning false" << endl;
			#endif
			return kFALSE;
		} 
	}
	#ifdef VERBOSE_MODE		
		cout << "IsNextPointOnBox - Point on surface of box. Return true" << endl;
	#endif
	return kTRUE;	
}
