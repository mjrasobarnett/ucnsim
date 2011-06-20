
//__________________________________________________________________________
Int_t plot_emptying_time(string filename, string statename) {
   // Read in Filename and check that it is a .root file
   if (Analysis::DataFile::IsRootFile(filename) == false) {return false;}
   // Read in list of states to be included in histogram and check that they are
   // valid state names
   if (Analysis::DataFile::IsValidStateName(statename) == false) {return false;}
   // Open Data File
   TFile* file = Analysis::DataFile::OpenRootFile(filename,"UPDATE");
   if (file == NULL) return EXIT_FAILURE;
   // Build the ConfigFile
   const RunConfig& runConfig = Analysis::DataFile::LoadRunConfig(*file);
   // Load the Particle Manifest
   const ParticleManifest& manifest = Analysis::DataFile::LoadParticleManifest(*file);
   manifest.Print();
   // Load the Data Tree
   TTree* dataTree = Analysis::DataFile::LoadParticleDataTree(*file);
   // Get a list of all particle tree indexes for the chosen states
   vector<int> particleIndexes = manifest.GetListing(statename).GetTreeIndexes();
   // Create a Histogram Director if one doesn't already exist in File
   TDirectory* histDir = Analysis::DataFile::NavigateToHistDir(*file);
   // Plot Emptying Time
   int bins = 100;
   double lLimit = 5.0;
   double uLimit = 200.0;
   Analysis::FinalStates::PlotEmptyingTime(statename, particleIndexes, dataTree, runConfig, bins, lLimit, uLimit);
   
   return 0;
}

