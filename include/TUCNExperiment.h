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
	// ConfigFile
	TUCNConfigFile*			fConfigFile;
	TUCNConfigFile*			ConfigFile() const {return fConfigFile;}
	// GeoManager
	TGeoManager*				fGeoManager;
	TGeoManager*				GeoManager() const {return fGeoManager;}
	// FieldManager
	TUCNFieldManager*			fFieldManager;
	TUCNFieldManager*			FieldManager() const {return fFieldManager;}
	// Runs
	Int_t							fNumberOfRuns;
	Int_t							NumberOfRuns() const {return fNumberOfRuns;}
	// Source Volume/Matrix
	Int_t 						fSourceVolumeIndex;
	Int_t							fSourceMatrixIndex;
	void							SetSourceVolume(TGeoVolume* sourceVolume);
	TGeoVolume*					GetSourceVolume() const;
	void							SetSourceMatrix(TGeoMatrix* sourceMatrix);
	TGeoMatrix*					GetSourceMatrix() const;
	// Geometry Building
	Bool_t						BuildGeometry(TGeoManager* geoManager, TUCNConfigFile* configFile);
	
	// Make UCNVolumes using TUCNGeoBuilder
	TGeoVolume*					MakeUCNBox(const char *name, TGeoMedium *medium, Double_t dx, Double_t dy, Double_t dz);
 	TGeoVolume*					MakeUCNTube(const char *name, TGeoMedium *medium, Double_t rmin, Double_t rmax, Double_t dz);
	
	Bool_t						CreateRuns(TUCNConfigFile* configFile);
	Bool_t 						GenerateParticles(TUCNRun* run);
	Bool_t						ClearTracks();
	void							WriteToFile(TFile* file);
	
public:
	// -- constructors
	TUCNExperiment();
	TUCNExperiment(std::string configFileName);
   TUCNExperiment(const TUCNExperiment&); 
	TUCNExperiment& operator=(const TUCNExperiment&);
	
	// -- destructor
	virtual ~TUCNExperiment();

	// -- methods
	Bool_t					Initialise();
	Bool_t					Run();
	
	
   ClassDef(TUCNExperiment, 1)      
};

#endif
