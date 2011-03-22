// Box
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <stdexcept>

#include "TMath.h"
#include "TParticle.h"
#include "TGeoManager.h"
#include "TVirtualGeoTrack.h"
#include "TGeoBBox.h"

#include "Particle.h"
#include "Parabola.h"
#include "Polynomial.h"

#include "Box.h"

using std::cout;
using std::endl;
using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(Box)
   
//_____________________________________________________________________________
Box::Box()
{
// Default constructor
	#ifdef PRINT_CONSTRUCTORS
      Info("Box", "Default Constructor");
   #endif
}   

//_____________________________________________________________________________
Box::Box(Double_t dx, Double_t dy, Double_t dz, Double_t *origin)
         	:TGeoBBox(dx, dy, dz, origin)
{
// Constructor where half-lengths are provided.
	#ifdef PRINT_CONSTRUCTORS
      Info("Box", "Constructor");
   #endif
}

//_____________________________________________________________________________
Box::Box(const char *name, Double_t dx, Double_t dy, Double_t dz, Double_t *origin)
         	:TGeoBBox(name, dx, dy, dz, origin)
{
// Constructor with shape name.
   #ifdef PRINT_CONSTRUCTORS
      Info("Box", "Constructor");
   #endif
}

//_____________________________________________________________________________
Box::Box(Double_t *param)
         	:TGeoBBox(param)
{
// Constructor based on the array of parameters
   #ifdef PRINT_CONSTRUCTORS
      Info("Box", "Constructor");
   #endif
}

//_____________________________________________________________________________
Box::~Box()
{
// Destructor
	#ifdef PRINT_CONSTRUCTORS
      Info("Box", "Destructor");
   #endif
}

//_____________________________________________________________________________
Double_t Box::TimeFromInside(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t /*stepTime*/, const Bool_t onBoundary) const
{
	// This method calculates the time of all possible intersections of the particle's future path
	// with all possible boundaries of the current shape.
	// Method then compares the times found and returns the smallest, non-zero value. 
	// In the case of an error, a value of zero, or a negative value will be returned.
	
	#ifdef VERBOSE_MODE		
		cout << "Box::TimeFromInside " << endl;
	#endif
	// -- Determine the local point in case the box origin is not as the local centre (ie: 0,0,0)
	// -- (Not sure of why this would ever be the case, but ROOT does allow you to set the origin...)
	Double_t localpt[3];
	for (Int_t i=0; i<3; i++) localpt[i] = point[i] - fOrigin[i];
	
	// ----------------------------------------------------------------------
	// -- Calculate actual time to boundary
	Double_t tfinal = 0.; // The smallest time to reach ANY boundary;
	Double_t boundary[3] = {fDX, fDY, fDZ}; // Store the coordinates of the box boundaries
		
	#ifdef VERBOSE_MODE		
		cout << "TimeFromInside - (Local) +Boundaries - +X: " << boundary[0] << "\t" << "+Y: " << boundary[1] << "\t" <<  "+Z: " << boundary[2] << endl;
		cout << "TimeFromInside - Origin - +X: " << fOrigin[0] << "\t" << "+Y: " << fOrigin[1] << "\t" <<  "+Z: " << fOrigin[2] << endl;
		cout << "TimeFromInside - (Local) point - +X: " << localpt[0] << "\t" << "+Y: " << localpt[1] << "\t" <<  "+Z: " << localpt[2] << endl;
		cout << "TimeFromInside - (Local) velocity - +X: " << velocity[0] << "\t" << "+Y: " << velocity[1] << "\t" <<  "+Z: " << velocity[2] << endl;
		cout << "TimeFromInside - (Local) field direction - +X: " << field[0] << "\t" << "+Y: " << field[1] << "\t" <<  "+Z: " << field[2] << endl;
	#endif
			
	// ----------------------------------------------------------------------	
	// -- New root-finding / selecting process implementing Polynomial
	// -- Algorithm for Selecting ROOTs for Approaching boundary from INSIDE
	// ----------------------------------------------------------------------	
	// -- Loop over x, y, z boundaries/projections of parabola
	for (Int_t i=0; i<3; i++) {
		// -- Loop over +/- fDX,fDY,fDZ boundaries
		for (Int_t j=0; j<2; j++) {
			// -- Define the constants in the quadratic equation, a, b, c - see note book for details
			Double_t params[3] = {0., 0., 0.};
			params[0] = 0.5*field[i];
			params[1] = velocity[i]; 
			params[2] = localpt[i] - (TMath::Power(-1,j)*boundary[i]);
			
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
				cout << i << "\t" << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
				cout << "solutions: " << solutions << "\t" << "root 1: " << roots[0] << "\t" << "root 2: " << roots[1] << endl;
			#endif
			// -- Find the smallest, non-zero root
			Double_t tmin = Box::SmallestInsideTime(solutions, &roots[0], onBoundary);
			
			if (tmin > 0.0 && tfinal == 0.0) {
				// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
				tfinal = tmin;
			} else if (tmin > 0.0 && tmin < tfinal) {
				// -- Check if this time is smaller than the current overall smallest time to any boundary
				tfinal = tmin;
			}
		}
	}	
	
	// -- Analyse the final value for the time to nearest boundary from inside
	if (tfinal > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TimeFromInside - time to nearest boundary: " << tfinal << endl; 
		#endif
		return tfinal;
	} else {
		cout << "Error in Box, TimeFromInside - no time to boundary is found!" << endl;
		return 0;
	}
}

