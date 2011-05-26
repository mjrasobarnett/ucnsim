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
#include "TClass.h"

#include "Particle.h"
#include "RunConfig.h"
#include "SpinData.h"

#include "Constants.h"
#include "Units.h"
#include "ValidStates.h"
#include "Algorithms.h"
#include "DataAnalysis.h"

using namespace std;
using namespace States;
using namespace Algorithms;

void PlotDensitySnapshots(TFile* const file, const vector<TDirectory*> stateDirs, const RunConfig& runConfig);

// -------------------------------------------------------------------------------------- 
Int_t main(int argc, char **argv) {
   ///////////////////////////////////////////////////////////////////////////////////////
   if (argc < 2) {
      cerr << "Usage:" << endl;
      cerr << "make_densityplot <data.root>" << endl;
      return EXIT_FAILURE;
   }
   // Read in Filename and check that it is a .root file
   string filename = argv[1];
   // Read in Filename and check that it is a .root file
   if (Analysis::DataFile::IsRootFile(filename) == false) {
      cerr << "Error: filename, " << filename << " does not have a .root extension" << endl;
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
   if (topDir->cd("") == false) {
      cerr << "No Folder named: " << "" << " in data file" << endl;
      return EXIT_FAILURE;
   }
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Loaded Data File: " << filename << endl;
   cout << "-------------------------------------------" << endl;
   TDirectory * const particleDir = gDirectory;
   Analysis::DataFile::CountParticles(particleDir);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   // Navigate to Config Folder   
   if (topDir->cd("") == false) {
      cerr << "No Folder named: " << "" << " in data file" << endl;
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
   if (runConfig.TrackMeasureInterval() == 0.0) {
      cout << "Error: No Track measurement frequency was set for this run." << endl;
      return EXIT_FAILURE;
   }
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Loaded RunConfig" << endl;
   cout << "-------------------------------------------" << endl;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Create a Histogram Dir inside File
   TDirectory* histDir = topDir->mkdir("");
   if (histDir == NULL) {
      // histogram folder already exists
      topDir->cd("");
      histDir = gDirectory;
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // Create a list of all the valid states to be included in histogram
   vector<string> statenames;
   statenames.push_back(States::propagating);
   statenames.push_back(States::absorbed);
   statenames.push_back(States::detected);
   statenames.push_back(States::decayed);
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
   PlotDensitySnapshots(file, stateDirs, runConfig);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Clean up and Finish
   cout << "Finished" << endl;
   theApp->Run();
   return EXIT_SUCCESS;
}

//_____________________________________________________________________________
void PlotDensitySnapshots(TFile* const file, const vector<TDirectory*> stateDirs, const RunConfig& runConfig)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   const Double_t runTime = runConfig.RunTime();
   const Double_t trackMeasInterval = runConfig.TrackMeasureInterval();
   const Int_t intervals = 1 + runTime/trackMeasInterval; 
   const Double_t minCoord = 0.0;
   const Double_t maxCoord = 3.0;
   const Int_t nbins = 300;
   Char_t histname[40];
   sprintf(histname,"Total:Storage_Time");
   TGraph* storageHist = new TGraph(intervals);
   storageHist->SetName(histname);

   vector<TH1F*> histograms;
   for (int intervalNum = 0; intervalNum < intervals; intervalNum++) {
      sprintf(histname,"Total:Density_Along_Source:%03i",intervalNum);
      TH1F* nextHistogram = new TH1F(histname, "Density Along Source", nbins, minCoord, maxCoord);
      nextHistogram->SetXTitle("Distance from Entrance Window (m)");
      nextHistogram->SetYTitle("Neutrons");
      nextHistogram->GetYaxis()->SetRangeUser(0,100);
      nextHistogram->SetLineColor(kBlack);
      nextHistogram->SetFillStyle(3001);
      nextHistogram->SetFillColor(kBlack);
      histograms.push_back(nextHistogram);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   
   // -- Loop over each state to be included in histogram
   vector<TDirectory*>::const_iterator dirIter;
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
               if (cl->InheritsFrom("Track")) {
                  // -- Extract Spin Observer Data if recorded
                  const Track* track = dynamic_cast<const Track*>(objKey->ReadObj());
                  //cout << particleDir->GetName() << "  ---------------------  ";
                  //cout << track->TotalPoints() << endl; 
                  int totalPoints = track->TotalPoints();
                  assert(totalPoints <= intervals);
                  //cout << track->GetPoint(totalPoints-1).T() << "\t" << t << endl;
                  for (unsigned int pointNum = 0; pointNum < track->TotalPoints(); pointNum++) {
                     const Point& point = track->GetPoint(pointNum);
                     const double t = pointNum*trackMeasInterval;
                     //cout << particleDir->GetName() << "\t Point.T: " << point.T();
                     //cout << "\t t: " << t << endl;
                     if (Precision::IsEqual(point.T(),t) == false) {
                        cout << "Error: " << point.T() << "\t" << t << endl;
                     }
                     histograms[pointNum]->Fill(point.Y());
                  }
                  delete track;
               }
            }
         }
      }
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd back into Histogram's dir
   assert(histograms.size() == intervals);
   cout << "Total Histograms: " << histograms.size() << endl;
   vector<TH1F*>::const_iterator histIter;
   for (histIter = histograms.begin(); histIter != histograms.end(); histIter++) {
      cout << (*histIter)->GetName() << "\t" << (*histIter)->GetEntries() << endl; 
      storageHist->SetPoint(histIter-histograms.begin(), histIter-histograms.begin(), ((*histIter)->GetEntries()));
      TCanvas *canvas = new TCanvas((*histIter)->GetName(),"Storage Time",60,0,1200,800);
      canvas->cd();
      (*histIter)->Draw();
      string filepath = "images/";
      string filename = filepath + (*histIter)->GetName();
      filename += ".png";
      canvas->Print(filename.c_str());
      delete canvas;
   }
   TCanvas *storageCanvas = new TCanvas("Storage Time","Storage Time",60,0,1200,800);
   storageCanvas->cd();
   storageHist->SetMarkerStyle(7);
   storageHist->Draw("AP");
   storageHist->GetXaxis()->SetTitle("Time (s)");
   //storageHist->GetXaxis()->SetRangeUser(0.0,runTime);
   storageHist->GetYaxis()->SetTitle("Neutrons");
   //storageHist->GetYaxis()->SetRangeUser(-1.0,1.0);
   storageHist->SetTitle("Storage time");
   storageCanvas->Print("images/storageTime.png");
   return;
}
