// MagFieldArray.cxx

#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Point.h"
#include "MagFieldArray.h"

#include "Units.h"
#include "Constants.h"

//#define VERBOSE_MODE

using namespace std;

ClassImp(MagFieldArray)

//_____________________________________________________________________________
MagFieldArray::MagFieldArray()
              :TNamed("MagFieldArray","Default Magnetic Field Manager"),
               Observable()
{
   // Default constructor
   Info("MagFieldArray", "Default Constructor");
}

//_____________________________________________________________________________
MagFieldArray::MagFieldArray(const MagFieldArray& other)
              :TNamed(other),
               Observable(other),
               fFieldList(other.fFieldList)
{
   //copy constructor
   Info("MagFieldArray", "Copy Constructor");
}

//_____________________________________________________________________________
MagFieldArray::~MagFieldArray()
{
   // Destructor
   Info("MagFieldArray", "Destructor");
   this->PurgeFields();
}

//_____________________________________________________________________________
void MagFieldArray::PurgeFields()
{
   // -- Cleanup container of field pointers
   FieldContainer::iterator fieldIter;
   for(fieldIter = fFieldList.begin(); fieldIter != fFieldList.end(); ++fieldIter) {
       if(fieldIter->second) delete fieldIter->second;
       fieldIter->second = 0;
   }
}

//_____________________________________________________________________________
void MagFieldArray::AddField(MagField* field)
{
   // -- Add field to container
   fFieldList.insert(pair<string, MagField*>(field->GetName(), field));
}

//_____________________________________________________________________________
const TVector3 MagFieldArray::GetMagField(const Point& point, const string /*volume*/) const
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
void MagFieldArray::NotifyObservers(const Point& point, const std::string& context)
{
   // -- Notify Observers of change
   for(int i = 0; i < this->CountObservers(); i++) {
      Observer* observer = this->GetObserver(i);
      observer->RecordEvent(point, context);
   }
}


