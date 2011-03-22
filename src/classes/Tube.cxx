// Tube.cxx
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

#include "Tube.h"
#include "Polynomial.h"
#include "Parabola.h"

using std::cout;
using std::endl;
using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(Tube)

//_____________________________________________________________________________
Tube::Tube()
     :fRootTube()
{
// Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Tube", "Default Constructor");
   #endif
   SetShapeBit(TGeoShape::kGeoTube);
   fRmin = 0.0;
   fRmax = 0.0;
   fDz   = 0.0;
}

//_____________________________________________________________________________
Tube::Tube(const char *name, Double_t rmin, Double_t rmax, Double_t dz)
     :Box(name, 0, 0, 0),
      fRootTube(name, rmin, rmax, dz)
{
// Default constructor specifying minimum and maximum radius
   #ifdef PRINT_CONSTRUCTORS
      Info("Tube", "Constructor");
   #endif
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
Tube::~Tube()
{
// destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("Tube", "Destructor");
   #endif
}

//_____________________________________________________________________________
Double_t Tube::TimeFromInside(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t /*stepTime*/, const Bool_t onBoundary) const
{
	// Compute time from inside point to surface of the tube
	#ifdef VERBOSE_MODE				
		cout << "Tube::TimeFromInside" << endl;
		cout << "TimeFromInside - Calling TimeFromInsideS" << endl;
	#endif
	// compute time to surface
	return TimeFromInsideS(point, velocity, field, fRmin, fRmax, fDz, onBoundary);
}

//_____________________________________________________________________________
Double_t Tube::TimeFromInsideS(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t rmin, const Double_t rmax, const Double_t dz, const Bool_t onBoundary)
{
	// Compute time from inside point to surface of the tube	
	#ifdef VERBOSE_MODE				
		cout << "TimeFromInsideS" << "\t" <<  "Local Field - X: " <<  field[0] << " Y: " << field[1] << " Z: " << field[2] << endl;
		cout << "TimeFromInsideS" << "\t" <<  "Local Point - X: " <<  point[0] << " Y: " << point[1] << " Z: " << point[2] << endl;
		cout << "TimeFromInsideS" << "\t" <<  "Local Vel - X: " <<  velocity[0] << " Y: " << velocity[1] << " Z: " << velocity[2] << endl;
	#endif
	// --------------------------------------------------------------------------------------
   // -- Storage for the overall smallest, non-zero time to the nearest boundary
	Double_t tfinal = 0.; 
	// --------------------------------------------------------------------------------------
	// -- 1. First check whether we will hit the end caps of the tube at +/- Z
	Double_t tmin = Tube::InsideTimeToZBoundary(point, velocity, field, dz, onBoundary);
	if (tmin == 0.0) {
		cout << "Error: Particle has failed to hit either z-boundary from Inside. Exiting." << endl;
		return 0.0;
	} else {
		// -- Solution found. Setting as the current smallest time
		#ifdef VERBOSE_MODE				
			cout << "Particle will reach first z-boundary in: " << tmin << endl;
		#endif
		tfinal = tmin;
	}
	
	// --------------------------------------------------------------------------------------
   // -- 2. Next Calculate the time to reach the circular boundaries of the tube.
	// --------------------------------------------------------------------------------------
	// -- CASE 1 - Inner Cylinder
	tmin = InsideTimeToRBoundary(point, velocity, field, rmin, onBoundary);
	if (tmin == 0.0) {
		#ifdef VERBOSE_MODE				
			cout << "Warning: Particle has failed to hit rmin boundary from Inside. " << endl;
		#endif
	} else {
		// -- Solution found. Check to see if it is smaller than the current smallest.
		#ifdef VERBOSE_MODE				
			cout << "Particle will reach rmin-boundary in: " << tmin << endl;
		#endif
		if (tmin < tfinal) {
			tfinal = tmin;
		}
	}
	
   // --------------------------------------------------------------------------------------
	// -- CASE 2 - Outer Cylinder
	tmin = InsideTimeToRBoundary(point, velocity, field, rmax, onBoundary);
	if (tmin == 0.0) {
		#ifdef VERBOSE_MODE				
			cout << "Error: Particle has failed to hit rmax boundary from Inside. " << endl;
		#endif
		// Particle should always hit the outer tube boundary at least at some positive time
		return 0.0;
	} else {
		// -- Solution found. Check to see if it is smaller than the current smallest.
		#ifdef VERBOSE_MODE				
			cout << "Particle will reach rmax-boundary in: " << tmin << endl;
		#endif
		if (tmin < tfinal) {
			tfinal = tmin;
		}
	}
	
	// --------------------------------------------------------------------------------------
   // Return the smallest time
	if (tfinal > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TimeFromInsideS - Time to nearest boundary: " <<  tfinal << endl; 
		#endif
		return tfinal;
	} else {
		#ifdef VERBOSE_MODE				
			cout << "Error - Particle has failed to hit any boundary: " << tfinal << endl;
		#endif
		return 0.;
	}	
}

//_____________________________________________________________________________
Double_t Tube::TimeFromOutside(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t stepTime, const Bool_t onBoundary) const
{
	// Compute time from outside point to surface of the tube and safe distance
	// Boundary safe algorithm.
	// first localize point w.r.t tube
	#ifdef VERBOSE_MODE				
		cout << "Tube::TimeFromOutside" << endl;
		cout << "TimeFromOutside - Check if Bounding box is within maximum step distance" << endl;
	#endif
	// Check if the bounding box is crossed within the requested distance
   Double_t tBox = Box::TimeFromOutsideS(point, velocity, field, fDX, fDY, fDZ, fOrigin, onBoundary);
   #ifdef VERBOSE_MODE				
		cout << "TimeFromOutside - Time to Box: " << tBox << endl;
	#endif
	if (tBox > stepTime + TGeoShape::Tolerance()) return TGeoShape::Big();
   // find time to shape
	return TimeFromOutsideS(point, velocity, field, fRmin, fRmax, fDz, onBoundary);
}

//_____________________________________________________________________________
Double_t Tube::TimeFromOutsideS(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t rmin, const Double_t rmax, const Double_t dz, const Bool_t onBoundary)
{
	// Compute time from outside point to the surface of the tube
	#ifdef VERBOSE_MODE				
		cout << "TimeFromOutsideS" << "\t" <<  "Local Field - X: " <<  field[0] << " Y: " << field[1] << " Z: " << field[2] << endl;
		cout << "TimeFromOutsideS" << "\t" <<  "Local Point - X: " <<  point[0] << " Y: " << point[1] << " Z: " << point[2] << endl;
		cout << "TimeFromOutsideS" << "\t" <<  "Local Vel - X: " <<  velocity[0] << " Y: " << velocity[1] << " Z: " << velocity[2] << endl;
	#endif
	// --------------------------------------------------------------------------------------
   // -- Storage for the overall smallest, non-zero time to the nearest boundary
	Double_t tfinal = 0.; 
	// --------------------------------------------------------------------------------------
	// -- 1. FIRST CHECK WHETHER WE WILL HIT THE ENDS OF THE TUBE AT +/-Zs
	// -- First calculate time to intersect the +/- Z planes.
	Double_t tmin = Tube::OutsideTimeToZBoundary(point, velocity, field, rmax, dz, onBoundary);
	if (tmin == 0.0) {
		// -- Failed to hit either of the z-boundaries
		#ifdef VERBOSE_MODE				
			cout << "Particle has failed to hit either z-boundary from Inside." << endl;
		#endif
	} else {
		// -- Solution found. Setting as the current smallest time
		#ifdef VERBOSE_MODE				
			cout << "TimeFromOutsideS - Particle has reached first z-boundary in: " << tmin << endl;
		#endif
		tfinal = tmin;
	}
	
	// --------------------------------------------------------------------------------------
   // -- 2. Next Calculate the time to reach the circular boundaries of the tube.
	// --------------------------------------------------------------------------------------
	// -- CASE 1 - Inner Cylinder
	tmin = OutsideTimeToRBoundary(point, velocity, field, rmin, dz, onBoundary);
	if (tmin == 0.0) {
		#ifdef VERBOSE_MODE				
			cout << "Warning: Particle has failed to hit rmin boundary from Outside. " << endl;
		#endif
	} else {
		// -- Solution found. Check to see if it is smaller than the current smallest.
		#ifdef VERBOSE_MODE				
			cout << "TimeFromOutsideS - Particle will reach rmin-boundary in: " << tmin << endl;
		#endif
		if (tmin < tfinal) {
			tfinal = tmin;
		}
	}
	
   // --------------------------------------------------------------------------------------
	// -- CASE 2 - Outer Cylinder
	tmin = OutsideTimeToRBoundary(point, velocity, field, rmax, dz, onBoundary);
	if (tmin == 0.0) {
		#ifdef VERBOSE_MODE				
			cout << "Warning: Particle has failed to hit rmax boundary from Outside. " << endl;
		#endif
	} else {
		// -- Solution found. Check to see if it is smaller than the current smallest.
		#ifdef VERBOSE_MODE				
			cout << "TimeFromOutsideS - Particle will reach rmax-boundary in: " << tmin << endl;
		#endif
		if (tmin < tfinal) {
			tfinal = tmin;
		}
	}
	
	// --------------------------------------------------------------------------------------
   // -- Return the smallest time to boundary (if any were hit)
	if (tfinal > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TimeFromInsideS - Time to nearest boundary: " <<  tfinal << endl; 
		#endif
		return tfinal;
	} else if (tfinal == 0.) {
		#ifdef VERBOSE_MODE				
			cout << "TimeFromOutsideS - Particle has failed to hit any boundary: " << tfinal << endl;
		#endif
		return TGeoShape::Big();
	} else {
		#ifdef VERBOSE_MODE				
			cout << "Error - TimeFromOutsideS - Calculation has failed" << tfinal << endl;
		#endif
		return 0.;
	}
}

//_____________________________________________________________________________
Bool_t Tube::IsNextPointOnTube(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t radius, const Double_t dz, const Double_t t)
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

//_____________________________________________________________________________
Double_t Tube::InsideTimeToZBoundary(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t dz, const Bool_t onBoundary)
{
	// -- Find the smallest, non-zero, positive time to one of the tube end caps
	#ifdef VERBOSE_MODE				
		cout << "InsideTimeToZBoundary" << "\t" <<  "Solve for +/- Z Boundaries" << endl;
	#endif
	Double_t tfinal = 0.0;
	Double_t boundary = dz;
	// -- Loop over +/- Z boundaries
	for (Int_t j = 0; j < 2; j++) {
		// -- Define the constants in the quadratic equation, a, b, c - see note book for details
		Double_t params[3] = {0., 0., 0.};
		params[0] = 0.5*field[2];
		params[1] = velocity[2]; 
		params[2] = point[2] - (TMath::Power(-1,j)*boundary);
		
		// Sometimes a,b or c (usually c) can be extremely small but non-zero, due to the fact that
		// we are usually sitting on a boundary and c measures the distance to the current boundary.
		if (TMath::Abs(params[0]) < TGeoShape::Tolerance()) {
			// a = 0 if the field component in this direction is zero
			params[0] = 0.;
		}
		if (TMath::Abs(params[1]) < TGeoShape::Tolerance()) {
			// b = 0 if the velocity component in this direction is zero (i.e it wont hit the boundary anyway)
			params[1] = 0.;
		}
		// We are usually sitting on a boundary so c will occasionally be within a few ~E-10 of zero.
		// Therefore if c is less than 1E-9, and the onBoundary flag is set to TRUE, then we assume that
		// we are on this boundary and set c = 0 to signify this. 
		if (TMath::Abs(params[2]) < 10.*TGeoShape::Tolerance() && onBoundary == kTRUE) {
			params[2] = 0.;
		}
	
		// -- Solve equation for potential roots
		Double_t roots[2] = {0., 0.};
		Int_t solutions = Polynomial::Instance()->QuadraticRootFinder(&params[0], &roots[0]);

		#ifdef VERBOSE_MODE		
			cout << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
			cout << "solutions: " << solutions << "\t" << "root 1: " << roots[0] << "\t" << "root 2: " << roots[1] << endl;
		#endif
		
		// -- Store the smallest non-zero root to the CURRENT boundary
		Double_t tmin = Box::SmallestInsideTime(solutions, &roots[0], onBoundary);
		
		if (tmin > 0.0 && tfinal == 0.0) {
			// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
			tfinal = tmin;
		} else if (tmin > 0.0 && tmin < tfinal) {
			// -- Check if this time, the smallest, non-zero time to the current boundary, is smaller than the current overall smallest time to any boundary
			tfinal = tmin;
		} else if (tmin <= 0.0) {
			// -- The returned time to the boundary was zero/negative
			#ifdef VERBOSE_MODE				
				cout << "InsideTimeToZBoundary" << "\t" <<  "Time to Boundary: " << TMath::Power(-1,j) << "z, is negative or zero: " << tmin << endl;
			#endif
		} else {
			// -- The time is larger than the current smallest time to z-boundary.
			#ifdef VERBOSE_MODE				
				cout << "InsideTimeToZBoundary" << "\t" <<  "Time to current boundary is larger than current smallest: " << tmin << endl;
			#endif
		}
		#ifdef VERBOSE_MODE				
			cout << "InsideTimeToZBoundary" << "\t" <<  "Current tfinal: " <<  tfinal << endl;
		#endif
	}
	
	// -- If either of the z-boundaries were hit, tfinal should be non-zero. Since we are coming from Inside, and 
	// -- the end-caps are effectively infinite planes here, we should always hit at least one of the boundaries.
	if (tfinal <= 0.0) {
		#ifdef VERBOSE_MODE				
			cout << "Error - InsideTimeToZBoundary has not found an intersection with either boundary: " <<  tfinal << endl;
		#endif
		return 0.0;
	}
	
	// -- Return the smallest time to the z-boundaries
	return tfinal;
}

//_____________________________________________________________________________
Double_t Tube::OutsideTimeToZBoundary(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t rmax, const Double_t dz, const Bool_t onBoundary)
{
	// -- Find the smallest, non-zero, positive time to one of the tube end caps from Outside
	#ifdef VERBOSE_MODE				
		cout << "OutsideTimeToZBoundary" << "\t" <<  "Solve for +/- Z Boundaries" << endl;
	#endif
	Double_t tfinal = 0.0;
	Double_t boundary = dz;
	// -- Loop over +/- Z boundaries
	for (Int_t j = 0; j < 2; j++) {
		// -- Define the constants in the quadratic equation, a, b, c - see note book for details
		Double_t params[3] = {0., 0., 0.};
		params[0] = 0.5*field[2];
		params[1] = velocity[2]; 
		params[2] = point[2] - (TMath::Power(-1,j)*boundary);
		
		// Sometimes a,b or c (usually c) can be extremely small but non-zero, due to the fact that
		// we are usually sitting on a boundary and c measures the distance to the current boundary.
		if (TMath::Abs(params[0]) < TGeoShape::Tolerance()) {
			// a = 0 if the field component in this direction is zero
			params[0] = 0.;
		}
		if (TMath::Abs(params[1]) < TGeoShape::Tolerance()) {
			// b = 0 if the velocity component in this direction is zero (i.e it wont hit the boundary anyway)
			params[1] = 0.;
		}
		// We are usually sitting on a boundary so c will occasionally be within a few ~E-10 of zero.
		// Therefore if c is less than 1E-9, and the onBoundary flag is set to TRUE, then we assume that
		// we are on this boundary and set c = 0 to signify this. 
		if (TMath::Abs(params[2]) < 10.*TGeoShape::Tolerance() && onBoundary == kTRUE) {
			params[2] = 0.;
		}
		
		// -- Solve equation for potential roots
		Double_t roots[2] = {0., 0.};
		Int_t solutions = Polynomial::Instance()->QuadraticRootFinder(&params[0], &roots[0]);

		#ifdef VERBOSE_MODE		
			cout << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
			cout << "solutions: " << solutions << "\t" << "root 1: " << roots[0] << "\t" << "root 2: " << roots[1] << endl;
		#endif

		// First check if we are sitting on a boundary
		if (onBoundary == kTRUE) {
			// If on boundary, we must be vigilant as one of the roots, representing the time to the boundary we are
			// sitting on, should be zero, or very close to zero.
			// If one is very very small, we need to set it to zero, to ensure that the larger root, indicating the correct
			// time to the next boundary is chosen.
			// If both are very very small, then we need to be wary. This could mean we are on a boundary, but also in the
			// corner of a box, where the next boundary is very very close by. 
			if (solutions == 2) {
				if (TMath::Abs(roots[0]) < 1.E-8 && TMath::Abs(roots[1]) < 1.E-8) {
					cout << "OutsideTimeToZBoundary - Two roots are both very small" << endl;
					cout << "Root 1: " << roots[0] << "\t" << "Root 2: " << roots[1] << endl;
					throw runtime_error("Two very small roots encountered. Unsure how to proceed.");
				}
				for (Int_t i = 0; i < 2; i++) {
					if (TMath::Abs(roots[i]) < 1.E-8) {
						cout << "OutsideTimeToZBoundary - Root[" << i << "]: "<< roots[i] << ", is < 1.E-8. Setting to zero." << endl;
						roots[i] = 0.0;
					}
				}
			} else if (solutions == 1) {
				// Need to be careful here. One solution is zero, which should/may be because we are sitting
				// on a boundary and this has been computed correctly by the RootFinder. 
				// It could also be because in this direction, the field is zero, and therefore we are moving along a straight line
				// and if we are coming from inside the box, there will only be one intersection. (Coming from outside would still
				// produce two intersections in this case).
				// We could also be very very close to another boundary, say if we are sitting right in the corner of a box.
				// In any case, we have a single solution which should be correct and not need setting to zero.
				if (TMath::Abs(roots[0]) < 1.E-8) cout << "OutsideTimeToZBoundary - Single Root found to be < 1.E-8 : " << roots[0] << endl;
			} else {
				// Nothing to be done - both roots should be zero anyway - no solutions found
			}
		}
		
		// -- Store the smallest non-zero root to the CURRENT boundary
		Double_t tmin = 0.0;
		// -- Determing number of roots, and select the smallest, real, non-zero value. 
		if (solutions == 2) {
			// -- Two roots
			// -- Check first root to see if it is positive and corresponds to a point actually on the surface of the tube
			if (roots[0] > 0. && Tube::IsNextPointOnTube(point, velocity, field, rmax, boundary, roots[0]) == kTRUE) {
				tmin = roots[0];
			}
			// -- Check the second root for the same criteria.
			if (roots[1] > 0. && Tube::IsNextPointOnTube(point, velocity, field, rmax, boundary, roots[1]) == kTRUE) {
				if (tmin == 0.) {
					// -- If the first root wasn't a valid solution, then just set tmin to the second root. 
					tmin = roots[1];
				} else if (tmin > 0. && roots[1] < tmin) {
					// -- If both roots are valid, then compare the two roots and pick the smallest value. 
					tmin = roots[1];
				} else {
					#ifdef VERBOSE_MODE		
						cout << "OutsideTimeToZBoundary - Both roots are negative, zero or invalid" << endl; 
					#endif
					return 0;
				}
			}
		} else if (solutions == 1) {
			// -- One Root
			if (roots[0] > 0. && Tube::IsNextPointOnTube(point, velocity, field, rmax, boundary, roots[0]) == kTRUE) {
				tmin = roots[0];
			} else {
				//-- Only Root is negative or zero
				#ifdef VERBOSE_MODE		
					cout << "OutsideTimeToZBoundary - Only root is negative, zero or invalid" << endl; 
				#endif
				return 0;
			}
		} else {
			// -- No Real Roots
			#ifdef VERBOSE_MODE		
				cout << "OutsideTimeToZBoundary - No solutions" << endl; 
			#endif
			return 0;
		}
		
		if (tmin > 0.0 && tfinal == 0.0) {
			// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
			tfinal = tmin;
		} else if (tmin > 0.0 && tmin < tfinal) {
			// -- Check if this time, the smallest, non-zero time to the current boundary, is smaller than the current overall smallest time to any boundary
			tfinal = tmin;
		} else if (tmin <= 0.0) {
			// -- The returned time to the boundary was zero/negative
			#ifdef VERBOSE_MODE				
				cout << "OutsideTimeToZBoundary - Time to Boundary: " << TMath::Power(-1,j) << "z, is negative or zero: " << tmin << endl;
			#endif
		} else {
			// -- The time is larger than the current smallest time to z-boundary.
			#ifdef VERBOSE_MODE				
				cout << "OutsideTimeToZBoundary - Time to current boundary is larger than current smallest: " << tmin << endl;
			#endif
		}
		#ifdef VERBOSE_MODE				
			cout << "OutsideTimeToZBoundary" << "\t" <<  "Boundary: " << TMath::Power(-1,j) << "z, --- current tfinal: " <<  tfinal << endl;
		#endif
	}
	
	// -- If either of the z-boundaries were hit, tfinal should be non-zero. Since we are coming from Inside, and 
	// -- the end-caps are effectively infinite planes here, we should always hit at least one of the boundaries.
	if (tfinal <= 0.0) {
		#ifdef VERBOSE_MODE				
			cout << "OutsideTimeToZBoundary has not found an intersection with either boundary" << endl;
		#endif
		return 0.0;
	}
	
	// -- Return the smallest time to the z-boundaries -- (may be zero, i.e: neither z-boundary was hit)
	return tfinal;
}

//_____________________________________________________________________________
Double_t Tube::InsideTimeToRBoundary(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t rBoundary, const Bool_t onBoundary) 
{
	// -- Calculate the time to reach the circular boundary of the tube.
	if (rBoundary <= 0.0) {
		// -- No intersection possible since there is no boundary to cross!
		return 0.0;
	}
	
	// -- The general case require us to solve a quartic polynomial. 
	Double_t a, b, c, d; 	// Four of the constants of the quartic polynomial. see notes for the equations.
	a = 0.25*(field[0]*field[0] + field[1]*field[1]);
	b = velocity[0]*field[0] + velocity[1]*field[1];
	c = point[0]*field[0] + point[1]*field[1] + velocity[0]*velocity[0] + velocity[1]*velocity[1];
	d = 2.0*(point[0]*velocity[0] + point[1]*velocity[1]);

	// The fifth constant of the quartic polynomial. see notes for equation
	// This is an important constant, since it represents the distance of the point from the the circular boundary	
	// First we calculate the current radius of the point
	Double_t rCurrent = TMath::Sqrt(point[0]*point[0] + point[1]*point[1]);
	// The fifth constant of the quartic polynomial. see notes for equation
	Double_t e = point[0]*point[0] + point[1]*point[1] - rBoundary*rBoundary; 
	
	// Now if the onBoundary flag is set, and our current radius is within 1E-9 of the boundary,
	// we will put this down to being the case of sitting on this boundary, and therefore set e = 0;
	if (TMath::Abs(rCurrent - rBoundary) < 10.*TGeoShape::Tolerance() && onBoundary == kTRUE) {
		#ifdef VERBOSE_MODE				
			cout << "InsideTimeToRBoundary - Rounding Down  e: " << e << " to zero." << endl;
		#endif
		e = 0.0;
	}
	if (TMath::Abs(e) < 10.*TGeoShape::Tolerance() && onBoundary == kTRUE) {
		#ifdef VERBOSE_MODE				
			cout << "InsideTimeToRBoundary - Rounding Down  e: " << e << " to zero." << endl;
		#endif
		e = 0.0;
	}
	
	// -- Check for rounding errors on the remaining parameters
	// Sometimes a,b,c or d can be extremely small but non-zero, due to rounding errors.
	// I have often seen this for the fields after they have been rotated into the local coordinate frame.
	if (TMath::Abs(a) < TGeoShape::Tolerance()) {
		// a = 0 if the field component in the x-y plane is zero
		// This happens if the tube is not rotated and is aligned with the field (conventially set along the z-axis)
		#ifdef VERBOSE_MODE				
			cout << "InsideTimeToRBoundary - Rounding Down a: " << a << " to zero." << endl;
		#endif
		a = 0.;
	}
	if (TMath::Abs(b) < TGeoShape::Tolerance()) {
		// b = 0 if the velocity or field component in the x-y plane is zero (i.e it wont hit the boundary anyway)
		#ifdef VERBOSE_MODE				
			cout << "InsideTimeToRBoundary - Rounding Down b: " << b << " to zero." << endl;
		#endif
		b = 0.;
	}
	if (TMath::Abs(c) < TGeoShape::Tolerance()) {
		// c = 0 if the velocity and field components in the x-y plane are zero (i.e it wont hit the boundary anyway)
		#ifdef VERBOSE_MODE				
			cout << "InsideTimeToRBoundary - Rounding Down c: " << c << " to zero." << endl;
		#endif
		c = 0.;
	}
	if (TMath::Abs(d) < TGeoShape::Tolerance()) {
		// d = 0 if the velocity components in the x-y plane are zero (i.e it wont hit the boundary anyway)
		#ifdef VERBOSE_MODE				
			cout << "InsideTimeToRBoundary - Rounding Down d: " << d << " to zero." << endl;
		#endif
		d = 0.;
	}
	
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
	
	// --------------------------------------------------------------------------------------
   // -- Solve the quartic equation to find the intersections of the parabola with the inner cylinder
	#ifdef VERBOSE_MODE				
		cout << "InsideTimeToRBoundary - Quartic Eqn - a: " <<  a << "  b: " << b << "  c: " << c << "  d: " << d << "  e: " << e << endl;
		cout << "InsideTimeToRBoundary - Polynomial Order: " <<  order << endl;
	#endif
	
	Int_t solutions = 0; // Counts the number of possible solutions
	Double_t params[5] = {a,b,c,d,e}; // equation parameters
	Double_t roots[4] = {0.,0.,0.,0.}; // roots of equation

	if (order == 4) {
		// We have a quartic equation
		solutions = Polynomial::Instance()->QuarticRootFinder(params, roots);
	} else {
		// We have a lower class of problem. 
		if (order == 3) {
			// We have a cubic. Organise parameters and solve. 
			Double_t cubicParams[4] = {params[1], params[2], params[3], params[4]};
			solutions = Polynomial::Instance()->CubicRootFinder(cubicParams, roots);
		
		} else if (order == 2) {
			Double_t quadraticParams[3] = {params[2], params[3], params[4]};
			solutions = Polynomial::Instance()->QuadraticRootFinder(quadraticParams, roots);

		} else if (order == 1) {
			// We have a linear equation. 
			assert(d != 0.0);
			roots[0] = -e/d;
			solutions = 1;
		
		} else {
			// No solutions - no intersection
			solutions = 0;
			return 0.0;
		}
	}
	// --------------------------------------------------------------------------------------
	// -- Find the smallest, positive, non-zero, real root of the polynomial
	
	// First check if we are sitting on a boundary
	if (onBoundary == kTRUE) {
		// If on boundary, we must be vigilant as one of the roots, representing the time to the boundary we are
		// sitting on, should be zero, or very close to zero.
		// If one is very very small, we need to set it to zero, to ensure that the larger root, indicating the correct
		// time to the next boundary is chosen.
		
		// If more than one are very very small, then we need to be wary. This could mean we are on the boundary, but
		// our angle of approach was so shallow that we are about to hit the boundary again very very soon (this should
		// be extremely rare, but may as well flag the case in case it causes problems in future)
		Int_t numberOfSuspectSolutions = 0;
		for (Int_t i = 0; i < solutions; i++) {
			if (roots[i] != 0.0 && TMath::Abs(roots[i]) < 1.E-8) numberOfSuspectSolutions++;
		}
		// Check the number of solutions that are very small
		if (numberOfSuspectSolutions > 1) {
			// This indicates the (hopefully) rare case when more than one solution is extremely small
			cout << "InsideTimeToRBoundary - More than one of the roots are very small" << endl;
			cout << "Root 1: " << roots[0] << "\t" << "Root 2: " << roots[1] << "\t" << "Root 3: " << roots[2] << "\t" << "Root 4: " << roots[3] << endl;
			throw runtime_error("At least two very small roots encountered. Unsure how to proceed.");
		} else if (numberOfSuspectSolutions == 1) {
			// This (hopefully) represents the time to the boundary we are currently sitting on
			// and should be set to zero to avoid confusion
			for (Int_t i = 0; i < solutions; i++) {
				if (TMath::Abs(roots[i]) < 1.E-8) {
					cout << "InsideTimeToRBoundary - Root[" << i << "]: "<< roots[i] << ", is < 1.E-8. Setting to zero." << endl;
					roots[i] = 0.0;
				}
			}
		} else {
			// No very small solutions
			// This is good (hopefully!). Should mean that we have no problems with the roots.
		}
	}
	
	#ifdef VERBOSE_MODE				
		cout << "InsideTimeToRBoundary" << "\t" <<  "Solutions: " <<  solutions << endl;
	#endif
	// -- Define the smallest non-zero time to boundary
	Double_t tmin = 0.0;
	for (Int_t i = 0; i < solutions; i++) {
		// -- Check if the current root positive and non-zero
		if (roots[i] > 0.0) {
			if (tmin == 0.0) {
				// -- If a particular root is positive and tfinal has not yet been set, then intialise tfinal with that value
				tmin = roots[i];
			} else if (roots[i] < tmin) {
				// -- If tfinal has been set to a certain solution, check if current solution is smaller
				// -- and if so set that to be the current smallest tfinal
				tmin = roots[i];
			}
		} else {
			// -- Root is negative or zero, nothing to see here...
			continue;
		}
	}
	#ifdef VERBOSE_MODE				
		cout << "InsideTimeToRBoundary - Smallest Time to Boundary: " <<  tmin << endl;
	#endif
	
	// -- Since we are coming from Inside, we would expect at least one solution, unless we have no 
	// -- horizontal velocity (which is checked above) or we are sitting on the boundary that we are looking
	// -- for intersections with.
	if (tmin <= 0.0) {
		#ifdef VERBOSE_MODE				
			cout << "Warning - InsideTimeToRBoundary has not found an positive intersection!" << endl;
		#endif
		return 0.0;
	}
	
	return tmin;
}

//_____________________________________________________________________________
Double_t Tube::OutsideTimeToRBoundary(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t rBoundary, const Double_t dz, const Bool_t onBoundary) 
{
	// -- Calculate the time to reach the circular boundary of the tube from outside.
	if (rBoundary <= 0.0) {
		// -- No intersection possible since there is no boundary to cross!
		return 0.0;
	}
	
	// -- The general case require us to solve a quartic polynomial. 
	Double_t a, b, c, d; 	// Four of the constants of the quartic polynomial. see notes for the equations.
	a = 0.25*(field[0]*field[0] + field[1]*field[1]);
	b = velocity[0]*field[0] + velocity[1]*field[1];
	c = point[0]*field[0] + point[1]*field[1] + velocity[0]*velocity[0] + velocity[1]*velocity[1];
	d = 2.0*(point[0]*velocity[0] + point[1]*velocity[1]);

	// The fifth constant of the quartic polynomial. see notes for equation
	// This is an important constant, since it represents the distance of the point from the the circular boundary	
	// First we calculate the current radius of the point
	Double_t rCurrent = TMath::Sqrt(point[0]*point[0] + point[1]*point[1]);
	// The fifth constant of the quartic polynomial. see notes for equation
	Double_t e = point[0]*point[0] + point[1]*point[1] - rBoundary*rBoundary; 
	
	// Now if the onBoundary flag is set, and our current radius is within 1E-9 of the boundary,
	// we will put this down to being the case of sitting on this boundary, and therefore set e = 0;
	if (TMath::Abs(rCurrent - rBoundary) < 10.*TGeoShape::Tolerance() && onBoundary == kTRUE) {
		e = 0.0;
	}
	if (TMath::Abs(e) < 10.*TGeoShape::Tolerance() && onBoundary == kTRUE) {
		e = 0.0;
	}
	
	// -- Check for rounding errors on the remaining parameters
	// Sometimes a,b,c or d can be extremely small but non-zero, due to rounding errors.
	// I have often seen this for the fields after they have been rotated into the local coordinate frame.
	if (TMath::Abs(a) < TGeoShape::Tolerance()) {
		// a = 0 if the field component in the x-y plane is zero
		// This happens if the tube is not rotated and is aligned with the field (conventially set along the z-axis)
		a = 0.;
	}
	if (TMath::Abs(b) < TGeoShape::Tolerance()) {
		// b = 0 if the velocity or field component in the x-y plane is zero (i.e it wont hit the boundary anyway)
		b = 0.;
	}
	if (TMath::Abs(c) < TGeoShape::Tolerance()) {
		// c = 0 if the velocity and field components in the x-y plane are zero (i.e it wont hit the boundary anyway)
		c = 0.;
	}
	if (TMath::Abs(d) < TGeoShape::Tolerance()) {
		// d = 0 if the velocity components in the x-y plane are zero (i.e it wont hit the boundary anyway)
		d = 0.;
	}
	
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
	
	// --------------------------------------------------------------------------------------
   // -- Solve the quartic equation to find the intersections of the parabola with the inner cylinder
	#ifdef VERBOSE_MODE				
		cout << "OutsideTimeToRBoundary - Quartic Eqn - a: " <<  a << "  b: " << b << "  c: " << c << "  d: " << d << "  e: " << e << endl;
		cout << "OutsideTimeToRBoundary - Polynomial Order: " <<  order << endl;
	#endif
	
	Int_t solutions = 0; // Counts the number of possible solutions
	Double_t params[5] = {a,b,c,d,e}; // equation parameters
	Double_t roots[4] = {0.,0.,0.,0.}; // roots of equation

	if (order == 4) {
		// We have a quartic equation
		solutions = Polynomial::Instance()->QuarticRootFinder(params, roots);
	} else {
		// We have a lower class of problem. 
		if (order == 3) {
			// We have a cubic. Organise parameters and solve. 
			Double_t cubicParams[4] = {params[1], params[2], params[3], params[4]};
			solutions = Polynomial::Instance()->CubicRootFinder(cubicParams, roots);
		
		} else if (order == 2) {
			Double_t quadraticParams[3] = {params[2], params[3], params[4]};
			solutions = Polynomial::Instance()->QuadraticRootFinder(quadraticParams, roots);

		} else if (order == 1) {
			// We have a linear equation. 
			assert(d != 0.0);
			roots[0] = -e/d;
			solutions = 1;
		
		} else {
			// No solutions - no intersection
			solutions = 0;
			return 0.0;
		}
	}
	// --------------------------------------------------------------------------------------
	// -- Find the smallest, positive, non-zero, real root of the polynomial
	
	// First check if we are sitting on a boundary
	if (onBoundary == kTRUE) {
		// If on boundary, we must be vigilant as one of the roots, representing the time to the boundary we are
		// sitting on, should be zero, or very close to zero.
		// If one is very very small, we need to set it to zero, to ensure that the larger root, indicating the correct
		// time to the next boundary is chosen.
		
		// If more than one are very very small, then we need to be wary. This could mean we are on the boundary, but
		// our angle of approach was so shallow that we are about to hit the boundary again very very soon (this should
		// be extremely rare, but may as well flag the case in case it causes problems in future)
		Int_t numberOfSuspectSolutions = 0;
		for (Int_t i = 0; i < solutions; i++) {
			if (roots[i] != 0.0 && TMath::Abs(roots[i]) < 1.E-8) numberOfSuspectSolutions++;
		}
		// Check the number of solutions that are very small
		if (numberOfSuspectSolutions > 1) {
			// This indicates the (hopefully) rare case when more than one solution is extremely small
			cout << "OutsideTimeToRBoundary - More than one of the roots are very small" << endl;
			cout << "Root 1: " << roots[0] << "\t" << "Root 2: " << roots[1] << "\t" << "Root 3: " << roots[2] << "\t" << "Root 4: " << roots[3] << endl;
			throw runtime_error("At least two very small roots encountered. Unsure how to proceed.");
		} else if (numberOfSuspectSolutions == 1) {
			// This (hopefully) represents the time to the boundary we are currently sitting on
			// and should be set to zero to avoid confusion
			for (Int_t i = 0; i < solutions; i++) {
				if (TMath::Abs(roots[i]) < 1.E-8) {
					cout << "OutsideTimeToRBoundary - Root[" << i << "]: "<< roots[i] << ", is < 1.E-8. Setting to zero." << endl;
					roots[i] = 0.0;
				}
			}
		} else {
			// No very small solutions
			// This is good (hopefully!). Should mean that we have no problems with the roots.
		}
	}
	
	#ifdef VERBOSE_MODE				
		cout << "OutsideTimeToRBoundary" << "\t" <<  "Solutions: " <<  solutions << endl;
	#endif
	// -- Define the smallest non-zero time to boundary
	Double_t tmin = 0.0;
	for (Int_t i = 0; i < solutions; i++) {
		// -- Check if the current root positive and non-zero
		if (roots[i] > 0.0 && IsNextPointOnTube(point, velocity, field, rBoundary, dz, roots[i]) == kTRUE) {
			if (tmin == 0.0) {
				// -- If a particular root is positive and tfinal has not yet been set, then intialise tfinal with that value
				tmin = roots[i];
			} else if (roots[i] < tmin) {
				// -- If tfinal has been set to a certain solution, check if current solution is smaller
				// -- and if so set that to be the current smallest tfinal
				tmin = roots[i];
			}
		} else {
			// -- Root is negative or zero, nothing to see here...
			continue;
		}
	}
	
	#ifdef VERBOSE_MODE				
		cout << "OutsideTimeToRBoundary - Smallest Time to Boundary: " <<  tmin << endl;
	#endif
	// -- Since we are coming from Outside, there are no guarentees we will hit anything. 
	if (tmin <= 0.0) {
		#ifdef VERBOSE_MODE				
			cout << "Warning - OusideTimeToRBoundary has not found an positive intersection!" << endl;
		#endif
		return 0.0;
	}	
	return tmin;
}

//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________
// -- Below are the TGeoTube methods that we are re-implementing (due to the 
// -- problems of multiple inheritance in this situation). This is mostly achieved
// -- through the TGeoTube member object, but just to be clear, all Code below is
// -- either copied from or derived from the ROOT TGeoTube source code

//_____________________________________________________________________________
Double_t Tube::Capacity() const
{
// Computes capacity of the shape in [length^3]
   return Tube::Capacity(fRmin,fRmax, fDz);
}   

//_____________________________________________________________________________
Double_t Tube::Capacity(Double_t rmin, Double_t rmax, Double_t dz)
{
// Computes capacity of the shape in [length^3]
   return TGeoTube::Capacity(rmin,rmax,dz);
}   

//_____________________________________________________________________________
void Tube::ComputeBBox()
{
// compute bounding box of the tube
   fDX = fDY = fRmax;
   fDZ = fDz;
}

//_____________________________________________________________________________
void Tube::ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm)
{
// Compute normal to closest surface from POINT.
   fRootTube.ComputeNormal(point,dir,norm);
}

