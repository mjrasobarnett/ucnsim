// TUCNUniformMagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_TUCNUniformMagField
#define ROOT_TUCNUniformMagField

#ifndef ROOT_TUCNMagField
#include "TUCNMagField.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TUCNUniformMagField                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNUniformMagField : public TUCNMagField
{
private:
   Double_t                fBField[3]; // Magnetic field vector

protected:
   TUCNUniformMagField(const TUCNUniformMagField&);
   TUCNUniformMagField& operator=(const TUCNUniformMagField&);
	
public:
	TUCNUniformMagField();
	TUCNUniformMagField(const char *name, Double_t Bx, Double_t By, Double_t Bz);
   virtual ~TUCNUniformMagField();
   
	virtual void Field(const Double_t *pos, Double_t *field);
   
   ClassDef(TUCNUniformMagField, 1)              // uniform mag field class
};

#endif