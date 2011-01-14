// Spin Data class
// Author: Matthew Raso-Barnett  
// 22/11/2010

#ifndef SPINDATA_H
#define SPINDATA_H

#include <map>
#include "TObject.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    SpinData -                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class SpinData : public std::map<Double_t, Bool_t>, public TObject
{
private:

public:
   SpinData();
   SpinData(const SpinData&);
   SpinData& operator=(const SpinData&);
   virtual ~SpinData();
   
   ClassDef(SpinData, 1)
};

#endif