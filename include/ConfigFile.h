// Written by Nick Devenish Nov 2009.

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include "TObject.h"
#include <map>

class ConfigFile {
public:
   // Constructors
   ConfigFile(std::string filename);
   ConfigFile(ConfigFile& other);
   ConfigFile& operator=(const ConfigFile&);
   
   // Destructor
   virtual ~ConfigFile();
   
   std::string GetString(std::string key, std::string section="", std::string defaultval="") const;
   int     GetInt(std::string key, std::string section = "", int defaultval = 0) const;
   double  GetFloat(std::string key, std::string section = "", double defaultval = 0.0) const;
   bool    GetBool(std::string key, std::string section = "", bool defaultval = false) const;
   std::string ExpandFilePath(std::string path) const;
   std::map<std::string, std::string> GetSection(std::string section) const;
   
private:
   /// The storage for the group/key-value from the file
   std::map<std::string, std::map<std::string, std::string> > fStore;
   
   // String utilities
   /// Strips the comments from a line
   std::string Decomment(const std::string& source);
   /// Trims the whitespace off of the beginning and end of a string
   std::string Trim(const std::string& source);
   
   /// Controls the file reading
   void ReadFile(std::ifstream &cfgfile);
   
   /// Tests for a section change in a line
   bool IsSectionChange(const std::string &line);
   /// Reads the section string out of a line
   std::string Section(const std::string &line);
   
   /// Reads a key-pair out of a line
   bool ReadKeyPair(const std::string &section, const std::string &line);
   
   /// Expand shell variable
   std::string ExpandShellVar(const std::string var) const;
   
};

#endif /* CONFIGFILE_H */

