// BounceData class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <cassert>

#include "BounceData.h"

using namespace std;

//#define PRINT_CONSTRUCTORS

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    BounceData                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
ClassImp(BounceData)

//_____________________________________________________________________________
BounceData::BounceData()
                      :TObject(), fSpecular(0), fDiffuse(0)
{
   // Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("BounceData","Default Constructor");
   #endif
}

//_____________________________________________________________________________
BounceData::BounceData(const BounceData& other)
                      :TObject(other), fSpecular(other.fSpecular), fDiffuse(other.fDiffuse)
{
   // Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("BounceData","Copy Constructor");
   #endif
}

//_____________________________________________________________________________
BounceData& BounceData::operator=(const BounceData& other)
{
   // Assignment
   #ifdef PRINT_CONSTRUCTORS
      Info("BounceData","Assignment");
   #endif
   if(this!=&other) {
      TObject::operator=(other);
      fSpecular=other.fSpecular;
      fDiffuse=other.fDiffuse;
   }
   return *this;
}

//_____________________________________________________________________________
BounceData::~BounceData()
{
   // Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("BounceData","Destructor");
   #endif
}

