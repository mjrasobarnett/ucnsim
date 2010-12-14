// FieldMap
// Author: Matthew Raso-Barnett  13/12/2010

#ifndef ROOT_FieldMap
#define ROOT_FieldMap

#include "TNamed.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// FieldMap -                                                             //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class FieldMap : public TNamed {
private:
   
public:
   FieldMap();
   FieldMap(const FieldMap&);
   FieldMap& operator=(const FieldMap&);
   virtual ~FieldMap();
   
   bool ReadFile(const std::string& filename);
   
   ClassDef(FieldMap, 1)              // Field Map class
};

#endif