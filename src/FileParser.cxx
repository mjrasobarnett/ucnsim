// FileParser.cxx
// Author: Matt Raso-Barnett

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "FileParser.h"
#include "FieldVertex.h"
#include "Algorithms.h"

#define PRINT_CONSTRUCTORS 
//#define VERBOSE_MODE

using namespace std;

//______________________________________________________________________________
FileParser::FileParser()
{
   // -- Constructor
   #ifdef PRINT_CONSTRUCTORS
      cout << "FileParser" << "\t" << "Default Constructor" << endl;
   #endif
}

//______________________________________________________________________________
FileParser::~FileParser()
{
   // -- Destructor
   #ifdef PRINT_CONSTRUCTORS
      cout << "FileParser" << "\t" << "Destructor" << endl;
   #endif
}

//______________________________________________________________________________
bool FileParser::ExtractFieldVertices(std::string filename, std::vector<FieldVertex*>& vertices) const
{
   // -- Open and read in supplied file, extract and return list of field vertices
   ifstream in(filename.c_str());
   if (!in.is_open()) { 
      cout << "Error::ExtractFieldVertices - cannot open file" << endl;
      return false;
   }
   string line;
   // Determine the end of line delimeter of the file by trying '\n' or '\r'.
   // Since file should be structured with a single 'header' line, followed 
   // by columns of data
   char lineDelim;
   if (getline(in, line, '\n').eof() == true) {
      // Reached end of file. Reset stream and try again with '\r'
      in.clear();
      in.seekg(0, ios::beg);
      if (getline(in, line, '\r').eof() == true) {
         // Reached end of file again. Exit.
         cout << "Error::ExtractFieldVertices - file: " << filename;
         cout << " appears to contain no data" << endl;
         return false;
      } else {
         // Otherwise, note the line delimeter as '\r'
         lineDelim = '\r';
      }
   } else {
      // Otherwise, note the line delimeter as '\n'
      lineDelim = '\n';
   }
   // At this point we know the line delimeter. All columns should be tab delimeted
   // according to structure 'x' 'y' 'z' 'bx' 'by' 'bz'
   // Loop over each line and extract its data
   while (getline(in, line, lineDelim).eof() == false) {
      vector<string> s_row = Algorithms::String::FactorString(line, '\t');
      vector<double> row;
      Algorithms::String::ConvertVectorToDouble(s_row, row);
      // Check if row size corresponds to the expected 6 columns
      if (row.size() != 6) {continue;}
      // Create new FieldVertex and add to list
      vertices.push_back(new FieldVertex(row[0],row[1],row[2],0.0,row[3],row[4],row[5]));
   }
   return true;
}