//_____________________________________________________________________________
void Tube::ComputeNormalS(Double_t *point, Double_t *dir, Double_t *norm,
                              Double_t rmin, Double_t rmax, Double_t dz)
{
// Compute normal to closest surface from POINT.
   TGeoTube::ComputeNormalS(point,dir,norm,rmin,rmax,dz);
}

//_____________________________________________________________________________
Bool_t Tube::Contains(Double_t *point) const
{
// test if point is inside this tube
   return fRootTube.Contains(point);
}

//_____________________________________________________________________________
Int_t Tube::DistancetoPrimitive(Int_t px, Int_t py)
{
// compute closest distance from point px,py to each corner
   return fRootTube.DistancetoPrimitive(px,py);
}

//_____________________________________________________________________________
Double_t Tube::DistFromInsideS(Double_t *point, Double_t *dir, Double_t rmin, Double_t rmax, Double_t dz)
{
// Compute distance from inside point to surface of the tube (static)
// Boundary safe algorithm.
   return TGeoTube::DistFromInsideS(point,dir,rmin,rmax,dz);
}

//_____________________________________________________________________________
Double_t Tube::DistFromInside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// Compute distance from inside point to surface of the tube
// Boundary safe algorithm.
   return fRootTube.DistFromInside(point,dir,iact,step,safe);
}

