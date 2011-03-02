#include "Algorithms.h"

#include "TDirectory.h"
#include "DataFileHierarchy.h"

using namespace Algorithms;

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