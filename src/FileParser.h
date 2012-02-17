#ifndef __FILE_PARSER_H
#define __FILE_PARSER_H

#include <vector>
#include <string>
#include <fstream>

class FieldVertex;

class FileParser {
   private:
      
   public:
      FileParser();
      virtual ~FileParser();
      
      bool ExtractFieldVertices(std::string filename, std::vector<FieldVertex*>& vertices) const;
      
};

#endif
