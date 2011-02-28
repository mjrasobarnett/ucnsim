#include <iostream>
#include <map>
#include <iterator>
#include <string>
#include <cassert>

#include "TH1F.h"
#include "TF1.h"
#include "TRint.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TKey.h"
#include "TMath.h"
#include "TGraph.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TDirectory.h"

#include "Particle.h"
#include "RunConfig.h"
#include "SpinData.h"

#include "Constants.h"
#include "Units.h"
#include "DataFileHierarchy.h"
#include "Algorithms.h"


using namespace std;

void PlotT2(TDirectory* const histDir, const vector<TDirectory*> stateDirs, const RunConfig& runConfig);

Double_t ExponentialDecay(Double_t *x, Double_t *par);

// -------------------------------------------------------------------------------------- 
Int_t main(int argc, char **argv) {
   ///////////////////////////////////////////////////////////////////////////////////////
   // Plot data needs a minimum of 2 arguments - a data file, and the name of the particle
   // 'state' you wish to include in your histograms.
   if (argc < 3) {
      cerr << "Usage:" << endl;
      cerr << "plot_data <data.root> <statename>" << endl;
      return EXIT_FAILURE;
   }
   // Read in Filename and check that it is a .root file
   string filename = argv[1];
   // Read in Filename and check that it is a .root file
   if (Algorithms::DataFile::ValidateRootFile(filename) == false) {
      cerr << "Error: filename, " << filename << " does not have a .root extension" << endl;
      return EXIT_FAILURE;
   }
   // Read in list of states to be included in histogram and check that they are
   // valid state names
   vector<string> statenames;
   for (int i = 2; i < argc; i++) {statenames.push_back(argv[i]);}
   if (Algorithms::DataFile::ValidateStateNames(statenames) == false) {
      cerr << "Error: statenames supplied are not valid" << endl;
      return EXIT_FAILURE;
   }
   cout << "-------------------------------------------" << endl;
   cout << "Loading Data File: " << filename << endl;
   cout << endl;
   // Start an interactive root session so we can view the plots as they are made
   TRint *theApp = new TRint("FittingApp", &argc, argv);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
   //////////////////////////////////////////////////////////////////////////////////////
   TFile *file = 0;
   file = TFile::Open(filename.c_str(), "UPDATE");
   if (!file || file->IsZombie()) {
      cerr << "Cannot open file: " << filename << endl;
      return -1;
   }
   file->cd();
   TDirectory * const topDir = gDirectory;
   if (topDir->cd(Folders::particles.c_str()) == false) {
      cerr << "No Folder named: " << Folders::particles << " in data file" << endl;
      return EXIT_FAILURE;
   }
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Loaded Data File: " << filename << endl;
   cout << "-------------------------------------------" << endl;
   TDirectory * const particleDir = gDirectory;
   Algorithms::DataFile::CountParticles(particleDir);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   // Navigate to Config Folder   
   if (topDir->cd(Folders::config.c_str()) == false) {
      cerr << "No Folder named: " << Folders::config << " in data file" << endl;
      return EXIT_FAILURE;
   }
   // -- Loop over all objects in folder and extract latest RunConfig
   RunConfig* ptr_config = NULL;
   TKey *configKey;
   TIter configIter(gDirectory->GetListOfKeys());
   while ((configKey = dynamic_cast<TKey*>(configIter.Next()))) {
      const char *classname = configKey->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("RunConfig")) {
         ptr_config = dynamic_cast<RunConfig*>(configKey->ReadObj());
         break;
      }
   }
   const RunConfig& runConfig = *ptr_config;
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Loaded RunConfig" << endl;
   cout << "-------------------------------------------" << endl;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Create a Histogram Dir inside File
   TDirectory* histDir = topDir->mkdir(Folders::histograms.c_str());
   if (histDir == NULL) {
      // histogram folder already exists
      topDir->cd(Folders::histograms.c_str());
      histDir = gDirectory;
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Navigate to and store folder for each selected state
   //////////////////////////////////////////////////////////////////////////////////////
   particleDir->cd();
   vector<TDirectory*> stateDirs;
   vector<string>::const_iterator stateIter;
   for (stateIter = statenames.begin(); stateIter != statenames.end(); stateIter++) {
      // Try to cd into state folder
      if (particleDir->cd((*stateIter).c_str()) == kFALSE) {
         cout << "Error: State, " << *stateIter << " is not found in file" << endl;
         return EXIT_FAILURE;
      }
      // Store pointer to state folder
      stateDirs.push_back(gDirectory);
   }
   cout << "-------------------------------------------" << endl;
   cout << "Loading Particles from the Following states: ";
   copy(statenames.begin(),statenames.end(),ostream_iterator<string>(cout,", "));
   cout << endl;
   cout << "-------------------------------------------" << endl;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Polarisation
   PlotT2(histDir, stateDirs, runConfig);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Clean up and Finish
   file->Close();
   cout << "Finished" << endl;
   theApp->Run();
   return EXIT_SUCCESS;
}

