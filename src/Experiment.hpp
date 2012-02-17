// Experiment
// Author: Matthew Raso-Barnett  01/08/2009

#ifndef ROOT_Experiment
#define ROOT_Experiment

#include "TNamed.h"
#include "Observable.hpp"
#include "TGeoManager.h"
#include "FieldManager.hpp"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    Experiment                                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Run;
class RunConfig;

class Experiment : public TNamed
{
protected:
   FieldManager     fFieldManager;
   TGeoManager*     fGeoManager;
   
   // Geometry Building
   Bool_t               BuildGeometry(const RunConfig& runConfig);
   
public:
   // -- constructors
   Experiment();
   Experiment(const Experiment&); 
   
   // -- destructor
   virtual ~Experiment();
   
   // -- methods
   Bool_t               Initialise(const RunConfig& runConfig);
   Bool_t               Export(Run& run);
   
   // GeoManager Interface
   void                 ClearManager() {fGeoManager = 0;}
   TGeoManager*         GetGeoManager() const {return fGeoManager;}
   TGeoNavigator*       GetNavigator() const  {return fGeoManager->GetCurrentNavigator();}
   
   // FieldManager Interface
   const FieldManager&     GetFieldManager() const {return fFieldManager;}
   const GravField* const  GetGravField() const {return fFieldManager.GetGravField();}
   const TVector3          GetMagField(const Point& point, const TVector3& vel, const string volume = "") const;
   
   ClassDef(Experiment, 1)
};

#endif
