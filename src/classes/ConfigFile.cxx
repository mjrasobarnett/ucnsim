// Configuration File Reader

/// ReadConfig
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>
#include <stdlib.h>

#include "ConfigFile.h"

//#define VERBOSE
//#define PRINT_CONSTRUCTORS

using namespace std;

//_____________________________________________________________________________
ConfigFile::ConfigFile(string filename)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "ConfigFile::Constructor" << endl;
   #endif
   /// Check that we have a .cfg extension
   size_t found = filename.find_last_of(".");
   if (found == std::string::npos) throw runtime_error("Could not recognise file extension");
   if (filename.substr(found) != ".cfg") throw runtime_error("No .cfg config file specified");
   /// Try opening the file
   ifstream cfg(filename.c_str());
   if(cfg.is_open()) {
    // All is well, read the file in!
    ReadFile(cfg); 
   } else {
    // Print an error message if we were trying to open a file
    if (filename != "") {throw runtime_error("Could not read configuration file.");}
   }
   /// Otherwise, we are blank! Nothing more needs to be done....
}

//_____________________________________________________________________________
ConfigFile::ConfigFile(ConfigFile& other)
           :fStore(other.fStore)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "ConfigFile::Copy Constructor" << endl;
   #endif
}

//_____________________________________________________________________________
ConfigFile& ConfigFile::operator=(const ConfigFile& other)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "ConfigFile::Assignment" << endl;
   #endif
   if (this!=&other) {
      fStore = other.fStore;
   }
   return *this;
}

//_____________________________________________________________________________
ConfigFile::~ConfigFile()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "ConfigFile::Destructor" << endl;
   #endif
}

//_____________________________________________________________________________
string ConfigFile::Decomment(const string &source)
{
  // Strips comments from a line
  // Find any instances of '#', and then remove them
  int comment = source.find_first_of("#%");
  if (comment == -1) return source;
  return source.substr(0, comment);
}

//_____________________________________________________________________________
string ConfigFile::Trim(const string &source)
{
  // Find the first non-whitespace
  int first = source.find_first_not_of("\n\r\t ");
  int last = source.find_last_not_of("\n\r\t ");
  if (last == -1) return "";
  return source.substr(first, last - first + 1);
}

//_____________________________________________________________________________
void ConfigFile::ReadFile(std::ifstream &file)
{
  int lineNo = 0;
  string line, section = "";
  // Loop over all lines in the file
  while(std::getline(file, line))
  {
    ++lineNo;
    
    // Clean up the line
    line = Decomment(line);
    line = Trim(line);
    
    // Ignore blank lines
    if (line.size() == 0) continue;
    
    // Several paths here: Section header, Key-Value pair or invalid.
    // First test for section header
    if (IsSectionChange(line)) {
      section = Section(line);
      continue;
    }
    
    // Must be a key pair, or invalid
    if (!ReadKeyPair(section, line)) {
       cout << "ReadFile::Invalid line " << lineNo << ": \" " << line << " \"" << endl; 
    }
  } // Go to next line
}

//_____________________________________________________________________________
bool ConfigFile::IsSectionChange(const string &line)
{
  // Work out if this is a change of section by checking the first char
  if (line[0] == '[') {
    // Check the last char too
    if (line[line.size()-1] != ']')
    {
      // cout << "Warning Parsing file " << lineNo << ": Invalid section change!" << endl;
      cout << "Invalid section change: " << line << endl;
    } else {
      // Both start and ends are valid... is a section change line
      // Make sure it is valid
      string newsec = line.substr(1, line.size()-2);
      if (newsec.size() == 0) {
         cout << "IsSectionChange::Invalid Section Change: Section cannot be blank" << endl;
      } else {
        return true;
      }
    }
  }
  
  return false;
}

//_____________________________________________________________________________
string ConfigFile::Section(const string &line)
{
  // Extracts the section from a line
  string newsec = line.substr(1, line.size()-2);
  return newsec;
}

//_____________________________________________________________________________
bool ConfigFile::ReadKeyPair(const string &section, const string &line)
{
  // Check to see if this is a key pair
  int split = line.find_first_of("=:");
  
  // what if we don't find a split?
  if (split == -1)
  {
    // MSG("ConfigFile",Msg::kWarning) << "Invalid key-value pair: \"" << line << "\"" << endl;
    return false;
  }
  
  // Read out the values
  string key = Trim(line.substr(0, split));
  string keyval = Trim(line.substr(split+1, line.size()-split+2));
  fStore[section][key] = keyval;
  return true;
}

//_____________________________________________________________________________
string ConfigFile::GetString(string key, string section, string defaultval) const
{
  string value = fStore.find(section)->second.find(key)->second;

  if (value.size() == 0)
    return defaultval;
  else
    return value;    
}

//_____________________________________________________________________________
int ConfigFile::GetInt(string key, string section, int defaultval) const
{
  string value = fStore.find(section)->second.find(key)->second;
  if (value.size() == 0) return defaultval;
  
  // Attempt to read an integer out of this stream
  istringstream iss (value);
  int ival = 0;
  iss >> ival;
  
  if (!iss.fail())
  {
    return ival;
  } else {
     cout << "GetInteger::Failed to read [ " << section << " ]. " << key;
     cout << " = " << value << " as integer." << endl;
    return defaultval;
  }
}

//_____________________________________________________________________________
double ConfigFile::GetFloat(string key, string section, double defaultval) const
{
  string value = fStore.find(section)->second.find(key)->second;
  if (value.size() == 0) return defaultval;
  
  // Attempt to read an integer out of this stream
  istringstream iss (value);
  double ival = 0.0;
  iss >> ival;
  
  if (!iss.fail())
  {
    return ival;
  } else {
    cout << "GetFloat:: Failed to read [ " << section << " ]. " << key;
    cout << " = " << value << " as float." << endl;
    return defaultval;
  }
}

//_____________________________________________________________________________
bool ConfigFile::GetBool(string key, string section, bool defaultval) const
{
   string value = fStore.find(section)->second.find(key)->second;
   if (value.size() == 0) return defaultval;

   // Now try to work out what it is
   if (value == "True" || value == "true" || value == "TRUE" || value == "ON" || value == "On" || value == "on" || value == "YES" || value == "Yes" || value == "yes") return true;
   if (value == "False" || value == "false" || value == "FALSE" || value == "OFF" || value == "Off" || value == "off" || value == "NO" || value == "No" || value == "no" ) return false;

   // Try to convert to an integer, and see if it is non-zero
   istringstream iss (value);
   int ival = 0;
   iss >> ival;

   // It worked - just return the value
   if (!iss.fail())
   {
    return ival;
   }

   // It failed..... give up here for now
   cout << "GetBool::Failed to read [ " << section << " ]. " << key;
   cout << " = " << value << " as bool." << endl;
   return defaultval;
}

//_____________________________________________________________________________
string ConfigFile::ExpandFilePath(const string path) const
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
      string var_expansion = ExpandShellVar(shell_var);
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
string ConfigFile::ExpandShellVar(const string var) const
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
map<string, string> ConfigFile::GetSection(string section) const
{
   // -- Return a map of the requested ConfigFile section
   map<string, map<string, string> >::const_iterator it = fStore.find(section);
   if (it == fStore.end()) {
      map<string, string> copy;
      return copy;
   }
   return it->second;
}

