#include <math.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <sys/stat.h> // Required by Progress bar
#include <iomanip>

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
bool String::ConvertVectorToInt(const vector<string>& input, vector<int>& output)
{
   // -- Convert vector of strings to integers
   vector<string>::const_iterator input_Iter;
   for (input_Iter = input.begin(); input_Iter != input.end(); input_Iter++) {
      int value = 0;
      if (String::ConvertToInt(*input_Iter, value) == false) return false;
      output.push_back(value);
   }
   return true;
}

//_____________________________________________________________________________
bool String::ConvertVectorToDouble(const std::vector<std::string>& input, std::vector<double>& output)
{
   // -- Convert vector of strings to doubles
   vector<string>::const_iterator input_Iter;
   for (input_Iter = input.begin(); input_Iter != input.end(); input_Iter++) {
      double value = 0;
      if (String::ConvertToDouble(*input_Iter, value) == false) return false;
      output.push_back(value);
   }
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
   // Finally check that final character is a '/' command, as this path serves as
   // a prefix to be able to locate specific files in this folder
   if (strcmp(&fullpath.at(fullpath.length() - 1), "/") != 0) {fullpath.append("/");}
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

//_____________________________________________________________________________
void ProgressBar::PrintProgress(int entry, float nEntriesF, int mintime)
{
   // -- Written by Nicholas Devenish - 01/2010
   int nEntries = (int)nEntriesF;

   // Are we streaming to a file? If so, show the old style
   struct stat buf;
   fstat(fileno(stdout), &buf);
   // Check if we are a file, or a pipe (i.e. in case the user tees)
   const bool isFile = buf.st_mode & (S_IFREG | S_IFIFO) ;
   if (isFile) {
     float fract = ceil(nEntries/20.);  
     if (ceil(((float)entry)/fract)==((float)entry)/fract)
     {
         cerr 
           <<"Fraction of loop complete: "<<entry 
           <<"/"<<nEntries<<"  ("
           <<(int)(100.*entry/nEntries)<<"%)"<<endl;
     }
     return;
   }


   // How wide should we make the progress bar?
   const int width = 70;
   // How long is the string for entries?
   static int countlen = -1;
   // How long is our progress bar?
   static int barlen = -1;
   // The entry number of the next bar entry
   static int nextbar = -1;
   // When did we start?
   static time_t starttime = 0;
   // when do we next update?
   static time_t nextupdate = 0;
   // If we are processing the first entry, reset everything
   if (entry <= 1)
   {
     // Get the new length of the entry string
     countlen = (int)ceil(log10(nEntries)) + 1;
     nextbar = -1;
     starttime = time(NULL);

     barlen = width - 14 - countlen*2 - 1;

     // Don't update until we get to the minimum time
     nextupdate = starttime + mintime;
   }

   // Check here to see if we should update; otherwise, return
   // Check to see if the bar would update
   // or, alternatively, it is time to refresh.
   if ((time(NULL) < nextupdate) && (entry < nextbar || (nextbar == -1)))return;
   nextupdate = time(NULL) + 10;

   // Because this is used in several places, make it here
   float frac = (float)entry / (float)nEntries;

   // Prepare the progress bar string
   string bar;
   if (entry <= nEntries)
   {
     // Work out how many characters we are in
     int numeq = floor(frac*barlen);

     // Work out when the next bar will occur
     nextbar = (int)((float)(numeq+1)/(float)barlen*nEntries);
     //cerr << "Next bar at: " << nextbar << "        " << endl;
     bar = string(numeq,'=');
     bar += string(barlen - numeq, ' ');
   } else if (entry > nEntries) {
     // We have gone over. Oh no!
     bar = string(barlen, '+');
   } else if (entry < 0) {
     // Somehow, we are below zero. Handle it nonetheless
     bar = string(barlen, '-');
   }


   // Prepare the ETA
   float elapsed_time = (float)(time(NULL) - starttime);
   float time_left = -60;
   if (frac > 1e-6) {
     time_left = (elapsed_time / frac) - elapsed_time;
   }
   int mins, seconds;
   mins    = (int)floor(time_left / 60.0f);
   seconds = (int)floor(time_left - (float)(mins*60.0f));
   // ETA;
   std::ostringstream ETA;

   ETA << "ETA ";
   if ((mins < 0 || seconds < 0) || (mins == 0 && seconds == 0)) {
     ETA << "--:--";
   } else {
     ETA << std::setfill('0') << std::setw(2) << mins << ":" << std::setw(2) << seconds;
   }

   cerr << " Progress: [" << bar << "] "
        << std::setw(countlen) << entry << "/" << nEntries
        << " " << ETA.str()
        << '\r'
        << std::flush;
   // Move to the next line, if this is the final entry!
   if (entry == nEntries) {
     cerr << endl;
   }
}

