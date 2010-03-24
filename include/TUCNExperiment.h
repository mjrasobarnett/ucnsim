// TUCNExperiment
// Author: Matthew Raso-Barnett  01/08/2009

#ifndef ROOT_TUCNExperiment
#define ROOT_TUCNExperiment

#include "TNamed.h"
#include "TGeoManager.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNExperiment                                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNConfigFile;
class TUCNFieldManager;
class TUCNRun;

class TUCNExperiment : public TNamed 
{
protected:
   TUCNFieldManager*    fFieldManager;
   TGeoManager*         fGeoManager;
   
   // Geometry Building
   Bool_t               BuildGeometry(TUCNConfigFile& configFile, const TUCNRun& run);
   
public:
   // -- constructors
   TUCNExperiment();
   TUCNExperiment(const TUCNExperiment&); 
   TUCNExperiment& operator=(const TUCNExperiment&);
   
   // -- destructor
   virtual ~TUCNExperiment();
   
   // -- methods
   Bool_t               Initialise(TUCNConfigFile& configFile, const TUCNRun& run);
   
   // GeoManager
   void                 ClearManager() {fGeoManager = 0;}
   TGeoManager*         GeoManager() const {return fGeoManager;}
   TGeoNavigator*       Navigator() const  {return fGeoManager->GetCurrentNavigator();}
   
   // FieldManager
   TUCNFieldManager*    FieldManager() const {return fFieldManager;}
   
   ClassDef(TUCNExperiment, 1)
};

#endif