//_____________________________________________________________________________
Double_t Tube::DistFromOutsideS(Double_t *point, Double_t *dir, Double_t rmin, Double_t rmax, Double_t dz)
{
// Static method to compute distance from outside point to a tube with given parameters
// Boundary safe algorithm.
   return TGeoTube::DistFromOutsideS(point,dir,rmin,rmax,dz);
}

//_____________________________________________________________________________
Double_t Tube::DistFromOutside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// Compute distance from outside point to surface of the tube and safe distance
// Boundary safe algorithm.
   return fRootTube.DistFromOutside(point,dir,iact,step,safe);
}

//_____________________________________________________________________________
void Tube::DistToTube(Double_t rsq, Double_t nsq, Double_t rdotn, Double_t radius, Double_t &b, Double_t &delta)
{
// Static method computing the distance to a tube with given radius, starting from
// POINT along DIR director cosines. 
   TGeoTube::DistToTube(rsq,nsq,rdotn,radius,b,delta);
}

//_____________________________________________________________________________
TGeoVolume* Tube::Divide(TGeoVolume* /*voldiv*/, const char* /*divname*/, Int_t /*iaxis*/, Int_t /*ndiv*/,
                             Double_t /*start*/, Double_t /*step*/)
{
   Error("Divide","Divide is not implemented for Tube. Sorry");
   return 0;
}

