#include <iostream>
#include <cassert>
#include <stdexcept>
#include "DataAnalysis.h"

#include "TRoot.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TClass.h"

#include "RunConfig.h"
#include "DataFileHierarchy.h"

using namespace Analysis;
using namespace std;

//_____________________________________________________________________________
TFile* DataFile::OpenRootFile(const std::string filename, const std::string option)
{
   // -- Simply open a Root file with the given name and option and return pointer to it.
   cout << "-------------------------------------------" << endl;
   cout << "Loading Data File: " << filename << endl;
   TFile *file = NULL;
   file = TFile::Open(filename.c_str(), option.c_str());
   if (!file || file->IsZombie()) {
      cerr << "Error: Cannot open file - " << filename << endl;
      return NULL;
   }
   file->cd();
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Loaded Data File: " << filename << endl;
   cout << "-------------------------------------------" << endl;
   return file;
}

//_____________________________________________________________________________
const RunConfig& DataFile::LoadRunConfig(TFile& file)
{
   // -- Attempt to navigate to the config file folder in supplied file and extract the runconfig
   // -- Throw an exception if this cannot be done.
   cout << "Attempting to load the RunConfig" << endl;
   // Navigate to Config Folder   
   if (file.cd(Folders::config.c_str()) == false) {
      cerr << "No Folder named: " << Folders::config << " in data file" << endl;
      throw runtime_error("Cannot find config folder");
   }
   TDirectory* const configDir = gDirectory;
   // -- Loop over all objects in folder and extract latest RunConfig
   RunConfig* ptr_config = NULL;
   TKey *configKey;
   TIter configIter(configDir->GetListOfKeys());
   while ((configKey = dynamic_cast<TKey*>(configIter.Next()))) {
      const char *classname = configKey->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("RunConfig")) {
         ptr_config = dynamic_cast<RunConfig*>(configKey->ReadObj());
         break;
      }
   }
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Loaded RunConfig" << endl;
   cout << "-------------------------------------------" << endl;
   return *ptr_config;
}

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

