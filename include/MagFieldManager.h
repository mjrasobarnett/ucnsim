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
#include "Observable.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    MagFieldManager                                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
using std::string;
using std::map;
using std::cout;
using std::endl;

class Point;

class MagFieldManager : public TNamed, public Observable
{
private:
   typedef map<string, MagField*> FieldContainer;
   FieldContainer fFieldList;
   
   void PurgeFields();
   
   //** Don't allow copy construction - otherwise we run into problems with ownership of pointers
   MagFieldManager(const MagFieldManager&);
   
public:
   // -- constructors
   MagFieldManager();
   
   void AddField(MagField* field);
   
   // -- destructor
   virtual ~MagFieldManager();

   // -- methods
   const TVector3 GetMagField(const Point& point, const string = "") const;
   virtual void   NotifyObservers(const Point& point, const std::string& context);
   
   ClassDef(MagFieldManager, 1)
};

#endif
