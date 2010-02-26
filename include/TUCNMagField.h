// TUCNMagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_TUCNMagField
#define ROOT_TUCNMagField

#ifndef ROOT_TNamed
#include "TNamed.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TUCNMagField - ABC for magnetic field.                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNParticle;
class TUCNGravField;

class TUCNMagField : public TNamed
{
public:
	TUCNMagField();
	TUCNMagField(const char *name);
   virtual ~TUCNMagField();
   
   virtual void Field(const Double_t *pos, Double_t *field) = 0;
	virtual Double_t FieldStrength(const Double_t *pos) const = 0;
	virtual Double_t IntegratedField(const Double_t stepTime, const TUCNParticle* initialState, const TUCNGravField* gravField=0) const = 0;
	
   ClassDef(TUCNMagField, 1)              // Abstract base Mag field class
};

#endif