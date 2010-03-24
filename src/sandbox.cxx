#include <iostream>
#include <string>

#include "TUCNExperiment.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;

Int_t main(Int_t argc,Char_t **argv)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   // Read in ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   string configFileName;
   if (argc == 2) {
      configFileName= argv[1];
   } else {
      cerr << "Usage:" << endl;
      cerr << "sandbox <configFile.cfg>" << endl;
      return -1;
   }
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Initialise Simulation
   ///////////////////////////////////////////////////////////////////////////////////////
   TUCNExperiment* experiment = new TUCNExperiment(configFileName);
   if (!(experiment->Initialise())) {
      cerr << "Failed to initialise the Experiment. Program aborting." << endl;
      return EXIT_FAILURE;
   }
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Run Simulation
   ///////////////////////////////////////////////////////////////////////////////////////
   if (!(experiment->Run())) {
      cerr << "Experiment failed to run. Program aborting." << endl;
      return EXIT_FAILURE;
   }
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Export to File
   ///////////////////////////////////////////////////////////////////////////////////////
   if (!(experiment->Export())) {
      cerr << "Experiment failed to write to File. Program aborting." << endl;
      return EXIT_FAILURE;
   }
   
   cout << endl << endl << "END OF SIMULATION" << endl << endl << endl;
   delete experiment;
   
   return EXIT_SUCCESS;
}
