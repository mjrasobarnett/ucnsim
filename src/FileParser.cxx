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
      vector<string> s_row = this->FactorString(line, '\t');
      vector<double> row = this->ConvertVecToDouble(s_row);
      // Check if row size corresponds to the expected 6 columns
      if (row.size() != 6) {continue;}
      // Create new FieldVertex and add to list
      vertices.push_back(new FieldVertex(row[0],row[1],row[2],0.0,row[3],row[4],row[5]));
   }
   return true;
}

//______________________________________________________________________________
vector<string> FileParser::FactorString(const string input, const char delim) const
{
   // -- Take input string and split it up into a vector of sub-strings using the supplied delimeter
   vector<string> result;
   size_t previousTabPos = 0, nextTabPos = 0;
   while (nextTabPos != string::npos) {
      // Find position of nex tab
      nextTabPos = input.find(delim, previousTabPos);
      // Extract string between found tab position and previous tab position
      string value = input.substr(previousTabPos, nextTabPos - previousTabPos);
      result.push_back(value);
      // Increment previous tab position to next tab position (plus 1 so that we search for the
      // next tab from right after this position)
      previousTabPos = nextTabPos+1;
   }
   return result;
}

//______________________________________________________________________________
int FileParser::ConvertToInt(string value) const
{
   // Attempt to read an integer out of this stream
   istringstream iss (value);
   int ival = 0;
   iss >> ival;
   if (!iss.fail()) {
      return ival;
   } else {
      cout << "ConvertToInt failed to convert, " << value << ", to an integer" << endl;
      return 0;
   } 
}

//______________________________________________________________________________
double FileParser::ConvertToDouble(string value) const
{
   // Attempt to read an double out of this stream
   istringstream iss (value);
   double fval = 0;
   iss >> fval;
   if (!iss.fail()) {
      return fval;
   } else {
      cout << "ConvertToDouble failed to convert, " << value << ", to a double" << endl;
      return 0;
   } 
}

//______________________________________________________________________________
vector<int> FileParser::ConvertVecToInt(const vector<string>& input) const
{
   // -- Convert vector of strings to integers
   vector<int> output;
   vector<string>::const_iterator input_Iter;
   for (input_Iter = input.begin(); input_Iter != input.end(); input_Iter++) {
      output.push_back(this->ConvertToInt(*input_Iter));
   }
   return output;
}

//______________________________________________________________________________
vector<double> FileParser::ConvertVecToDouble(const vector<string>& input) const
{
   // -- Convert vector of strings to integers
   vector<double> output;
   vector<string>::const_iterator input_Iter;
   for (input_Iter = input.begin(); input_Iter != input.end(); input_Iter++) {
      output.push_back(this->ConvertToDouble(*input_Iter));
   }
   return output;
}
