#include <iostream>
#include <map>
#include <iterator>
#include <string>

#include "/Users/Matt/Projects/ucnsim/source/include/DataFileHierarchy.h"
#include "/Users/Matt/Projects/ucnsim/source/include/Constants.h"
#include "/Users/Matt/Projects/ucnsim/source/include/Units.h"

using namespace std;
Double_t ExponentialDecay(Double_t *x, Double_t *par);

// -------------------------------------------------------------------------------------- 
Int_t fit_storage_time(const char* dataFileName) {
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
   TFile *file = 0;
   file = TFile::Open(dataFileName, "READ");
   if (!file || file->IsZombie()) {
      cerr << "Cannot open file: " << dataFileName << endl;
      return -1;
   }
   file->cd();
   TDirectory* const topDir = gDirectory;
   if (topDir->cd(Folders::histograms.c_str()) == false) {
      cerr << "No Folder named: " << Folders::histograms << " in data file" << endl;
      return -1;
   }
   TDirectory* const histDir = gDirectory;
   histDir->ls();
   
   // -- Loop over all objects in histogram folder and extract Time Histogram
   histDir->cd();
   TH1F* histogram = NULL;
   string statename;
   TKey *key;
   TIter keyIter(histDir->GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(keyIter.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TH1")) {
         string histName = key->GetName();
         size_t position = histName.find_last_of(":");
         if (position != string::npos) {
            if (histName.substr(position+1) == "Time") {
               histogram = dynamic_cast<TH1F*>(key->ReadObj());
               statename = histName.substr(0,position);
            }
         }
      }
   }
   if (histogram == NULL) {
      cout << "No Time Histogram found" << endl;
      return -1;
   }
   
   TCanvas *timecanvas = new TCanvas("Times","Final Time (s)",60,0,1200,800);
   timecanvas->cd();
   histogram->Draw();
   // Fit Final Time to Exponential - determine emptying time
   Char_t linename[40];
   sprintf(linename,"%s:Expo",statename.c_str());
   TF1 *expo = new TF1(linename, ExponentialDecay, 0.0, 100.0, 2);
   expo->SetParameters(100.0,10.0);
   expo->SetParNames("Initial Number","Storage Decay Time(s)");
   histogram->Fit(expo, "R");
   Double_t decayConstant = expo->GetParameter(1);
   Double_t decayConstantError = expo->GetParError(1);
   cout << "Storage Decay Time: " << decayConstant << "\t" << "Error: " << decayConstantError << endl;
   return 0;
}

//_____________________________________________________________________________
Double_t ExponentialDecay(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Exp(-t/par[1]);
   return f;
}

