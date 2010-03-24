#include <iostream>
#include <string>

#include "TUCNConfigFile.h"
#include "TUCNRun.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;

Int_t main(Int_t argc,Char_t **argv)
{
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
      // Create the Run
      Char_t name[20], title[20];
      sprintf(name,"Run%d",runNumber); 
      sprintf(title,"Run no:%d",runNumber);
      cout << "-------------------------------------------" << endl;
      cout << "Creating New Run, Name: " << name << " Title: " << title << endl;
      TUCNRun run(name, title);
      ///////////////////////////////////////////////////////////////////////////////////////
      // -- Initialise Run
      ///////////////////////////////////////////////////////////////////////////////////////
      if (!(run.Initialise(configFile))) {
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
      if (!(run.Finish(configFile))) {
         cerr << name << " failed to finish successfully. Program aborting." << endl;
         return EXIT_FAILURE;
      }
      cout << name << " successfully completed." << endl;
      cout << "-------------------------------------------" << endl << endl;
   }
   return EXIT_SUCCESS;
}
