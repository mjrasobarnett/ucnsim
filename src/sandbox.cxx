#include <iostream>
#include <string>
#include <memory>

#include <boost/shared_ptr.hpp>

#include "TUCNConfigFile.h"
#include "TUCNRun.h"
#include "TUCNParticle.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TRint.h"

#include "TUCNUniformMagField.h"
#include "TUCNMagFieldManager.h"

#include "Constants.h"
#include "Units.h"

using namespace std;

int TestFieldManager();

// -------------------------------------------------------------------------------------- 
Int_t main(Int_t argc,Char_t **argv)
{
   TestFieldManager();
   
   return EXIT_SUCCESS;
}


// -------------------------------------------------------------------------------------- 
int TestFieldManager()
{
   // -- Simple function tests putting a magfield into MagFieldManager object, and then writing
   // the manager out to file and succesfully reading it back in. We should find that root makes a
   // new copy of the fields stored by the manager when this is read back in from file.
   
   TUCNMagField* magfield1 = new TUCNUniformMagField("1", 0.,0.,0.);
   TUCNMagField* magfield2 = new TUCNUniformMagField("2", 0.,0.,0.);
   
   TUCNMagFieldManager* manager1(new TUCNMagFieldManager());
   manager1->AddField(magfield1);
   manager1->AddField(magfield2);
   
   TFile *f = TFile::Open("test.root","recreate");
   if (!f || f->IsZombie()) {
      Error("Export","Cannot open file");
      return EXIT_FAILURE;
   }
   manager1->Write(manager1->GetName());
   f->ls();
   cout << "-------------------------------------------" << endl;
   // -- Clean up
   delete f;
   f = 0;
   
   
   // -- Open File
   f = TFile::Open("test.root", "read");
   if (!f || f->IsZombie()) {
      Error("Export","Cannot open file");
      return EXIT_FAILURE;
   }
   
   // -- Extract Object
   cout << "Fetching manaer" << endl;
   TUCNMagFieldManager* manager2(new TUCNMagFieldManager());
   f->GetObject("MagFieldManager", manager2);
   if (manager2 == NULL) {
      Error("Export","Cannot extract manager from file");
      return EXIT_FAILURE;
   }
   cout << "Successfully Loaded Manager" << endl;
   
   // Look to see if addresses of fields held are different. If yes, then these objects were copied
   // when read in from file.
   // These methods need to be reimplemented in Manager when testing as I am not committing them.
//   manager1->List(); // List addresses of fields held
//   manager2->List(); // List addresses of fields held
   
   delete manager1;
   manager1 = 0;
   delete manager2;
   manager2 = 0;
   delete f;
   f = 0;
   
   return EXIT_SUCCESS;
}


/* 
// -------------------------------------------------------------------------------------- 
Double_t ExponentialDecay(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Exp(-t/par[1]);
   return f;
}



{
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   string configFileName, treeName;
   if (argc == 3) {
      configFileName= argv[1];
      treeName = argv[2];
   } else {
      cerr << "Error: No configuration file has been specified." << endl;
      cerr << "Usage, ucnsim <configFile.cfg> <treename>" << endl;
      return EXIT_FAILURE;
   }
   TUCNConfigFile configFile(configFileName);
   
   TRint *theApp = new TRint("FittingApp", &argc, argv);
   
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
         particle = 0;
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
      emptyingTimes[runNumber-1] = emptyingTime;
      emptyingTimeErrors[runNumber-1] = emptyingTimeError;
      
      file->ls();
      
      delete expo;
      delete timeHist;
      delete run;
      delete file;
   }
   
   cout << "Finished fitting emptying times, preparing plot..." << endl;
   
   
   TGraphErrors* graph  = new TGraphErrors(numberOfRuns, surfaceRoughness, emptyingTimes, 0, emptyingTimeErrors);
   graph->GetXaxis()->SetTitle("Roughness parameter");
   graph->GetYaxis()->SetTitle("Emptying time, (s)");
   graph->GetXaxis()->SetLimits(-0.1,1.1);
//   graph->GetYaxis()->SetRangeUser(0.,18.);
   graph->SetMarkerColor(4);
   graph->SetMarkerStyle(21);
   cout << "Created graph" << endl;
   graph->Draw("AR");
   
   for (Int_t i = 0; i < 20; i++) {
      cout << emptyingTimes[i] << ", "; 
   }
   cout << endl;
   
   for (Int_t i = 0; i < 20; i++) {
      cout << emptyingTimeErrors[i] << ", "; 
   }
   cout << endl;
   
   for (Int_t i = 0; i < 20; i++) {
      cout << surfaceRoughness[i] << ", "; 
   }
   cout << endl;
      
   Double_t a;
   cout << "Wait for user input" << endl;
   cin >> a;
   
   theApp->Run();
}   
*/


