#include <iostream>
#include <string>
#include "TBenchmark.h"

#include "ConfigFile.h"
#include "RunConfig.h"
#include "InitialConfig.h"
#include "Run.h"
#include "Algorithms.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;

bool PerformSimulation(const ConfigFile& configFile, const int runNumber);

//__________________________________________________________________________
Int_t main(Int_t argc, Char_t **argv)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Set up benchmark
   TBenchmark benchmark;
   benchmark.SetName("UCNSIM");
   benchmark.Start("UCNSIM");   
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   string configFileName;
   string suppliedRun = "";
   if (argc == 2) {
      configFileName = argv[1];
      suppliedRun = "1";
   } else if (argc == 3) {
      configFileName = argv[1];
      suppliedRun = argv[2];
   } else {
      cerr << "Error: No configuration file has been specified." << endl;
      cerr << "Usage, ucnsim <configFile.cfg> <run no.>" << endl;
      return EXIT_FAILURE;
   }
   cout << "-------------------------------------------" << endl;
   cout << "Opening Batch ConfigFile: " << configFileName << endl;
   ConfigFile configFile(configFileName);
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Fetch the Number of Runs
   ///////////////////////////////////////////////////////////////////////////////////////
   int specifiedRunNumber = 0;
   Algorithms::String::ConvertToInt(suppliedRun, specifiedRunNumber);
   if (specifiedRunNumber == 0) {
      cerr << "Invalid run number supplied." << endl;
      return EXIT_FAILURE;
   }
   cout << "-------------------------------------------" << endl;
   cout << "Initialising Run" << endl;
   cout << "-------------------------------------------" << endl;
   const int numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
   if (numberOfRuns < 1 || numberOfRuns < specifiedRunNumber) {
      cerr << "Cannot read valid number of runs from ConfigFile: " << numberOfRuns << endl;
      return EXIT_FAILURE;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Create the Runs
   if (PerformSimulation(configFile, specifiedRunNumber) == false) {
      return EXIT_FAILURE;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Output up benchmark
   benchmark.Stop("UCNSIM");
   cout << "-------------------------------------------" << endl;
   benchmark.Print("UCNSIM");
   cout << "-------------------------------------------" << endl;
   return EXIT_SUCCESS;
}

//__________________________________________________________________________
bool PerformSimulation(const ConfigFile& configFile, const int runNumber)
{
   // -- Perform a simulation for the given run number
   // Read in the Run Configuration
   cout << "-------------------------------------------" << endl;
   cout << "Reading in Run Configuration " << runNumber << endl;
   RunConfig runConfig(configFile, runNumber);
   // Create the Run
   cout << "Creating Run" << endl;
   Run run(runConfig);
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Initialise Run
   ///////////////////////////////////////////////////////////////////////////////////////
   if (!(run.Initialise())) {
      cerr << "Run" << runNumber << " failed to initialise successfully." << endl;
      return false;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Run Simulation
   ///////////////////////////////////////////////////////////////////////////////////////
   if (!(run.Start())) {
      cerr << "Run" << runNumber << " failed to proceed successfully." << endl;
      return false;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Export to File
   ///////////////////////////////////////////////////////////////////////////////////////
   if (!(run.Finish())) {
      cerr << "Run" << runNumber << " failed to finish successfully." << endl;
      return false;
   }
   cout << "Run" << runNumber << " successfully completed." << endl;
   cout << "-------------------------------------------" << endl;
   return true;
}
