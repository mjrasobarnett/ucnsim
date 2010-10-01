// TUCNMagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_TUCNMagField
#define ROOT_TUCNMagField

#include "TNamed.h"
#include "TVector3.h"
#include "TUCNParticle.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TUCNMagField - ABC for magnetic field.                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNMagField : public TNamed {

protected:
   
public:
   TUCNMagField();
   TUCNMagField(const std::string& name);
   TUCNMagField(const TUCNMagField&);
   TUCNMagField& operator=(const TUCNMagField&);
   virtual ~TUCNMagField();
   
   virtual Bool_t Contains(const TVector3& point) const = 0;
   
   virtual void GetFieldVector(const TVector3& pos, TVector3& field) const = 0;
   
   virtual Bool_t Interact(TUCNParticle& particle, const Double_t stepTime) const = 0;
   
   ClassDef(TUCNMagField, 1)              // Abstract base Mag field class
};

#endif