// FileParser.cxx
// Author: Matt Raso-Barnett

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "FileParser.h"

//#define PRINT_CONSTRUCTORS 
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
