// UniformMagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_UniformMagField
#define ROOT_UniformMagField

#include "MagField.h"
#include "TVector3.h"

#include <string>
#include "Particle.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// UniformMagField                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class UniformMagField : public MagField
{
private:
   // Magnetic field vector
   TVector3 fField;
   
   const TVector3& GetFieldVector(const TVector3& pos) const;
   
public:
   UniformMagField();
   UniformMagField(const std::string& name, const TVector3& field, const TGeoShape* fieldShape, const TGeoMatrix* fieldPosition);
   UniformMagField(const UniformMagField&);
   UniformMagField& operator=(const UniformMagField&);
   virtual ~UniformMagField();
   
   virtual Bool_t Interact(Particle& particle, const Double_t stepTime) const;
   
   ClassDef(UniformMagField, 1)              // uniform mag field class
};

#endif