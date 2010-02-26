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
	// GeoManager
	TGeoManager*				fGeoManager;
	TGeoManager*				GeoManager() const {return fGeoManager;}
	
	// Runs
	TObjArray*					fRuns;
	// Source Volume
	Int_t 						fSourceVolumeIndex;
	Int_t							fSourceMatrixIndex;
	void							SetSourceVolume(TGeoVolume* sourceVolume);
	TGeoVolume*					GetSourceVolume() const;
	void							SetSourceMatrix(TGeoMatrix* sourceMatrix);
	TGeoMatrix*					GetSourceMatrix() const;
	
	// Make UCNVolumes using TUCNGeoBuilder
	TGeoVolume*					MakeUCNBox(const char *name, TGeoMedium *medium, Double_t dx, Double_t dy, Double_t dz);
 	TGeoVolume*					MakeUCNTube(const char *name, TGeoMedium *medium, Double_t rmin, Double_t rmax, Double_t dz);
	
public:
	// -- constructors
   TUCNExperiment();
   TUCNExperiment(const TUCNExperiment&); 
	TUCNExperiment& operator=(const TUCNExperiment&);
	
	// -- destructor
	virtual ~TUCNExperiment();

	// -- methods
	Bool_t					Initialise(TUCNConfigFile& configFile);
	Bool_t					BuildGeometry(TGeoManager& geoManager, TUCNConfigFile& configFile);
	
	Int_t						NumberOfRuns() const {return fRuns->GetEntries();}
	TUCNRun*					GetRun(Int_t index) const;
	void						CreateRuns(Int_t numberOfRuns, TUCNConfigFile& configFile);
	void						WriteToFile(TFile* file);
	
   ClassDef(TUCNExperiment, 1)      
};

#endif
