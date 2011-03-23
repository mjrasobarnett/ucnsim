// DataAnalysis.h -- Defines some project-wide analysis algorithms useful for macros and other programs

#ifndef DATAANALYSIS_H
#define DATAANALYSIS_H

#include <vector>
#include <string>

class TDirectory;
class TFile;
class RunConfig;
class TGeoManager;

namespace Analysis {
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace DataFile
   {
      //_____________________________________________________________________________
      TFile* OpenRootFile(const std::string filename, const std::string option = "UPDATE");
      //_____________________________________________________________________________
      const RunConfig& LoadRunConfig(TFile& file);
      //_____________________________________________________________________________
      TGeoManager& LoadGeometry(TFile& file);
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
