// Parabola
// Author: Matthew Raso-Barnett  06/07/2009
#include <iostream>
#include <stdexcept>
#include <cassert>

#include "TMath.h"
#include "TGeoShape.h"

#include "Parabola.h"

using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(Parabola)

Parabola *Parabola::fgParabola = NULL;

//_____________________________________________________________________________
Parabola::Parabola()
				 :TObject()
{
// Default constructor.
//   Info("Parabola", "Constructor");
	fgParabola = this;
} 

//_____________________________________________________________________________
Parabola::Parabola(const Parabola& other)
				 :TObject(other)
{
// Copy constructor.
   Error("Parabola","Copying not allowed for Parabola");
} 

//_____________________________________________________________________________
Parabola::~Parabola()
{
// Destructor.
   Info("Parabola", "Destructor");
	fgParabola = NULL;
}   

//_____________________________________________________________________________
Parabola &Parabola::operator=(const Parabola&)
{
// Assignment.
   Error("Parabola","Assignment not allowed for TGeoBuilder");
   return *this;
}

//_____________________________________________________________________________
Parabola *Parabola::Instance() 
{
// Return pointer to singleton.  
   if (!fgParabola) fgParabola = new Parabola();
	return fgParabola;
}   

//_____________________________________________________________________________
Double_t Parabola::ArcLength(const Double_t* velocity, const Double_t* field, const Double_t steptime) const
{
	// This method takes a parabola and calculates its length for a given period of time. These formulas are worked out completely in my notes
	
	// -- First check the value of the time given.
	#ifdef VERBOSE_MODE
		Info("ArcLength","steptime: %f", steptime);
	#endif
	
	if (steptime == TGeoShape::Big()) {
		// -- Large steptime indicates that no boundary is within range
		return TGeoShape::Big();
	} else if (steptime == 0.0) {
		#ifdef VERBOSE_MODE
			Warning("ArcLength", "steptime is 0 - returning distance as zero");
		#endif
		return 0.0;
	}
	
	
	Double_t initial_velocity = TMath::Sqrt((velocity[0]*velocity[0]) + (velocity[1]*velocity[1]) + (velocity[2]*velocity[2]));
	
	// -- Calculate the constants of quadratic equation
	Double_t a, b, c;
	a = (field[0]*field[0]) + (field[1]*field[1]) + (field[2]*field[2]);
	b = 2.0*((velocity[0]*field[0]) + (velocity[1]*field[1]) + (velocity[2]*field[2])); 
	c = initial_velocity*initial_velocity;
	
	#ifdef VERBOSE_MODE
		Info("ArcLength","Velocity - x: %f, y: %f, z: %f", velocity[0], velocity[1], velocity[2]);
		Info("ArcLength","Field - x: %f, y: %f, z: %f", field[0], field[1], field[2]);
		Info("ArcLength","Velocity: %f ", initial_velocity);
		Info("ArcLength","a: %f, b: %f, c: %f", a, b, c);
	#endif
	
	Double_t t = 0.; 
	Double_t method1[2], method2[2];
	
	// -- Method 1 and 2 correspond to the integral of our equation for the distance according to two equivalent methods. 
	// -- Eventually kill the second method so we only calculate this once. 
	
	// -- The array indices simily indicate the integral limits - values for t that we input. 
	// -- t = 0
	method1[0] = ((2.0*a*t + b)*TMath::Sqrt(a*t*t + b*t + c))/(4.0*a) + ((4.0*a*c - b*b)*TMath::ASinH((2.0*a*t +b)/TMath::Sqrt(4.0*a*c - b*b)))/(8.0*a*TMath::Sqrt(a));
	
	method2[0] = ((2.0*a*t + b)*TMath::Sqrt(a*t*t + b*t + c))/(4.0*a) + ((4.0*a*c - b*b)*TMath::Log(2.0*TMath::Sqrt(a*a*t*t +a*b*t +a*c) + (2.0*a*t) + b))/(8.0*a*TMath::Sqrt(a));
	
	// -- t = steptime
	t = steptime;
	method1[1] = ((2.0*a*t + b)*TMath::Sqrt(a*t*t + b*t + c))/(4.0*a) + ((4.0*a*c - b*b)*TMath::ASinH((2.0*a*t +b)/TMath::Sqrt(4.0*a*c - b*b)))/(8.0*a*TMath::Sqrt(a));
	
	method2[1] = ((2.0*a*t + b)*TMath::Sqrt(a*t*t + b*t + c))/(4.0*a) + ((4.0*a*c - b*b)*TMath::Log(2.0*TMath::Sqrt(a*a*t*t +a*b*t +a*c) + (2.0*a*t) + b))/(8.0*a*TMath::Sqrt(a));
	
	// -- Calculate solution from integral limits
	Double_t solution1, solution2;
	solution1 = method1[1] - method1[0];
	solution2 = method2[1] - method2[0];
	
	// -- Check that both calculation methods give the same answer within tolerance
	assert(TMath::Abs(solution1 - solution2) < 1.e-10); 
	// -- Check solution is greater than zero
	assert(solution1 > 0.0);	
	
	#ifdef VERBOSE_MODE
		Info("ArcLength","Distance: %f", solution1);
	#endif
	return solution1;
}
