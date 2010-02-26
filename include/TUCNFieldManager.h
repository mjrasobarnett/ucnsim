// TUCNFieldManager
// Author: Matthew Raso-Barnett  28/10/2009

#ifndef ROOT_TUCNFieldManager
#define ROOT_TUCNFieldManager

#include "TNamed.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNFieldManager													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNGravField;
class TUCNMagField;

class TUCNFieldManager : public TNamed 
{
protected:
	TUCNMagField*		fMagField;
	TUCNGravField*		fGravField;
	
public:
	// -- constructors
	TUCNFieldManager();
	TUCNFieldManager(const TUCNFieldManager&); 
	TUCNFieldManager& operator=(const TUCNFieldManager&);
	
	// -- destructor
	virtual ~TUCNFieldManager();
	
	// -- methods
	
	// Grav Fields
	TUCNGravField*				AddGravField();
	TUCNGravField*				GravField() const {return fGravField;}
	
	// Mag Fields
	TUCNMagField*				AddUniformMagField(const Double_t Bx, const Double_t By, const Double_t Bz);
	TUCNMagField*				AddParabolicMagField(const Double_t maxB, const Double_t alpha, const Double_t maxR);
	TUCNMagField*				MagField() const {return fMagField;};
	
	ClassDef(TUCNFieldManager,1)
};

#endif
