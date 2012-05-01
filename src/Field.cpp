// Field
// Author: Matthew Raso-Barnett  30/09/2009

#include "Field.hpp"
#include "FieldVertex.hpp"
//______________________________________________________________________________
// Field - ABC for magnetic field.
//
//______________________________________________________________________________

ClassImp(Field)

//_____________________________________________________________________________
Field::Field()
      :TNamed(),
       fFieldShape(NULL), 
       fFieldMatrix(NULL)
{
   // Default constructor.
   #ifdef PRINT_CONSTRUCTORS
      Info("Field", "Default Constructor");
   #endif
}

//_____________________________________________________________________________
Field::Field(const std::string& name, const TGeoShape* shape, const TGeoMatrix* matrix)
      :TNamed(name, ""), 
       fFieldShape(shape), 
       fFieldMatrix(matrix)
          
{
   // Default constructor.
   #ifdef PRINT_CONSTRUCTORS
      Info("Field", "Constructor");
   #endif
}

//_____________________________________________________________________________
Field::Field(const Field& other)
      :TNamed(other),
       fFieldShape(other.fFieldShape),
       fFieldMatrix(other.fFieldMatrix)
{
// Copy constructor.
} 

//______________________________________________________________________________
Field::~Field()
{
// Destructor.
   #ifdef PRINT_CONSTRUCTORS
      Info("Field", "Destructor");
   #endif
   if(fFieldShape) {delete fFieldShape; fFieldShape = NULL;}
   if(fFieldMatrix) {delete fFieldMatrix; fFieldMatrix = NULL;}
}

//______________________________________________________________________________
Bool_t Field::Contains(const TVector3& point) const
{
   // -- Check whether supplied point is contained by Field-shape
   // First convert point to local coordinates
   Double_t masterPoint[3] = {point[0], point[1], point[2]};
   Double_t localPoint[3] = {0.,0.,0.};
   fFieldMatrix->MasterToLocal(masterPoint, localPoint);
   // Check and return whether its contained
   return fFieldShape->Contains(localPoint);
}

//______________________________________________________________________________
FieldVertex Field::ConvertToGlobalFrame(const FieldVertex& point) const
{
   // -- Convert supplied point to global frame
   FieldVertex globalPoint;
   Double_t localPoint[3] = {point.X(), point.Y(), point.Z()};
   Double_t masterPoint[3] = {0.,0.,0.};
   fFieldMatrix->LocalToMaster(localPoint, masterPoint);
   // Rotate the field (but dont apply translation)
   Double_t localField[3] = {point.Fx(), point.Fy(), point.Fz()};
   Double_t masterField[3] = {0.,0.,0.};
   TGeoRotation rotationMatrix(*fFieldMatrix);
   rotationMatrix.LocalToMaster(localField, masterField);
   globalPoint.SetPosition(masterPoint[0],masterPoint[1],masterPoint[2]);
   globalPoint.SetField(masterField[0],masterField[1],masterField[2]);
   return globalPoint;
}

//______________________________________________________________________________
FieldVertex Field::ConvertToLocalFrame(const FieldVertex& point) const
{
   // -- Convert supplied point to local, Field's frame
   FieldVertex globalPoint;
   Double_t localPoint[3] = {0.,0.,0.};
   Double_t masterPoint[3] = {point.X(), point.Y(), point.Z()};
   fFieldMatrix->MasterToLocal(masterPoint, localPoint);
   // Rotate the field (but dont apply translation)
   Double_t localField[3] = {0.,0.,0.};
   Double_t masterField[3] = {point.Fx(), point.Fy(), point.Fz()};
   TGeoRotation rotationMatrix(*fFieldMatrix);
   rotationMatrix.MasterToLocal(masterField, localField);
   globalPoint.SetPosition(localPoint[0],localPoint[1],localPoint[2]);
   globalPoint.SetField(localField[0],localField[1],localField[2]);
   return globalPoint;
}
