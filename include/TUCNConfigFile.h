// Written by Nick Devenish Nov 2009.

#ifndef TUCNCONFIGFILE_H
#define TUCNCONFIGFILE_H

#include "TObject.h"

class TUCNConfigFile : public TObject {
	public:
		TUCNConfigFile();
		TUCNConfigFile(std::string filename);

	  std::string  GetString(std::string key, std::string section = "", std::string defaultval = "");
	  int     GetInt(std::string key, std::string section = "", int defaultval = 0);
	  double  GetFloat(std::string key, std::string section = "", double defaultval = 0.0);
	  bool    GetBool(std::string key, std::string section = "", bool defaultval = false);


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

  ClassDef(TUCNConfigFile, 1) // 
};

#endif /* TUCNCONFIGFILE_H */

