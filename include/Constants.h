// CLHEP/Units/PhysicalConstants.h

// -*- C++ -*-
// $Id: PhysicalConstants.h,v 1.4 1998/07/03 15:11:51 evc Exp $
// ----------------------------------------------------------------------
// HEP coherent Physical Constants
//
// This file has been provided by Geant4 (simulation toolkit for HEP).
//
// The basic units are :
//  		millimeter  
// 		nanosecond  
// 		Mega electron Volt  
// 		positon charge 
// 		degree Kelvin
//              amount of substance (mole)
//              luminous intensity (candela)
// 		radian  
//              steradian 
//
// Below is a non exhaustive list of Physical CONSTANTS,
// computed in the Internal HEP System Of Units.
//
// Most of them are extracted from the Particle Data Book :
//        Phys. Rev. D  volume 50 3-1 (1994) page 1233
// 
//        ...with a meaningful (?) name ...
//
// You can add your own constants.
//
// Author: M.Maire
//
// History:
//
// 23.02.96 Created
// 26.03.96 Added constants for standard conditions of temperature
//          and pressure; also added Gas threshold.


#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Units.h"

using namespace Units;

namespace Constants 
{
	//
	//
	//
	static const Double_t     pi  = 3.14159265358979323846;
	static const Double_t  twopi  = 2*pi;
	static const Double_t halfpi  = pi/2;
	static const Double_t     pi2 = pi*pi;

	//
	// c   = 299.792458 mm/ns
	// c^2 = 898.7404 (mm/ns)^2 
	//
	static const Double_t c_light   = 2.99792458e+8 *m/s;
	static const Double_t c_squared = c_light * c_light;

	//
	// h     = 4.13566e-12 MeV*ns
	// hbar  = 6.58212e-13 MeV*ns
	// hbarc = 197.32705e-12 MeV*mm
	//
	static const Double_t h_Planck      = 6.6260755e-34 ; // J.s
	static const Double_t hbar          = h_Planck/twopi;
	static const Double_t hbarc         = hbar * c_light;
	static const Double_t hbarc_squared = hbarc * hbarc;
   static const Double_t hbar_eV       = hbar * joule * s;
	
	//
	//
	static const Double_t electron_charge = - eplus; // Units.h
	static const Double_t e_squared = eplus * eplus;

	//
	// amu_c2 - atomic equivalent mass unit
	// amu    - atomic mass unit
	//
	static const Double_t electron_mass_c2 = 0.51099906 * MeV;
	static const Double_t   proton_mass_c2 = 938.27231 * MeV;
	static const Double_t  neutron_mass_c2 = 939.56563 * MeV;
	static const Double_t           amu_c2 = 931.49432 * MeV;
	static const Double_t              amu = amu_c2/c_squared;

	static const Double_t     neutron_mass = (neutron_mass_c2/c_squared);
	static const Double_t     neutron_mass_kg = neutron_mass*e_SI;
	

	//
	// permeability of free space mu0    = 2.01334e-16 Mev*(ns*eplus)^2/mm
	// permittivity of free space epsil0 = 5.52636e+10 eplus^2/(MeV*mm)
	//
	static const Double_t mu0      = 4*pi*1.e-7 * henry/m;
	static const Double_t epsilon0 = 1./(c_squared*mu0);

	//
	// electromagnetic coupling = 1.43996e-12 MeV*mm/(eplus^2)
	//
	static const Double_t elm_coupling           = e_squared/(4*pi*epsilon0);
	static const Double_t fine_structure_const   = elm_coupling/hbarc;
	static const Double_t classic_electr_radius  = elm_coupling/electron_mass_c2;
	static const Double_t electron_Compton_length = hbarc/electron_mass_c2;
	static const Double_t Bohr_radius = electron_Compton_length/fine_structure_const;

	static const Double_t alpha_rcl2 = fine_structure_const
	                                   *classic_electr_radius
	                                   *classic_electr_radius;

	static const Double_t twopi_mc2_rcl2 = twopi*electron_mass_c2
	                                             *classic_electr_radius
	                                             *classic_electr_radius;
	//
	//
	static const Double_t k_Boltzmann = 8.617385e-11 * MeV/kelvin;

	//
	//
	static const Double_t STP_Temperature = 273.15*kelvin;
	static const Double_t STP_Pressure    = 1.*atmosphere;

	//
	//
	static const Double_t grav_acceleration = 9.80665*m/(s*s);
	static const Double_t height_equivalent_conversion = grav_acceleration*neutron_mass;
	//
	//
   static const Double_t avagadro_number = 6.02214E+23/mole;
}
#endif /* CONSTANTS_H */
