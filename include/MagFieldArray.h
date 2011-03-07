// MagFieldArray
// Author: Matthew Raso-Barnett  29/09/2010

#ifndef ROOT_MagFieldArray
#define ROOT_MagFieldArray

#include "TNamed.h"
#include <string>
#include <map>
#include <iostream>
#include "TVector3.h"

#include "MagField.h"
#include "Observable.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    MagFieldArray                                                 //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
using std::string;
using std::map;
using std::cout;
using std::endl;

class Point;

class MagFieldArray : public TNamed, public Observable
{
private:
   typedef map<string, MagField*> FieldContainer;
   FieldContainer fFieldList;
   
   void PurgeFields();
   
   //** Don't allow copy construction - otherwise we run into problems with ownership of pointers
   MagFieldArray(const MagFieldArray&);
   
public:
   // -- constructors
   MagFieldArray();
   
   void AddField(MagField* field);
   
   // -- destructor
   virtual ~MagFieldArray();

   // -- methods
   const TVector3 GetMagField(const Point& point, const string = "") const;
   virtual void   NotifyObservers(const Point& point, const std::string& context);
   
   ClassDef(MagFieldArray, 1)
};

#endif
