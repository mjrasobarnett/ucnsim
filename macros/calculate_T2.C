#include <iostream>
#include <string>

using namespace std;

//_____________________________________________________________________________
Int_t calculate_T2(string configFileName) {

   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ConfigFile configFile(configFileName);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Fetch the Number of Runs
   const int numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
   if (numberOfRuns < 1 || numberOfRuns < specifiedRunNumber) {
      cerr << "Cannot read valid number of runs from ConfigFile: " << numberOfRuns << endl;
      return -1;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // Loop over runs specified in ConfigFile
   for (int runNum = 1; runNum <= numberOfRuns; runNum++) {
      // Load Run Config for this run
      RunConfig runConfig(configFile, runNum);
      // Fetch this run's Data file
      const string dataFileName = runConfig.OutputFileName();
      TFile* dataFile = Analysis::DataFile::OpenRootFile(dataFileName);
      //_____________________________________________________________________________
      // Calculate T2 for this run
      double t2 = 0., t2error = 0.;
      if (CalculateT2(dataFile, t2, t2error) == false) {
         cerr << "Failed to calculate T2 for datafile: " << dataFileName << endl;
         return EXIT_FAILURE;
      }
      // Add T2 to graph
      cout << "T2: " << t2 << "\t" << "Error: " << t2error << endl;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // Draw Graph
   
   return 0;
}