// FieldMap
// Author: Matthew Raso-Barnett  13/12/2010

#include "FieldMap.h"
#include "FileParser.h"
#include "FieldVertex.h"
#include "Particle.h"

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

//______________________________________________________________________________
FieldMap::~FieldMap()
{
// Destructor.
   Info("FieldMap", "Destructor");
}

//______________________________________________________________________________
// MagFieldMap - 
//
//______________________________________________________________________________

ClassImp(MagFieldMap)

//_____________________________________________________________________________
MagFieldMap::MagFieldMap()
            :FieldMap(),
             fTree(NULL)
{
// Default constructor.
   Info("MagFieldMap", "Default Constructor");
}

//_____________________________________________________________________________
MagFieldMap::MagFieldMap(const string& name, const TGeoShape* shape, const TGeoMatrix* matrix)
            :FieldMap(),
             MagField(name, shape, matrix), 
             fTree(NULL)
{
// Default constructor.
   Info("MagFieldMap", "Constructor");
}


//_____________________________________________________________________________
MagFieldMap::MagFieldMap(const MagFieldMap& other)
            :FieldMap(other), 
             MagField(other),
             fTree(NULL)
{
// Copy constructor.
   Info("MagFieldMap", "Copy Constructor");
} 

//______________________________________________________________________________
MagFieldMap::~MagFieldMap()
{
// Destructor.
   Info("MagFieldMap", "Destructor");
   if (fTree) delete fTree;
}

//______________________________________________________________________________
Bool_t MagFieldMap::Interact(Particle& /*particle*/, const Double_t /*stepTime*/) const
{
   return true;
}

//______________________________________________________________________________
Bool_t MagFieldMap::BuildMap(const std::string& filename)
{
   // -- Take input file and initialise a KDTree object data structure to hold the field points.
   FileParser parser;
   // Get flat list of Field vertices from provided file using the parser
   vector<const FieldVertex*> vertices;
   if (parser.ExtractFieldVertices(filename, vertices) == false) {
      Error("BuildMap","Failed to extract field vertices from: %s", filename.c_str());
      return false;
   }
   // Build Tree structure of FieldVertices
   fTree = new KDTree(vertices);
   return true;
}
