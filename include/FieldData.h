// Field Data class
// Author: Matthew Raso-Barnett  
// 22/11/2010

#ifndef FIELDDATA_H
#define FIELDDATA_H

#include <vector>
#include "TObject.h"
#include "FieldVertex.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    FieldData -                                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class FieldData : public std::vector<const FieldVertex*>, public TObject
{
private:
   void PurgeContainer();
   
public:
   FieldData();
   FieldData(const FieldData&);
   virtual ~FieldData();
   
   ClassDef(FieldData, 1)
};

#endif
