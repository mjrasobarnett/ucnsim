// UniformElecField
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>

#include "TMath.h"
#include "TGeoMatrix.h"
#include "UniformElecField.h"

#include "Algorithms.h"

using namespace std;
using namespace Algorithms;

ClassImp(UniformElecField)

//_____________________________________________________________________________
UniformElecField::UniformElecField()
                 :ElecField(),
                  fField()
{
// Default constructor.
   Info("UniformElecField", "Default Constructor");
} 

//_____________________________________________________________________________
UniformElecField::UniformElecField(const string& name, const TVector3& field, const TGeoShape* shape, const TGeoMatrix* matrix)
                 :ElecField(name, shape, matrix),
                  fField()
{
// Default constructor.
   Info("UniformElecField", "Constructor");
   // Convert field vector supplied to global coordinates
   Double_t global[3], local[3] = {field[0],field[1],field[2]};
   matrix->LocalToMaster(local, global);
   // Zero out any rounding errors
   for (int i=0; i<3; i++) {global[i] = Precision::IsEqual(global[i], 0.0) ? 0.0 : global[i];}
   fField.SetXYZ(global[0],global[1],global[2]);
   cout << "Built Uniform Electric Field (kV) - ";
   cout << "Ex: " << fField[0] << "\t Ey: " << fField[1];
   cout << "\t Ez: " << fField[2] << endl;
}

//_____________________________________________________________________________
UniformElecField::UniformElecField(const UniformElecField& other)
                 :ElecField(other),
                  fField(other.fField)
{
   // Copy constructor.
} 

//_____________________________________________________________________________
UniformElecField::~UniformElecField()
{
   // Destructor.
   Info("UniformElecField", "Destructor");
}

//______________________________________________________________________________
const TVector3 UniformElecField::GetField(const Point& /*point*/) const
{
   // No position dependence for a Uniform field so return field vector
   return fField;
}
