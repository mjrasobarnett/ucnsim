// Configuration File Reader

/// ReadConfig
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>

#include "TUCNConfigFile.h"

using namespace std;

ClassImp(TUCNConfigFile)

//_____________________________________________________________________________
TUCNConfigFile::TUCNConfigFile()
{
	Info("ConfigFile", "Dummy Constructor");
}

//_____________________________________________________________________________
TUCNConfigFile::TUCNConfigFile(string filename)
{
  	Info("ConfigFile", "Constructor");
	/// Try opening the file
  ifstream cfg(filename.c_str());
  if(cfg.is_open())
  {
    // All is well, read the file in!
    ReadFile(cfg);
    
  } else {
    // Print an error message if we were trying to open a file
    if (filename != "") {
      Fatal("ConfigFile", "Could not read configuration file");
      throw runtime_error("Could not read configuration file.");
    }
  }
  
  /// Otherwise, we are blank! Nothing more needs to be done....
}

//_____________________________________________________________________________
TUCNConfigFile::TUCNConfigFile(TUCNConfigFile& other)
					:TObject(other),
					 fStore(other.fStore)
{
  	Info("ConfigFile", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNConfigFile& TUCNConfigFile::operator=(const TUCNConfigFile& other)
{
  	Info("ConfigFile", "Assignment");
	if (this!=&other) {
		TObject::operator=(other);
		fStore = other.fStore;
	}
	return *this;
}

//_____________________________________________________________________________
TUCNConfigFile::~TUCNConfigFile()
{
  	Info("ConfigFile", "Destructor");
}

//_____________________________________________________________________________
string TUCNConfigFile::Decomment(const string &source)
{
  // Strips comments from a line
  
  // Find any instances of '#', and then remove them
  int comment = source.find_first_of("#%");
  if (comment == -1) return source;
  return source.substr(0, comment);
}

//_____________________________________________________________________________
string TUCNConfigFile::Trim(const string &source)
{
  // Find the first non-whitespace
  int first = source.find_first_not_of("\n\r\t ");
  int last = source.find_last_not_of("\n\r\t ");
  if (last == -1) return "";
  return source.substr(first, last - first + 1);
}

//_____________________________________________________________________________
void TUCNConfigFile::ReadFile(std::ifstream &file)
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
    if (IsSectionChange(line))
    {
      section = Section(line);
      continue;
    }
    
    // Must be a key pair, or invalid
    if (!ReadKeyPair(section, line))
    {
      Warning("ReadFile", "Invalid line %i: \" %s \" ", lineNo, line.c_str()); 
    }
    
  } // Go to next line
}

//_____________________________________________________________________________
bool TUCNConfigFile::IsSectionChange(const string &line)
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
		  Warning("IsSectionChange","Invalid Section Change: Section cannot be blank");
      } else {
        return true;
      }
    }
  }
  
  return false;
}

//_____________________________________________________________________________
string TUCNConfigFile::Section(const string &line)
{
  // Extracts the section from a line
  string newsec = line.substr(1, line.size()-2);
  
  return newsec;
}

//_____________________________________________________________________________
bool TUCNConfigFile::ReadKeyPair(const string &section, const string &line)
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
string TUCNConfigFile::GetString(string key, string section, string defaultval)
{
  string value = fStore[section][key];

  if (value.size() == 0)
    return defaultval;
  else
    return value;    
}

//_____________________________________________________________________________
int TUCNConfigFile::GetInt(string key, string section, int defaultval)
{
  string value = fStore[section][key];
  if (value.size() == 0) return defaultval;
  
  // Attempt to read an integer out of this stream
  istringstream iss (value);
  int ival = 0;
  iss >> ival;
  
  if (!iss.fail())
  {
    return ival;
  } else {
    Warning("GetInteger", "Failed to read [ %s ]. %s = %s as integer.", section.c_str(), key.c_str(), value.c_str());
    return defaultval;
  }
}

//_____________________________________________________________________________
double TUCNConfigFile::GetFloat(string key, string section, double defaultval)
{
  string value = fStore[section][key];
  if (value.size() == 0) return defaultval;
  
  // Attempt to read an integer out of this stream
  istringstream iss (value);
  double ival = 0.0;
  iss >> ival;
  
  if (!iss.fail())
  {
    return ival;
  } else {
    Warning("GetFloat", "Failed to read [ %s ]. %s = %s as float.", section.c_str(), key.c_str(), value.c_str());
    return defaultval;
  }
}

//_____________________________________________________________________________
bool TUCNConfigFile::GetBool(string key, string section, bool defaultval)
{
  string value = fStore[section][key];
  if (value.size() == 0) return defaultval;
  
  // Now try to work out what it is
  if (value == "True" || value == "true" || value == "TRUE") return true;
  if (value == "False" || value == "false" || value == "FALSE") return false;
  
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
  Warning("GetBool", "Failed to read [ %s ]. %s = %s as bool.", section.c_str(), key.c_str(), value.c_str());
  return defaultval;
}
