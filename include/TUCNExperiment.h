// TUCNExperiment
// Author: Matthew Raso-Barnett  01/08/2009

#ifndef ROOT_TUCNExperiment
#define ROOT_TUCNExperiment

#include "TNamed.h"

#include "TUCNGeoManager.h"
#include "TUCNRun.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNExperiment													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNConfigFile;

class TUCNExperiment : public TNamed 
{

protected:
	TUCNExperiment(const TUCNExperiment&); 
	TUCNExperiment& operator=(const TUCNExperiment&);

	TObjArray*						fRuns;
	
public:
	// -- constructors
   TUCNExperiment();
   
	// -- destructor
	virtual ~TUCNExperiment();

	// -- methods
	Bool_t					Initialise(TUCNConfigFile& configFile);
	Int_t						NumberOfRuns() const {return fRuns->GetEntries();}
	TUCNRun*					GetRun(Int_t index) const;
	void						CreateRuns(Int_t numberOfRuns, TUCNConfigFile& configFile);
	void						WriteToFile(TFile* file);
	
   ClassDef(TUCNExperiment, 1)      
};

#endif
