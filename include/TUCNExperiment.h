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
class TUCNRunConfig;
class TUCNFieldManager;

class TUCNExperiment : public TNamed 
{
protected:
   TUCNFieldManager*    fFieldManager;
   TGeoManager*         fGeoManager;
   
   // Geometry Building
   Bool_t               BuildGeometry(const TUCNRunConfig& runConfig);
   
public:
   // -- constructors
   TUCNExperiment();
   TUCNExperiment(const TUCNExperiment&); 
   TUCNExperiment& operator=(const TUCNExperiment&);
   
   // -- destructor
   virtual ~TUCNExperiment();
   
   // -- methods
   Bool_t               Initialise(const TUCNRunConfig& runConfig);
   
   // GeoManager
   void                 ClearManager() {fGeoManager = 0;}
   TGeoManager*         GeoManager() const {return fGeoManager;}
   TGeoNavigator*       Navigator() const  {return fGeoManager->GetCurrentNavigator();}
   
   // FieldManager
   TUCNFieldManager*    FieldManager() const {return fFieldManager;}
   
   ClassDef(TUCNExperiment, 1)
};

#endif