//_____________________________________________________________________________
Double_t Box::TimeFromInsideS(const Double_t* point, const Double_t* velocity, const Double_t* field, 
											const Double_t dx, const Double_t dy, const Double_t dz, const Double_t *origin, const Bool_t onBoundary)
{
	// This method calculates the time of all possible intersections of the particle's future
	// path with all possible boundaries of the current shape.
	// Method then compares the times found and returns the smallest, non-zero value. 
	
	#ifdef VERBOSE_MODE		
	cout << "Box::TimeFromInsideS" << endl;
	#endif
	// -- Determine the local point in case the box origin is not as the local centre (ie: 0,0,0)
	// -- (Not sure of why this would ever be the case, but ROOT does allow you to set the origin...)
	Double_t localpt[3];
	for (Int_t i=0; i<3; i++) localpt[i] = point[i] - origin[i];
	
	// ----------------------------------------------------------------------
	
	// -- Calculate actual time to boundary
	Double_t tfinal = 0.; // The smallest time to reach ANY boundary;
	Double_t boundary[3] = {dx, dy, dz}; // Store the coordinates of the box boundaries
		
	#ifdef VERBOSE_MODE		
		cout << "TimeFromInsideS - Positive (Local) Boundaries - +X: " << boundary[0] << "  +Y: " << boundary[1] << "  +Z: " << boundary[2] << endl;
		cout << "TimeFromInsideS - Origin - X: " << origin[0] << "\t" << "Y: " << origin[1] << "\t" <<  "Z: " << origin[2] << endl;
		cout << "TimeFromInsideS - (Local) point - +X: " << localpt[0] << "  +Y: " << localpt[1] << "  +Z: " << localpt[2] << endl;
		cout << "TimeFromInsideS - (Local) velocity - +X: " << velocity[0] << "  +Y: " << velocity[1] << "  +Z: " << velocity[2] << endl;
		cout << "TimeFromInsideS - (Local) field direction - +X: " << field[0] << "  +Y: " << field[1] << "  +Z: " << field[2] << endl;
	#endif
			
	// ----------------------------------------------------------------------	
	// -- New root-finding / selecting process implementing Polynomial
	// -- Algorithm for Selecting ROOTs for Approaching boundary from INSIDE
	// ----------------------------------------------------------------------	
	// -- Loop over x, y, z boundaries/projections of parabola
	for (Int_t i=0; i<3; i++) {
		// -- Loop over +/- fDX,fDY,fDZ boundaries
		for (Int_t j=0; j<2; j++) {
			// -- Define the constants in the quadratic equation, a, b, c - see note book for details
			Double_t params[3] = {0., 0., 0.};
			params[0] = 0.5*field[i];
			params[1] = velocity[i]; 
			params[2] = localpt[i] - (TMath::Power(-1,j)*boundary[i]);
			
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
				cout << i << "\t" << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
				cout << "solutions: " << solutions << "\t" << "root 1: " << roots[0] << "\t" << "root 2: " << roots[1] << endl;
			#endif
			
			// -- Find the smallest, non-zero root
			Double_t tmin = Box::SmallestInsideTime(solutions, &roots[0], onBoundary);
			
			if (tmin > 0.0 && tfinal == 0.0) {
				// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
				tfinal = tmin;
			} else if (tmin > 0.0 && tmin < tfinal) {
				// -- Check if this time is smaller than the current overall smallest time to any boundary
				tfinal = tmin;
			}
		}
	}	

	// -- Analyse the final value for the time to nearest boundary from inside
	if (tfinal > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TimeFromInside - time to nearest boundary: " << tfinal << endl; 
		#endif
		return tfinal;
	} else {
		cout << "Error in Box, TimeFromInside - no time to boundary is found!" << endl;
		return 0;
	}
}

