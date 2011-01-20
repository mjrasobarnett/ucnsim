#ifndef __FILE_PARSER_H
#define __FILE_PARSER_H

#include <vector>
#include <string>
#include <fstream>

class FieldVertex;

class FileParser {
   private:
      
      int   ConvertToInt(std::string value) const;
      double ConvertToDouble(std::string value) const;
      std::vector<int> ConvertVecToInt(const std::vector<std::string>& input) const;
      std::vector<double> ConvertVecToDouble(const std::vector<std::string>& input) const;
      
      std::vector<std::string> FactorString(const std::string input, const char delim) const;
      
   public:
      FileParser();
      virtual ~FileParser();
      
      bool ExtractFieldVertices(std::string filename, std::vector<const FieldVertex*>& vertices) const;
      
};

#endif