// TUCNParabolicMagField
// Author: Matthew Raso-Barnett  30/09/2009

#ifndef ROOT_TUCNParabolicMagField
#define ROOT_TUCNParabolicMagField

#ifndef ROOT_TUCNMagField
#include "TUCNMagField.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TUCNParabolicMagField                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class TUCNParabolicMagField : public TUCNMagField
{
private:
   Double_t                fParabolicGradient; 	// Parabolic Gradient: A, in the formula
	Double_t						fBMax;					// Field along z-axis, r = 0
	Double_t						fFieldRadius;			// Radius of the containing vessel
	
protected:
   TUCNParabolicMagField(const TUCNParabolicMagField&);
   TUCNParabolicMagField& operator=(const TUCNParabolicMagField&);
	
public:
	TUCNParabolicMagField();
	TUCNParabolicMagField(const char *name, Double_t parabolicGradient, Double_t maxB, Double_t fieldRadius);
   virtual ~TUCNParabolicMagField();
   
	virtual void Field(const Double_t* pos, Double_t* field);
	virtual Double_t FieldStrength(const Double_t* pos) const;
	
   ClassDef(TUCNParabolicMagField, 1)              // parabolic mag field class
};

#endif