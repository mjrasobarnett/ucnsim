// TUCNUniformMagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_TUCNUniformMagField
#define ROOT_TUCNUniformMagField

#include "TUCNMagField.h"
#include "TVector3.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TUCNUniformMagField                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNUniformMagField : public TUCNMagField
{
private:
   Double_t fBx, fBy, fBz; // Magnetic field vector
   Double_t fBMag;
   
public:
   TUCNUniformMagField();
   TUCNUniformMagField(const std::string& name, const Double_t& Bx, const Double_t& By, const Double_t& Bz);
   TUCNUniformMagField(const TUCNUniformMagField&);
   TUCNUniformMagField& operator=(const TUCNUniformMagField&);
   virtual ~TUCNUniformMagField();
   
   virtual Double_t Bx(const TVector3& /*pos*/) const {return fBx;}
   virtual Double_t By(const TVector3& /*pos*/) const {return fBy;}
   virtual Double_t Bz(const TVector3& /*pos*/) const {return fBz;}
   virtual Double_t B(const TVector3& /*pos*/) const {return fBMag;}
   
   virtual void GetFieldVector(const TVector3& pos, TVector3& field) const;
   
   ClassDef(TUCNUniformMagField, 1)              // uniform mag field class
};

#endif