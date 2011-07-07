
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
   // Plot Time histogram
   int bins = 200;
   double runTime = runConfig.RunTime();
   Char_t histname[40];
   sprintf(histname,"%s:Time",statename.c_str());
   TH1F* timeHist = new TH1F(histname,"Time: Units of s", bins, 0.0, 200.0);
   timeHist->SetXTitle("Time (s)");
   timeHist->SetYTitle("Neutrons");
   Analysis::FinalStates::PlotFinalTime(statename, particleIndexes, dataTree, timeHist);
   
   TCanvas *timecanvas = new TCanvas("Times","Final Time (s)",60,0,1200,800);
   timecanvas->cd();
   timeHist->SetMarkerSize(0.8);
   timeHist->SetMarkerStyle(8);
   timeHist->Draw("P E1 X0");
   
   // Fit exponential to Graph
   int numParams = 2;
   double lLimit = 5.0;
   double uLimit = 200.0;
   TF1* expo = new TF1("Exponential", Analysis::FitFunctions::ExponentialDecay, lLimit, uLimit, numParams);
   expo->SetParNames("Amplitude","Decay lifetime");
   expo->SetParameters(10.0,50.0);
   timeHist->Fit(expo, "Q R P L 0");
   expo->SetLineWidth(3);
   expo->SetLineStyle(2); // Dashed
   expo->SetLineColor(kRed);
   expo->Draw("SAME");
   
   // Extract Emptying Time
   double constant = expo->GetParameter(0);
   double constantError = expo->GetParError(0);
   
   double t2 = expo->GetParameter(1);
   double t2error = expo->GetParError(1);
   
   cout << "Constant: " << constant << "\t Error: " << constantError << endl;
   cout << "Decay Lifetime: " << t2 << "\t Error: " << t2error << endl;
   cout << "Chi-Sq: " << expo->GetChisquare() << "\t NDF: " << expo->GetNDF() << endl;
   
   
/*   int numParams = 4;
   double lLimit = 5.0;
   double uLimit = 200.0;
   TF1* expo = new TF1("Exponential", Analysis::FitFunctions::DoubleExponential, lLimit, uLimit, numParams);
   expo->SetParNames("Amplitude ","Decay lifetime 1","Amplitude 2","Decay lifetime 2");
   expo->SetParameters(4000.0,10.0,4000.0,50.0);
   timeHist->Fit(expo, "Q R P L 0");
   expo->SetLineWidth(3);
   expo->SetLineStyle(2); // Dashed
   expo->SetLineColor(kRed);
   expo->Draw("SAME");
   
   // Extract Emptying Time
   double constant_1 = expo->GetParameter(0);
   double constantError_1 = expo->GetParError(0);
   
   double tau_1 = expo->GetParameter(1);
   double tauerror_1 = expo->GetParError(1);
   
   cout << "Constant 1: " << constant_1 << "\t Error: " << constantError_1 << endl;
   cout << "Decay Lifetime 1: " << tau_1 << "\t Error: " << t2error_1 << endl;
   cout << "Constant 2: " << constant_2 << "\t Error: " << constantError_2 << endl;
   cout << "Decay Lifetime 2: " << tau_2 << "\t Error: " << t2error_2 << endl;
   cout << "Chi-Sq: " << expo->GetChisquare() << "\t NDF: " << expo->GetNDF() << endl;
*/   return 0;
}

