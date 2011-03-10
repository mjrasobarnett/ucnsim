#include <math.h>

#include "Algorithms.h"

#include "TDirectory.h"
#include "DataFileHierarchy.h"

using namespace Algorithms;

//_____________________________________________________________________________
bool Precision::IsEqual(double left, double right, double precision) {
   return fabs(left - right) < precision ? true : false;;
}
//_____________________________________________________________________________
bool Precision::IsNotEqual(double left, double right, double precision) {
   return fabs(left - right) < precision ? false : true;
}
//_____________________________________________________________________________
bool Precision::IsGreaterOrEqual(double left, double right, double precision) {
   if (fabs(left - right) < precision) {
      // Check equality
      return true;
   } else if (left > right) {
      // Check if greater
      return true;
   } else {
      return false;
   }
}
//_____________________________________________________________________________
bool Precision::IsLessOrEqual(double left, double right, double precision) {
   if (fabs(left - right) < precision) {
      // Check equality
      return true;
   } else if (left < right) {
      // Check if less than
      return true;
   } else {
      return false;
   }
}


//_____________________________________________________________________________
bool DataFile::ValidateRootFile(const std::string filename)
{
   // -- Check that the filename supplied has a .root extension
   size_t found = filename.find_last_of(".");
   if (found == std::string::npos) return false;
   if (filename.substr(found) == ".root") return true;
   return false;
}
//_____________________________________________________________________________
bool DataFile::ValidateStateNames(const std::vector<std::string>& statenames)
{
   // -- Check that each statename in list is a valid state as defined
   // -- in DataFileHierarchy lvl 3 and is unique
   std::vector<std::string>::const_iterator iter;
   for (iter = statenames.begin(); iter != statenames.end(); iter++) {
      // Check state-name
      if (*iter != Folders::initial &&
          *iter != Folders::propagating &&
          *iter != Folders::absorbed &&
          *iter != Folders::lost &&
          *iter != Folders::decayed &&
          *iter != Folders::detected &&
          *iter != Folders::anomalous) {
         std::cerr << "Argument, " << *iter << " is not a valid statename" << std::endl;
         return false;
      }
      // Check for duplicates
      std::vector<std::string>::const_iterator second_iter;
      for (second_iter = iter+1; second_iter != statenames.end(); second_iter++) {
         if (*second_iter == *iter) {
            std::cerr << "Duplicate statenames given: " << *iter << std::endl;
            return false;
         }
      }
   }
   return true;
}
//_____________________________________________________________________________
bool DataFile::ValidateStateNames(const std::string statename)
{
   // -- Check that each statename in list is a valid state as defined
   // -- in DataFileHierarchy lvl 3 and is unique
   // Check state-name
   if (statename != Folders::initial &&
         statename != Folders::propagating &&
         statename != Folders::absorbed &&
         statename != Folders::lost &&
         statename != Folders::decayed &&
         statename != Folders::detected &&
         statename != Folders::anomalous) {
      std::cerr << "Argument, " << statename << " is not a valid statename" << std::endl;
      return false;
   }
   return true;
}
//_____________________________________________________________________________
void DataFile::CountParticles(TDirectory * const particleDir)
{
   // -- Given the particle state directory, count the number of particles
   // -- in each state subfolder
   if (particleDir->cd(Folders::initial.c_str()) == false) return;
   Int_t initial = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::propagating.c_str()) == false) return;
   Int_t propagating = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::absorbed.c_str()) == false) return;
   Int_t absorbed = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::detected.c_str()) == false) return;
   Int_t detected = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::decayed.c_str()) == false) return;
   Int_t decayed = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::lost.c_str()) == false) return;
   Int_t lost = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::anomalous.c_str()) == false) return;
   Int_t anomalous = gDirectory->GetNkeys();
   std::cout << "Initial Particles: " << initial << std::endl;
   std::cout << "Final Total: ";
   std::cout << propagating+detected+absorbed+decayed+lost+anomalous << std::endl;
   std::cout << "Propagating: " << propagating << std::endl;
   std::cout << "Detected: " << detected << std::endl;
   std::cout << "Absorbed: " << absorbed << std::endl;
   std::cout << "Decayed: " << decayed << std::endl;
   std::cout << "Lost: " << lost << std::endl;
   std::cout << "Anomalous: " << anomalous << std::endl;
   return;
}