//_____________________________________________________________________________
Double_t Box::TimeFromOutside(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t /*stepTime*/, const Bool_t onBoundary) const
{
	// This method calculates the time of all possible intersections of the particle's future path
	// with all possible boundaries of the current shape.
	// Method then compares the times found and checks that the corresponding point of intersection
	// with each boundary plane actually corresponds to a point  on the box. From these valid solutions,
	// the method	returns the smallest, non-zero value.
	
	#ifdef VERBOSE_MODE		
		cout << "Box::TimeFromOutside" << endl;
	#endif
	
   // -- Determine the local point in case the box origin is not as the local centre (ie: 0,0,0)
	// -- (Not sure of why this would ever be the case, but ROOT does allow you to set the origin...)
	Double_t localpt[3];
	for (Int_t i=0; i<3; i++) localpt[i] = point[i] - fOrigin[i];

	// ----------------------------------------------------------------------
	// -- Compute actual time to reach boundary
	Double_t tfinal = 0.; // The smallest time to reach any boundary;
	Double_t boundary[3] = {fDX, fDY, fDZ}; // Store the coordinates of the box boundaries
	
	#ifdef VERBOSE_MODE		
		cout << "TimeFromOutside - Positive (Local) Boundaries - +X: " << boundary[0] << "  +Y: " << boundary[1] << "  +Z: " << boundary[2] << endl;
		cout << "TimeFromOutside - Origin - +X: " << fOrigin[0] << "\t" << "+Y: " << fOrigin[1] << "\t" <<  "+Z: " << fOrigin[2] << endl;
		cout << "TimeFromOutside - (Local) point - +X: " << localpt[0] << "  +Y: " << localpt[1] << "  +Z: " << localpt[2] << endl;
		cout << "TimeFromOutside - (Local) velocity - +X: " << velocity[0] << "  +Y: " << velocity[1] << "  +Z: " << velocity[2] << endl;
		cout << "TimeFromOutside - (Local) field direction - +X: " << field[0] << "  +Y: " << field[1] << "  +Z: " << field[2] << endl;
	#endif
		
	// ----------------------------------------------------------------------	
	// -- New root-finding / selecting process implementing Polynomial
	// -- Algorithm for Selecting ROOTs for Approaching boundary from OUTSIDE
	// ----------------------------------------------------------------------	
	// -- Loop over x, y, z boundaries/projections of parabola
	for (Int_t i=0; i<3; i++) {
		// -- Loop over +/- fDX,fDY,fDZ boundaries
		for (Int_t j=0; j<2; j++) {
			// -- Define the constants in the quadratic equation, a, b, c - see note book for details
			Double_t params[3] = {0., 0., 0.};
			params[0] = 0.5*field[i];
			params[1] = velocity[i]; 
			params[2] = localpt[i] - (TMath::Power(-1,j)*boundary[i]);
			
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
				cout << i << "\t" << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
				cout << "solutions: " << solutions << "\t" << "root 1: " << roots[0] << "\t" << "root 2: " << roots[1] << endl;
			#endif

			// -- Find the smallest, non-zero root
			Double_t tmin = Box::SmallestOutsideTime(solutions, &roots[0], onBoundary, point, velocity, field, &boundary[0]);
		
			if (tmin > 0.0 && tfinal == 0.0) {
				// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
				tfinal = tmin;
			} else if (tmin > 0.0 && tmin < tfinal) {
				// -- Check if this time is smaller than the current overall smallest time to any boundary
				tfinal = tmin;
			}
		}
	}	

	// -- Analyse the final value of the shortest time to hit the boundary from outside
	if (tfinal > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TimeFromOutside - Time to nearest boundary: " << tfinal << endl; 
		#endif
		return tfinal;
	} else if (tfinal == 0.) {
		#ifdef VERBOSE_MODE		
			cout << "TimeFromOutside - No Boundary hit" << endl;
		#endif
		return TGeoShape::Big();
	} else {
		cout << "Error In Box, TimeFromOutside - Calculation error - time to boundary is negative" << endl;
		return 0;
	}
}

