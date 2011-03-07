// MagFieldManager.cxx

#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Point.h"
#include "MagFieldManager.h"

#include "Units.h"
#include "Constants.h"

//#define VERBOSE_MODE

using namespace std;

ClassImp(MagFieldManager)

//_____________________________________________________________________________
MagFieldManager::MagFieldManager()
                :TNamed("MagFieldManager","Default Magnetic Field Manager"),
                 Observable()
{
   // Default constructor
   Info("MagFieldManager", "Default Constructor");
}

//_____________________________________________________________________________
MagFieldManager::MagFieldManager(const MagFieldManager& other)
                :TNamed(other),
                 Observable(other),
                 fFieldList(other.fFieldList)
{
   //copy constructor
   Info("MagFieldManager", "Copy Constructor");
}

//_____________________________________________________________________________
MagFieldManager::~MagFieldManager()
{
   // Destructor
   Info("MagFieldManager", "Destructor");
   this->PurgeFields();
}

//_____________________________________________________________________________
void MagFieldManager::PurgeFields()
{
   // -- Cleanup container of field pointers
   FieldContainer::iterator fieldIter;
   for(fieldIter = fFieldList.begin(); fieldIter != fFieldList.end(); ++fieldIter) {
       if(fieldIter->second) delete fieldIter->second;
       fieldIter->second = 0;
   }
}

//_____________________________________________________________________________
void MagFieldManager::AddField(MagField* field)
{
   // -- Add field to container
   fFieldList.insert(pair<string, MagField*>(field->GetName(), field));
}

//_____________________________________________________________________________
const TVector3 MagFieldManager::GetMagField(const Point& point, const string /*volume*/) const
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
void MagFieldManager::NotifyObservers(const Point& point, const std::string& context)
{
   // -- Notify Observers of change
   for(int i = 0; i < this->CountObservers(); i++) {
      Observer* observer = this->GetObserver(i);
      observer->RecordEvent(point, context);
   }
}


