// MagFieldArray.cxx

#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Point.h"
#include "MagFieldArray.h"

//#define VERBOSE_MODE

using namespace std;

ClassImp(MagFieldArray)

//_____________________________________________________________________________
MagFieldArray::MagFieldArray()
              :FieldArray()
{
   // Default constructor
   Info("MagFieldArray", "Default Constructor");
}

//_____________________________________________________________________________
MagFieldArray::MagFieldArray(const MagFieldArray& other)
              :FieldArray(other)
{
   //copy constructor
   Info("MagFieldArray", "Copy Constructor");
}

//_____________________________________________________________________________
MagFieldArray::~MagFieldArray()
{
   // Destructor
   Info("MagFieldArray", "Destructor");
}
