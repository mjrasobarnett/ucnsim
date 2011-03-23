// Algorithms.h -- Stores some project-wide algorithms that are too general to go into classes


#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include <string>

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
   // Namespace holding functions relevant to string transformations
   //_____________________________________________________________________________
   namespace String
   {
      //_____________________________________________________________________________
      bool ConvertToInt(const std::string input, int& output);
      //_____________________________________________________________________________
      bool ConvertToBool(const std::string input, bool& output);
      //_____________________________________________________________________________
      bool ConvertToDouble(const std::string input, double& output);
   }
   //_____________________________________________________________________________
   // Namespace holding functions relevant to underlying filesystem
   //_____________________________________________________________________________
   namespace FileSystem
   {
      //_____________________________________________________________________________
      std::string ExpandShellVar(const std::string var);
      //_____________________________________________________________________________
      std::string ExpandFilePath(const std::string path);
   }
}
#endif
