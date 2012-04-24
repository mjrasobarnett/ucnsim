// FieldArray.cxx

#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Point.hpp"
#include "FieldArray.hpp"

//#define VERBOSE_MODE

using namespace std;

ClassImp(FieldArray)

//_____________________________________________________________________________
FieldArray::FieldArray()
           :TNamed("FieldArray","Field Array"),
            Observable()
{
   // Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("FieldArray", "Default Constructor");
   #endif
}

//_____________________________________________________________________________
FieldArray::FieldArray(const std::string name)
           :TNamed(name,"FieldArray"),
            Observable()
{
   // Named Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("FieldArray", "Constructor");
   #endif
}

//_____________________________________________________________________________
FieldArray::FieldArray(const FieldArray& other)
           :TNamed(other),
            Observable(other),
            fFieldList(other.fFieldList)
{
   //copy constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("FieldArray", "Copy Constructor");
   #endif
}

//_____________________________________________________________________________
FieldArray::~FieldArray()
{
   // Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("FieldArray", "Destructor");
   #endif
   this->PurgeFields();
}

//_____________________________________________________________________________
void FieldArray::PurgeFields()
{
   // -- Cleanup container of field pointers
   FieldContainer::iterator fieldIter;
   for(fieldIter = fFieldList.begin(); fieldIter != fFieldList.end(); ++fieldIter) {
       if(fieldIter->second) delete fieldIter->second;
       fieldIter->second = 0;
   }
}

//_____________________________________________________________________________
void FieldArray::AddField(Field* field)
{
   // -- Add field to container
   fFieldList.insert(pair<string, Field*>(field->GetName(), field));
}

//_____________________________________________________________________________
const TVector3 FieldArray::GetField(const Point& point, const string /*volume*/) const
{
   // -- Determine which field contains the current particle
   FieldContainer::const_iterator fieldIter;
   for(fieldIter = fFieldList.begin(); fieldIter != fFieldList.end(); ++fieldIter) {
      if (fieldIter->second->Contains(point.GetPosition()) == kTRUE) {
         return fieldIter->second->GetField(point);
      }
   }
   cout << "NO FIELD FOUND!" << endl;
   point.GetPosition().Print();
   return TVector3(0.,0.,0.);
}

