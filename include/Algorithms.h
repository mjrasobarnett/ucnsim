// Algorithms.h -- Stores some project-wide algorithms that are too general to go into classes


#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include <string>
#include <iostream>

class TDirectory;

namespace Algorithms {
   //_____________________________________________________________________________
   // Namespace holding functions relevant to floating point precision
   //_____________________________________________________________________________
   namespace Precision 
   {
      //_____________________________________________________________________________
      bool IsEqual(double left, double right, double precision = 1.E-10);
      //_____________________________________________________________________________
      bool IsNotEqual(double left, double right, double precision = 1.E-10);
      //_____________________________________________________________________________
      bool IsGreaterOrEqual(double left, double right, double precision = 1.E-10);
      //_____________________________________________________________________________
      bool IsLessOrEqual(double left, double right, double precision = 1.E-10);
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
      bool ValidateStateNames(const std::string statename);
      //_____________________________________________________________________________
      void CountParticles(TDirectory * const particleDir);
   }
}
#endif
