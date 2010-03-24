// TUCNExperiment
// Author: Matthew Raso-Barnett  01/08/2009

#ifndef ROOT_TUCNExperiment
#define ROOT_TUCNExperiment

#include "TNamed.h"

#include "TGeoManager.h"
#include "TUCNRun.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNExperiment													  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNConfigFile;
class TUCNFieldManager;

class TUCNExperiment : public TNamed 
{
protected:
	// ConfigFile
	TUCNConfigFile*			fConfigFile;
	TUCNConfigFile*			ConfigFile() const {return fConfigFile;}
	// GeoManager
	TGeoManager*				GeoManager() const {return gGeoManager;}
	// FieldManager
	TUCNFieldManager*			fFieldManager;
	TUCNFieldManager*			FieldManager() const {return fFieldManager;}
	// Runs
	Int_t							fNumberOfRuns;
	// Source Volume/Matrix
	Int_t 						fSourceVolumeIndex;
	Int_t							fSourceMatrixIndex;
	void							SetSourceVolume(TGeoVolume* sourceVolume);
	TGeoVolume*					GetSourceVolume() const;
	void							SetSourceMatrix(TGeoMatrix* sourceMatrix);
	TGeoMatrix*					GetSourceMatrix() const;
	// Geometry Building
	Bool_t						BuildGeometry(TUCNConfigFile* configFile);
	
	// Particle & Track Management
//	Bool_t 						GenerateParticles(TUCNRun* run);
	Bool_t						ClearTracks();
		
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
	Bool_t					Export();
	
	Int_t						NumberOfRuns() const {return fNumberOfRuns;}
	
	
   ClassDef(TUCNExperiment, 1)      
};

#endif
