// Experiment
// Author: Matthew Raso-Barnett  01/08/2009

#ifndef ROOT_Experiment
#define ROOT_Experiment

#include "TNamed.h"
#include "TGeoManager.h"
#include "FieldManager.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    Experiment                                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class RunConfig;

class Experiment : public TNamed 
{
protected:
   FieldManager*    fFieldManager;
   TGeoManager*         fGeoManager;
   
   // Geometry Building
   Bool_t               BuildGeometry(const RunConfig& runConfig);
   
public:
   // -- constructors
   Experiment();
   Experiment(const Experiment&); 
   Experiment& operator=(const Experiment&);
   
   // -- destructor
   virtual ~Experiment();
   
   // -- methods
   Bool_t               Initialise(const RunConfig& runConfig);
   
   // GeoManager
   void                 ClearManager() {fGeoManager = 0;}
   TGeoManager*         GetGeoManager() const {return fGeoManager;}
   TGeoNavigator*       GetNavigator() const  {return fGeoManager->GetCurrentNavigator();}
   
   // FieldManager
   FieldManager*        GetFieldManager() const {return fFieldManager;}
   
   ClassDef(Experiment, 1)
};

#endif
