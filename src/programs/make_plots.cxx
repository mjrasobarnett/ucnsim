#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>

#include "TRint.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TGeoManager.h"

#include "RunConfig.h"

#include "Algorithms.h"
#include "DataAnalysis.h"
#include "DataFileHierarchy.h"

using namespace std;

//_____________________________________________________________________________
Int_t main(int argc, char **argv)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   // Plot data needs a minimum of 2 arguments - a data file, and the name of the particle
   // 'state' you wish to include in your histograms.
   if (argc < 3) {
      cerr << "Usage:" << endl;
      cerr << "plot_data <data.root> <statename>" << endl;
      return EXIT_FAILURE;
   }
   // Read in Filename and check that it is a .root file
   string filename = argv[1];
   if (Analysis::DataFile::ValidateRootFile(filename) == false) {
      cerr << "Error: filename, " << filename << " does not have a .root extension" << endl;
      return EXIT_FAILURE;
   }
   // Read in list of states to be included in histogram and check that they are
   // valid state names
   vector<string> statenames;
   for (int i = 2; i < argc; i++) {statenames.push_back(argv[i]);}
   if (Analysis::DataFile::IsValidStateName(statenames) == false) {
      cerr << "Error: statenames supplied are not valid" << endl;
      return EXIT_FAILURE;
   }
   // Start an interactive root session so we can view the plots as they are made
   TRint *theApp = new TRint("FittingApp", &argc, argv);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
   //////////////////////////////////////////////////////////////////////////////////////
   TFile* file = Analysis::DataFile::OpenRootFile(filename,"UPDATE");
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   const RunConfig& runConfig = Analysis::DataFile::LoadRunConfig(*file);
   ///////////////////////////////////////////////////////////////////////////////////////
   TGeoManager& geoManager = Analysis::DataFile::LoadGeometry(*file);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Navigate to and store folder for each selected state
   //////////////////////////////////////////////////////////////////////////////////////
   vector<TDirectory*> stateDirs;
   if (Analysis::DataFile::FetchStateDirectories(*file, statenames, stateDirs) == false) {
      return EXIT_FAILURE;
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Create a Histogram Director if one doesn't already exist in File
   TDirectory* histDir = file->mkdir(Folders::histograms.c_str());
   if (histDir == NULL) {
      // histogram folder already exists
      file->cd(Folders::histograms.c_str());
      histDir = gDirectory;
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Particle final state
   Analysis::FinalStates::PlotFinalStates(histDir, stateDirs, runConfig, geoManager);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Polarisation
   if (runConfig.ObserveSpin() == kTRUE) {
      Analysis::Polarisation::PlotSpinPolarisation(histDir, stateDirs, runConfig);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounce Data
   if (runConfig.ObserveBounces() == kTRUE) {
      Analysis::Bounces::PlotBounceCounters(histDir, stateDirs);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Track History
   if (runConfig.ObserveTracks() == kTRUE) {
//      Analysis::Tracks::PlotParticleHistories(histDir, stateDirs, geoManager);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Track History
   if (runConfig.ObserveField() == kTRUE) {
      Analysis::Polarisation::PlotField(histDir, stateDirs, runConfig);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Clean up and Finish
   cout << "Finished" << endl;
   theApp->Run();
   return EXIT_SUCCESS;
}

