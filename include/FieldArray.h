// FieldArray
// Author: Matthew Raso-Barnett  29/09/2010

#ifndef FIELDARRAY_H
#define FIELDARRAY_H

#include "TNamed.h"
#include <string>
#include <map>
#include <iostream>
#include "TVector3.h"

#include "Field.h"
#include "Observable.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//    FieldArray                                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
using std::string;
using std::map;

class Point;

class FieldArray : public TNamed, public Observable
{
protected:
   typedef map<string, Field*> FieldContainer;
   FieldContainer fFieldList;
   
   // Don't allow copy construction - otherwise we run into problems with ownership of pointers
   FieldArray(const FieldArray&);
   
private:
   void PurgeFields();
   
public:
   // -- constructors
   FieldArray();
   // -- destructor
   virtual ~FieldArray();
   
   // -- methods
   void AddField(Field* field);
   const TVector3 GetField(const Point& point, const string = "") const;
   virtual void   NotifyObservers(const Point& point, const std::string& context);
   
   ClassDef(FieldArray, 1)
};

#endif /* FIELDARRAY_H */
