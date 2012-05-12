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

#include "Particle.hpp"
#include "RunConfig.hpp"
#include "SpinData.hpp"

#include "Constants.hpp"
#include "Units.hpp"
#include "ValidStates.hpp"
#include "Algorithms.hpp"
#include "DataAnalysis.hpp"
#include "ParticleManifest.hpp"

#include <boost/program_options.hpp>
namespace po = boost::program_options;
using namespace std;

int make_t2_plot(string filename, vector<string> statenames);

//_____________________________________________________________________________
// A helper function to simplify the printing of command line options.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, " ")); 
    return os;
}

//_____________________________________________________________________________
int main(int argc, char **argv)
{
   try {
      // -- Create a description for all command-line options
      po::options_description description("Allowed options");
      description.add_options()
        ("help", "produce help message")
        ("file", po::value<string>(), "filename of the datafile to be plotted")
        ("state", po::value< vector<string> >()->multitoken(), "list of states to be included in the plots");
      ;
      
      // -- Create a description for all command-line options
      po::variables_map variables;
      po::store(po::parse_command_line(argc, argv, description), variables);
      po::notify(variables);
      
      // -- If user requests help, print the options description
      if (variables.count("help")) {
        cout << description << "\n";
        return 1;
      }
      
      // -- Check whether a datafile was given. If not, exit with a warning
      if (variables.count("file")) {
        cout << "DataFile name was set to: "
             << variables["file"].as<string>() << endl;
      } else {
        cout << "DataFile name was not set.\n";
        return EXIT_FAILURE;
      }
      
      // -- Check whether a list of states was given. If not, exit with a warning
      if (variables.count("state")) {
          cout << "There are " << variables["state"].as< vector<string> >().size()
              << " included States which are: " 
              << variables["state"].as< vector<string> >() << "\n";
      } else {
         cout << "No states have been selected.\n";
         return EXIT_FAILURE;
      }
      // -- Call make_plots with the datafile and list of states as arguments
      make_t2_plot(variables["file"].as<string>(), variables["state"].as< vector<string> >());
   } catch(exception& e) {
      cerr << "error: " << e.what() << "\n";
      return 1;
   } catch(...) {
      cerr << "Exception of unknown type!\n";
   }
   return EXIT_SUCCESS;
}

//_____________________________________________________________________________
int make_t2_plot(string filename, vector<string> statenames) {
   // Start an interactive root session so we can view the plots as they are made
   TRint *theApp = new TRint("make_t2_plot", NULL, NULL);
   // Read in Filename and check that it is a .root file
   if (Analysis::DataFile::IsRootFile(filename) == false) {return false;}
   // Read in list of states to be included in histogram and check that they are
   // valid state names
   if (Analysis::DataFile::IsValidStateName(statenames) == false) {return false;}
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
   TFile* file = Analysis::DataFile::OpenRootFile(filename,"UPDATE");
   if (file == NULL) return EXIT_FAILURE;
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   const RunConfig& runConfig = Analysis::DataFile::LoadRunConfig(*file);
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
   // -- Polarisation
   Analysis::Polarisation::CalculateT2(state, particleIndexes, dataTree, runConfig);
   //////////////////////////////////////////////////////////////////////////////////////
   theApp->Run();
   return 0;
}


