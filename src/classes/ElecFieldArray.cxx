// ElecFieldArray.cxx

#include <iostream>
#include <cassert>
#include <stdexcept>

#include "ElecFieldArray.h"

//#define VERBOSE_MODE

using namespace std;

ClassImp(ElecFieldArray);

//_____________________________________________________________________________
ElecFieldArray::ElecFieldArray()
               :FieldArray()
{
   // Default constructor
   Info("ElecFieldArray", "Default Constructor");
}

//_____________________________________________________________________________
ElecFieldArray::ElecFieldArray(const ElecFieldArray& other)
               :FieldArray(other)
{
   //copy constructor
   Info("ElecFieldArray", "Copy Constructor");
}

//_____________________________________________________________________________
ElecFieldArray::~ElecFieldArray()
{
   // Destructor
   Info("ElecFieldArray", "Destructor");
}

//_____________________________________________________________________________
const TVector3 ElecFieldArray::GetMagField(const Point& /*point*/, const TVector3& velocity, const string /*volume*/) const
{
   return velocity;
}
