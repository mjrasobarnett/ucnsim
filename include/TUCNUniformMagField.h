// TUCNUniformMagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_TUCNUniformMagField
#define ROOT_TUCNUniformMagField

#include "TUCNMagField.h"
#include "TVector3.h"
#include "TGeoShape.h"
#include "TGeoMatrix.h"

#include <string>
#include "TUCNParticle.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TUCNUniformMagField                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNUniformMagField : public TUCNMagField
{
private:
   // Magnetic field vector
   TVector3 fField;
   // Field Extent -- a virtual volume that defines the extent of this field in the geometry
   const TGeoShape* fFieldShape;
   const TGeoMatrix* fFieldMatrix;
   
public:
   TUCNUniformMagField();
   TUCNUniformMagField(const std::string& name, const TVector3& field, const TGeoShape* fieldShape, const TGeoMatrix* fieldPosition);
   TUCNUniformMagField(const TUCNUniformMagField&);
   TUCNUniformMagField& operator=(const TUCNUniformMagField&);
   virtual ~TUCNUniformMagField();
   
   virtual void GetFieldVector(const TVector3& pos, TVector3& field) const;
   
   virtual Bool_t Interact(TUCNParticle& particle, const Double_t stepTime) const;
   
   ClassDef(TUCNUniformMagField, 1)              // uniform mag field class
};

#endif