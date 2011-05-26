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
#include "ParticleManifest.h"

#include "Algorithms.h"
#include "DataAnalysis.h"
#include "ValidStates.h"

using namespace std;

//_____________________________________________________________________________
Int_t main(int argc, char **argv)
{

   // Start an interactive root session so we can view the plots as they are made
   TRint *theApp = new TRint("FittingApp", NULL, NULL);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Plot data needs a minimum of 2 arguments - a data file, and the name of the particle
   // 'state' you wish to include in your histograms.
   if (argc < 3) {
      cerr << "Usage:" << endl;
      cerr << "make_plots <data.root> <statename1> <statename2>..." << endl;
      return EXIT_FAILURE;
   }
   // Read in Filename and check that it is a .root file
   string filename = argv[1];
   if (Analysis::DataFile::IsRootFile(filename) == false) {return EXIT_FAILURE;}
   // Read in list of states to be included in histogram and check that they are
   // valid state names
   vector<string> statenames;
   for (int i = 2; i < argc; i++) {statenames.push_back(argv[i]);}
   if (Analysis::DataFile::IsValidStateName(statenames) == false) {return EXIT_FAILURE;}
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
   TFile* file = Analysis::DataFile::OpenRootFile(filename,"UPDATE");
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   const RunConfig& runConfig = Analysis::DataFile::LoadRunConfig(*file);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Load the Geometry
   TGeoManager& geoManager = Analysis::DataFile::LoadGeometry(*file);
   //////////////////////////////////////////////////////////////////////////////////////
   // Load the Particle Manifest
   const ParticleManifest& manifest = Analysis::DataFile::LoadParticleManifest(*file);
   manifest.Print();
   //////////////////////////////////////////////////////////////////////////////////////
   // Load the Data Tree
   TTree* dataTree = Analysis::DataFile::LoadParticleDataTree(*file);
   //////////////////////////////////////////////////////////////////////////////////////
   // Get a list of all particle tree indexes for the chosen states
   vector<int> particleIndexes = manifest.GetListing(statenames).GetTreeIndexes();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Create a Histogram Director if one doesn't already exist in File
   TDirectory* histDir = Analysis::DataFile::NavigateToHistDir(*file);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Get name of states used in plots
   string state = Analysis::DataFile::ConcatenateStateNames(statenames);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Particle final state
   Analysis::FinalStates::PlotFinalState(state, particleIndexes, dataTree, runConfig);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Particle final state
   double cameraCentre[3] = {0.0,0.0,0.0};
   Analysis::FinalStates::DrawFinalPositions(state, particleIndexes, dataTree, geoManager, cameraCentre);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Polarisation
   if (runConfig.ObserveSpin() == kTRUE) {
      Analysis::Polarisation::PlotSpinPolarisation(state, particleIndexes, dataTree, runConfig);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Field Measured
   if (runConfig.ObserveField() == kTRUE) {
      Analysis::Polarisation::PlotField(state, particleIndexes, dataTree, runConfig);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounce Data
   if (runConfig.ObserveBounces() == kTRUE) {
      Analysis::Bounces::PlotBounceCounters(state, particleIndexes, dataTree);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Clean up and Finish
   cout << "Finished" << endl;
   theApp->Run();
   return EXIT_SUCCESS;
}

