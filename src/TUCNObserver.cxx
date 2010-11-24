// UCN Observer class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <sstream>
#include <cassert>

#include "TUCNObserver.h"

#include "TUCNParticle.h"
#include "TUCNRunConfig.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TF1.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TUCNData.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNObserver                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

using namespace std;

ClassImp(TUCNObserver)

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNSpinObserver                                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNSpinObserver)

//_____________________________________________________________________________
TUCNSpinObserver::TUCNSpinObserver()
                 :TUCNObserver(),
                  fParticleData()
{
   // Constructor
   Info("TUCNSpinObserver","Default Constructor");
}

//_____________________________________________________________________________
TUCNSpinObserver::TUCNSpinObserver(const TUCNRunConfig& runConfig)
                 :TUCNObserver()
{
   // Constructor
   Info("TUCNSpinObserver","Constructor");
   fMeasAxis = runConfig.PolarisationAxis();
}

//_____________________________________________________________________________
TUCNSpinObserver::TUCNSpinObserver(const TUCNSpinObserver& other)
                 :TUCNObserver(other),
                  fParticleData(other.fParticleData),
                  fMeasAxis(other.fMeasAxis)
{
   // Copy Constructor
   Info("TUCNSpinObserver","Copy Constructor");
}

//_____________________________________________________________________________
TUCNSpinObserver& TUCNSpinObserver::operator=(const TUCNSpinObserver& other)
{
   // Assignment
   Info("TUCNSpinObserver","Assignment");
   if(this!=&other) {
      TUCNObserver::operator=(other);
      PurgeContainer();
      fParticleData = other.fParticleData;
      fMeasAxis = other.fMeasAxis;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNSpinObserver::~TUCNSpinObserver()
{
   // Destructor
   Info("TUCNSpinObserver","Destructor");
   cout << "No. of Particles Stored: " << fParticleData.size() << endl;
   PurgeContainer();
   cout << "After Purge: " << fParticleData.size() << endl;
   
}

//_____________________________________________________________________________
void TUCNSpinObserver::RegisterInterest(TUCNParticle& particle)
{
   // Create storage for this particle's data
   fParticleData.insert(pair<Int_t,TUCNSpinObservables*>(particle.Id(), new TUCNSpinObservables()));
   // Register as an observer with the particle
   particle.Attach(this);
}

//_____________________________________________________________________________
void TUCNSpinObserver::RecordEvent(const TUCNParticle& particle)
{
   // -- Record the current polarisation
   map<Int_t, TUCNSpinObservables*>::iterator entry = fParticleData.find(particle.Id());
   TUCNSpinObservables* spinobservable = (entry->second);
   spinobservable->insert(pair<Double_t, Bool_t>(particle.T(), particle.IsSpinUp(fMeasAxis)));
}

//_____________________________________________________________________________
void TUCNSpinObserver::PurgeContainer()
{
   // -- Clean up all Observables in container
   if (fParticleData.empty() == kFALSE) {
      map<Int_t,TUCNSpinObservables*>::iterator it;
      for(it = fParticleData.begin(); it != fParticleData.end(); ++it) {
         if (it->second != NULL) delete (it->second);
         it->second = NULL;
         fParticleData.erase(it);
      }
   }
}

// -------------------------------------------------------------------------------------- 
Double_t SpinPrecession(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Sin((par[1]*t + par[2]*(TMath::Pi()/2.0)))*TMath::Sin((par[1]*t + par[2]*(TMath::Pi()/2.0)));
   return f;
}

//_____________________________________________________________________________
void TUCNSpinObserver::Plot(TUCNData* data, TTree* tree)
{
   TH1F* spinUpHist = new TH1F("SpinUpHist","SpinUpHist", 500, 0.0, 20.0);      
   spinUpHist->SetXTitle("Time (s)");
   spinUpHist->SetYTitle("Spin Up Neutrons");
   spinUpHist->SetTitle("Volume: HV-Ramsey-Cell. BField: 0.01uT along z-axis. UCN initially polarised Spin Up along X-Axis.");
   //spinUpHist->SetLineColor(kBlue);
   spinUpHist->SetFillStyle(1001);
   spinUpHist->SetFillColor(kBlue-7);
   
   TH1F* spinDownHist = new TH1F("SpinDownHist","", 500, 0.0, 20.0);      
   spinDownHist->SetXTitle("Time (s)");
   spinDownHist->SetYTitle("Spin Down Neutrons");
   spinDownHist->SetTitle("Volume: HV-Ramsey-Cell. BField: 0.01uT along z-axis. UCN initially polarised Spin Up along X-Axis.");
   //spinDownHist->SetLineColor(kRed);
   spinDownHist->SetFillStyle(3001);
   spinDownHist->SetFillColor(kRed-7);
   
   // Fill Histograms
   for (Int_t i = 0; i < tree->GetEntriesFast(); i++) {
      TUCNParticle* particle = data->GetParticleState(tree, i);
      if (particle == NULL) continue;
      // First look through the data for an entry matching the particle's ID number 
      map<Int_t, TUCNSpinObservables*>::iterator partIter = fParticleData.find(particle->Id());
      if (partIter != fParticleData.end()) {
         // If particle is found, walk through the datapoints 
         TUCNSpinObservables* datapoints = partIter->second;
         map<Double_t, Bool_t>::iterator dataIter;
         for (dataIter = datapoints->begin(); dataIter != datapoints->end(); dataIter++) {
            if (dataIter->second == kTRUE) {
               // If spin up, bin the time
               spinUpHist->Fill(dataIter->first);
            } else {
               // If spin down, bin the time
               spinDownHist->Fill(dataIter->first);
            }
         }
      }
   }
   
   TCanvas *downCanvas = new TCanvas("Spin Down","Spin Down Polarisation",60,0,1000,800);
   TF1 *downprecession = new TF1("DownSpinPrecessionFunc", SpinPrecession, 0.0, 20.0, 3);
   downprecession->SetParameters(150.0,1.0,0.0);
   downprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
   spinDownHist->Fit(downprecession, "R");
   Double_t downamplitude = downprecession->GetParameter(0);
   Double_t downomega = downprecession->GetParameter(1);
   Double_t downphase = downprecession->GetParameter(2);
   cout << "Amplitude: " << downamplitude << "\t" << "Omega: " << downomega << "\t" << "Phase: " << downphase << endl;
   
   spinDownHist->GetYaxis()->SetRangeUser(0.0,200.0);
   TPaveText *downinfo = new TPaveText(0.15,0.71,0.6,0.79,"brNDC");
   downinfo->SetFillColor(0);
   downinfo->SetTextAlign(12);
   downinfo->SetTextFont(62);
   // -- Using string streams to write parameter values into text
   ostringstream down_omega;
   down_omega << "Expected Omega: 29.16 Hz/uT * 0.01uT * Pi = 0.916";
   downinfo->AddText(down_omega.str().c_str());
   downinfo->Draw();
   
   
   //
   TCanvas *upCanvas = new TCanvas("Spin Up","Spin Up Polarisation",60,0,1000,800);
   TF1 *upprecession = new TF1("UpSpinPrecessionFunc", SpinPrecession, 0.0, 20.0, 3);
   upprecession->SetParameters(150.0,1.0,0.0);
   upprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
   spinUpHist->Fit(upprecession, "R");
   Double_t upamplitude = upprecession->GetParameter(0);
   Double_t upomega = upprecession->GetParameter(1);
   Double_t upphase = upprecession->GetParameter(2);
   cout << "Amplitude: " << upamplitude << "\t" << "Omega: " << upomega << "\t" << "Phase: " << upphase << endl;
   
   spinUpHist->GetYaxis()->SetRangeUser(0.0,200.0);
   downinfo->Draw();
   
   
   TCanvas *combCanvas = new TCanvas("Spin","Spin Polarisation",60,0,1000,800);
   spinUpHist->Draw();
   spinDownHist->Draw("SAME");
   TLegend* leg = new TLegend(0.7,0.91,0.9,0.99); //coordinates are fractions of pad dimensions
   leg->SetFillColor(0);
   leg->AddEntry(spinUpHist,"Spin Up"); // l means line, p shows marker
   leg->AddEntry(spinDownHist,"Spin Down");
   leg->Draw();
   
}


