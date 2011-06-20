// Bounce Data class
// Author: Matthew Raso-Barnett  
// 22/11/2010

#ifndef BOUNCEDATA_H
#define BOUNCEDATA_H

#include "TObject.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    BounceData -                                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class BounceData : public TObject
{
private:
   Int_t fSpecular;
   Int_t fDiffuse;
   
public:
   BounceData();
   BounceData(const BounceData&);
   BounceData& operator=(const BounceData&);
   virtual ~BounceData();
   
   void RecordSpecular() {fSpecular++;}
   void RecordDiffuse() {fDiffuse++;}
   
   Int_t CountSpecular() const {return fSpecular;}
   Int_t CountDiffuse() const {return fDiffuse;}
   Int_t CountTotal() const {return (fDiffuse+fSpecular);}
   
   ClassDef(BounceData, 1)
};

#endif
