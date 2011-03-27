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
   // Navigate to Particle States Folder
   TDirectory * const topDir = gDirectory;
   if (topDir->cd(Folders::particles.c_str()) == false) {
      cerr << "No Folder named: " << Folders::particles << " in data file" << endl;
      return EXIT_FAILURE;
   }
   TDirectory * const particleDir = gDirectory;
   Analysis::DataFile::CountParticles(particleDir);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   const RunConfig& runConfig = Analysis::DataFile::LoadRunConfig(*file);
   ///////////////////////////////////////////////////////////////////////////////////////
   TGeoManager& geoManager = Analysis::DataFile::LoadGeometry(*file);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Create a Histogram Director if one doesn't already exist in File
   TDirectory* histDir = topDir->mkdir(Folders::histograms.c_str());
   if (histDir == NULL) {
      // histogram folder already exists
      topDir->cd(Folders::histograms.c_str());
      histDir = gDirectory;
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Navigate to and store folder for each selected state
   //////////////////////////////////////////////////////////////////////////////////////
   particleDir->cd();
   vector<TDirectory*> stateDirs;
   vector<string>::const_iterator stateIter;
   for (stateIter = statenames.begin(); stateIter != statenames.end(); stateIter++) {
      // Try to cd into state folder
      if (particleDir->cd((*stateIter).c_str()) == kFALSE) {
         cout << "Error: State, " << *stateIter << " is not found in file" << endl;
         return EXIT_FAILURE;
      }
      // Store pointer to state folder
      stateDirs.push_back(gDirectory);
   }
   cout << "-------------------------------------------" << endl;
   cout << "Loading Particles from the Following states: ";
   copy(statenames.begin(),statenames.end(),ostream_iterator<string>(cout,", "));
   cout << endl;
   cout << "-------------------------------------------" << endl;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Particle final state
   Analysis::FinalStates::PlotFinalStates(histDir, stateDirs, runConfig, geoManager);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Polarisation
   if (runConfig.ObserveSpin() == kTRUE) {
      Analysis::Spins::PlotSpinPolarisation(histDir, stateDirs, runConfig);
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
      Analysis::Spins::PlotField(histDir, stateDirs, runConfig);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Clean up and Finish
   file->ls();
   cout << "Finished" << endl;
   theApp->Run();
   return EXIT_SUCCESS;
}

