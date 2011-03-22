// FieldArray.cxx

#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Point.h"
#include "FieldArray.h"

//#define VERBOSE_MODE

using namespace std;

ClassImp(FieldArray)

//_____________________________________________________________________________
FieldArray::FieldArray()
           :TNamed("FieldArray","Field Array"),
            Observable()
{
   // Default constructor
   Info("FieldArray", "Default Constructor");
}

//_____________________________________________________________________________
FieldArray::FieldArray(const std::string name)
           :TNamed(name,"FieldArray"),
            Observable()
{
   // Named Constructor
   Info("FieldArray", "Constructor");
}

//_____________________________________________________________________________
FieldArray::FieldArray(const FieldArray& other)
           :TNamed(other),
            Observable(other),
            fFieldList(other.fFieldList)
{
   //copy constructor
   Info("FieldArray", "Copy Constructor");
}

//_____________________________________________________________________________
FieldArray::~FieldArray()
{
   // Destructor
   Info("FieldArray", "Destructor");
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

//_____________________________________________________________________________
void FieldArray::NotifyObservers(const Point& point, const TVector3& velocity, const std::string& context)
{
   // -- Notify Observers of change
   for(int i = 0; i < this->CountObservers(); i++) {
      Observer* observer = this->GetObserver(i);
      observer->RecordEvent(point, velocity, context);
   }
}

