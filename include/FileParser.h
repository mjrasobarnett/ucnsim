#ifndef __FILE_PARSER_H
#define __FILE_PARSER_H

#include <string>

using namespace std;

class FieldVertex;

class FileParser {
   private:
      
   public:
      FileParser();
      virtual ~FileParser();
      
      vector<const FieldVertex*> ExtractFieldVertices(std::string filename) const;
      
};

#endif