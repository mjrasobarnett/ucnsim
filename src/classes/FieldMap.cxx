// FieldMap
// Author: Matthew Raso-Barnett  13/12/2010
#include <list>
#include <cmath>
#include <iostream>

#include "FieldMap.h"
#include "FileParser.h"
#include "VertexStack.h"
#include "Particle.h"
#include "Run.h"

//#define VERBOSE

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
const TVector3 MagFieldMap::GetField(const Point& point) const
{
   // -- Perform interpolation to get current field
   // If point requested was recently measured, return its value. Saves recomputing the value
   static FieldVertex cachedReading(0.,0.,0.,0.,0.,0.,0.);
   if (cachedReading.GetPoint() == point) {
      cout << "Using Cache! Point: " << point.X() << "\t" << point.Y() << "\t" << point.Z() << endl;
      return cachedReading.GetField();
   }
   // Number of interpolation points is currently an arbitrary number. Need to define this
   // at runtime perhaps through another config variable.
   Int_t numInterpolatePoints = 6;
   const TVector3 field = this->Interpolate(point.GetPosition(), numInterpolatePoints);
   // Update cached reading before return vale
   cachedReading.SetField(field);
   return field;
}

//______________________________________________________________________________
Bool_t MagFieldMap::BuildMap(const std::string& filename)
{
   // -- Take input file and initialise a KDTree object data structure to hold the field points.
   cout << "Building MagFieldMap from file: " << filename << endl;
   FileParser parser;
   // Get flat list of Field vertices from provided file using the parser
   vector<FieldVertex*> vertices;
   if (parser.ExtractFieldVertices(filename, vertices) == false) {
      Error("BuildMap","Failed to extract field vertices from: %s", filename.c_str());
      return false;
   }
   // For each vertex, convert coordinates to the global coordinate system
   vector<const FieldVertex*> global_vertices;
   vector<FieldVertex*>::const_iterator iter;
   for (iter = vertices.begin(); iter != vertices.end(); iter++) {
      FieldVertex global = this->ConvertToGlobalFrame((**iter));
      global_vertices.push_back(new FieldVertex(global));
   }
   // Build Tree structure of FieldVertices
   fTree = new KDTree(global_vertices);
   // Delete original local points
   for (iter = vertices.begin(); iter != vertices.end(); iter++) {if (*iter) delete *iter;}
   cout << "Successfully created MagFieldMap from " << global_vertices.size() << " points." << endl;
   return true;
}

//______________________________________________________________________________
TVector3 MagFieldMap::Interpolate(const TVector3& position, const Int_t numInterpolatePoints) const
{
   // -- For given position, find the n-nearest-neighbouring vertices (n being numInterpolatePoints)
   // -- and perform IDW interpolation using Modified Shepard's Method,
   // -- http://en.wikipedia.org/wiki/Inverse_distance_weighting
   FieldVertex vertex(position.X(), position.Y(), position.Z(), 0.,0.,0.,0.);
   const VertexStack* neighbours = fTree->NearestNeighbours(vertex, numInterpolatePoints);
   list<StackElement>::const_iterator stackIter;
   double radius = (neighbours->back()).second;
   double sumWeights = 0.;
   TVector3 avgField;
   for (stackIter = neighbours->begin(); stackIter != neighbours->end(); stackIter++) {
      double distance = stackIter->second;
      const FieldVertex* vertex = stackIter->first;
      double weight = pow((radius - distance)/(radius*distance),2.0);
      #ifdef VERBOSE
         cout << "Vertex: " << vertex->ToString() << "\t";
         cout << "Distance: " << distance << "\t";
         cout << "Weight: " << weight << endl;
      #endif
      sumWeights += weight;
      avgField += weight*(vertex->GetField());
   }
   avgField = avgField*(1.0/sumWeights);
   // Delete nearest neighbours now that we are finished with it
   delete neighbours;
   return avgField;
}

