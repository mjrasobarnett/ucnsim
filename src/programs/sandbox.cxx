#include <iostream>
#include <string>
#include <memory>

#include "gsl/gsl_poly.h"

#include "ConfigFile.h"
#include "Run.h"
#include "Particle.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TRint.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "UniformMagField.h"
#include "MagFieldArray.h"
#include "Data.h"
#include "FieldMap.h"
#include "FileParser.h"
#include "Tube.h"
#include "Polynomial.h"

#include "Constants.h"
#include "Units.h"
#include "GeomParameters.h"
#include "DataAnalysis.h"

using namespace std;

Int_t calculate_T2(string configFileName, vector<string> stateNames);

//__________________________________________________________________________
Int_t main(Int_t argc,Char_t ** argv)
{
   if (argc < 3) {
      cerr << "Usage:" << endl;
      cerr << "sandbox <data.root> <statename>" << endl;
      return EXIT_FAILURE;
   }
   // Read in Filename and check that it is a .root file
   string configFileName = argv[1];
   // Read in list of states to be included in histogram and check that they are valid state names
   vector<string> stateNames;
   for (int i = 2; i < argc; i++) {stateNames.push_back(argv[i]);}
   if (Analysis::DataFile::IsValidStateName(stateNames) == false) {
      cerr << "Error: statenames supplied are not valid" << endl;
      return EXIT_FAILURE;
   }
//   TRint *theApp = new TRint("FittingApp", NULL, NULL);
   calculate_T2(configFileName, stateNames);
//   theApp->Run();
   return EXIT_SUCCESS;
}

//_____________________________________________________________________________
Int_t calculate_T2(string configFileName, vector<string> stateNames) {

   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ConfigFile configFile(configFileName);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Fetch the Number of Runs
   const int numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
   if (numberOfRuns < 1) {
      cerr << "Cannot read valid number of runs from ConfigFile: " << numberOfRuns << endl;
      return -1;
   }
   // Make T2 graph
   TGraphErrors* graph = new TGraphErrors(numberOfRuns);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Loop over runs specified in ConfigFile
   for (int runNum = 1; runNum <= numberOfRuns; runNum++) {
      // Load Run Config for this run
      RunConfig runConfig(configFile, runNum);
      // Fetch this run's Data file
      const string dataFileName = runConfig.OutputFileName();
      TFile* dataFile = Analysis::DataFile::OpenRootFile(dataFileName, "UPDATE");
      // Calculate T2 for this run
      double t2 = 0., t2error = 0.;
      if (Analysis::Polarisation::CalculateT2(*dataFile, stateNames, t2, t2error) == false) {
         cerr << "Failed to calculate T2 for datafile: " << dataFileName << endl;
         return -1;
      }
      // Add T2 to graph
      cout << "T2: " << t2 << "\t" << "Error: " << t2error << endl;
      graph->SetPoint(runNum-1, runNum, t2);
      graph->SetPointError(runNum-1, 0, t2error);
      // Close File
      dataFile->Close();
      delete dataFile;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // Draw Graph
   TCanvas* canvas = new TCanvas("Phase","Phase",60,0,1200,800);
   canvas->cd();
   graph->SetMarkerStyle(8);
   graph->Draw("AP");
   graph->GetXaxis()->SetTitle("Config Num");
   graph->GetYaxis()->SetTitle("T2 (s)");
   graph->GetYaxis()->SetRangeUser(0.0, 20.0);
   graph->SetTitle("T2");
   // Write canvas to file
   canvas->SaveAs("t2.png");
   // Clean up graph
   delete graph;
   delete canvas;
   return 0;
}
