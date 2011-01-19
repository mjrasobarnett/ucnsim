// MagFieldManager
// Author: Matthew Raso-Barnett  29/09/2010

#ifndef ROOT_MagFieldManager
#define ROOT_MagFieldManager

#include "TNamed.h"
#include <string>
#include <map>
#include <iostream>
#include "TVector3.h"

#include "MagField.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    MagFieldManager                                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
using std::string;
using std::map;
using std::cout;
using std::endl;

class MagFieldManager : public TNamed 
{
private:
   typedef map<string, MagField*> FieldContainer;
   FieldContainer fFieldList;
   
   void PurgeFields();
   
   //** Don't allow copy construction - otherwise we run into problems with ownership of pointers
   MagFieldManager(const MagFieldManager&);
   MagFieldManager& operator=(const MagFieldManager&);
   
public:
   // -- constructors
   MagFieldManager();
   
   void AddField(MagField* field);
   
   // -- destructor
   virtual ~MagFieldManager();

   // -- methods
   const MagField* const GetMagField(const TVector3&, const string = "") const;
   
   ClassDef(MagFieldManager, 1)
};

#endif
