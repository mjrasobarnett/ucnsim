// DataAnalysis.h -- Defines some project-wide analysis algorithms useful for macros and other programs

#ifndef DATAANALYSIS_H
#define DATAANALYSIS_H

#include <vector>
#include <string>

class TDirectory;
class TFile;

namespace Analysis {
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

#endif /* DATAANALYSIS */
