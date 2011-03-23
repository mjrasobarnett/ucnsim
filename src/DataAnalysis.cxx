
#include "DataAnalysis.h"

#include "TFile.h"
#include "TDirectory.h"

#include "DataFileHierarchy.h"

using namespace Analysis;
using namespace std;

//_____________________________________________________________________________
bool DataFile::ValidateRootFile(const string filename)
{
   // -- Check that the filename supplied has a .root extension
   size_t found = filename.find_last_of(".");
   if (found == string::npos) return false;
   if (filename.substr(found) == ".root") return true;
   return false;
}
//_____________________________________________________________________________
bool DataFile::ValidateStateNames(const vector<string>& statenames)
{
   // -- Check that each statename in list is a valid state as defined
   // -- in DataFileHierarchy lvl 3 and is unique
   vector<string>::const_iterator iter;
   for (iter = statenames.begin(); iter != statenames.end(); iter++) {
      // Check state-name
      if (*iter != Folders::initial &&
          *iter != Folders::propagating &&
          *iter != Folders::absorbed &&
          *iter != Folders::lost &&
          *iter != Folders::decayed &&
          *iter != Folders::detected &&
          *iter != Folders::anomalous) {
         cerr << "Argument, " << *iter << " is not a valid statename" << endl;
         return false;
      }
      // Check for duplicates
      vector<string>::const_iterator second_iter;
      for (second_iter = iter+1; second_iter != statenames.end(); second_iter++) {
         if (*second_iter == *iter) {
            cerr << "Duplicate statenames given: " << *iter << endl;
            return false;
         }
      }
   }
   return true;
}
//_____________________________________________________________________________
bool DataFile::ValidateStateNames(const string statename)
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
      cerr << "Argument, " << statename << " is not a valid statename" << endl;
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
   cout << "Initial Particles: " << initial << endl;
   cout << "Final Total: ";
   cout << propagating+detected+absorbed+decayed+lost+anomalous << endl;
   cout << "Propagating: " << propagating << endl;
   cout << "Detected: " << detected << endl;
   cout << "Absorbed: " << absorbed << endl;
   cout << "Decayed: " << decayed << endl;
   cout << "Lost: " << lost << endl;
   cout << "Anomalous: " << anomalous << endl;
   return;
}

