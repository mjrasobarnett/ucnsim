// Materials.h -- Stores all relevant UCN-material parameters


#ifndef MATERIALS_H
#define MATERIALS_H

#include <iostream>

#include "Units.h"
#include "Constants.h"

#include "TMath.h"
#include "TGeoManager.h"

#include "Material.h"
#include "Element.h"

using std::cout;
using std::endl;

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
   }
   
   namespace FermiPotential {
      // List of Experimentally determined fermi potentials
      // Useful for large discrepancies between experiment and theoretical prediction
   }
   
   namespace WPotential {
      // List of Experimentally determined Loss potentials
      // Useful for large discrepancies between experiment and theoretical prediction
   }
   
   namespace LossFactor {
      // List of Experimentally determined Loss potentials
      // Useful for large discrepancies between experiment and theoretical prediction
      static const Double_t beryllium = 2.0E-4;
   }
   
   //_____________________________________________________________________________
   Bool_t BuildMaterials(TGeoManager* geoManager) {
      // Create the materials/media defined above
      cout << "---------------------------------" << endl;
      cout << "Building Experiment Materials List" << endl;
      cout << "---------------------------------" << endl;
      if (!geoManager) {
         cout << "Error: Initialise GeoManager First before instantiating Materials" << endl;
         return kFALSE;
      }
      //________________________________________
      // BOUNDARY MATERIALS      
      // - Hydrogen
      Element* elemHydrogen = new Element("H", "Hydrogen", Hydrogen::z, Hydrogen::a, Hydrogen::scatLength, Hydrogen::cohCrossSec, Hydrogen::incohCrossSec, Hydrogen::absCrossSec);
      Material* matHydrogen = new Material("Hydrogen", elemHydrogen, Densities::hydrogen);
      TGeoMedium* hydrogen = new TGeoMedium("Hydrogen", 1, matHydrogen);
      cout << "Added Material: " << hydrogen->GetName() << endl;
      /*
      cout << "---------------------------------" << endl;
      cout << "Density (kg/m^3): " << Densities::hydrogen << endl;
      cout << "Coherent Cross Sec: " << elemHydrogen->CoherentCrossSec() << endl;
      cout << "InCoh Cross Sec: " << elemHydrogen->InCoherentCrossSec() << endl;
      cout << "Scat Length: " << elemHydrogen->ScatLength() << endl;
      cout << "Abs Cross Sec: " << elemHydrogen->AbsorbtionCrossSec() << endl;
      cout << "Loss Cross Sec: " << elemHydrogen->LossCrossSec() << endl;
      cout << "V_f: " << matHydrogen->FermiPotential() << endl;
      cout << "W: " << matHydrogen->WPotential() << endl;
      cout << "LossFactor: " << matHydrogen->LossFactor() << endl;
      cout << "---------------------------------" << endl;
      */
      // - Beryllium
      Element* elemBeryllium = new Element("Be", "Beryllium", Beryllium::z, Beryllium::a, Beryllium::scatLength, Beryllium::cohCrossSec, Beryllium::incohCrossSec, Beryllium::absCrossSec);
      Material* matBeryllium = new Material("Beryllium", elemBeryllium, Densities::beryllium);
      TGeoMedium* beryllium = new TGeoMedium("Beryllium", 1, matBeryllium);
      // Set Beryllium Loss factor to the experimentally determined value
      matBeryllium->LossFactor(LossFactor::beryllium);
      cout << "Added Material: " << beryllium->GetName() << endl;
      /*
      cout << "---------------------------------" << endl;
      cout << "Density (kg/m^3): " << Densities::beryllium << endl;
      cout << "Coherent Cross Sec: " << elemBeryllium->CoherentCrossSec() << endl;
      cout << "InCoh Cross Sec: " << elemBeryllium->InCoherentCrossSec() << endl;
      cout << "Scat Length: " << elemBeryllium->ScatLength() << endl;
      cout << "Abs Cross Sec: " << elemBeryllium->AbsorbtionCrossSec() << endl;
      cout << "Loss Cross Sec: " << elemBeryllium->LossCrossSec() << endl;
      cout << "V_f: " << matBeryllium->FermiPotential() << endl;
      cout << "W: " << matBeryllium->WPotential() << endl;
      cout << "LossFactor: " << matBeryllium->LossFactor() << endl;
      cout << "---------------------------------" << endl;
      */
      // - Copper
      Element* elemCopper = new Element("Cu", "Copper", Copper::z, Copper::a, Copper::scatLength, Copper::cohCrossSec, Copper::incohCrossSec, Copper::absCrossSec);
      Material* matCopper = new Material("Copper", elemCopper, Densities::copper);
      TGeoMedium* copper = new TGeoMedium("Copper", 1, matCopper);
      cout << "Added Material: " << copper->GetName() << endl;
      /*
      cout << "---------------------------------" << endl;
      cout << "Density (kg/m^3): " << Densities::copper << endl;
      cout << "Coherent Cross Sec: " << elemCopper->CoherentCrossSec() << endl;
      cout << "InCoh Cross Sec: " << elemCopper->InCoherentCrossSec() << endl;
      cout << "Scat Length: " << elemCopper->ScatLength() << endl;
      cout << "Abs Cross Sec: " << elemCopper->AbsorbtionCrossSec() << endl;
      cout << "Loss Cross Sec: " << elemCopper->LossCrossSec() << endl;
      cout << "V_f: " << matCopper->FermiPotential() << endl;
      cout << "W: " << matCopper->WPotential() << endl;
      cout << "LossFactor: " << matCopper->LossFactor() << endl;
      cout << "---------------------------------" << endl;
      */
      // - Aluminium
      Element* elemAluminium = new Element("Al", "Aluminium", Aluminium::z, Aluminium::a, Aluminium::scatLength, Aluminium::cohCrossSec, Aluminium::incohCrossSec, Aluminium::absCrossSec);
      Material* matAluminium = new Material("Aluminium", elemAluminium, Densities::aluminium);
      TGeoMedium* aluminium = new TGeoMedium("Aluminium", 1, matAluminium);
      cout << "Added Material: " << aluminium->GetName() << endl;
      /*
      cout << "---------------------------------" << endl;
      cout << "Density (kg/m^3): " << Densities::aluminium << endl;
      cout << "Coherent Cross Sec: " << elemAluminium->CoherentCrossSec() << endl;
      cout << "InCoh Cross Sec: " << elemAluminium->InCoherentCrossSec() << endl;
      cout << "Scat Length: " << elemAluminium->ScatLength() << endl;
      cout << "Abs Cross Sec: " << elemAluminium->AbsorbtionCrossSec() << endl;
      cout << "Loss Cross Sec: " << elemAluminium->LossCrossSec() << endl;
      cout << "V_f: " << matAluminium->FermiPotential() << endl;
      cout << "W: " << matAluminium->WPotential() << endl;
      cout << "LossFactor: " << matAluminium->LossFactor() << endl;
      cout << "---------------------------------" << endl;
      */
      //________________________________________
      // TRACKING MATERIALS
      // - HeliumII
      Element* elemHeliumII = new Element("He", "HeliumII", HeliumII::z, HeliumII::a, HeliumII::scatLength, HeliumII::cohCrossSec, HeliumII::incohCrossSec, HeliumII::absCrossSec);
      Material* matHeliumII = new Material("HeliumII", elemHeliumII, Densities::heliumII);
      TGeoMedium* heliumII = new TGeoMedium("HeliumII", 1, matHeliumII);
      cout << "Added Material: " << heliumII->GetName() << endl;
      /*
      cout << "---------------------------------" << endl;
      cout << "Density (kg/m^3): " << Densities::heliumII << endl;
      cout << "Coherent Cross Sec: " << elemHeliumII->CoherentCrossSec() << endl;
      cout << "InCoh Cross Sec: " << elemHeliumII->InCoherentCrossSec() << endl;
      cout << "Scat Length: " << elemHeliumII->ScatLength() << endl;
      cout << "Abs Cross Sec: " << elemHeliumII->AbsorbtionCrossSec() << endl;
      cout << "Loss Cross Sec: " << elemHeliumII->LossCrossSec() << endl;
      cout << "V_f: " << matHeliumII->FermiPotential() << endl;
      cout << "W: " << matHeliumII->WPotential() << endl;
      cout << "LossFactor: " << matHeliumII->LossFactor() << endl;
      cout << "---------------------------------" << endl;
      */
      //________________________________________
      // BLACKHOLE MATERIALS
      // - Vacuum
      Element* elemVacuum = new Element("H", "Vacuum", Vacuum::z, Vacuum::a, Vacuum::scatLength, Vacuum::cohCrossSec, Vacuum::incohCrossSec, Vacuum::absCrossSec);
      Material* matVacuum = new Material("Vacuum", elemVacuum, Densities::vacuum);
      TGeoMedium* vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
      cout << "Added Material: " << vacuum->GetName() << endl;
      /*
      cout << "---------------------------------" << endl;
      cout << "Density (kg/m^3): " << Densities::vacuum << endl;
      cout << "Coherent Cross Sec: " << elemVacuum->CoherentCrossSec() << endl;
      cout << "InCoh Cross Sec: " << elemVacuum->InCoherentCrossSec() << endl;
      cout << "Scat Length: " << elemVacuum->ScatLength() << endl;
      cout << "Abs Cross Sec: " << elemVacuum->AbsorbtionCrossSec() << endl;
      cout << "Loss Cross Sec: " << elemVacuum->LossCrossSec() << endl;
      cout << "V_f: " << matVacuum->FermiPotential() << endl;
      cout << "W: " << matVacuum->WPotential() << endl;
      cout << "LossFactor: " << matVacuum->LossFactor() << endl;
      cout << "---------------------------------" << endl;
      */
      //________________________________________
      // DETECTOR MATERIALS
      // - Lithium6
      Element* elemLithium6 = new Element("H", "Lithium6", Lithium6::z, Lithium6::a, Lithium6::scatLength, Lithium6::cohCrossSec, Lithium6::incohCrossSec, Lithium6::absCrossSec);
      Material* matLithium6 = new Material("Lithium6", elemLithium6, Densities::lithium6);
      TGeoMedium* lithium6 = new TGeoMedium("Lithium6", 1, matLithium6);
      cout << "Added Material: " << lithium6->GetName() << endl;
      /*
      cout << "---------------------------------" << endl;
      cout << "Density (kg/m^3): " << Densities::lithium6 << endl;
      cout << "Coherent Cross Sec: " << elemLithium6->CoherentCrossSec() << endl;
      cout << "InCoh Cross Sec: " << elemLithium6->InCoherentCrossSec() << endl;
      cout << "Scat Length: " << elemLithium6->ScatLength() << endl;
      cout << "Abs Cross Sec: " << elemLithium6->AbsorbtionCrossSec() << endl;
      cout << "Loss Cross Sec: " << elemLithium6->LossCrossSec() << endl;
      cout << "V_f: " << matLithium6->FermiPotential() << endl;
      cout << "W: " << matLithium6->WPotential() << endl;
      cout << "LossFactor: " << matLithium6->LossFactor() << endl;
      cout << "---------------------------------" << endl;
      */
      cout << "Completed Initialising UCN Materials" << endl;
      return kTRUE;
   }
}
#endif /* MATERIALS_H */
