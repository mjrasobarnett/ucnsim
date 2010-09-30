// TUCNMagFieldManager
// Author: Matthew Raso-Barnett  29/09/2010

#ifndef ROOT_TUCNMagFieldManager
#define ROOT_TUCNMagFieldManager

#include "TNamed.h"
#include <string>
#include <map>
#include <iostream>

#include "TUCNMagField.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    TUCNMagFieldManager                                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
using std::string;
using std::map;
using std::cout;
using std::endl;

class TUCNMagFieldManager : public TNamed 
{
private:
   typedef map<string, TUCNMagField*> FieldContainer;
   FieldContainer fFieldList;
   
   void PurgeFields();
   
   //** Don't allow copy construction - otherwise we run into problems with ownership of pointers
   TUCNMagFieldManager(const TUCNMagFieldManager&);
   TUCNMagFieldManager& operator=(const TUCNMagFieldManager&);
   
public:
   // -- constructors
   TUCNMagFieldManager();
   
   void AddField(TUCNMagField* field);
   
   // -- destructor
   virtual ~TUCNMagFieldManager();

   ClassDef(TUCNMagFieldManager, 1)
};

#endif
