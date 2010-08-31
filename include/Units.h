// CLHEP/Units/SystemOfUnits.h

// -*- C++ -*-
// $Id: SystemOfUnits.h,v 1.11 2001/03/05 16:09:30 evc Exp $
// ----------------------------------------------------------------------
// HEP coherent system of Units
//
// This file has been provided to CLHEP by Geant4 (simulation toolkit for HEP).
//
// The basic units are :
//  		millimeter              (millimeter)
// 		nanosecond              (nanosecond)
// 		Mega electron Volt      (MeV)
// 		positron charge         (eplus)
// 		degree Kelvin           (kelvin)
//       the amount of substance (mole)
//       luminous intensity      (candela)
// 		radian                  (radian)
//       steradian               (steradian)
//
// Below is a non exhaustive list of derived and pratical units
// (i.e. mostly the SI units).
// You can add your own units.
//
// The SI numerical value of the positron charge is defined here,
// as it is needed for conversion factor : positron charge = e_SI (coulomb)
//
// The others physical constants are defined in the header file :
//			PhysicalConstants.h
//
// Authors: M.Maire, S.Giani
//
// History:
//
// 06.02.96   Created.
// 28.03.96   Added miscellaneous constants.
// 05.12.97   E.Tcherniaev: Redefined pascal (to avoid warnings on WinNT)
// 20.05.98   names: meter, second, gram, radian, degree
//            (from Brian.Lasiuk@yale.edu (STAR)). Added luminous units.
// 05.08.98   angstrom, picobarn, microsecond, picosecond, petaelectronvolt
// 01.03.01   parsec    

#ifndef HEP_SYSTEM_OF_UNITS_H
#define HEP_SYSTEM_OF_UNITS_H

#include "TROOT.h"

namespace Units 
{
	// 
	// Length [L]
	//
	static const Double_t meter  = 1.0;                  
	static const Double_t meter2 = meter*meter;
	static const Double_t meter3 = meter*meter*meter;

	static const Double_t millimeter  = 1.e-3*meter;                        
	static const Double_t millimeter2 = millimeter*millimeter;
	static const Double_t millimeter3 = millimeter*millimeter*millimeter;

	static const Double_t centimeter  = 1.e-2*meter;   
	static const Double_t centimeter2 = centimeter*centimeter;
	static const Double_t centimeter3 = centimeter*centimeter*centimeter;

	static const Double_t kilometer = 1000.*meter;                   
	static const Double_t kilometer2 = kilometer*kilometer;
	static const Double_t kilometer3 = kilometer*kilometer*kilometer;

	static const Double_t parsec = 3.0856775807e+16*meter;

	static const Double_t micrometer = 1.e-6 *meter;             
	static const Double_t  nanometer = 1.e-9 *meter;
	static const Double_t   angstrom = 1.e-10*meter;
	static const Double_t femtometer = 1.e-15*meter;

	static const Double_t      barn = 1.e-28*meter2;
	static const Double_t millibarn = 1.e-3 *barn;
	static const Double_t microbarn = 1.e-6 *barn;
	static const Double_t  nanobarn = 1.e-9 *barn;
	static const Double_t  picobarn = 1.e-12*barn;

   static const Double_t      litre = 1.E-3*meter3;
   static const Double_t millilitre = 1.E-3*litre;

	// symbols
   static const Double_t fm  = femtometer;
	
	static const Double_t mm  = millimeter;
	static const Double_t mm2 = millimeter2;
	static const Double_t mm3 = millimeter3;

	static const Double_t cm  = centimeter;   
	static const Double_t cm2 = centimeter2;
	static const Double_t cm3 = centimeter3;

	static const Double_t m  = meter;
	static const Double_t m2 = meter2;
	static const Double_t m3 = meter3;

	static const Double_t km  = kilometer;
	static const Double_t km2 = kilometer2;
	static const Double_t km3 = kilometer3;

	static const Double_t pc = parsec;
	
	static const Double_t  l = litre;
   static const Double_t ml = millilitre;

	//
	// Angle
	//
	static const Double_t radian      = 1.;                  
	static const Double_t milliradian = 1.e-3*radian;
	static const Double_t degree = (3.14159265358979323846/180.0)*radian;

	static const Double_t   steradian = 1.;
	
	// symbols
	static const Double_t rad  = radian;	
	static const Double_t mrad = milliradian;
	static const Double_t sr   = steradian;
	static const Double_t deg  = degree;

	//
	// Time [T]
	//
	static const Double_t second      = 1.;
	static const Double_t millisecond = 1.e-3 *second;
	static const Double_t microsecond = 1.e-6 *second;
	static const Double_t nanosecond  = 1.e-9 *second;
	static const Double_t  picosecond = 1.e-12*second;

	static const Double_t hertz = 1./second;
	static const Double_t kilohertz = 1.e+3*hertz;
	static const Double_t megahertz = 1.e+6*hertz;

	// symbols
	static const Double_t ns = nanosecond;			
	static const Double_t  s = second;
	static const Double_t ms = millisecond;

