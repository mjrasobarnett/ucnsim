// TUCNGeoNavigator
// Author: Matthew Raso-Barnett  27/05/2009

#ifndef ROOT_TUCNGeoNavigator
#define ROOT_TUCNGeoNavigator

#ifndef 	ROOT_TGeoNavigator
#include "TGeoNavigator.h"
#endif

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNGeoNavigator												  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNParticle;
class TUCNGravField;
class TUCNMagField;
class TUCNGeoManager;
class TUCNGeoMaterial;
class TUCNFieldManager;
class TGeoManager;
class TGeoNode;
class TGeoVolume;
class TGeoMatrix;
class TGeoHMatrix;


class TUCNGeoNavigator : public TGeoNavigator 
{

   ClassDef(TUCNGeoNavigator, 1)          // UCN geometry navigator
};

#endif
