#include <iostream>

#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"
#include "TGeoManager.h"
#include "TH1F.h"
#include "TF1.h"
#include "TRint.h"
#include "TCanvas.h"
#include "TPolyMarker3D.h"
#include "TFile.h"
#include "TMath.h"

#include "TUCNParticle.h"
#include "TUCNConfigFile.h"
#include "TUCNRun.h"
#include "TUCNData.h"

#include "Constants.h"
#include "Units.h"

using namespace std;

Bool_t PlotPositions(TGeoManager* geoManager, TUCNRun* run, const TString& treeName);
Bool_t PlotAngularDistribution(TUCNRun* run, const TString& treeName);
Bool_t PlotPhaseSpace(TUCNRun* run, const TString& treeName); 
Double_t ExponentialDecay(Double_t *x, Double_t *par);

Int_t main(Int_t argc,Char_t **argv)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   TString configFileName, treeName;
   if (argc == 3) {
      configFileName = argv[1];
      treeName = argv[2];
   } else {
      cerr << "Usage:" << endl;
      cerr << "fitdata <configfile.cfg> <treename>" << endl;
      return -1;
   }
   
   // Start 'the app' -- this is so we are able to enter into a ROOT session
   // after the program has run, instead of just quitting.
   TRint *theApp = new TRint("FittingApp", &argc, argv);
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   TUCNConfigFile configFile(configFileName.Data());
   // Check number of runs
   const Int_t numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
   if (numberOfRuns < 1) {
      cerr << "Cannot read valid number of runs from ConfigFile" << endl;
      return EXIT_FAILURE;
   }
   cout << "Number of Runs: " << numberOfRuns << endl << endl;
   if (numberOfRuns > 1) {
      cout << "More than one Run specified. Plot_rundata needs updating to handle this." << endl;
      return EXIT_FAILURE;
   }
   // Read the outputfile name
   Char_t runName[20];
   sprintf(runName,"Run%d",numberOfRuns);
   TString dataFileName = configFile.GetString("OutputDataFile",runName);
   if (dataFileName == "") { 
      cout << "No File holding the particle data has been specified" << endl;
      return kFALSE;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Open File
   TFile *file = 0;
   file = TFile::Open(dataFileName, "update");
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
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Plot angles and momenta
   PlotAngularDistribution(run, treeName);
   PlotPhaseSpace(run, treeName);
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Plot the positions
   TString geomFileName = configFile.GetString("GeomVisFile",runName);
   if (geomFileName == "") { 
      cout << "No File holding the geometry can be found" << endl;
      return kFALSE;
   }
   
   cout << "-------------------------------------------" << endl;
   cout << "Import GeoManager from: " << geomFileName << endl;
   cout << "-------------------------------------------" << endl;
   TGeoManager* geoManager = TGeoManager::Import(geomFileName); 
   // Needs to be imported first because we draw the volumes in certain histograms
   // from it, so we do not want it deleted in each function.
   PlotPositions(geoManager, run, treeName);
   
   cout << "Finished" << endl;
   theApp->Run();
   return 0;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotPositions(TGeoManager* geoManager, TUCNRun* run, const TString& treeName) 
{
// -- Create a TPolyMarker3D object to store the final positions
// -- of the neutrons and write this to file. 
   cout << "-------------------------------------------" << endl;
   cout << "PlotPositions" <<  endl;
   cout << "-------------------------------------------" << endl;
   // -- Fetch Tree
   TTree* tree = run->GetData()->FetchTree(treeName);
   if (tree == NULL) {
      cout << "Failed to Fetch Tree: " << treeName << endl;
      return kFALSE;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Create the points
   Int_t neutrons = tree->GetEntriesFast();
   TPolyMarker3D* points = new TPolyMarker3D(neutrons, 1); // 1 is marker style
   for (Int_t i = 0; i < neutrons; i++) {
      TUCNParticle* particle = run->GetData()->GetParticleState(tree, i);
      if (particle == NULL) {
         cout << "Error" << endl;
         continue;
      }
      points->SetPoint(i, particle->X(), particle->Y(), particle->Z());
   }
   cout << "Created Initial Points: " << points->GetN() << endl;
   points->SetMarkerColor(2);
   points->SetMarkerStyle(6);
   // -- Write the points to the File
   Char_t name[40];
   sprintf(name,"%s:NeutronPositions",tree->GetName());
   points->SetName(name);
   points->Print();
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Clean Up
   TCanvas *canvas = new TCanvas("Positions","Neutron Positions",60,0,800,800);
   canvas->cd();
   geoManager->GetTopVolume()->Draw();
   geoManager->SetVisLevel(4);
   geoManager->SetVisOption(0);
   cout << "Drawing positions" << endl;
   points->Draw();
   return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotAngularDistribution(TUCNRun* run, const TString& treeName) 
{
// -- Create a Histogram object to store the angular distribution 
// -- (as in, their initial and final directions about the origin). 
   cout << "-------------------------------------------" << endl;
   cout << "PlotInitialAndFinalAngularDistribution" <<  endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // Get Tree
   TTree* tree = run->GetData()->FetchTree(treeName);
   if (tree == NULL) return kFALSE;
   // -- Plot the Initial and Final Directions
   Int_t nbins = 50;
   Char_t name[20];
   sprintf(name,"%s:Theta",tree->GetName());
   TH1F* thetaHist = new TH1F(name,"Direction: Theta, Units of Pi", nbins, 0.0, 1.0);
   sprintf(name,"%s:Phi",tree->GetName());
   TH1F* phiHist = new TH1F(name,"Direction: Phi, Units of Pi", nbins, 0.0, 2.0);
   
   for (Int_t i = 0; i < tree->GetEntriesFast(); i++) {
      TUCNParticle* particle = run->GetData()->GetParticleState(tree, i);
      if (particle == NULL) continue;
      thetaHist->Fill(particle->Theta()/TMath::Pi());
      phiHist->Fill(particle->Phi()/TMath::Pi());
   }
   // -- Write the points to the File
   TCanvas *canvas = new TCanvas("Directions","Neutron Directions",60,0,800,800);
   canvas->Divide(2,1);
   canvas->cd(1);
   thetaHist->Draw();
   canvas->cd(2);
   phiHist->Draw();
   return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotPhaseSpace(TUCNRun* run, const TString& treeName) 
{
// -- Create a Histogram object to store the angular distribution (as in, their initial and final directions about the origin). 
   cout << "-------------------------------------------" << endl;
   cout << "PlotInitialAndFinalPhaseSpace" <<  endl;
   cout << "-------------------------------------------" << endl;
   // Get Tree
   TTree* tree = run->GetData()->FetchTree(treeName);
   if (tree == NULL) return kFALSE;
   
   Double_t maximumVelocity = 15.0;
   Double_t maximumMomentum = 20*Units::eV;
   Double_t runTime = 100*Units::s;
   Double_t maximumDistance = 1000*Units::m;
   // -- Set up the histograms
   Int_t nbins = 50;
   Char_t name[20];
   
   sprintf(name,"%s:Velocity",tree->GetName());
   TH1F* energyHist = new TH1F(name,"Velocity: Units of m/s", nbins, 0.0, maximumVelocity);      
   energyHist->SetXTitle("Velocity (m/s)");
   energyHist->SetYTitle("Neutrons");
   
   sprintf(name,"%s:Px",tree->GetName());
   TH1F* pxHist = new TH1F(name,"Px: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   pxHist->SetXTitle("Px (eV)");
   pxHist->SetYTitle("Neutrons");
   
   sprintf(name,"%s:Py",tree->GetName());
   TH1F* pyHist = new TH1F(name,"Py: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   pyHist->SetXTitle("Py (eV)");
   pyHist->SetYTitle("Neutrons");
   
   sprintf(name,"%s:Pz",tree->GetName());
   TH1F* pzHist = new TH1F(name,"Pz: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   pzHist->SetXTitle("Pz (eV)");
   pzHist->SetYTitle("Neutrons");
   
   sprintf(name,"%s:Time",tree->GetName());
   TH1F* timeHist = new TH1F(name,"Time: Units of s", nbins, 0.0, runTime+10);
   timeHist->SetXTitle("Time (s)");
   timeHist->SetYTitle("Neutrons");
   
   sprintf(name,"%s:Dist",tree->GetName());
   TH1F* distHist = new TH1F(name,"Distance: Units of m", nbins, 0.0, maximumDistance);
   distHist->SetXTitle("Distance (m)");
   distHist->SetYTitle("Neutrons");
   // Fill Histograms
   for (Int_t i = 0; i < tree->GetEntriesFast(); i++) {
      TUCNParticle* particle = run->GetData()->GetParticleState(tree, i);
      if (particle == NULL) continue;
      energyHist->Fill(particle->V());
      pxHist->Fill(particle->Px()/Units::eV);
      pyHist->Fill(particle->Py()/Units::eV);
      pzHist->Fill(particle->Pz()/Units::eV);
      timeHist->Fill(particle->T()/Units::s);
      distHist->Fill(particle->Distance()/Units::m);
   }
   
   // -- Draw histograms
   TCanvas *canvas = new TCanvas("PhaseSpace","Phase Space",60,0,1000,800);
   canvas->Divide(3,2);
   canvas->cd(1);
   energyHist->Draw();
   canvas->Update();   
   canvas->cd(2);
   pxHist->Draw();
   canvas->cd(3);
   pyHist->Draw();
   canvas->cd(4);
   pzHist->Draw();
   canvas->cd(5);
   distHist->Draw();
   canvas->cd(6);
   timeHist->Draw();
   
   // Fit Final Time to Exponential - determine emptying time
   sprintf(name,"%s:Expo",tree->GetName());
   TF1 *expo = new TF1(name, ExponentialDecay, 2., run->RunTime(), 2);
   expo->SetParameters(100.0,10.0);
   expo->SetParNames("Initial Number","Emptying Time(s)");
   timeHist->Fit(expo, "R");
   Double_t decayConstant = expo->GetParameter(1);
   Double_t decayConstantError = expo->GetParError(1);
   cout << "DecayConstant: " << decayConstant << "\t" << "Error: " << decayConstantError << endl;
   
   Double_t emptyingTime = 1.0/decayConstant;
   Double_t emptyingTimeError = decayConstantError/(decayConstant*decayConstant);
   
   cout << "EmptyingTime: " << emptyingTime << "\t" << "Error: " << emptyingTimeError << endl;
   
   return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Double_t ExponentialDecay(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Exp(-t/par[1]);
   return f;
}


