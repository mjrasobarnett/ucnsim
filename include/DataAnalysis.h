// DataAnalysis.h -- Defines some project-wide analysis algorithms useful for macros and other programs

#ifndef DATAANALYSIS_H
#define DATAANALYSIS_H

#include <vector>
#include <string>

class TDirectory;
class TFile;
class TGeoManager;
class TGraph;
class TTree;
class TCanvas;
class TBranch;

class RunConfig;
class Track;
class ParticleManifest;

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
      const ParticleManifest& LoadParticleManifest(TFile& file);
      //_____________________________________________________________________________
      TTree* LoadParticleDataTree(TFile& file);
      //_____________________________________________________________________________
      bool IsRootFile(const std::string filename);
      //_____________________________________________________________________________
      bool IsValidStateName(const std::vector<std::string>& statenames);
      bool IsValidStateName(const std::string statename);
      //_____________________________________________________________________________
      std::string ConcatenateStateNames(const std::vector<std::string>& states);
      //_____________________________________________________________________________
      TDirectory* NavigateToHistDir(TFile& file);
      //_____________________________________________________________________________
      void CopyDirectory(TDirectory* const sourceDir, TDirectory* const outputDir);
      //_____________________________________________________________________________
      void CopyDirectoryContents(TDirectory* const sourceDir, TDirectory* const outputDir);
      //_____________________________________________________________________________
      TBranch* GetParticleBranch(const std::string& state, TTree* dataTree);
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
      void PlotSpinPolarisation(const std::string states, const std::vector<int> particleIndexes, TTree* dataTree, const RunConfig& runConfig);
      //_____________________________________________________________________________
      void PlotField(const std::string state, const std::vector<int> particleIndexes, TTree* dataTree, const RunConfig& runConfig);
      //_____________________________________________________________________________
      bool CalculateT2(TFile& dataFile, std::vector<std::string> states, double& t2, double& t2error);
      //_____________________________________________________________________________
      TGraph* CreateT2AlphaGraph(std::vector<TDirectory*> stateDirs, double runTime, unsigned int intervals);
      //_____________________________________________________________________________
      void PlotPhaseAngleSnapShots(std::vector<std::vector<Analysis::Polarisation::Coords> >& phase_data, const unsigned int intervals);
      //_____________________________________________________________________________
      bool PlotT2_vs_Runs(const std::string configFileName, const std::string statename);
      //_____________________________________________________________________________
      double CalculateMeanPhase(std::vector<std::vector<Analysis::Polarisation::Coords> >& phase_data, const unsigned int intervalNum);
      //_____________________________________________________________________________
      double CalculateAlpha(std::vector<std::vector<Analysis::Polarisation::Coords> >& phase_data, const unsigned int intervalNum, const double meanPhase);
      
   }
   
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace Bounces {
      //_____________________________________________________________________________
      void PlotBounceCounters(const std::string states, const std::vector<int> particleIndexes, TTree* dataTree);
      
   }
   
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace FinalStates {
      //_____________________________________________________________________________
      void PlotFinalState(const std::string states, const std::vector<int> particleIndexes, TTree* dataTree, const RunConfig& runConfig);
      //_____________________________________________________________________________
      void DrawFinalPositions(const std::string state, const std::vector<int> particleIndexes, TTree* dataTree, TGeoManager& geoManager, double* cameraCentre);
      //_____________________________________________________________________________
      bool PlotEmptyingTime(const std::vector<TDirectory*> stateDirs, const RunConfig& runConfig, const double lLimit, const double uLimit);   
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
   
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace Geometry {
      void DrawGeometry(TCanvas& canvas, TGeoManager& geoManager, double* cameraCentre);
   }

}

#endif /* DATAANALYSIS */