//_____________________________________________________________________________
const char *Tube::GetAxisName(Int_t iaxis) const
{
// Returns name of axis IAXIS.
   return fRootTube.GetAxisName(iaxis);
}

//_____________________________________________________________________________
Double_t Tube::GetAxisRange(Int_t iaxis, Double_t &xlo, Double_t &xhi) const
{
// Get range of shape for a given axis.
   return fRootTube.GetAxisRange(iaxis,xlo,xhi);
}

//_____________________________________________________________________________
void Tube::GetBoundingCylinder(Double_t *param) const
{
//--- Fill vector param[4] with the bounding cylinder parameters.
   fRootTube.GetBoundingCylinder(param);
}

//_____________________________________________________________________________
TGeoShape *Tube::GetMakeRuntimeShape(TGeoShape* /*mother*/, TGeoMatrix * /*mat*/) const
{
// in case shape has some negative parameters, these has to be computed
// in order to fit the mother
   Error("GetMakeRuntimeShape","This function has been disable for Tube. Sorry");
   return NULL;
}

//_____________________________________________________________________________
void Tube::InspectShape() const
{
// print shape parameters
   fRootTube.InspectShape();
}

//_____________________________________________________________________________
TBuffer3D *Tube::MakeBuffer3D() const
{
   // Creates a TBuffer3D describing *this* shape.
   // Coordinates are in local reference frame.
   return fRootTube.MakeBuffer3D();
}

