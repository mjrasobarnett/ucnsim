// Field Data class
// Author: Matthew Raso-Barnett  
// 22/11/2010

#ifndef FIELDDATA_H
#define FIELDDATA_H

#include <vector>
#include <string>
#include "TNamed.h"
#include "FieldVertex.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    FieldData -                                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class FieldData : public std::vector<const FieldVertex*>, public TNamed
{
private:
   void PurgeContainer();
   
public:
   FieldData();
   FieldData(const std::string name);
   FieldData(const FieldData&);
   virtual ~FieldData();
   
   ClassDef(FieldData, 1)
};

#endif