	//
	// Electric charge [Q]
	//
	static const Double_t eplus = 1. ;		// positron charge
	static const Double_t e_SI  = 1.60217733e-19;	// positron charge in coulomb
	static const Double_t coulomb = eplus/e_SI;	// coulomb = 6.24150 e+18 * eplus

	//
	// Energy [E]
	//
	static const Double_t     electronvolt = 1.;
	static const Double_t nanoelectronvolt = 1.e-9*electronvolt;
	static const Double_t kiloelectronvolt = 1.e+3*electronvolt;
	static const Double_t megaelectronvolt = 1.e+6*electronvolt;
	static const Double_t gigaelectronvolt = 1.e+9*electronvolt;
	static const Double_t teraelectronvolt = 1.e+12*electronvolt;
	static const Double_t petaelectronvolt = 1.e+15*electronvolt;

	static const Double_t joule = electronvolt/e_SI;	// joule = 6.24150 e+12 * MeV

	// symbols
	static const Double_t neV = nanoelectronvolt;
	static const Double_t  eV = electronvolt;
	static const Double_t keV = kiloelectronvolt;
	static const Double_t MeV = megaelectronvolt;
	static const Double_t GeV = gigaelectronvolt;
	static const Double_t TeV = teraelectronvolt;
	static const Double_t PeV = petaelectronvolt;

	//
	// Mass [E][T^2][L^-2]
	//
	static const Double_t  kilogram = 1.; // joule*second*second/(meter*meter);   
	static const Double_t      gram = 1.e-3*kilogram;
	static const Double_t milligram = 1.e-3*gram;

	// symbols
	static const Double_t  kg = kilogram;
	static const Double_t   g = gram;
	static const Double_t  mg = milligram;

	//
	// Power [E][T^-1]
	//
	static const Double_t watt = joule/second;	// watt = 6.24150 e+3 * MeV/ns

	//
	// Force [E][L^-1]
	//
	static const Double_t newton = joule/meter;	// newton = 6.24150 e+9 * MeV/mm

	//
	// Pressure [E][L^-3]
	//
	static const Double_t pascal = newton/m2;	   	// pascal = 6.24150 e+3 * MeV/mm3
	static const Double_t bar        = 100000*pascal; // bar    = 6.24150 e+8 * MeV/mm3
	static const Double_t atmosphere = 101325*pascal; // atm    = 6.32420 e+8 * MeV/mm3

	//
	// Electric current [Q][T^-1]
	//
	static const Double_t      ampere = coulomb/second; // ampere = 6.24150 e+9 * eplus/ns
	static const Double_t milliampere = 1.e-3*ampere;
	static const Double_t microampere = 1.e-6*ampere;
	static const Double_t  nanoampere = 1.e-9*ampere;

	//
	// Electric potential [E][Q^-1]
	//
	static const Double_t megavolt = megaelectronvolt/eplus;
	static const Double_t kilovolt = 1.e-3*megavolt;
	static const Double_t     volt = 1.e-6*megavolt;

	//
	// Electric resistance [E][T][Q^-2]
	//
	static const Double_t ohm = volt/ampere;	// ohm = 1.60217e-16*(MeV/eplus)/(eplus/ns)

	//
	// Electric capacitance [Q^2][E^-1]
	//
	static const Double_t farad = coulomb/volt;	// farad = 6.24150e+24 * eplus/Megavolt
	static const Double_t millifarad = 1.e-3*farad;
	static const Double_t microfarad = 1.e-6*farad;
	static const Double_t  nanofarad = 1.e-9*farad;
	static const Double_t  picofarad = 1.e-12*farad;

	//
	// Magnetic Flux [T][E][Q^-1]
	//
	static const Double_t weber = volt*second;	// weber = 1000*megavolt*ns
	
	//
	// Magnetic Field [T]
	//
	static const Double_t tesla     = 1.0;
	static const Double_t uT        = 1.E-6*tesla;
   static const Double_t nT        = 1.E-9*tesla;
   
	//
	// Inductance [T^2][E][Q^-2]
	//
	static const Double_t henry = weber/ampere;	// henry = 1.60217e-7*MeV*(ns/eplus)**2

	//
	// Temperature
	//
	static const Double_t kelvin = 1.;

	//
	// Amount of substance
	//
	static const Double_t mole = 1.;

	//
	// Activity [T^-1]
	//
	static const Double_t becquerel = 1./second ;
	static const Double_t curie = 3.7e+10 * becquerel;

	//
	// Absorbed dose [L^2][T^-2]
	//
	static const Double_t gray = joule/kilogram ;

	//
	// Luminous intensity [I]
	//
	static const Double_t candela = 1.;

	//
	// Luminous flux [I]
	//
	static const Double_t lumen = candela*steradian;

	//
	// Illuminance [I][L^-2]
	//
	static const Double_t lux = lumen/meter2;

	//
	// Miscellaneous
	//
	static const Double_t perCent     = 0.01 ;
	static const Double_t perThousand = 0.001;
	static const Double_t perMillion  = 0.000001;
	//
	static const Double_t tolerance = 1.E-10;
	

}

#endif /* HEP_SYSTEM_OF_UNITS_H */