//_____________________________________________________________________________
Double_t Box::TimeFromOutsideS(const Double_t* point, const Double_t* velocity, const Double_t* field, 
											const Double_t dx, const Double_t dy, const Double_t dz, const Double_t *origin, const Bool_t onBoundary) 
{
	// This method calculates the time of all possible intersections of the particle's future path
	// with all possible boundaries of the current shape. Method then compares the times found and
	// checks that the corresponding point of intersection with each boundary plane actually corresponds
	// to a point on the box. From these valid solutions, the method	returns the smallest, non-zero value.
		
	// -- Determine the local point in case the box origin is not as the local centre (ie: 0,0,0)
	// -- (Not sure of why this would ever be the case, but ROOT does allow you to set the origin...)
	Double_t localpt[3];
	for (Int_t i=0; i<3; i++) localpt[i] = point[i] - origin[i];
	
	// ----------------------------------------------------------------------
	// -- Calculate the actual time to the boundary
	Double_t tfinal = 0.; // The smallest time to reach any boundary;
	Double_t boundary[3] = {dx, dy, dz}; // Store the coordinates of the box boundaries
	
	#ifdef VERBOSE_MODE		
		cout << "TimeFromOutsideS - Positive (Local) Boundaries - +X: " << boundary[0] << "  +Y: " << boundary[1] << "  +Z: " << boundary[2] << endl;
		cout << "TimeFromOutsideS - Origin - X: " << origin[0] << "\t" << "Y: " << origin[1] << "\t" <<  "Z: " << origin[2] << endl;
		cout << "TimeFromOutsideS - (Local) point - +X: " << localpt[0] << "  +Y: " << localpt[1] << "  +Z: " << localpt[2] << endl;
		cout << "TimeFromOutsideS - (Local) velocity - +X: " << velocity[0] << "  +Y: " << velocity[1] << "  +Z: " << velocity[2] << endl;
		cout << "TimeFromOutsideS - (Local) field direction - +X: " << field[0] << "  +Y: " << field[1] << "  +Z: " << field[2] << endl;
	#endif
	
	// ----------------------------------------------------------------------	
	// -- New root-finding / selecting process implementing Polynomial
	// -- Algorithm for Selecting ROOTs for Approaching boundary from OUTSIDE
	// ----------------------------------------------------------------------	
	// -- Loop over x, y, z boundaries/projections of parabola
	for (Int_t i=0; i<3; i++) {
		// -- Loop over +/- fDX,fDY,fDZ boundaries
		for (Int_t j=0; j<2; j++) {
			// -- Define the constants in the quadratic equation, a, b, c - see note book for details
			Double_t params[3] = {0., 0., 0.};
			params[0] = 0.5*field[i];
			params[1] = velocity[i]; 
			params[2] = localpt[i] - (TMath::Power(-1,j)*boundary[i]);
			
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
				cout << i << "\t" << j << "\t" << "a: " << params[0] << "\t" << "b: " << params[1] << "\t" << "c: " << params[2] << "\t";
				cout << "solutions: " << solutions << "\t" << "root 1: " << roots[0] << "\t" << "root 2: " << roots[1] << endl;
			#endif
			
			// -- Find the smallest, non-zero root
			Double_t tmin = Box::SmallestOutsideTime(solutions, &roots[0], onBoundary, point, velocity, field, &boundary[0]);
			
			if (tmin > 0.0 && tfinal == 0.0) {
				// -- If the current overall smallest time to any boundary is zero, initialise it to the first non-zero time to a boundary
				tfinal = tmin;
			} else if (tmin > 0.0 && tmin < tfinal) {
				// -- Check if this time, the smallest, non-zero time to the current boundary, is smaller
				// -- than the current overall smallest time to any boundary
				tfinal = tmin;
			}
		}
	}	

	// -- Analyse the final value of the shortest time to hit the boundary from outside
	if (tfinal > 0.) { 
		#ifdef VERBOSE_MODE		
			cout << "TimeFromOutside - Time to nearest boundary: " << tfinal << endl; 
		#endif
		return tfinal;
	} else if (tfinal == 0.) {
		#ifdef VERBOSE_MODE		
			cout << "TimeFromOutside - No Boundary hit" << endl;
		#endif
		return TGeoShape::Big();
	} else {
		cout << "Error In Box, TimeFromOutside - Calculation error - time to boundary is negative" << endl;
		return 0;
	}
}

