// TUCNGeoBuilder
// Author: Matthew Raso-Barnett  22/05/2009

#ifndef ROOT_TUCNGeoBuilder
#define ROOT_TUCNGeoBuilder

#ifndef ROOT_TGeoBuilder
#include "TGeoBuilder.h"
#endif

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  TUCNGeoBuilder                                                       //
//                                                                       //
// Geometry construction class                                           //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class TUCNGeoManager;

class TUCNGeoBuilder : public TGeoBuilder
{
protected:
	static TUCNGeoBuilder    *fgUCNInstance;  
	
	TUCNGeoBuilder();
	TUCNGeoBuilder(const TUCNGeoBuilder&); 
   TUCNGeoBuilder& operator=(const TUCNGeoBuilder&);

private:
	TUCNGeoManager* fUCNGeometry;             //! current geometry
	
   void SetUCNGeometry(TUCNGeoManager *geom) {fUCNGeometry = geom;}

public:
	virtual ~TUCNGeoBuilder();
   
	static TUCNGeoBuilder* UCNInstance(TUCNGeoManager *geom);
	
   TGeoVolume* MakeUCNBox(const char *name, const TGeoMedium *medium, Double_t dx, Double_t dy, Double_t dz);
   TGeoVolume* MakeUCNTube(const char *name, const TGeoMedium *medium, Double_t rmin, Double_t rmax, Double_t dz);
   
   ClassDef(TUCNGeoBuilder, 0)          // geometry builder singleton
};

#endif