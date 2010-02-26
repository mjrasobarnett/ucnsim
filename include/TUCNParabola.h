// TUCNParabola
// Author: Matthew Raso-Barnett  06/07/2009

#ifndef ROOT_TUCNParabola
#define ROOT_TUCNParabola

#ifndef ROOT_TObject
#include "TObject.h"
#endif

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  TUCNParabola                                                         //
//                                                                       //
//  Geometry construction class                                          //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class TUCNParabola : public TObject
{
protected:
	static TUCNParabola    *fgParabola;  
	
	TUCNParabola();
	TUCNParabola(const TUCNParabola&); 
   TUCNParabola& operator=(const TUCNParabola&);

private:
	
public:
	virtual ~TUCNParabola();
   
	static TUCNParabola* 	Instance();
	
	virtual Double_t			ArcLength(Double_t* velocity, Double_t* field, Double_t steptime)	const;
	
	 
   ClassDef(TUCNParabola, 1)          // parabola singleton
};

#endif
