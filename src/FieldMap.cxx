// FieldMap
// Author: Matthew Raso-Barnett  13/12/2010

#include "FieldMap.h"
#include "FileParser.h"
#include "FieldVertex.h"
#include "Particle.h"
#include "Run.h"

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
Bool_t MagFieldMap::Interact(Particle& particle, const Run& run, const Double_t stepTime) const
{
   // -- Precess particle's spin for a time 'stepTime'. For a field map this means
   // -- splitting up the journey stepTime into smaller steps and performing interpolation
   // -- to find the approximate field along that step.
   Double_t currentTime = 0.;
   Double_t interval = 1.0*Units::ms;
   Int_t numInterpolatePoints = 6;
   // Check that our small step interval isn't larger than the stepTime
   if (interval > stepTime) {
      interval = stepTime;
   }
   // Store the particle's current position and momentum
   Double_t currentPos[3] = {particle.X(), particle.Y(), particle.Z()};
   Double_t currentVel[3] = {particle.Vx(), particle.Vy(), particle.Vz()};
   const GravField* gravField = run.GetFieldManager()->GetGravField();
   Double_t grav_acc[3] = {gravField->Gx(), gravField->Gy(), gravField->Gz()}; 
   // Loop over small steps
   while (currentTime < stepTime) {
      // Update current time. Check if we will reach the 'end' within the small step
      if (currentTime+interval > stepTime) {interval = stepTime - currentTime;} 
      currentTime += interval;
      // Update current coordinates
      for (int i = 0; i < 3; i++) {
         currentPos[i] += currentVel[i]*interval + 0.5*grav_acc[i]*interval*interval;
         currentVel[i] += grav_acc[i]*interval;
      }
      TVector3 position(currentPos[0],currentPos[1],currentPos[2]);
      // Perform interpolation to get current field
      TVector3 field = this->Interpolate(position, numInterpolatePoints);
      // Precess particle's spin about this field
      particle.PrecessSpin(field, interval);
   }
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

//______________________________________________________________________________
TVector3 MagFieldMap::Interpolate(const TVector3& position, const Int_t numInterpolatePoints) const
{
   // -- For given position, find the n-nearest-neighbouring vertices (n being numInterpolatePoints)
   // -- and perform IDW interpolation using Modified Shepard's Method,
   // -- http://en.wikipedia.org/wiki/Inverse_distance_weighting
   TVector3 field;
   return field;
}