//_____________________________________________________________________________
void PlotT2(TDirectory* const histDir, const vector<TDirectory*> stateDirs, const RunConfig& runConfig)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   const Double_t runTime = runConfig.RunTime();
   const Double_t spinMeasFreq = runConfig.SpinMeasurementFreq();
   const Int_t nbins = runTime/spinMeasFreq;
   const TVector3 xAxis(1.0,0.0,0.0);
   const TVector3 yAxis(0.0,1.0,0.0);
   const TVector3 zAxis(0.0,0.0,1.0);
   // -- Define name of combined states
   string stateName;
   vector<TDirectory*>::const_iterator dirIter;
   for (dirIter = stateDirs.begin(); dirIter != stateDirs.end(); dirIter++) {
      if (stateName.empty() == false) stateName += "/";
      stateName += (*dirIter)->GetName();
   }
   //////////////////////////////////////////////////////////////////////////////////////
   TH1F time_data("T2 Time Data","T2 Time Data", nbins, 0.0, runTime);
   vector<vector<double> > phase_data;
   TGraph* alphaT2 = new TGraph(nbins);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over each state to be included in histogram
   for (dirIter = stateDirs.begin(); dirIter != stateDirs.end(); dirIter++) {
      // -- cd into the State's folder
      (*dirIter)->cd();
      //////////////////////////////////////////////////////////////////////////////////////
      // -- Loop over all particle folders in the current state's folder
      TKey *folderKey;
      TIter folderIter((*dirIter)->GetListOfKeys());
      while ((folderKey = dynamic_cast<TKey*>(folderIter.Next()))) {
         const char *classname = folderKey->GetClassName();
         TClass *cl = gROOT->GetClass(classname);
         if (!cl) continue;
         if (cl->InheritsFrom("TDirectory")) {
            // Loop over all objects in particle dir
            (*dirIter)->cd(folderKey->GetName());
            TDirectory* particleDir = gDirectory;
            TKey *objKey;
            TIter objIter(particleDir->GetListOfKeys());
            while ((objKey = static_cast<TKey*>(objIter.Next()))) {
               // For Each object in the particle's directory, check its class name and what it
               // inherits from to determine what to do.
               classname = objKey->GetClassName();
               cl = gROOT->GetClass(classname);
               if (!cl) continue;
               if (cl->InheritsFrom("SpinData")) {
                  // -- Extract Spin Observer Data if recorded
                  const SpinData* data = dynamic_cast<const SpinData*>(objKey->ReadObj());
                  // Create storage for this particle's phase information
                  vector<double> phases;
                  // Loop over spin data recorded for particle
                  SpinData::const_iterator dataIter;
                  for (dataIter = data->begin(); dataIter != data->end(); dataIter++) {
                     // Bin the time in the histogram 
                     time_data.Fill(dataIter->first);
                     // -- For a holding Field aligned along the X-Axis, we want to find the
                     // -- phase of the spin in the Y-Z plane. 
                     const Spin* spin = dataIter->second;
                     // Calculate probability of spin up along Y axis
                     Double_t ycoord = spin->CalculateProbSpinUp(yAxis);
                     // Calculate probability of spin up along Z axis
                     Double_t zcoord = spin->CalculateProbSpinUp(zAxis);
                     // Calculate theta (the phase)
                     Double_t theta = TMath::ATan2((2.0*zcoord - 1.0), (2.0*ycoord - 1.0));
                     // Add each phase to list
                     phases.push_back(theta);
                  }
                  // Store particle's phases
                  phase_data.push_back(phases);
                  delete data;
               }
            }
         }
      }
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd back into Histogram's dir
   histDir->cd();
   TCanvas *alphaT2canvas = new TCanvas("Alpha T2","Alpha T2",60,0,1200,800);
   alphaT2canvas->cd();
   // Calculate T2 polarisation
   for (int timeIndex = 0; timeIndex < time_data.GetNbinsX(); timeIndex++) {
      // Calculate mean phase for each time
      Double_t sumPhases = 0.;
      for (UInt_t particleIndex  = 0; particleIndex < phase_data.size(); particleIndex++) {
         assert(phase_data[particleIndex].size() == (size_t)time_data.GetNbinsX());
         sumPhases += phase_data[particleIndex][timeIndex];
      }
      Double_t meanPhase = sumPhases/phase_data.size();
      // Calculate the number spin up and down based on phase difference
      Int_t numSpinUp = 0, numSpinDown = 0;
      for (unsigned int particleIndex  = 0; particleIndex < phase_data.size(); particleIndex++) {
         // Calculate the angle between each particle's phase and the mean phase
         Double_t phasediff = TMath::Abs(phase_data[particleIndex][timeIndex] - meanPhase);
         Double_t probSpinDown = TMath::Cos(phasediff);
         if (gRandom->Uniform(0.,1.0) < probSpinDown) {
            numSpinDown++;
         } else {
            numSpinUp++;
         }
      }
      // Calculate polarisation at this time
      Double_t alpha = TMath::Abs(((double)(numSpinUp - numSpinDown)) / ((double)(numSpinUp + numSpinDown)));
      Double_t timebin = time_data.GetBinLowEdge(timeIndex);
      // Add point to graph
      alphaT2->SetPoint(timeIndex, timebin, alpha);
   }
   // Draw graph
   alphaT2->SetMarkerStyle(7);
   alphaT2->Draw("AP");
   alphaT2->GetXaxis()->SetTitle("Time (s)");
   alphaT2->GetXaxis()->SetRangeUser(0.0,runTime);
   alphaT2->GetYaxis()->SetTitle("Alpha");
   alphaT2->GetYaxis()->SetRangeUser(-1.0,1.0);
   alphaT2->SetTitle("T2 Polarisation");
   alphaT2->Write(alphaT2->GetName(),TObject::kOverwrite);
   return;
}

//_____________________________________________________________________________
Double_t ExponentialDecay(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Exp(-t/par[1]);
   return f;
}

