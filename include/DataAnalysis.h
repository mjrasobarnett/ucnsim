// DataAnalysis.h -- Defines some project-wide analysis algorithms useful for macros and other programs

#ifndef DATAANALYSIS_H
#define DATAANALYSIS_H

#include <vector>
#include <string>

class TDirectory;
class TFile;
class TGeoManager;
class TGraph;

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
      bool IsValidStateName(const std::vector<std::string>& statenames);
      bool IsValidStateName(const std::string statename);
      //_____________________________________________________________________________
      void CountParticles(TDirectory * const particleDir);
      //_____________________________________________________________________________
      bool FetchStateDirectories(TFile& file, std::vector<std::string>& stateNames, std::vector<TDirectory*>& stateDirs);
      //_____________________________________________________________________________
      std::string ConcatenateStateNames(std::vector<TDirectory*>& stateDirs);
      //_____________________________________________________________________________
      TDirectory* NavigateToHistDir(TFile& file);
   }
   
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace FitFunctions {
      //_____________________________________________________________________________
      double SpinPrecession(double *x, double *par);
      //_____________________________________________________________________________
      double ExponentialDecay(double *x, double *par);
   }
   
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace Polarisation {
      //_____________________________________________________________________________
      typedef struct {
         double fCosTheta;
         double fSinTheta;
         double fTheta;
      } Coords;
      //_____________________________________________________________________________
      void PlotSpinPolarisation(TDirectory* const histDir, const std::vector<TDirectory*> stateDirs, const RunConfig& runConfig);
      //_____________________________________________________________________________
      void PlotField(TDirectory* const histDir, const std::vector<TDirectory*> stateDirs, const RunConfig& runConfig);
      //_____________________________________________________________________________
      void PlotT2(TDirectory* const histDir, const std::vector<TDirectory*> stateDirs, const RunConfig& runConfig);
      //_____________________________________________________________________________
      bool CalculateT2(TFile& dataFile, std::vector<std::string> states, double& t2, double& t2error);
      //_____________________________________________________________________________
      TGraph* CreateAlphaGraph(std::vector<TDirectory*> stateDirs, double runTime, unsigned int intervals);
      //_____________________________________________________________________________
      //void PlotPhaseAngleSnapShots(std::vector<std::vector<Coords> >& phase_data, const unsigned int intervals);
      //_____________________________________________________________________________
      bool PlotT2_vs_Runs(const std::string configFileName, const std::string statename);
      
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
