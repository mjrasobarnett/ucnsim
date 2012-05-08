//_____________________________________________________________________________
bool calculate_T1(const string filename, const string state) {
   // Load Shared Library
   gSystem->Load("libUCN.so");
   // Read in Filename and check that it is a .root file
   if (Analysis::DataFile::IsRootFile(filename) == false) {return false;}
   // Read in list of states to be included in histogram and check that they are
   // valid state names
   if (Analysis::DataFile::IsValidStateName(state) == false) {return false;}
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
   TFile* file = Analysis::DataFile::OpenRootFile(filename,"UPDATE");
   if (file == NULL) return EXIT_FAILURE;
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   const RunConfig& runConfig = Analysis::DataFile::LoadRunConfig(*file);
   if (runConfig.ObserveSpin() == false) return false;
   //////////////////////////////////////////////////////////////////////////////////////
   // Load the Particle Manifest
   const ParticleManifest& manifest = Analysis::DataFile::LoadParticleManifest(*file);
   manifest.Print();
   //////////////////////////////////////////////////////////////////////////////////////
   // Load the Data Tree
   TTree* dataTree = Analysis::DataFile::LoadParticleDataTree(*file);
   //////////////////////////////////////////////////////////////////////////////////////
   // Get a list of all particle tree indexes for the chosen states
   vector<int> particleIndexes = manifest.GetListing(state).GetTreeIndexes();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Polarisation
   TVector3 axis(0.,0.,1.);
   TGraph* spinPolarisation = Analysis::Polarisation::PlotSpinPolarisationAlongAxis(state, particleIndexes, dataTree, runConfig, axis);
   // Draw Plot of Spin polarisation
   const float lower_range = 0.0;
   const float upper_range = runConfig.RunTime();   
   TCanvas *spincanvas = new TCanvas("SpinAlongX","Spin Polarisation Along X",60,0,1200,800);
   spincanvas->cd();
   Char_t histname[100];
   sprintf(histname,"%s:Polarisation Along Axis (%.1f,%.1f,%.1f)",state.c_str(),axis.X(),axis.Y(),axis.Z());
   const double runTime = runConfig.RunTime();
   spinPolarisation->SetName(histname);
   spinPolarisation->SetMarkerStyle(7);
   spinPolarisation->Draw("AP");
   spinPolarisation->GetXaxis()->SetTitle("Time (s)");
   spinPolarisation->GetXaxis()->SetRangeUser(0.0,runTime);
   spinPolarisation->GetYaxis()->SetTitle("Alpha");
   spinPolarisation->GetYaxis()->SetRangeUser(-1.0,1.0);
   spinPolarisation->SetTitle(histname);
   spinPolarisation->Draw();

   // Fit to exponential to calculate T1
   const float fit_lower_lim = 0.0;
   const float fit_upper_lim = 40.0;

   const float init_guess_amplitude = particleIndexes.size();
   const float init_guess_t1 = 10.0;

   TF1 *fitfunc = new TF1("T1 Fit Func", Analysis::FitFunctions::ExponentialDecay, fit_lower_lim, fit_upper_lim, 2);
   fitfunc->SetParNames("Amplitude","T1 (s)");
   fitfunc->SetParameters(init_guess_amplitude,init_guess_t1);
   spinPolarisation->Fit(fitfunc, "R 0");
   fitfunc->Draw("SAME");
   double t1 = fitfunc->GetParameter(0);
   double amplitude = fitfunc->GetParameter(1);
   double t1_err = fitfunc->GetParError(0);
   double amplitude_err = fitfunc->GetParError(1);
   cout << "------------------------------------" << endl;
   cout << "T1: " << t1 << "\t" << "Err: " << t1_err << endl;
   cout << "Amplitude: " << amplitude << "\t" << "Err: " << amplitude_err << endl;
   cout << "------------------------------------" << endl;
   return true;
}
