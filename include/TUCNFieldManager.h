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
class TUCNConfigFile;

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
	Bool_t						Initialise(TUCNConfigFile* configFile);
	
	// Grav Fields
	TUCNGravField*				AddGravField();
	TUCNGravField*				GravField() const {return fGravField;}
	
	// Mag Fields
	TUCNMagField*				AddUniformMagField(const Double_t Bx=0, const Double_t By=0, const Double_t Bz=0);
	TUCNMagField*				AddParabolicMagField(const Double_t maxB, const Double_t alpha, const Double_t maxR);
	TUCNMagField*				MagField() const {return fMagField;};
	
	ClassDef(TUCNFieldManager,1)
};

#endif
