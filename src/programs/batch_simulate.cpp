#include <iostream>
#include <string>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

#include "ConfigFile.hpp"
#include "Algorithms.hpp"

using namespace std;

//__________________________________________________________________________
Int_t main(Int_t argc, Char_t **argv)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- batch_process takes a config file and submits each run inside it to the feynman cluster
   // -- as an individual job. It will only work if launched from the feynman cluster itself
   ///////////////////////////////////////////////////////////////////////////////////////
   // Fetch the configfile name and queue name from commandline args
   string configFileName, queueName;
   if (argc == 3) {
      configFileName = argv[1];
      queueName = argv[2];
   } else {
      cerr << "Usage, batch_process <configFile.cfg> <queue name>" << endl;
      return EXIT_FAILURE;
   }
   // Check formatting of queue name
   boost::to_lower(queueName);
   if (queueName != "epp_short" && queueName != "epp_general" && queueName != "epp_long") {
      cerr << "Queue name must be either: epp_short, epp_general, epp_long" << endl;
      return EXIT_FAILURE;
   }
   // Load config file
   ConfigFile configFile(configFileName);
   // Get number of runs from ConfigFile
   const int numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
   if (numberOfRuns < 1) {
      cerr << "Cannot read valid number of runs from ConfigFile: " << numberOfRuns << endl;
      return EXIT_FAILURE;
   }
   cout << "-------------------------------------------" << endl;
   cout << "Beginning Batch Job on Feynman" << endl;
   cout << "ConfigFile: " << configFileName << endl;
   cout << "Number of Runs: " << numberOfRuns << endl;
   // Loop over all runs in the config file
   for (int runNum = 1; runNum <= numberOfRuns; runNum++) {
      // Get the current run name and check that a section for this run exists in the config file
      ostringstream runName, jobName;
      runName << "Run" << runNum;
      boost::posix_time::ptime today(boost::posix_time::second_clock::local_time());
      jobName << "job_" << today.date().year() << "_" << today.date().month() << "_" << today.date().day() << "_";
      jobName << today.time_of_day().hours() << today.time_of_day().minutes() << "_run" << runNum;
      map<string,string> section = configFile.GetSection(runName.str());
      if (section.empty()) {
         cerr << "Error: Could not find Section - " << runName.str() << endl;
         return EXIT_FAILURE;
      }
      // Format a string that will specify the qsub command we wish to issue on feynman
      // that will submit a job
      string script = Algorithms::FileSystem::ExpandFilePath("$UCN_DIR");
      script += "/scripts/submit_job.sh";
      ostringstream command;
      command << "qsub -j y "; // Merge error stream into output stream 
      command << "-q " << queueName << ".q ";
      command << "-N " << jobName.str() << " ";
      command << script << " ";
      command << configFileName << " ";
      command << runNum;
      // Submit qsub command to enter job into queue
      cout << "Executing command: " << command.str() << endl;
      int errorcode = system(command.str().c_str());
      cout << "The value returned was: " << (errorcode % 256) << endl;
   }
   cout << "-------------------------------------------" << endl;
   return EXIT_SUCCESS;
}

