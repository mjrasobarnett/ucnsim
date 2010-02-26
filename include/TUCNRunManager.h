// TUCNRunManager
// Author: Matthew Raso-Barnett  01/08/2009

#ifndef ROOT_TUCNRunManager
#define ROOT_TUCNRunManager

#include "TNamed.h"

#include "TUCNGeoManager.h"
#include "TUCNRun.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNRunManager													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNConfigFile;

class TUCNRunManager : public TNamed 
{

protected:
	TUCNRunManager(const TUCNRunManager&); 
	TUCNRunManager& operator=(const TUCNRunManager&);

	TObjArray*						fRuns;
	
public:
	// -- constructors
   TUCNRunManager();
   
	// -- destructor
	virtual ~TUCNRunManager();

	// -- methods
	Bool_t					Initialise(TUCNConfigFile& configFile);
	Int_t						NumberOfRuns() const {return fRuns->GetEntries();}
	TUCNRun*					GetRun(Int_t index) const;
	void						CreateRuns(Int_t numberOfRuns, TUCNConfigFile& configFile);
	void						WriteToFile(TFile* file);
	
   ClassDef(TUCNRunManager, 1)      
};

#endif
