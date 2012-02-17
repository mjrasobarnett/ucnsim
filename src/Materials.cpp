#include <iostream>
#include <iomanip>

#include "Materials.h"

#include "TMath.h"
#include "TGeoManager.h"
#include "Material.h"
#include "Element.h"

using namespace std;
using namespace Elements;

Bool_t Materials::BuildMaterials(TGeoManager* geoManager) {
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
   cout.setf(std::ios::left, std::ios::adjustfield);
   cout << std::setw(15) << "Added Material" << "\t";
   cout << std::setw(15) << "Fermi (neV)" << "\t";
   cout << std::setw(15) << "Eta" << endl;
   // - Hydrogen
   Element* elemHydrogen = new Element("Hydrogen", Hydrogen::z, Hydrogen::a, Hydrogen::scatLength, Hydrogen::cohCrossSec, Hydrogen::incohCrossSec, Hydrogen::absCrossSec);
   Material* matHydrogen = new Material("Hydrogen", elemHydrogen, Densities::hydrogen);
   TGeoMedium* hydrogen = new TGeoMedium("Hydrogen", 1, matHydrogen);
   cout << std::setw(15) << hydrogen->GetName() << "\t";
   cout << std::setw(15) << matHydrogen->FermiPotential()/Units::neV << "\t";
   cout << std::setw(15) << matHydrogen->LossFactor() << endl;
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
   Element* elemBeryllium = new Element("Beryllium", Beryllium::z, Beryllium::a, Beryllium::scatLength, Beryllium::cohCrossSec, Beryllium::incohCrossSec, Beryllium::absCrossSec);
   Material* matBeryllium = new Material("Beryllium", elemBeryllium, Densities::beryllium);
   TGeoMedium* beryllium = new TGeoMedium("Beryllium", 1, matBeryllium);
   // Set Beryllium Loss factor to the experimentally determined value
   matBeryllium->LossFactor(LossFactor::beryllium);
   cout << std::setw(15) << beryllium->GetName() << "\t";
   cout << std::setw(15) << matBeryllium->FermiPotential()/Units::neV << "\t";
   cout << std::setw(15) << matBeryllium->LossFactor() << endl;
   
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
   Element* elemCopper = new Element("Copper", Copper::z, Copper::a, Copper::scatLength, Copper::cohCrossSec, Copper::incohCrossSec, Copper::absCrossSec);
   Material* matCopper = new Material("Copper", elemCopper, Densities::copper);
   TGeoMedium* copper = new TGeoMedium("Copper", 1, matCopper);
   cout << std::setw(15) << copper->GetName() << "\t";
   cout << std::setw(15) << matCopper->FermiPotential()/Units::neV << "\t";
   cout << std::setw(15) << matCopper->LossFactor() << endl;
   
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
   Element* elemAluminium = new Element("Aluminium", Aluminium::z, Aluminium::a, Aluminium::scatLength, Aluminium::cohCrossSec, Aluminium::incohCrossSec, Aluminium::absCrossSec);
   Material* matAluminium = new Material("Aluminium", elemAluminium, Densities::aluminium);
   TGeoMedium* aluminium = new TGeoMedium("Aluminium", 1, matAluminium);
   cout << std::setw(15) << aluminium->GetName() << "\t";
   cout << std::setw(15) << matAluminium->FermiPotential()/Units::neV << "\t";
   cout << std::setw(15) << matAluminium->LossFactor() << endl;
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
   // - PTFE
   Element* elemPTFE = new Element("PTFE", 0., 0., 0., 0., 0., 0.);
   Material* matPTFE = new Material("PTFE", elemPTFE, Densities::ptfe);
   TGeoMedium* ptfe = new TGeoMedium("PTFE", 1, matPTFE);
   // Set Beryllium Loss factor to the experimentally determined value
   matPTFE->FermiPotential(FermiPotential::ptfe);
   matPTFE->LossFactor(LossFactor::ptfe);
   cout << std::setw(15) << ptfe->GetName() << "\t";
   cout << std::setw(15) << matPTFE->FermiPotential()/Units::neV << "\t";
   cout << std::setw(15) << matPTFE->LossFactor() << endl;
   /*
   cout << "---------------------------------" << endl;
   cout << "Density (kg/m^3): " <<  << endl;
   cout << "Coherent Cross Sec: " << ->CoherentCrossSec() << endl;
   cout << "InCoh Cross Sec: " << ->InCoherentCrossSec() << endl;
   cout << "Scat Length: " << ->ScatLength() << endl;
   cout << "Abs Cross Sec: " << ->AbsorbtionCrossSec() << endl;
   cout << "Loss Cross Sec: " << ->LossCrossSec() << endl;
   cout << "V_f: " << ->FermiPotential() << endl;
   cout << "W: " << ->WPotential() << endl;
   cout << "LossFactor: " << ->LossFactor() << endl;
   cout << "---------------------------------" << endl;
   */
   // - Quartz
   Element* elemQuartz = new Element("Quartz", 0., 0., 0., 0., 0., 0.);
   Material* matQuartz = new Material("Quartz", elemQuartz, Densities::quartz);
   TGeoMedium* quartz = new TGeoMedium("Quartz", 1, matQuartz);
   // Set Beryllium Loss factor to the experimentally determined value
   matQuartz->FermiPotential(FermiPotential::quartz);
   matQuartz->LossFactor(LossFactor::quartz);
   cout << std::setw(15) << quartz->GetName() << "\t";
   cout << std::setw(15) << matQuartz->FermiPotential()/Units::neV << "\t";
   cout << std::setw(15) << matQuartz->LossFactor() << endl;
   /*
   cout << "---------------------------------" << endl;
   cout << "Density (kg/m^3): " <<  << endl;
   cout << "Coherent Cross Sec: " << ->CoherentCrossSec() << endl;
   cout << "InCoh Cross Sec: " << ->InCoherentCrossSec() << endl;
   cout << "Scat Length: " << ->ScatLength() << endl;
   cout << "Abs Cross Sec: " << ->AbsorbtionCrossSec() << endl;
   cout << "Loss Cross Sec: " << ->LossCrossSec() << endl;
   cout << "V_f: " << ->FermiPotential() << endl;
   cout << "W: " << ->WPotential() << endl;
   cout << "LossFactor: " << ->LossFactor() << endl;
   cout << "---------------------------------" << endl;
   */
   //________________________________________
   // TRACKING MATERIALS
   // - HeliumII
   Element* elemHeliumII = new Element("HeliumII", HeliumII::z, HeliumII::a, HeliumII::scatLength, HeliumII::cohCrossSec, HeliumII::incohCrossSec, HeliumII::absCrossSec);
   Material* matHeliumII = new Material("HeliumII", elemHeliumII, Densities::heliumII);
   TGeoMedium* heliumII = new TGeoMedium("HeliumII", 1, matHeliumII);
   cout << std::setw(15) << heliumII->GetName() << "\t";
   cout << std::setw(15) << matHeliumII->FermiPotential()/Units::neV << "\t";
   cout << std::setw(15) << matHeliumII->LossFactor() << endl;
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
   Element* elemVacuum = new Element("Vacuum", Vacuum::z, Vacuum::a, Vacuum::scatLength, Vacuum::cohCrossSec, Vacuum::incohCrossSec, Vacuum::absCrossSec);
   Material* matVacuum = new Material("Vacuum", elemVacuum, Densities::vacuum);
   TGeoMedium* vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
   cout << std::setw(15) << vacuum->GetName() << "\t";
   cout << std::setw(15) << matVacuum->FermiPotential()/Units::neV << "\t";
   cout << std::setw(15) << matVacuum->LossFactor() << endl;
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
   Element* elemLithium6 = new Element("Lithium6", Lithium6::z, Lithium6::a, Lithium6::scatLength, Lithium6::cohCrossSec, Lithium6::incohCrossSec, Lithium6::absCrossSec);
   Material* matLithium6 = new Material("Lithium6", elemLithium6, Densities::lithium6);
   TGeoMedium* lithium6 = new TGeoMedium("Lithium6", 1, matLithium6);
   cout << std::setw(15) << lithium6->GetName() << "\t";
   cout << std::setw(15) << matLithium6->FermiPotential()/Units::neV << "\t";
   cout << std::setw(15) << matLithium6->LossFactor() << endl;
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