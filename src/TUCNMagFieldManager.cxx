// TUCNMagFieldManager.cxx

#include <iostream>
#include <cassert>
#include <stdexcept>

#include "TUCNMagFieldManager.h"

#include "Units.h"
#include "Constants.h"

//#define VERBOSE_MODE

using namespace std;

ClassImp(TUCNMagFieldManager)

//_____________________________________________________________________________
TUCNMagFieldManager::TUCNMagFieldManager()
                    :TNamed("MagFieldManager","Default Magnetic Field Manager")
{
   // Default constructor
   Info("TUCNMagFieldManager", "Default Constructor");
}

//_____________________________________________________________________________
TUCNMagFieldManager::TUCNMagFieldManager(const TUCNMagFieldManager& other)
                    :TNamed(other), fFieldList(other.fFieldList)
{
   //copy constructor
   Info("TUCNMagFieldManager", "Copy Constructor");  
}

//_____________________________________________________________________________
TUCNMagFieldManager& TUCNMagFieldManager::operator=(const TUCNMagFieldManager& other) 
{
   //assignment operator
   Info("TUCNMagFieldManager", "Assignment");
   if(this!=&other) {
      TNamed::operator=(other);
      fFieldList = other.fFieldList;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNMagFieldManager::~TUCNMagFieldManager()
{
   // Destructor
   Info("TUCNMagFieldManager", "Destructor");
   this->PurgeFields();
}

//_____________________________________________________________________________
void TUCNMagFieldManager::PurgeFields()
{
   // -- Cleanup container of field pointers
   FieldContainer::iterator fieldIter;
   for(fieldIter = fFieldList.begin(); fieldIter != fFieldList.end(); ++fieldIter) {
       if(fieldIter->second) delete fieldIter->second;
       fieldIter->second = 0;
   }
}

//_____________________________________________________________________________
void TUCNMagFieldManager::AddField(TUCNMagField* field)
{
   // -- Add field to container
   fFieldList.insert(pair<string, TUCNMagField*>(field->GetName(), field));
}

//_____________________________________________________________________________
const TUCNMagField* const TUCNMagFieldManager::GetMagField(const TVector3& point, const string& /*volume*/) const
{
   // -- Determine which field contains the current particle
   FieldContainer::const_iterator fieldIter;
   for(fieldIter = fFieldList.begin(); fieldIter != fFieldList.end(); ++fieldIter) {
      if (fieldIter->second->Contains(point) == kTRUE) {
         return fieldIter->second;
      }
   }
   return NULL;
}