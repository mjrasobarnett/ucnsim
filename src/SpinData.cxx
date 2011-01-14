// SpinData class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <cassert>

#include "SpinData.h"

using namespace std;

//#define PRINT_CONSTRUCTORS

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    SpinData                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
ClassImp(SpinData)

//_____________________________________________________________________________
SpinData::SpinData()
                    :std::map<Double_t, Bool_t>(),
                     TObject()
{
   // Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinData","Default Constructor");
   #endif
}

//_____________________________________________________________________________
SpinData::SpinData(const SpinData& other)
                    :std::map<Double_t, Bool_t>(other),
                     TObject(other)
{
   // Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinData","Copy Constructor");
   #endif
}

//_____________________________________________________________________________
SpinData& SpinData::operator=(const SpinData& other)
{
   // Assignment
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinData","Assignment");
   #endif
   if(this!=&other) {
      std::map<Double_t,Bool_t>::operator=(other);
      TObject::operator=(other);
   }
   return *this;
}

//_____________________________________________________________________________
SpinData::~SpinData()
{
   // Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("SpinData","Destructor");
   #endif
}