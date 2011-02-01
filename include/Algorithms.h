// Algorithms.h -- Stores some project-wide algorithms that are too general to go into classes


#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <cmath>

// Namespace holding functions relevant to floating point precision
namespace Precision 
{
   //_____________________________________________________________________________
   bool IsEqual(double left, double right, double precision = 1.E-10) {
      return abs(left - right) < precision ? true : false;
   }
   //_____________________________________________________________________________
   bool IsNotEqual(double left, double right, double precision = 1.E-10) {
      return abs(left - right) < precision ? false : true;
   }
   //_____________________________________________________________________________
   bool IsGreaterOrEqual(double left, double right, double precision = 1.E-10) {
      if (abs(left - right) < precision) {
         // Check equality
         return true;
      } else if (left > right) {
         // Check if greater
         return true;
      } else {
         return false;
      }
   }
   //_____________________________________________________________________________
   bool IsLessOrEqual(double left, double right, double precision = 1.E-10) {
      if (abs(left - right) < precision) {
         // Check equality
         return true;
      } else if (left < right) {
         // Check if less than
         return true;
      } else {
         return false;
      }
   }
}
#endif