//_____________________________________________________________________________
Bool_t Box::IsNextPointOnBox(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t* boundary, const Double_t t) 
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

//_____________________________________________________________________________
Double_t Box::SmallestInsideTime(const Int_t solutions, Double_t* roots, const Bool_t onBoundary)
{
	// -- Determining number of roots, and select the smallest, real, non-zero value. 
	
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
				cout << "SmallestInsideTime - Two roots are both very small" << endl;
				cout << "Root 1: " << roots[0] << "\t" << "Root 2: " << roots[1] << endl;
				throw runtime_error("Two very small roots encountered. Unsure how to proceed.");
			}
			for (Int_t i = 0; i < 2; i++) {
				if (TMath::Abs(roots[i]) < 1.E-8) { 
					#ifdef VERBOSE_MODE
						cout << "SmallestInsideTime - Root[" << i << "]: "<< roots[i] << ", is < 1.E-8. Setting to zero." << endl;
					#endif
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
			if (TMath::Abs(roots[0]) < 1.E-8) {
				#ifdef VERBOSE_MODE
					cout << "SmallestInsideTime - Single Root found to be < 1.E-8 : " << roots[0] << endl;
				#endif
			}
		} else {
			// Nothing to be done - both roots should be zero anyway - no solutions found
		}
	}	
	
	Double_t tmin = 0.0;
	// Now detemine the smallest, non-zero root.
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
				cout << "SmallestInsideTime - Both roots are negative or zero" << endl; 
			#endif
			return 0;
		}
	} else if (solutions == 1) {
		// -- One Root
		if (roots[0] > 0.) {
			tmin = roots[0];
		} else {
			//-- Only Root is negative or zero
			#ifdef VERBOSE_MODE
				cout << "SmallestInsideTime - Only root is nagative or zero" << endl;
			#endif
			return 0;
		}
	} else {
		// -- No Real Roots
		#ifdef VERBOSE_MODE
			cout << "SmallestInsideTime - No real roots - Cannot cross boundary" << endl;
		#endif
		return 0;
	}
	// Return the smallest time found
	return tmin;
}

//_____________________________________________________________________________
Double_t Box::SmallestOutsideTime(const Int_t solutions, Double_t* roots, const Bool_t onBoundary, const Double_t* point, 
											const Double_t* velocity, const Double_t* field, const Double_t* boundary)
{
	// -- Determing number of roots, and select the smallest, real, non-zero value. 
	
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
				cout << "SmallestOutsideTime - Two roots are both very small" << endl;
				cout << "Root 1: " << roots[0] << "\t" << "Root 2: " << roots[1] << endl;
				throw runtime_error("Two very small roots encountered. Unsure how to proceed.");
			}
			for (Int_t i = 0; i < 2; i++) {
				if (TMath::Abs(roots[i]) < 1.E-8) {
					#ifdef VERBOSE_MODE
						cout << "SmallestOutsideTime - Root[" << i << "]: "<< roots[i] << ", is < 1.E-8. Setting to zero." << endl;
					#endif
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
			if (TMath::Abs(roots[0]) < 1.E-8) {
				#ifdef VERBOSE_MODE
					cout << "SmallestOutsideTime - Single Root found to be < 1.E-8 : " << roots[0] << endl;
				#endif
			}
		} else {
			// Nothing to be done - both roots should be zero anyway - no solutions found
		}
	}
	
	Double_t tmin = 0.0;
	// Now detemine the smallest, non-zero root.
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
					cout << "SmallestOutsideTime - Both roots are negative, zero or invalid" << endl; 
				#endif
				return 0;
			}
		}
	} else if (solutions == 1) {
		// -- One Root
		if (roots[0] > 0. && IsNextPointOnBox(point, velocity, field, boundary, roots[0]) == kTRUE) {
			tmin = roots[0];
		} else {
			//-- Only Root is negative or zero
			#ifdef VERBOSE_MODE		
				cout << "SmallestOutsideTime - Only root is negative, zero or invalid" << endl; 
			#endif
			return 0;
		}
	} else {
		// -- No Real Roots
		#ifdef VERBOSE_MODE		
			cout << "SmallestOutsideTime - No solutions" << endl; 
		#endif
		return 0;
	}
	// Return the smallest time found
	return tmin;
}
