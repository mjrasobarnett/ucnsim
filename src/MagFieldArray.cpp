// MagFieldArray.cxx

#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Point.hpp"
#include "MagFieldArray.hpp"

//#define VERBOSE_MODE

using namespace std;

ClassImp(MagFieldArray)

//_____________________________________________________________________________
MagFieldArray::MagFieldArray()
              :FieldArray("MagFieldArray")
{
  // Default constructor
  #ifdef PRINT_CONSTRUCTORS
    Info("MagFieldArray", "Default Constructor");
  #endif
}

//_____________________________________________________________________________
MagFieldArray::MagFieldArray(const MagFieldArray& other)
              :FieldArray(other)
{
   //copy constructor
  #ifdef PRINT_CONSTRUCTORS
    Info("MagFieldArray", "Copy Constructor");
  #endif
}

//_____________________________________________________________________________
MagFieldArray::~MagFieldArray()
{
  // Destructor
  #ifdef PRINT_CONSTRUCTORS
    Info("MagFieldArray", "Destructor");
  #endif
}

//_____________________________________________________________________________
const TVector3 MagFieldArray::GetMagField(const Point& point, const TVector3& /*velocity*/, const std::string volume) const
{
   return this->GetField(point,volume);
}
