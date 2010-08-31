// TUCNMagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_TUCNMagField
#define ROOT_TUCNMagField

#include "TNamed.h"
#include "TVector3.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TUCNMagField - ABC for magnetic field.                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNMagField : public TNamed {

public:
   TUCNMagField();
   TUCNMagField(const std::string& name);
   TUCNMagField(const TUCNMagField&);
   TUCNMagField& operator=(const TUCNMagField&);
   virtual ~TUCNMagField();
   
   virtual Double_t Bx(const TVector3& pos) const = 0;
   virtual Double_t By(const TVector3& pos) const = 0;
   virtual Double_t Bz(const TVector3& pos) const = 0;
   virtual Double_t B(const TVector3& pos) const = 0;
   
   virtual void GetFieldVector(const TVector3& pos, TVector3& field) const = 0;

   ClassDef(TUCNMagField, 1)              // Abstract base Mag field class
};

#endif