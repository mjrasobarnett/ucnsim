#include <math.h>
#include <algorithm>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include "Algorithms.h"

#include "TDirectory.h"
#include "DataFileHierarchy.h"

using namespace Algorithms;
using namespace std;

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

//_____________________________________________________________________________
bool String::ConvertToInt(const string input, int& output)
{
   // Attempt to read an integer out of this string
   istringstream stream(input);
   int value = 0;
   stream >> value;
   if (stream.fail()) {
      cout << "ConvertToInt::Failed to read " << input;
      cout << " = " << value << " as integer." << endl;
      return false;
   }
   output = value;
   return true;
}

//_____________________________________________________________________________
bool String::ConvertToBool(const string input, bool& output)
{
   string l_input = boost::to_lower_copy(input);
   // Attempt to read a boolean out of this string
   if (l_input == "true" || l_input == "on" || l_input == "yes") {
      // 
      output = true;
      return true;
   } else if (l_input == "false" || l_input == "off" || l_input == "no" ) {
      output = false;
      return true;
   } else {
      cout << "ConvertToBool::Failed to read " << input << " as boolean." << endl;
      return false;
   }
}

//_____________________________________________________________________________
bool String::ConvertToDouble(const string input, double& output)
{
   // Attempt to read a float out of this string
   istringstream stream(input);
   double value = 0.;
   stream >> value;
   if (stream.fail()) {
      cout << "ConvertToDouble::Failed to read " << input;
      cout << " = " << value << " as double." << endl;
      return false;
   }
   output = value;
   return true;
}


//_____________________________________________________________________________
string FileSystem::ExpandFilePath(const string path)
{
   // -- Take string, search for a shell variable inside the path '*/$VAR/*' and expand
   // -- it out to its literal value. Returns empty string if expansion fails
   if (path.empty()) return path;
   string fullpath = path;
   // Locate a shell variable in string
   size_t start_pos = fullpath.find_first_of("$");
   while (start_pos != string::npos) {
      size_t end_pos = fullpath.find_first_of("/", start_pos);
      size_t length = end_pos - start_pos;
      // Extract shell variable
      string shell_var = fullpath.substr(start_pos, length);
      #ifdef VERBOSE
         cout << "Start: " << start_pos << "\t" << "End: " << end_pos << endl;
         cout << "Shellvar: " << shell_var << endl;
      #endif
      // Expand shell variable
      string var_expansion = FileSystem::ExpandShellVar(shell_var);
      if (var_expansion.empty()) {
         cout << "Failed to expand shell variable: " << shell_var << endl;
         // Return empty string
         fullpath.clear();
         return fullpath;
      }
      // Insert expansion back into path
      fullpath.replace(start_pos, length, var_expansion);
      // Check fullpath for any more shell variables to expand
      start_pos = fullpath.find_first_of("$");
   }
   #ifdef VERBOSE
      cout << "Full path: " << fullpath << endl;
   #endif
   return fullpath;
}

//_____________________________________________________________________________
string FileSystem::ExpandShellVar(const string var)
{
   // -- Take shell variable and expand it using the OS
   char* p_val;
   #ifdef VERBOSE
      cout << "ExpandShellVar: " << var << "\t";
   #endif
   // Remove $ from front of shell var
   string var_name = var;
   var_name.erase(0,1); 
   // Use cstdlib function to get environment variable
   p_val = getenv(var_name.c_str());
   // Check pointer
   if (p_val!=NULL) {
      string value(p_val);
      #ifdef VERBOSE
         cout << "Result: " << value << endl;
      #endif
      return value;
   }
   // Return empty string
   #ifdef VERBOSE
      cout << "Expansion Failed!" << endl;
   #endif
   string empty_str;
   return empty_str;
}
