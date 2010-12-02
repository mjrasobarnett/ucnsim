// MagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_MagField
#define ROOT_MagField

#include "TNamed.h"
#include "TVector3.h"
#include "Particle.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// MagField - ABC for magnetic field.                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class MagField : public TNamed {

protected:
   
public:
   MagField();
   MagField(const std::string& name);
   MagField(const MagField&);
   MagField& operator=(const MagField&);
   virtual ~MagField();
   
   virtual Bool_t Contains(const TVector3& point) const = 0;
   
   virtual Bool_t Interact(Particle& particle, const Double_t stepTime) const = 0;
   
   ClassDef(MagField, 1)              // Abstract base Mag field class
};

#endif