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
{
// Default constructor.
   Info("FieldMap", "Default Constructor");
}

//_____________________________________________________________________________
FieldMap::FieldMap(const FieldMap& /*other*/)
{
// Copy constructor.
} 

//_____________________________________________________________________________
FieldMap &FieldMap::operator=(const FieldMap& other)
{
// Assignment.
   if(this != &other) {
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
Bool_t FieldMap::ReadFile(const std::string& /*filename*/)
{
   // Take input file and import field map data.
   
   
   return true;
}


//______________________________________________________________________________
// MagFieldMap - 
//
//______________________________________________________________________________

ClassImp(MagFieldMap)

//_____________________________________________________________________________
MagFieldMap::MagFieldMap()
            :FieldMap()
{
// Default constructor.
   Info("MagFieldMap", "Default Constructor");
}

//_____________________________________________________________________________
MagFieldMap::MagFieldMap(const MagFieldMap& other)
            :FieldMap(other), 
             MagField(other)
{
// Copy constructor.
} 

//_____________________________________________________________________________
MagFieldMap &MagFieldMap::operator=(const MagFieldMap& other)
{
// Assignment.
   if(this != &other) {
      FieldMap::operator=(other);
      MagField::operator=(other);
   }
   return *this;
}

//______________________________________________________________________________
MagFieldMap::~MagFieldMap()
{
// Destructor.
   Info("MagFieldMap", "Destructor");
}

//______________________________________________________________________________
Bool_t MagFieldMap::Contains(const TVector3& /*point*/) const
{
   return true;
}

//______________________________________________________________________________
Bool_t MagFieldMap::Interact(Particle& /*particle*/, const Double_t /*stepTime*/) const
{
   return true;
}

