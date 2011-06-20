// Materials.h -- Stores all relevant UCN-material parameters
#ifndef MATERIALS_H
#define MATERIALS_H

#include "Units.h"
#include "Constants.h"

class TGeoManager;

// Namespace holding all ucn relevant elements properties
namespace Elements {
   
   // Elements and their UCN-relevant properties
   static const Double_t reference_energy = 0.0253*Units::eV;
   static const Double_t reference_velocity = 2.20*Units::km/Units::s;
   
   namespace Hydrogen {
      // Atomic number
      static const Int_t z = 1;
      // Atomic mass
      static const Double_t a = 1.00794*Units::g/Units::mole;
      // Bound Coherent Scattering Length
      static const Double_t scatLength = -3.7409*Units::fm;
      // Total Scattering Cross Section
      static const Double_t scatCrossSec = 82.06*Units::barn;
      // Coherent Scattering Cross Section
      static const Double_t cohCrossSec = 1.7568*Units::barn;
      // In-coherent Scattering Cross Section
      static const Double_t incohCrossSec = 80.26*Units::barn;
      // Thermal Absorption Cross Section at 0.0253eV
      static const Double_t absCrossSec = 0.3326*Units::barn;
   }
   
   namespace Beryllium {
      // Atomic number
      static const Int_t z = 4;
      // Atomic mass
      static const Double_t a = 9.0121*Units::g/Units::mole;
      // Bound Coherent Scattering Length
      static const Double_t scatLength = 7.79*Units::fm;
      // Total Scattering Cross Section
      static const Double_t scatCrossSec = 7.63*Units::barn;
      // Coherent Scattering Cross Section
      static const Double_t cohCrossSec = 7.63*Units::barn;
      // In-coherent Scattering Cross Section
      static const Double_t incohCrossSec = 0.0018*Units::barn;
      // Thermal Absorption Cross Section at 0.0253eV
      static const Double_t absCrossSec = 0.0076*Units::barn;
   }
   
   namespace Copper {
      // Atomic number
      static const Int_t z = 29;
      // Atomic mass
      static const Double_t a = 63.546*Units::g/Units::mole;
      // Bound Coherent Scattering Length
      static const Double_t scatLength = 7.718*Units::fm;
      // Total Scattering Cross Section
      static const Double_t scatCrossSec = 8.03*Units::barn;
      // Coherent Scattering Cross Section
      static const Double_t cohCrossSec = 7.485*Units::barn;
      // In-coherent Scattering Cross Section
      static const Double_t incohCrossSec = 0.55*Units::barn;
      // Thermal Absorption Cross Section at 0.0253eV
      static const Double_t absCrossSec = 3.78*Units::barn;
   }
   
   namespace Aluminium {
      // Atomic number
      static const Int_t z = 13;
      // Atomic mass
      static const Double_t a = 26.981*Units::g/Units::mole;
      // Bound Coherent Scattering Length
      static const Double_t scatLength = 3.449*Units::fm;
      // Total Scattering Cross Section
      static const Double_t scatCrossSec = 1.503*Units::barn;
      // Coherent Scattering Cross Section
      static const Double_t cohCrossSec = 1.495*Units::barn;
      // In-coherent Scattering Cross Section
      static const Double_t incohCrossSec = 0.0082*Units::barn;
      // Thermal Absorption Cross Section at 0.0253eV
      static const Double_t absCrossSec = 0.231*Units::barn;
   }
   
   namespace HeliumII {
      // Atomic number
      static const Int_t z = 2;
      // Atomic mass
      static const Double_t a = 4.0026*Units::g/Units::mole;
      // Bound Coherent Scattering Length
      static const Double_t scatLength = 3.26*Units::fm;
      // Total Scattering Cross Section
      static const Double_t scatCrossSec = 1.34*Units::barn;
      // Coherent Scattering Cross Section
      static const Double_t cohCrossSec = 1.34*Units::barn;
      // In-coherent Scattering Cross Section
      static const Double_t incohCrossSec = 0.0*Units::barn;
      // Thermal Absorption Cross Section at 0.0253eV
      static const Double_t absCrossSec = 0.0*Units::barn;
   }

   namespace Lithium6 {
      // Atomic number
      static const Int_t z = 3;
      // Atomic mass
      static const Double_t a = 6.015*Units::g/Units::mole;
      // Bound Coherent Scattering Length
      static const Double_t scatLength = 2.0*Units::fm;
      // Total Scattering Cross Section
      static const Double_t scatCrossSec = 0.97*Units::barn;
      // Coherent Scattering Cross Section
      static const Double_t cohCrossSec = 0.51*Units::barn;
      // In-coherent Scattering Cross Section
      static const Double_t incohCrossSec = 0.46*Units::barn;
      // Thermal Absorption Cross Section at 0.0253eV
      static const Double_t absCrossSec = 940.0*Units::barn;
   }
   
   namespace Vacuum {
      // Atomic number
      static const Int_t z = 0;
      // Atomic mass
      static const Double_t a = 0.0*Units::g/Units::mole;
      // Bound Coherent Scattering Length
      static const Double_t scatLength = 0.0*Units::fm;
      // Total Scattering Cross Section
      static const Double_t scatCrossSec = 0.0*Units::barn;
      // Coherent Scattering Cross Section
      static const Double_t cohCrossSec = 0.0*Units::barn;
      // In-coherent Scattering Cross Section
      static const Double_t incohCrossSec = 0.0*Units::barn;
      // Thermal Absorption Cross Section at 0.0253eV
      static const Double_t absCrossSec = 0.0*Units::barn;
   }
   
// End of Elements Namespace
}

// Namespace holding all ucn relevant materials properties
namespace Materials 
{
   using namespace Elements;
   
   namespace Densities {
      // List of material densities
      static const Double_t hydrogen = 0.07*Units::g/Units::cm3;
      static const Double_t beryllium = 1.85*Units::g/Units::cm3;
      static const Double_t copper = 8.94*Units::g/Units::cm3;
      static const Double_t aluminium = 2.70*Units::g/Units::cm3;
      static const Double_t lithium6 = 0.534*Units::g/Units::cm3;
      static const Double_t heliumII = 0.125*Units::g/Units::ml;
      static const Double_t vacuum = 0.0*Units::g/Units::cm3;
      
      // See Bodek et al. Storage of ultracold neutrons in high resistivity... Nuc. Inst. and Meth. (2008)
      static const Double_t ptfe = 2.18*Units::g/Units::cm3;
      static const Double_t quartz = 2.31*Units::g/Units::cm3;
   }
   
   namespace FermiPotential {
      // List of Experimentally determined fermi potentials
      // Useful for large discrepancies between experiment and theoretical prediction
      static const Double_t ptfe = 123.0*Units::neV;
      static const Double_t quartz = 95.0*Units::neV;
   }
   
   namespace WPotential {
      // List of Experimentally determined Loss potentials
      // Useful for large discrepancies between experiment and theoretical prediction
      
   }
   
   namespace LossFactor {
      // List of Experimentally determined Loss potentials
      // Useful for large discrepancies between experiment and theoretical prediction
      static const Double_t beryllium = 2.0E-4;
      
      // See Golub Lamoreaux, UCN Book, 
      static const Double_t ptfe = 0.0;
      // See Bodek et al. Storage of ultracold neutrons in high resistivity... Nuc. Inst. and Meth. (2008)
      static const Double_t quartz = 4.0E-4;
   }
   
   //_____________________________________________________________________________
   Bool_t BuildMaterials(TGeoManager* geoManager);
}
#endif /* MATERIALS_H */
