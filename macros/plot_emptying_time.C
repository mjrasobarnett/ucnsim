
//__________________________________________________________________________
Int_t plot_emptying_time(string filename, string statename) {
   ///////////////////////////////////////////////////////////////////////////////////////
   // Read in Filename and check that it is a .root file
   if (Analysis::DataFile::ValidateRootFile(filename) == false) {
      cerr << "Error: filename, " << filename << " does not have a .root extension" << endl;
      return -1;
   }
   // Read in list of states to be included in histogram and check that they are
   // valid state names
   vector<string> statenames;
   statenames.push_back(statename);
   if (Analysis::DataFile::IsValidStateName(statenames) == false) {
      cerr << "Error: statenames supplied are not valid" << endl;
      return -1;
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
   TFile* file = Analysis::DataFile::OpenRootFile(filename,"UPDATE");
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   const RunConfig& runConfig = Analysis::DataFile::LoadRunConfig(*file);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Navigate to and store folder for each selected state
   vector<TDirectory*> stateDirs;
   if (Analysis::DataFile::FetchStateDirectories(*file, statenames, stateDirs) == false) {
      return -1;
   }
   // -- Plot Emptying Time
   Analysis::FinalStates::PlotEmptyingTime(stateDirs,runConfig,1,60);
   return 0;
}

