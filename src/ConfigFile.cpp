// Configuration File Reader

/// ReadConfig
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>
#include <stdlib.h>

#include "ConfigFile.hpp"
#include "Algorithms.hpp"

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
      if (filename != "") {
         cout << "Could not open file: " << filename << endl;
         throw runtime_error("Could not read configuration file");
      }
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
string ConfigFile::GetString(string key, string sectionName, string defaultval) const
{
   // If it exists, find the section named 'sectionName' 
   map<string, map<string, string> >::const_iterator sectionIter = fStore.find(sectionName);
   if (sectionIter == fStore.end()) return defaultval;
   const map<string,string>& section = sectionIter->second;
   // If it exists, find the option named 'key'
   map<string, string>::const_iterator optionIter = section.find(key);
   if (optionIter == section.end()) return defaultval;
   // If the option existed, store its value (as a string)
   string value = optionIter->second;
   // If option is empty return defaultval
   if (value.size() == 0) return defaultval;
   return value;
}

//_____________________________________________________________________________
int ConfigFile::GetInt(string key, string section, int defaultval) const
{
   string value = this->GetString(key, section);
   if (value.size() == 0) return defaultval;
   int returnval;
   if (Algorithms::String::ConvertToInt(value, returnval) == false) {
      return defaultval;
   }
   return returnval;
}

//_____________________________________________________________________________
double ConfigFile::GetFloat(string key, string section, double defaultval) const
{
   string value = this->GetString(key, section);
   if (value.size() == 0) return defaultval;
   double returnval;
   if (Algorithms::String::ConvertToDouble(value, returnval) == false) {
      return defaultval;
   }
   return returnval;
}

//_____________________________________________________________________________
bool ConfigFile::GetBool(string key, string section, bool defaultval) const
{
   string value = this->GetString(key, section);
   if (value.size() == 0) return defaultval;
   bool returnval;
   if (Algorithms::String::ConvertToBool(value, returnval) == false) {
      return defaultval;
   }
   return returnval;
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

