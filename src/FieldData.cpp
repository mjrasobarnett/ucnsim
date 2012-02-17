// FieldData class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <cassert>

#include "FieldData.hpp"

using namespace std;

//#define PRINT_CONSTRUCTORS

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    FieldData                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
ClassImp(FieldData)


//_____________________________________________________________________________
FieldData::FieldData()
                 :std::vector<const FieldVertex*>(),
                  TNamed()
{
   // Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("FieldData","Default Constructor");
   #endif
}

//_____________________________________________________________________________
FieldData::FieldData(const string name)
                 :std::vector<const FieldVertex*>(),
                  TNamed(name,name)
{
   // Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("FieldData","Default Constructor");
   #endif
}

//_____________________________________________________________________________
FieldData::FieldData(const FieldData& other)
                    :std::vector<const FieldVertex*>(other),
                     TNamed(other)
{
   // Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("FieldData","Copy Constructor");
   #endif
}

//_____________________________________________________________________________
FieldData::~FieldData()
{
   // Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("FieldData","Destructor");
   #endif
   PurgeContainer();
}

//______________________________________________________________________________
void FieldData::PurgeContainer()
{
   // -- Delete all elements in container
   vector<const FieldVertex*>::iterator it;
   for(it = this->begin(); it != this->end(); ++it) {
      delete *it;
      *it = 0;
   }
}
