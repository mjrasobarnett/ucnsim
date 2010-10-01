// UCN Particle State class
// Author: Matthew Raso-Barnett  
// 19/08/2010

#ifndef TUCNSPIN_H
#define TUCNSPIN_H

#include "TVector3.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNSpin - Spin vector object                                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TUCNSpin : public TObject
{
protected:
   TVector3 fSpin;
   
public:
   // -- Constructors
   TUCNSpin();
   TUCNSpin(const TUCNSpin&);
   TUCNSpin& operator=(const TUCNSpin&);
   virtual ~TUCNSpin();
   
   // -- Methods
   Bool_t Precess(const TVector3& localMagField, const Double_t precessTime);

   ClassDef(TUCNSpin, 1)
};

#endif  /*TUCNSPIN_H*/
