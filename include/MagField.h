// MagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_MagField
#define ROOT_MagField

#include "TNamed.h"
#include "TVector3.h"
#include "TGeoShape.h"
#include "TGeoMatrix.h"

#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// MagField - ABC for magnetic field.                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Particle;
class Run;

class MagField : public TNamed {
private:
   // Field Extent -- a virtual volume that defines the extent of this field in the geometry
   const TGeoShape* fFieldShape;
   const TGeoMatrix* fFieldMatrix;
   
protected:
   TVector3& ConvertToLocalFrame(const TVector3& point) const; 
   
public:
   MagField();
   MagField(const std::string& name, const TGeoShape* shape, const TGeoMatrix* matrix);
   MagField(const MagField&);
   virtual ~MagField();
   
   virtual Bool_t Contains(const TVector3& point) const;
   virtual Bool_t Interact(Particle& particle, const Run& run, const Double_t stepTime) const = 0;
   
   ClassDef(MagField, 1)              // Abstract base Mag field class
};

#endif