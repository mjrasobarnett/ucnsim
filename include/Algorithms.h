// Algorithms.h -- Stores some project-wide algorithms that are too general to go into classes


#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include <string>
#include <iostream>
#include <cmath>

class TDirectory;

namespace Algorithms {
   //_____________________________________________________________________________
   // Namespace holding functions relevant to floating point precision
   //_____________________________________________________________________________
   namespace Precision 
   {
      //_____________________________________________________________________________
      inline bool IsEqual(double left, double right, double precision = 1.E-10) {
         return abs(left - right) < precision ? true : false;
      }
      //_____________________________________________________________________________
      inline bool IsNotEqual(double left, double right, double precision = 1.E-10) {
         return abs(left - right) < precision ? false : true;
      }
      //_____________________________________________________________________________
      inline bool IsGreaterOrEqual(double left, double right, double precision = 1.E-10) {
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
      inline bool IsLessOrEqual(double left, double right, double precision = 1.E-10) {
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
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace DataFile
   {
      //_____________________________________________________________________________
      bool ValidateRootFile(const std::string filename);
      //_____________________________________________________________________________
      bool ValidateStateNames(const std::vector<std::string>& statenames);
      //_____________________________________________________________________________
      void CountParticles(TDirectory * const particleDir);
   }
}
#endif