//_____________________________________________________________________________
void Tube::SetSegsAndPols(TBuffer3D &buffer) const
{
// Fill TBuffer3D structure for segments and polygons.
   fRootTube.SetSegsAndPols(buffer);
}

//_____________________________________________________________________________
Double_t Tube::Safety(Double_t *point, Bool_t in) const
{
// computes the closest distance from given point to this shape, according
// to option. The matching point on the shape is stored in spoint.
   return fRootTube.Safety(point,in);
}

//_____________________________________________________________________________
Double_t Tube::SafetyS(Double_t *point, Bool_t in, Double_t rmin, Double_t rmax, Double_t dz, Int_t skipz)
{
// computes the closest distance from given point to this shape, according
// to option. The matching point on the shape is stored in spoint.
   return TGeoTube::SafetyS(point,in,rmin,rmax,dz,skipz);
}

//_____________________________________________________________________________
void Tube::SavePrimitive(ostream &out, Option_t * option /*= ""*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   fRootTube.SavePrimitive(out,option);
}

//_____________________________________________________________________________
void Tube::SetTubeDimensions(Double_t rmin, Double_t rmax, Double_t dz)
{
// Set tube dimensions.
   fRmin = rmin;
   fRmax = rmax;
   fDz   = dz;
   if (fRmin>0 && fRmax>0 && fRmin>=fRmax) {
      Error("SetTubeDimensions", "In shape %s wrong rmin=%g rmax=%g", GetName(), rmin,rmax);
   }
}

//_____________________________________________________________________________
void Tube::SetDimensions(Double_t *param)
{
// Set tube dimensions starting from a list.
   Double_t rmin = param[0];
   Double_t rmax = param[1];
   Double_t dz   = param[2];
   SetTubeDimensions(rmin, rmax, dz);
}

//_____________________________________________________________________________
Bool_t Tube::GetPointsOnSegments(Int_t npoints, Double_t *array) const
{
// Fills array with n random points located on the line segments of the shape mesh.
// The output array must be provided with a length of minimum 3*npoints. Returns
// true if operation is implemented.
   return fRootTube.GetPointsOnSegments(npoints,array);
}                    

//_____________________________________________________________________________
void Tube::SetPoints(Double_t *points) const
{
// create tube mesh points
   fRootTube.SetPoints(points);
}

//_____________________________________________________________________________
void Tube::SetPoints(Float_t *points) const
{
// create tube mesh points
   fRootTube.SetPoints(points);
}

//_____________________________________________________________________________
Int_t Tube::GetNmeshVertices() const
{
// Return number of vertices of the mesh representation
   return fRootTube.GetNmeshVertices();
}

//_____________________________________________________________________________
void Tube::GetMeshNumbers(Int_t &nvert, Int_t &nsegs, Int_t &npols) const
{
// Returns numbers of vertices, segments and polygons composing the shape mesh.
   return fRootTube.GetMeshNumbers(nvert,nsegs,npols);
}

//_____________________________________________________________________________
void Tube::Sizeof3D() const
{
   fRootTube.Sizeof3D();
}

//_____________________________________________________________________________
const TBuffer3D & Tube::GetBuffer3D(Int_t reqSections, Bool_t localFrame) const
{
// Fills a static 3D buffer and returns a reference.
   return fRootTube.GetBuffer3D(reqSections,localFrame);
}


