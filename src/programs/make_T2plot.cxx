#include <iostream>
#include <iomanip>
#include <map>
#include <iterator>
#include <string>
#include <cassert>
#include <stdexcept>

#include "TH1F.h"
#include "TF1.h"
#include "TRint.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TKey.h"
#include "TMath.h"
#include "TGraph.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TDirectory.h"
#include "TClass.h"

#include "Particle.h"
#include "RunConfig.h"
#include "SpinData.h"

#include "Constants.h"
#include "Units.h"
#include "ValidStates.h"
#include "Algorithms.h"
#include "DataAnalysis.h"

using namespace std;

//_____________________________________________________________________________
Int_t main(int argc, char **argv) {
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
   // Read in Filename and check that it is a .root file
   if (Analysis::DataFile::IsRootFile(filename) == false) {
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
   cout << "-------------------------------------------" << endl;
   cout << "Loading Data File: " << filename << endl;
   cout << endl;
   // Start an interactive root session so we can view the plots as they are made
   TRint *theApp = new TRint("FittingApp", &argc, argv);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
   //////////////////////////////////////////////////////////////////////////////////////
   TFile* file = Analysis::DataFile::OpenRootFile(filename,"UPDATE");
   TDirectory * const topDir = gDirectory;
   if (topDir->cd("") == false) {
      cerr << "No Folder named: " << "" << " in data file" << endl;
      return EXIT_FAILURE;
   }
   TDirectory * const particleDir = gDirectory;
   Analysis::DataFile::CountParticles(particleDir);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   const RunConfig& runConfig = Analysis::DataFile::LoadRunConfig(*file);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Create a Histogram Dir inside File
   TDirectory* histDir = topDir->mkdir("");
   if (histDir == NULL) {
      // histogram folder already exists
      topDir->cd("");
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
   // -- Polarisation
   double t2 = 0;
   double t2error = 0.;
   Analysis::Polarisation::CalculateT2(*file, statenames, t2, t2error);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Clean up and Finish
   cout << "Finished" << endl;
   theApp->Run();
   return EXIT_SUCCESS;
}

