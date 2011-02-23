#include <iostream>
#include <string>
#include "TBenchmark.h"

#include "ConfigFile.h"
#include "RunConfig.h"
#include "InitialConfig.h"
#include "Run.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;

Int_t main(Int_t argc,Char_t **argv)
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
   if (argc == 2) {
      configFileName= argv[1];
   } else {
      cerr << "Error: No configuration file has been specified." << endl;
      cerr << "Usage, ucnsim <configFile.cfg>" << endl;
      return EXIT_FAILURE;
   }
   ConfigFile configFile(configFileName);
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Fetch the Number of Runs
   ///////////////////////////////////////////////////////////////////////////////////////
   cout << "-------------------------------------------" << endl;
   cout << "Initialising the Runs" << endl;
   cout << "-------------------------------------------" << endl;
   const Int_t numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
   if (numberOfRuns < 1) {
      cerr << "Cannot read valid number of runs from ConfigFile" << endl;
      return EXIT_FAILURE;
   }
   cout << "Number of Runs: " << numberOfRuns << endl; 
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Create the Runs
   ///////////////////////////////////////////////////////////////////////////////////////
   for (Int_t runNumber = 1; runNumber <= numberOfRuns; runNumber++) {
      // Read in the Run Configuration
      RunConfig runConfig(configFile, runNumber);
      // Create the Run
      cout << "-------------------------------------------" << endl;
      cout << "Creating Run" << runNumber << endl;
      Run run(runConfig);
      ///////////////////////////////////////////////////////////////////////////////////////
      // -- Initialise Run
      ///////////////////////////////////////////////////////////////////////////////////////
      if (!(run.Initialise())) {
         cerr << "Run" << runNumber << " failed to initialise successfully." << endl;
         return EXIT_FAILURE;
      }
      ///////////////////////////////////////////////////////////////////////////////////////
      // -- Run Simulation
      ///////////////////////////////////////////////////////////////////////////////////////
      if (!(run.Start())) {
         cerr << "Run" << runNumber << " failed to proceed successfully." << endl;
         return EXIT_FAILURE;
      }
      ///////////////////////////////////////////////////////////////////////////////////////
      // -- Export to File
      ///////////////////////////////////////////////////////////////////////////////////////
      if (!(run.Finish())) {
         cerr << "Run" << runNumber << " failed to finish successfully." << endl;
         return EXIT_FAILURE;
      }
      cout << "Run" << runNumber << " successfully completed." << endl;
      cout << "-------------------------------------------" << endl;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Output up benchmark
   benchmark.Stop("UCNSIM");
   cout << "-------------------------------------------" << endl;
   benchmark.Print("UCNSIM");
   cout << "-------------------------------------------" << endl;
   return EXIT_SUCCESS;
}
