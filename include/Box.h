// Box
// Author: Matthew Raso-Barnett  22/05/2009

#ifndef ROOT_Box
#define ROOT_Box

#include "TGeoBBox.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						Box														  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class Box : public TGeoBBox 
{

public:
   // constructors
   Box();
   Box(Double_t dx, Double_t dy, Double_t dz, Double_t *origin=0);
   Box(const char *name, Double_t dx, Double_t dy, Double_t dz, Double_t *origin=0);
   Box(Double_t *param);
   // destructor
   virtual ~Box();
   
   // methods
   virtual Double_t TimeFromInside(const Double_t* point, const Double_t* velocity, const Double_t* field,
                               const Double_t stepTime, const Bool_t onBoundary) const;
   static  Double_t TimeFromInsideS(const Double_t* point, const Double_t* velocity, const Double_t* field,
   const Double_t dx, const Double_t dy, const Double_t dz, const Double_t *origin, const Bool_t onBoundary);
   virtual Double_t TimeFromOutside(const Double_t* point, const Double_t* velocity, const Double_t* field,
                               const Double_t stepTime, const Bool_t onBoundary) const;
   static  Double_t TimeFromOutsideS(const Double_t* point, const Double_t* velocity, const Double_t* field,
   const Double_t dx, const Double_t dy, const Double_t dz, const Double_t *origin, const Bool_t onBoundary);
   static  Bool_t IsNextPointOnBox(const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t* boundary, const Double_t t);
   static Double_t SmallestInsideTime(const Int_t solutions, Double_t* roots, const Bool_t onBoundary);
   static Double_t SmallestOutsideTime(const Int_t solutions, Double_t* roots, const Bool_t onBoundary, const Double_t* point, const Double_t* velocity, const Double_t* field, const Double_t* boundary);


   ClassDef(Box, 1) // Box
};

#endif
