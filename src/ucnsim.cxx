#include <iostream>
#include <string>
#include "TBenchmark.h"

#include "TUCNConfigFile.h"
#include "TUCNRunConfig.h"
#include "TUCNInitialConfig.h"
#include "TUCNRun.h"

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
   TUCNConfigFile configFile(configFileName);
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
   cout << "Number of Runs: " << numberOfRuns << endl << endl; 
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Create the Runs
   ///////////////////////////////////////////////////////////////////////////////////////
   for (Int_t runNumber = 1; runNumber <= numberOfRuns; runNumber++) {
      // Read in the Run Configuration
      Char_t name[20];
      sprintf(name,"Run%d",runNumber);
      const string runConfigFile = configFile.GetString("Config",name);
      cout << runConfigFile << endl;
      TUCNRunConfig runConfig(runConfigFile);
      // Create the Run
      cout << "-------------------------------------------" << endl;
      cout << "Creating: " << name << endl;
      TUCNRun run;
      
      ///////////////////////////////////////////////////////////////////////////////////////
      // -- Initialise Run
      ///////////////////////////////////////////////////////////////////////////////////////
      if (!(run.Initialise(runConfig))) {
         cerr << name << " failed to initialise successfully. Program aborting." << endl;
         return EXIT_FAILURE;
      }
      ///////////////////////////////////////////////////////////////////////////////////////
      // -- Run Simulation
      ///////////////////////////////////////////////////////////////////////////////////////
      if (!(run.Start())) {
         cerr << name << " failed to proceed successfully. Program aborting." << endl;
         return EXIT_FAILURE;
      }
      ///////////////////////////////////////////////////////////////////////////////////////
      // -- Export to File
      ///////////////////////////////////////////////////////////////////////////////////////
      if (!(run.Finish())) {
         cerr << name << " failed to finish successfully. Program aborting." << endl;
         return EXIT_FAILURE;
      }
      cout << name << " successfully completed." << endl;
      cout << "-------------------------------------------" << endl << endl;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Output up benchmark
   benchmark.Stop("UCNSIM");
   cout << "-------------------------------------------" << endl;
   benchmark.Print("UCNSIM");
   cout << "-------------------------------------------" << endl;
   return EXIT_SUCCESS;
}
