// DataAnalysis.h -- Defines some project-wide analysis algorithms useful for macros and other programs

#ifndef DATAANALYSIS_H
#define DATAANALYSIS_H

#include <vector>
#include <string>

class TDirectory;
class TFile;
class TGeoManager;

class RunConfig;
class Track;

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
   
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace Spins {
      //_____________________________________________________________________________
      void PlotSpinPolarisation(TDirectory* const histDir, const std::vector<TDirectory*> stateDirs, const RunConfig& runConfig);
      //_____________________________________________________________________________
      void PlotField(TDirectory* const histDir, const std::vector<TDirectory*> stateDirs, const RunConfig& runConfig);
      //_____________________________________________________________________________
      double SpinPrecession(double *x, double *par);
      
   }
   
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace Bounces {
      //_____________________________________________________________________________
      void PlotBounceCounters(TDirectory* const histDir, const std::vector<TDirectory*> stateDirs);
      
   }
   
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace FinalStates {
      //_____________________________________________________________________________
      void PlotFinalStates(TDirectory* const histDir, const std::vector<TDirectory*> stateDirs, const RunConfig& runConfig, TGeoManager& geoManager);
   }
   
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace Tracks {
      //_____________________________________________________________________________
      void PlotParticleHistories(TDirectory* const histDir, const std::vector<TDirectory*> stateDirs, TGeoManager& geoManager);
      //_____________________________________________________________________________
      std::vector<double> CalculateParticleHistory(const Track& track, TGeoManager& geoManager);
      
   }

}

#endif /* DATAANALYSIS */
