// FieldMap
// Author: Matthew Raso-Barnett  13/12/2010

#include "FieldMap.h"

using namespace std;

//______________________________________________________________________________
// FieldMap - 
//
//______________________________________________________________________________

ClassImp(FieldMap)

//_____________________________________________________________________________
FieldMap::FieldMap()
         :TNamed("FieldMap","Field Map")
{
// Default constructor.
   Info("FieldMap", "Default Constructor");
}

//_____________________________________________________________________________
FieldMap::FieldMap(const FieldMap& other)
         :TNamed(other)
{
// Copy constructor.
} 

//_____________________________________________________________________________
FieldMap &FieldMap::operator=(const FieldMap& other)
{
// Assignment.
   if(this != &other) {
      TNamed::operator=(other);
   }
   return *this;
}

//______________________________________________________________________________
FieldMap::~FieldMap()
{
// Destructor.
   Info("FieldMap", "Destructor");
}

//______________________________________________________________________________
bool FieldMap::ReadFile(const std::string& filename)
{
   // Take input file and import field map data.
   
   
   return true;
}
