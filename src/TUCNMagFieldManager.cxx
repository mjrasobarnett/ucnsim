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
                    :TNamed(other)
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
   }
   return *this;
}

//_____________________________________________________________________________
TUCNMagFieldManager::~TUCNMagFieldManager()
{
// Destructor
   Info("TUCNMagFieldManager", "Destructor");
}