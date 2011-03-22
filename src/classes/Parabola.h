// Parabola
// Author: Matthew Raso-Barnett  06/07/2009

#ifndef ROOT_Parabola
#define ROOT_Parabola

#ifndef ROOT_TObject
#include "TObject.h"
#endif

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Parabola                                                         //
//                                                                       //
//  Geometry construction class                                          //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class Parabola : public TObject
{
protected:
	static Parabola    *fgParabola;  
	
	Parabola();
	Parabola(const Parabola&); 
   Parabola& operator=(const Parabola&);

private:
	
public:
	virtual ~Parabola();
   
	static Parabola* 	Instance();
	
	virtual Double_t			ArcLength(const Double_t* velocity, const Double_t* field, const Double_t steptime)	const;
	
	 
   ClassDef(Parabola, 1)          // parabola singleton
};

#endif
