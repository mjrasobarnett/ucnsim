#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Units.hpp"

using namespace Units;

namespace Constants 
{
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
   static const Double_t electron_charge = -eplus; // Units.h
   static const Double_t e_squared = eplus * eplus;
   static const Double_t mu0 = 4.0 * pi * 1.e-7; // (V.s)/(A.m)
   static const Double_t mu0_4pi = 1.e-7; // (V.s)/(A.m)

   //
   //
   static const Double_t grav_acceleration = 9.80665*m/(s*s);
   //
   //
   static const Double_t avagadro_number = 6.02214E+23/mole;
   
   //
   //
   static const Double_t boltzmann = 1.3806488E-23; // J/K
}

namespace Neutron {
   static const Double_t   mass_eV = 939.56563 * Units::MeV;  // Units of eV
   static const Double_t   mass_eV_c = mass_eV / Constants::c_light;  // Units of eV
   static const Double_t   mass_eV_c2 = mass_eV / Constants::c_squared;  // Units of eV
   static const Double_t   mass_Kg = mass_eV_c2 * Units::e_SI;  // Units of eV
   
   static const Double_t   g_factor = -3.82608545;
   static const Double_t   gyromag_ratio = 1.83247185626E+8; /*29.1646954 Hz/uT * 2pi*/
   
   static const Double_t   lifetime = 885.7;     // Units s
   
   static const Double_t height_equivalent_conversion = Constants::grav_acceleration * mass_eV_c2;
}

#endif /* CONSTANTS_H */
