#include <iostream>

#include "include/Constants.h"
#include "include/Units.h"

using namespace std;

Double_t ExponentialDecay(Double_t *x, Double_t *par);

Int_t emptyingtimes(/*const char* configFileName, const char* treeName*/) {
   
      const Int_t points = 20;
      Double_t x[points] = {0.0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};
      Double_t y[points] = {14.5322, 12.9504, 11.3106, 10.5013, 9.49659, 9.29704, 9.16154, 8.88218, 8.32997, 8.2368, 8.44589, 8.49352, 8.03124, 7.58258, 8.49701, 8.37223, 8.44916, 8.39309, 8.41251, 8.24851};
      Double_t ex[points] = 0;
      Double_t ey[points] = {0.500321, 0.498883, 0.444557, 0.418585, 0.405315, 0.413178, 0.414535, 0.401859, 0.379879, 0.368537, 0.370997, 0.42074, 0.322411, 0.324185, 0.350183, 0.362389, 0.379329, 0.435951, 0.391378, 0.407638};
      
      TGraph* graph  = new TGraphErrors(points, x, y, 0, ey);
      graph->SetTitle("roughness vs emptying time");
      graph->GetXaxis()->SetTitle("Roughness parameter");
      graph->GetYaxis()->SetTitle("Emptying time, (s)");
      graph->GetXaxis()->SetLimits(-0.1,1.1);
      graph->GetYaxis()->SetRangeUser(0.,18.);
      graph->SetMarkerColor(4);
      graph->SetMarkerStyle(21);
      graph->Draw("AP");
      
      
      const Int_t points2 = 11;
      Double_t x2[points2] = {0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};
      Double_t y2[points2] = {15.66,12.11,12.93,13.95,12.75,14.74,13.81,12.7,13.19,13.73,14.4};
      Double_t ex2[points2] = 0;
      Double_t ey2[points2] = {0.79,0.58,0.55,0.66,0.61,0.67,0.64,0.6,0.61,0.67,0.7};

      TGraph* graph2  = new TGraphErrors(points2, x2, y2, 0, ey2);
      graph2->GetYaxis()->SetRangeUser(0.,18.);
      graph2->SetMarkerColor(21);
      graph2->SetMarkerStyle(21);
      graph2->Draw("SAMEP");
      
  
/*   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   TUCNConfigFile configFile(configFileName);
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Fetch the Number of Runs
   ///////////////////////////////////////////////////////////////////////////////////////
   const Int_t numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
   if (numberOfRuns < 1) {
      cerr << "Cannot read valid number of runs from ConfigFile" << endl;
      return EXIT_FAILURE;
   }
   cout << "Number of Runs: " << numberOfRuns << endl << endl; 
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Create storage for emptying times
   ///////////////////////////////////////////////////////////////////////////////////////
   Double_t emptyingTimes[20];
   Double_t emptyingTimeErrors[20];
   Double_t surfaceRoughness[20] = {0.00, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all the Runs
   ///////////////////////////////////////////////////////////////////////////////////////
   for (Int_t runNumber = 1; runNumber <= numberOfRuns; runNumber++) {
      // Find the run
      Char_t runName[20];
      sprintf(runName,"Run%d",runNumber); 
      // Read the outputfile name
      TString dataFileName = configFile.GetString("OutputDataFile",runName);
      if (dataFileName == "") { 
         cout << "No File holding the particle data has been specified" << endl;
         return kFALSE;
      }
      // -- Open File
      TFile *file = 0;
      file = TFile::Open(dataFileName, "read");
      if (!file || file->IsZombie()) {
         cerr << "Cannot open file: " << dataFileName << endl;
         return 0;
      }
      // -- Extract Run Object
      cout << "Fetching run: " << runName << endl;
      TUCNRun* run = new TUCNRun();
      file->GetObject(static_cast<const char*>(runName), run);
      if (run == NULL) {
         cerr << "Could not find run: " << runName << endl;
         return kFALSE;
      }
      cout << "Successfully Loaded Run: " << runName << endl;
      
      // -- Fetch required Tree
      TTree* tree = run->GetData()->FetchTree(treeName);
      if (tree == NULL) return kFALSE;
      
      // Create Time Histogram
      Int_t runTime = configFile.GetInt("RunTime(s)",runName);
      Int_t fillTime = configFile.GetInt("FillingTime",runName);
      Int_t nbins = 50;
      
      Char_t histname[20];
      sprintf(histname,"%s:Time",tree->GetName());
      TH1F* timeHist = new TH1F(histname,"Time: Units of s", nbins, 0.0, runTime+10);
      timeHist->SetXTitle("Time (s)");
      timeHist->SetYTitle("Neutrons");
      
      // Fill Histograms
      for (Int_t i = 0; i < tree->GetEntriesFast(); i++) {
         TUCNParticle* particle = run->GetData()->GetParticleState(tree, i);
         if (particle == NULL) continue;
         timeHist->Fill(particle->T()/Units::s);
      }
      
      // Fit Final Time to Exponential - determine emptying time
      Char_t funcName[20];
      sprintf(funcName,"%s:Expo",tree->GetName());
      TF1 *expo = new TF1(funcName, ExponentialDecay, fillTime, run->RunTime(), 2);
      expo->SetParameters(100.0,10.0);
      expo->SetParNames("Initial Number","Emptying Time(s)");
      timeHist->Fit(expo, "RN");
      Double_t emptyingTime = expo->GetParameter(1);
      Double_t emptyingTimeError = expo->GetParError(1);
      cout << "EmptyingTime: " << emptyingTime << "\t" << "Error: " << emptyingTimeError << endl;
      
      // Fill emptyingtime arrays for fitting
      emptyingTimes[runNumber] = emptyingTime;
      emptyingTimeErrors[runNumber] = emptyingTimeError;
      
      delete expo;
      delete timeHist;
      delete run;
      delete file;
   }
   
   cout << "Finished fitting emptying times, preparing plot..." << endl;
   
   TCanvas* canvas = new TCanvas("EmptyingTimes","EmptyingTimes vs Roughness",60,0,1000,800);
   canvas->cd();
   
   cout << "Created canvas" << endl;
   
   TGraph* graph  = new TGraphErrors(numberOfRuns, surfaceRoughness, emptyingTimes, 0, emptyingTimeErrors);
   graph->GetXaxis()->SetTitle("Roughness parameter");
   graph->GetYaxis()->SetTitle("Emptying time, (s)");
   graph->GetXaxis()->SetLimits(-0.1,1.1);
//   graph->GetYaxis()->SetRangeUser(0.,18.);
   graph->SetMarkerColor(4);
   graph->SetMarkerStyle(21);
   cout << "Create graph" << endl;
   
   graph->Draw();
   
*/   
   return 0;
}

// -------------------------------------------------------------------------------------- 
Double_t ExponentialDecay(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Exp(-t/par[1]);
   return f;
}

/*

   const Int_t points = 11;
   Double_t x[points] = {0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};
   Double_t y[points] = {15.66,12.11,12.93,13.95,12.75,14.74,13.81,12.7,13.19,13.73,14.4};
   Double_t ex[points] = 0;
   Double_t ey[points] = {0.79,0.58,0.55,0.66,0.61,0.67,0.64,0.6,0.61,0.67,0.7};
   
   TGraph* graph  = new TGraphErrors(points, x, y, 0, ey);
   graph->SetTitle("roughness vs emptying time");
   graph->GetXaxis()->SetTitle("Roughness parameter");
   graph->GetYaxis()->SetTitle("Emptying time, (s)");
   graph->GetXaxis()->SetLimits(-0.1,1.1);
//   graph->GetYaxis()->SetRangeUser(0.,18.);
   graph->SetMarkerColor(4);
   graph->SetMarkerStyle(21);
   graph->Draw("AP");

*/