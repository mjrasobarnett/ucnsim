// MagField
// Author: Matthew Raso-Barnett  30/09/2009

#include "MagField.h"

//______________________________________________________________________________
// MagField - ABC for magnetic field.
//
//______________________________________________________________________________

ClassImp(MagField)

//_____________________________________________________________________________
MagField::MagField()
         :TNamed(),
          fFieldShape(NULL), 
          fFieldMatrix(NULL)
{
// Default constructor.
   Info("MagField", "Default Constructor");
}

//_____________________________________________________________________________
MagField::MagField(const std::string& name, const TGeoShape* shape, const TGeoMatrix* matrix)
         :TNamed(name, ""), 
          fFieldShape(shape), 
          fFieldMatrix(matrix)
          
{
// Default constructor.
   Info("MagField", "Constructor");
}

//_____________________________________________________________________________
MagField::MagField(const MagField& other)
             :TNamed(other),
              fFieldShape(other.fFieldShape),
              fFieldMatrix(other.fFieldMatrix)
{
// Copy constructor.
} 

//______________________________________________________________________________
MagField::~MagField()
{
// Destructor.
   Info("MagField", "Destructor");
   if(fFieldShape) delete fFieldShape;
   if(fFieldMatrix) delete fFieldMatrix;
}

//______________________________________________________________________________
Bool_t MagField::Contains(const TVector3& point) const
{
   // -- Check whether supplied point is contained by Field-shape
   // First convert point to local coordinates
   Double_t masterPoint[3] = {point[0], point[1], point[2]};
   Double_t localPoint[3] = {0.,0.,0.};
   fFieldMatrix->MasterToLocal(masterPoint, localPoint);
   // Check and return whether its contained
   return fFieldShape->Contains(localPoint);
}
