#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>

#include "TGeoManager.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TRint.h"
#include "TCanvas.h"
#include "TPolyMarker3D.h"
#include "TFile.h"
#include "TMath.h"
#include "TLegend.h"
#include "TKey.h"
#include "TPaveText.h"
#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"
#include "TGraph.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TDirectory.h"

#include "Particle.h"
#include "ConfigFile.h"
#include "InitialConfig.h"
#include "RunConfig.h"
#include "SpinData.h"
#include "BounceData.h"
#include "FieldData.h"

#include "Constants.h"
#include "Units.h"
#include "DataFileHierarchy.h"

using namespace std;

void PlotFinalStates(TDirectory* const histDir, const vector<TDirectory*> stateDirs, const RunConfig& runConfig, TGeoManager* geoManager);
void PlotSpinPolarisation(TDirectory* const histDir, const vector<TDirectory*> stateDirs, const RunConfig& runConfig);
void PlotT2(TDirectory* const histDir, const vector<TDirectory*> stateDirs, const RunConfig& runConfig);
void PlotBounceCounters(TDirectory* const histDir, const vector<TDirectory*> stateDirs);
void PlotField(TDirectory* const histDir, const vector<TDirectory*> stateDirs, const RunConfig& runConfig);
void PlotParticleHistories(TDirectory* const histDir, const vector<TDirectory*> stateDirs, TGeoManager* geoManager);
vector<Double_t> CalculateParticleHistory(const Track& track, TGeoManager* geoManager);

Double_t SpinPrecession(Double_t *x, Double_t *par);

void CountParticles(TDirectory * const particleDir);
bool ValidateRootFile(const string filename);
bool ValidateStateNames(const vector<string>& statenames);

//_____________________________________________________________________________
Int_t main(int argc, char **argv)
{
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
   if (ValidateRootFile(filename) == false) {
      cerr << "Error: filename, " << filename << " does not have a .root extension" << endl;
      return EXIT_FAILURE;
   }
   // Read in list of states to be included in histogram and check that they are
   // valid state names
   vector<string> statenames;
   for (int i = 2; i < argc; i++) {statenames.push_back(argv[i]);}
   if (ValidateStateNames(statenames) == false) {
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
   CountParticles(particleDir);
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
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Load the Geometry
   TString geomFileName = runConfig.GeomVisFileName();
   if (geomFileName == "") { 
      cout << "No File holding the geometry can be found" << endl;
      return -1;
   }
   TGeoManager* geoManager = TGeoManager::Import(geomFileName);
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Loaded Geometry" << endl;
   cout << "-------------------------------------------" << endl;
   // -- Write copy of Geometry to data file
   TDirectory* geomDir = topDir->mkdir(Folders::geometry.c_str());
   if (geomDir == NULL) {
      topDir->cd(Folders::geometry.c_str());
      geomDir = gDirectory;
   }
   geomDir->cd();
   geoManager->Write("GeoManager",TObject::kOverwrite);
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
   // -- Particle final state
   PlotFinalStates(histDir, stateDirs, runConfig, geoManager);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Polarisation
   if (runConfig.ObserveSpin() == kTRUE) {
      PlotSpinPolarisation(histDir, stateDirs, runConfig);
      PlotT2(histDir, stateDirs, runConfig);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounce Data
   if (runConfig.ObserveBounces() == kTRUE) {
      PlotBounceCounters(histDir, stateDirs);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Track History
   if (runConfig.ObserveTracks() == kTRUE) {
//      PlotParticleHistories(histDir, stateDirs, geoManager);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Track History
   if (runConfig.ObserveField() == kTRUE) {
      PlotField(histDir, stateDirs, runConfig);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Clean up and Finish
//   file->Close();
   cout << "Finished" << endl;
   theApp->Run();
   return EXIT_SUCCESS;
}

//_____________________________________________________________________________
void PlotFinalStates(TDirectory* const histDir, const vector<TDirectory*> stateDirs, const RunConfig& runConfig, TGeoManager* geoManager)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into the Histogram's directory
   histDir->cd();
   Char_t histname[40];
   // -- Count total Neutrons, and define name of combined states
   Int_t total_neutrons = 0;
   string stateName;
   vector<TDirectory*>::const_iterator dirIter;
   for (dirIter = stateDirs.begin(); dirIter != stateDirs.end(); dirIter++) {
      total_neutrons += (*dirIter)->GetNkeys();
      if (stateName.empty() == false) stateName += "/";
      stateName += (*dirIter)->GetName();
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Final Positions
   TPolyMarker3D* points = new TPolyMarker3D(total_neutrons, 1); // 1 is marker style
   sprintf(histname,"%s:NeutronPositions",stateName.c_str());
   points->SetName(histname);
   points->SetMarkerColor(2);
   points->SetMarkerStyle(6);   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Angular Distribution
   sprintf(histname,"%s:Theta",stateName.c_str());
   TH1F* thetaHist = new TH1F(histname,"Direction: Theta, Degrees", 50, 0., 180.);
   thetaHist->SetXTitle("Degrees");
   thetaHist->SetYTitle("Neutrons");
   thetaHist->SetLineColor(kRed);
   thetaHist->SetFillStyle(3001);
   thetaHist->SetFillColor(kRed);
   sprintf(histname,"%s:Phi",stateName.c_str());
   TH1F* phiHist = new TH1F(histname,"Direction: Phi, Degrees", 50, -180.0, 180.0);
   phiHist->SetXTitle("Degrees");
   phiHist->SetYTitle("Neutrons");
   phiHist->SetLineColor(kRed);
   phiHist->SetFillStyle(3001);
   phiHist->SetFillColor(kRed);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Energy/Momentum
   const Double_t maximumVelocity = 8.0;
   const Int_t nbins = 50;   
   sprintf(histname,"%s:Velocity",stateName.c_str());
   TH1F* energyHist = new TH1F(histname,"Velocity: Units of m/s", nbins, 0.0, maximumVelocity);      
   energyHist->SetXTitle("Velocity (m/s)");
   energyHist->SetYTitle("Neutrons");
   energyHist->SetLineColor(kBlack);
   energyHist->SetFillStyle(3001);
   energyHist->SetFillColor(kBlack);
   sprintf(histname,"%s:Vx",stateName.c_str());
   TH1F* vxHist = new TH1F(histname,"Vx (m/s)", nbins, -maximumVelocity, maximumVelocity);
   vxHist->SetXTitle("Vx (m/s)");
   vxHist->SetYTitle("Neutrons");
   vxHist->SetLineColor(kBlue);
   vxHist->SetFillStyle(3001);
   vxHist->SetFillColor(kBlue);
   sprintf(histname,"%s:Vy",stateName.c_str());
   TH1F* vyHist = new TH1F(histname,"Vy (m/s)", nbins, -maximumVelocity, maximumVelocity);
   vyHist->SetXTitle("Vy (m/s)");
   vyHist->SetYTitle("Neutrons");
   vyHist->SetLineColor(kBlue);
   vyHist->SetFillStyle(3001);
   vyHist->SetFillColor(kBlue);
   sprintf(histname,"%s:Vz",stateName.c_str());
   TH1F* vzHist = new TH1F(histname,"Vz (m/s)", nbins, -maximumVelocity, maximumVelocity);
   vzHist->SetXTitle("Vz (m/s)");
   vzHist->SetYTitle("Neutrons");
   vzHist->SetLineColor(kBlue);
   vzHist->SetFillStyle(3001);
   vzHist->SetFillColor(kBlue);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Run Time
   const Double_t runTime = runConfig.RunTime();
   sprintf(histname,"%s:Time",stateName.c_str());
   TH1F* timeHist = new TH1F(histname,"Time: Units of s", ((int)runTime), 0.0, runTime+1);
   timeHist->SetXTitle("Time (s)");
   timeHist->SetYTitle("Neutrons");
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
               if (cl->InheritsFrom("Particle")) {
                  // -- Extract Final Particle State Data
                  Particle* particle = dynamic_cast<Particle*>(objKey->ReadObj());
                  // -- Fill Histograms
                  points->SetPoint(particle->Id()-1, particle->X(), particle->Y(), particle->Z());
                  thetaHist->Fill((particle->Theta()*180.0)/TMath::Pi());
                  phiHist->Fill((particle->Phi()*180.0)/TMath::Pi());
                  energyHist->Fill(particle->V());
                  vxHist->Fill(particle->Vx());
                  vyHist->Fill(particle->Vy());
                  vzHist->Fill(particle->Vz());
                  timeHist->Fill(particle->T());
                  delete particle;
               }
            }
         }
      }
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd back into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Draw Histograms
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Time Distribution
   TCanvas *timecanvas = new TCanvas("Times","Final Time (s)",60,0,1200,800);
   timecanvas->cd();
   timeHist->Draw();
   timeHist->Write(timeHist->GetName(),TObject::kOverwrite);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Velocity Distribution
   TCanvas *velcanvas = new TCanvas("Velocity","Velocity Space",60,0,1200,800);
   velcanvas->Divide(3,2);
   velcanvas->cd(1);
   energyHist->Draw();
   energyHist->Write(energyHist->GetName(),TObject::kOverwrite);
   velcanvas->Update();
   velcanvas->cd(2);
   thetaHist->Draw();
   thetaHist->Write(thetaHist->GetName(),TObject::kOverwrite);
   velcanvas->cd(3);
   phiHist->Draw();
   phiHist->Write(phiHist->GetName(),TObject::kOverwrite);
   velcanvas->cd(4);
   vxHist->Draw();
   vxHist->Write(vxHist->GetName(),TObject::kOverwrite);
   velcanvas->cd(5);
   vyHist->Draw();
   vyHist->Write(vyHist->GetName(),TObject::kOverwrite);
   velcanvas->cd(6);
   vzHist->Draw();
   vzHist->Write(vzHist->GetName(),TObject::kOverwrite);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Final Positions
   TCanvas *poscanvas = new TCanvas("Positions","Neutron Positions",10,10,50,50);
   poscanvas->cd();
   geoManager->GetTopVolume()->Draw("ogl");
   geoManager->SetVisLevel(4);
   geoManager->SetVisOption(0);
   points->Draw();
   points->Write(points->GetName(),TObject::kOverwrite);
   // -- Get the GLViewer so we can manipulate the camera
   TGLViewer * glViewer = dynamic_cast<TGLViewer*>(gPad->GetViewer3D());
   // -- Select Draw style 
   glViewer->SetStyle(TGLRnrCtx::kFill);
   // -- Set Background colour
   glViewer->SetClearColor(kWhite);
   // -- Set Camera type
   TGLViewer::ECameraType camera = TGLViewer::kCameraPerspXOY;
   glViewer->SetCurrentCamera(camera);
   glViewer->CurrentCamera().SetExternalCenter(kTRUE);
   Double_t cameraCentre[3] = {0,0,0};
   glViewer->SetPerspectiveCamera(camera,4,100,&cameraCentre[0],0,0);
   // -- Draw Reference Point, Axes
   Double_t refPoint[3] = {0.,0.,0.};
   // Int_t axesType = 0(Off), 1(EDGE), 2(ORIGIN), Bool_t axesDepthTest, Bool_t referenceOn, const Double_t referencePos[3]
   glViewer->SetGuideState(0, kFALSE, kFALSE, refPoint);
   glViewer->UpdateScene();
   glViewer = 0;
   return;
}

//_____________________________________________________________________________
void PlotSpinPolarisation(TDirectory* const histDir, const vector<TDirectory*> stateDirs, const RunConfig& runConfig) 
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   Char_t histname[40];
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
   // -- X Axis Spin Polarisation
   sprintf(histname,"%s:SpinUp AlongX",stateName.c_str());
   TH1F* spinUpAlongXHist = new TH1F(histname,"SpinUp AlongX", nbins, 0.0, runTime);      
   spinUpAlongXHist->SetXTitle("Time (s)");
   spinUpAlongXHist->SetYTitle("Spin Up Neutrons");
   spinUpAlongXHist->SetFillStyle(1001);
   spinUpAlongXHist->SetFillColor(kBlue-7);
   spinUpAlongXHist->SetLineColor(kBlue-7);

   sprintf(histname,"%s:SpinDown AlongX",stateName.c_str());
   TH1F* spinDownAlongXHist = new TH1F(histname,"SpinDown AlongX", nbins, 0.0, runTime);      
   spinDownAlongXHist->SetXTitle("Time (s)");
   spinDownAlongXHist->SetYTitle("Spin Down Neutrons");
   spinDownAlongXHist->SetFillStyle(3001);
   spinDownAlongXHist->SetFillColor(kRed-7);
   spinDownAlongXHist->SetLineColor(kRed-7);
   
   sprintf(histname,"%s:SpinUp+Down Along X",stateName.c_str());
   TH1F* spinUpDownAlongXHist  = new TH1F(histname,"SpinUp+Down Along X", nbins, 0.0, runTime);   
   spinUpDownAlongXHist->SetXTitle("Time (s)");
   spinUpDownAlongXHist->SetYTitle("Spin Up+Down Neutrons");
   spinUpDownAlongXHist->SetFillStyle(3001);
   spinUpDownAlongXHist->SetFillColor(kBlack);
   spinUpDownAlongXHist->SetLineColor(kBlack);
   
   sprintf(histname,"%s:Polarisation Along X",stateName.c_str());
   TGraph* spinAlphaX = new TGraph(nbins);
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Y Axis Spin Polarisation
   sprintf(histname,"%s:SpinUp Along Y",stateName.c_str());
   TH1F* spinUpAlongYHist = new TH1F(histname,"SpinUp Along Y", nbins, 0.0, runTime);      
   spinUpAlongYHist->SetXTitle("Time (s)");
   spinUpAlongYHist->SetYTitle("Spin Up Neutrons");
   spinUpAlongYHist->SetLineColor(kBlue-7);
   spinUpAlongYHist->SetFillStyle(1001);
   spinUpAlongYHist->SetFillColor(kBlue-7);

   sprintf(histname,"%s:SpinDown Along Y",stateName.c_str());
   TH1F* spinDownAlongYHist = new TH1F(histname,"SpinDown Along Y", nbins, 0.0, runTime);      
   spinDownAlongYHist->SetXTitle("Time (s)");
   spinDownAlongYHist->SetYTitle("Spin Down Neutrons");
   spinDownAlongYHist->SetLineColor(kRed-7);
   spinDownAlongYHist->SetFillStyle(3001);
   spinDownAlongYHist->SetFillColor(kRed-7);
   
   sprintf(histname,"%s:SpinUp+Down Along Y",stateName.c_str());
   TH1F* spinUpDownAlongYHist  = new TH1F(histname,"SpinUp+Down Along Y", nbins, 0.0, runTime);   
   spinUpDownAlongYHist->SetXTitle("Time (s)");
   spinUpDownAlongYHist->SetYTitle("Spin Up+Down Neutrons");
   spinUpDownAlongYHist->SetLineColor(kBlack);
   spinUpDownAlongYHist->SetFillStyle(3001);
   spinUpDownAlongYHist->SetFillColor(kBlack);
   
   sprintf(histname,"%s:Polarisation Along Y",stateName.c_str());
   TGraph* spinAlphaY = new TGraph(nbins);
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Z Axis Spin Polarisation
   sprintf(histname,"%s:SpinUp Along Z",stateName.c_str());
   TH1F* spinUpAlongZHist = new TH1F(histname,"SpinUp Along Z", nbins, 0.0, runTime);      
   spinUpAlongZHist->SetXTitle("Time (s)");
   spinUpAlongZHist->SetYTitle("Spin Up Neutrons");
   spinUpAlongZHist->SetLineColor(kBlue-7);
   spinUpAlongZHist->SetFillStyle(1001);
   spinUpAlongZHist->SetFillColor(kBlue-7);

   sprintf(histname,"%s:SpinDown Along Z",stateName.c_str());
   TH1F* spinDownAlongZHist = new TH1F(histname,"SpinDown Along Z", nbins, 0.0, runTime);      
   spinDownAlongZHist->SetXTitle("Time (s)");
   spinDownAlongZHist->SetYTitle("Spin Down Neutrons");
   spinDownAlongZHist->SetLineColor(kRed-7);
   spinDownAlongZHist->SetFillStyle(3001);
   spinDownAlongZHist->SetFillColor(kRed-7);
   
   sprintf(histname,"%s:SpinUp+Down Along Z",stateName.c_str());
   TH1F* spinUpDownAlongZHist  = new TH1F(histname,"SpinUp+Down Along Z", nbins, 0.0, runTime);   
   spinUpDownAlongZHist->SetXTitle("Time (s)");
   spinUpDownAlongZHist->SetYTitle("Spin Up+Down Neutrons");
   spinUpDownAlongZHist->SetLineColor(kBlack);
   spinUpDownAlongZHist->SetFillStyle(3001);
   spinUpDownAlongZHist->SetFillColor(kBlack);
   
   sprintf(histname,"%s:Polarisation Along Z",stateName.c_str());
   TGraph* spinAlphaZ = new TGraph(nbins);
   
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
                  // Loop over spin data recorded for particle
                  SpinData::const_iterator dataIter;
                  for (dataIter = data->begin(); dataIter != data->end(); dataIter++) {
                     spinUpDownAlongXHist->Fill(dataIter->first);
                     spinUpDownAlongYHist->Fill(dataIter->first);
                     spinUpDownAlongZHist->Fill(dataIter->first);
                     // For each data point, record the spin polarisation along each axis
                     const Spin* spin = dataIter->second;
                     // Measure polarisation along X
                     if (spin->IsSpinUp(xAxis)) {
                        // If spin up, bin the time
                        if (spinUpAlongXHist) spinUpAlongXHist->Fill(dataIter->first);
                     } else {
                        // If spin down, bin the time
                        if (spinUpAlongXHist) spinDownAlongXHist->Fill(dataIter->first);
                     }
                     // Measure polarisation along Y
                     if (spin->IsSpinUp(yAxis)) {
                        // If spin up, bin the time
                        if (spinUpAlongYHist) spinUpAlongYHist->Fill(dataIter->first);
                     } else {
                        // If spin down, bin the time
                        if (spinUpAlongYHist) spinDownAlongYHist->Fill(dataIter->first);
                     }
                     // Measure polarisation along Y
                     if (spin->IsSpinUp(zAxis)) {
                        // If spin up, bin the time
                        if (spinUpAlongZHist) spinUpAlongZHist->Fill(dataIter->first);
                     } else {
                        // If spin down, bin the time
                        if (spinUpAlongZHist) spinDownAlongZHist->Fill(dataIter->first);
                     }
                  }
                  delete data;
               }
            }
         }
      }
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd back into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Spin Precession Plots
   // -- Down
   TCanvas *spinXcanvas = new TCanvas("SpinAlongX","Spin Polarisation Along X",60,0,1200,800);
   spinXcanvas->Divide(2,2);
   spinXcanvas->cd(1);
   spinUpAlongXHist->Draw();
   spinUpAlongXHist->Write(spinUpAlongXHist->GetName(),TObject::kOverwrite);
   //   spinUpHist->GetYaxis()->SetRangeUser(0.0,200.0);
   //   TF1 *upprecession = new TF1("UpSpinPrecessionFunc", SpinPrecession, 0.0, 20., 3);
   //   upprecession->SetParameters(150.0,1.0,0.0);
   //   upprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
   //   spinUpHist->Fit(upprecession, "RQ");
   //   Double_t upamplitude = upprecession->GetParameter(0);
   //   Double_t upomega = upprecession->GetParameter(1);
   //   Double_t upphase = upprecession->GetParameter(2);
   //   cout << "------------------------------------" << endl;
   //   cout << "Spin Up:" << endl;
   //   cout << "Amplitude: " << upamplitude << "\t" << "Omega: " << upomega;
   //   cout << "\t" << "Phase: " << upphase << endl;
   //   cout << "------------------------------------" << endl;
   spinXcanvas->cd(2);
   spinDownAlongXHist->Draw();
   spinDownAlongXHist->Write(spinDownAlongXHist->GetName(),TObject::kOverwrite);
   //   spinDownHist->GetYaxis()->SetRangeUser(0.0,200.0);
   //   TF1 *downprecession = new TF1("DownSpinPrecessionFunc", SpinPrecession, 0.0, 20., 3);
   //   downprecession->SetParameters(150.0,1.0,0.0);
   //   downprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
   //   spinDownHist->Fit(downprecession, "RQ");
   //   Double_t downamplitude = downprecession->GetParameter(0);
   //   Double_t downomega = downprecession->GetParameter(1);
   //   Double_t downphase = downprecession->GetParameter(2);
   //   cout << "------------------------------------" << endl;
   //   cout << "Spin Down:" << endl;
   //   cout << "Amplitude: " << downamplitude << "\t" << "Omega: " << downomega;
   //   cout << "\t" << "Phase: " << downphase << endl;   
   //   cout << "------------------------------------" << endl; 
   spinXcanvas->cd(3);
   spinUpDownAlongXHist->Draw();
   spinUpDownAlongXHist->Write(spinUpDownAlongXHist->GetName(),TObject::kOverwrite);
   //   TLegend* leg = new TLegend(0.7,0.91,0.9,0.99); //coordinates are fractions of pad dimensions
   //   leg->SetFillColor(0);
   //   leg->AddEntry(spinUpHist,"Spin Up"); // l means line, p shows marker
   //   leg->AddEntry(spinDownHist,"Spin Down");
   //   leg->Draw();
   spinXcanvas->cd(4);
   // Calculate polarisation
   assert(spinUpAlongXHist->GetNbinsX() == nbins);
   for (int i = 0; i < spinUpAlongXHist->GetNbinsX(); i++) {
      Double_t binCentre = spinUpAlongXHist->GetBinLowEdge(i);
      Double_t upCounts = spinUpAlongXHist->GetBinContent(i);
      Double_t downCounts = spinDownAlongXHist->GetBinContent(i);
      Double_t totalCounts = upCounts + downCounts;
      Double_t alpha = totalCounts == 0 ? 0.0 : (upCounts - downCounts) / totalCounts;
//      cout << "Time: " << binCentre << "\t" << "Up: " << upCounts << "\t";
//      cout << "Down: " << downCounts << "\t" << "Alpha: " << alpha << endl;
      spinAlphaX->SetPoint(i, binCentre, alpha);
   }
   spinAlphaX->SetMarkerStyle(7);
   spinAlphaX->Draw("AP");
   spinAlphaX->GetXaxis()->SetTitle("Time (s)");
   spinAlphaX->GetXaxis()->SetRangeUser(0.0,runTime);
   spinAlphaX->GetYaxis()->SetTitle("Alpha");
   spinAlphaX->GetYaxis()->SetRangeUser(-1.0,1.0);
   spinAlphaX->SetTitle("Polarisation along X");
   spinAlphaX->Write(spinAlphaX->GetName(),TObject::kOverwrite);
   
   TCanvas *spinYcanvas = new TCanvas("SpinAlongY","Spin Polarisation Along Y",60,0,1200,800);
   spinYcanvas->Divide(2,2);
   spinYcanvas->cd(1);
   spinUpAlongYHist->Draw();
   spinUpAlongYHist->Write(spinUpAlongYHist->GetName(),TObject::kOverwrite);
   spinYcanvas->cd(2);
   spinDownAlongYHist->Draw();
   spinDownAlongYHist->Write(spinDownAlongYHist->GetName(),TObject::kOverwrite);
   spinYcanvas->cd(3);
   spinUpDownAlongYHist->Draw();
   spinUpDownAlongYHist->Write(spinUpDownAlongYHist->GetName(),TObject::kOverwrite);
   spinYcanvas->cd(4);
   // Calculate polarisation
   assert(spinUpAlongYHist->GetNbinsX() == nbins);
   for (int i = 0; i < spinUpAlongYHist->GetNbinsX(); i++) {
      Double_t binCentre = spinUpAlongYHist->GetBinLowEdge(i);
      Double_t upCounts = spinUpAlongYHist->GetBinContent(i);
      Double_t downCounts = spinDownAlongYHist->GetBinContent(i);
      Double_t totalCounts = upCounts + downCounts;
      Double_t alpha = totalCounts == 0 ? 0.0 : (upCounts - downCounts) / totalCounts;
//      cout << "Time: " << binCentre << "\t" << "Up: " << upCounts << "\t";
//      cout << "Down: " << downCounts << "\t" << "Alpha: " << alpha << endl;
      spinAlphaY->SetPoint(i, binCentre, alpha);
   }
   spinAlphaY->SetMarkerStyle(7);
   spinAlphaY->Draw("AP");
   spinAlphaY->GetXaxis()->SetTitle("Time (s)");
   spinAlphaY->GetXaxis()->SetRangeUser(0.0,runTime);
   spinAlphaY->GetYaxis()->SetTitle("Alpha");
   spinAlphaY->GetYaxis()->SetRangeUser(-1.0,1.0);
   spinAlphaY->SetTitle("Polarisation along Y");
   spinAlphaY->Write(spinAlphaY->GetName(),TObject::kOverwrite);
   
   TCanvas *spinZcanvas = new TCanvas("SpinAlongZ","Spin Polarisation Along Z",60,0,1200,800);
   spinZcanvas->Divide(2,2);
   spinZcanvas->cd(1);
   spinUpAlongZHist->Draw();
   spinUpAlongZHist->Write(spinUpAlongZHist->GetName(),TObject::kOverwrite);
   spinZcanvas->cd(2);
   spinDownAlongZHist->Draw();
   spinDownAlongZHist->Write(spinDownAlongZHist->GetName(),TObject::kOverwrite);
   spinZcanvas->cd(3);
   spinUpDownAlongZHist->Draw();
   spinUpDownAlongZHist->Write(spinUpDownAlongZHist->GetName(),TObject::kOverwrite);
   spinZcanvas->cd(4);
   // Calculate polarisation
   assert(spinUpAlongZHist->GetNbinsX() == nbins);
   for (int i = 0; i < spinUpAlongZHist->GetNbinsX(); i++) {
      Double_t binCentre = spinUpAlongZHist->GetBinLowEdge(i);
      Double_t upCounts = spinUpAlongZHist->GetBinContent(i);
      Double_t downCounts = spinDownAlongZHist->GetBinContent(i);
      Double_t totalCounts = upCounts + downCounts;
      Double_t alpha = totalCounts == 0 ? 0.0 : (upCounts - downCounts) / totalCounts;
//      cout << "Time: " << binCentre << "\t" << "Up: " << upCounts << "\t";
//      cout << "Down: " << downCounts << "\t" << "Alpha: " << alpha << endl;
      spinAlphaZ->SetPoint(i, binCentre, alpha);
   }
   spinAlphaZ->SetMarkerStyle(7);
   spinAlphaZ->Draw("AP");
   spinAlphaZ->GetXaxis()->SetTitle("Time (s)");
   spinAlphaZ->GetXaxis()->SetRangeUser(0.0,runTime);
   spinAlphaZ->GetYaxis()->SetTitle("Alpha");
   spinAlphaZ->GetYaxis()->SetRangeUser(-1.0,1.0);
   spinAlphaZ->SetTitle("Polarisation along Z");
   spinAlphaZ->Write(spinAlphaZ->GetName(),TObject::kOverwrite);
   return;
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
void PlotBounceCounters(TDirectory* const histDir, const vector<TDirectory*> stateDirs)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   Char_t histname[40];
   // -- Define name of combined states
   string stateName;
   vector<TDirectory*>::const_iterator dirIter;
   for (dirIter = stateDirs.begin(); dirIter != stateDirs.end(); dirIter++) {
      if (stateName.empty() == false) stateName += "/";
      stateName += (*dirIter)->GetName();
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounces
   sprintf(histname,"%s:Bounces",stateName.c_str());
   TH1F* bounceHist = new TH1F(histname,"Bounces", 100, 0.0, 100000.0);
   bounceHist->SetXTitle("Bounces");
   bounceHist->SetYTitle("Neutrons");
   bounceHist->SetLineColor(kBlack);
   bounceHist->SetFillStyle(3001);
   bounceHist->SetFillColor(kBlack);
   sprintf(histname,"%s:Specular",stateName.c_str());
   TH1F* specHist = new TH1F(histname,"Specular", 100, 0.0, 100.0);
   specHist->SetXTitle("Percentage");
   specHist->SetYTitle("Neutrons");
   specHist->SetLineColor(kRed);
   specHist->SetFillStyle(3001);
   specHist->SetFillColor(kRed);
   sprintf(histname,"%s:Diffuse",stateName.c_str());
   TH1F* diffHist = new TH1F(histname,"Diffuse", 100, 0.0, 100.0);
   diffHist->SetXTitle("Percentage");
   diffHist->SetYTitle("Neutrons");
   diffHist->SetLineColor(kBlue);
   diffHist->SetFillStyle(3001);
   diffHist->SetFillColor(kBlue);
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
               if (cl->InheritsFrom("BounceData")) {
                  // -- Extract Bounce Observer Data if recorded
                  BounceData* data =dynamic_cast<BounceData*>(objKey->ReadObj());
                  bounceHist->Fill(data->CountTotal());
                  specHist->Fill(data->CountSpecular()*100.0/data->CountTotal());
                  diffHist->Fill(data->CountDiffuse()*100.0/data->CountTotal());
                  delete data;
               }
            }
         }
      }
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd back into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounce Counters
   TCanvas *bouncecanvas = new TCanvas("Bounces","Bounce counters",60,0,1200,800);
   bouncecanvas->Divide(3,1);
   bouncecanvas->cd(1);
   bounceHist->Draw();
   bounceHist->Write(bounceHist->GetName(),TObject::kOverwrite);
   bouncecanvas->cd(2);
   specHist->Draw();
   specHist->Write(specHist->GetName(),TObject::kOverwrite);
   bouncecanvas->cd(3);
   diffHist->Draw();
   specHist->Write(specHist->GetName(),TObject::kOverwrite);
   return;
}

//_____________________________________________________________________________
void PlotField(TDirectory* const histDir, const vector<TDirectory*> stateDirs, const RunConfig& runConfig)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   Char_t histname[40];
   const Double_t runTime = runConfig.RunTime();
   // -- Define name of combined states
   string stateName;
   vector<TDirectory*>::const_iterator dirIter;
   for (dirIter = stateDirs.begin(); dirIter != stateDirs.end(); dirIter++) {
      if (stateName.empty() == false) stateName += "/";
      stateName += (*dirIter)->GetName();
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bx
   sprintf(histname,"%s:Field Bx",stateName.c_str());
   TH2F* bxHist = new TH2F(histname,"Bx", 500, 4.998*Units::uT, 5.002*Units::uT, 500, 0.0, runTime);
   bxHist->SetXTitle("Field measured (T)");
   bxHist->SetYTitle("Time (s)");
   bxHist->SetZTitle("Neutrons");
   // -- By
   sprintf(histname,"%s:Field By",stateName.c_str());
   TH2F* byHist = new TH2F(histname,"By", 500, -5.5*Units::uT, 5.5*Units::uT, 500, 0.0, runTime);
   byHist->SetXTitle("Field measured (T)");
   byHist->SetYTitle("Time (s)");
   byHist->SetZTitle("Neutrons");
   // -- B`
   sprintf(histname,"%s:Field Bz",stateName.c_str());
   TH2F* bzHist = new TH2F(histname,"Bz", 500, -5.5*Units::uT, 5.5*Units::uT, 500, 0.0, runTime);
   bzHist->SetXTitle("Field measured (T)");
   bzHist->SetYTitle("Time (s)");
   bzHist->SetZTitle("Neutrons");
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
               if (cl->InheritsFrom("FieldData")) {
                  // -- Extract Spin Observer Data if recorded
                  const FieldData* fieldData = dynamic_cast<const FieldData*>(objKey->ReadObj());
                  FieldData::const_iterator dataIter;
                  for (dataIter = fieldData->begin(); dataIter != fieldData->end(); dataIter++) {
                     bxHist->Fill((*dataIter)->Bx(),(*dataIter)->T());
                     byHist->Fill((*dataIter)->By(),(*dataIter)->T());
                     bzHist->Fill((*dataIter)->Bz(),(*dataIter)->T());
                  }
                  delete fieldData;
               }
            }
         }
      }
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd back into Histogram's dir
   histDir->cd();
   TCanvas *bxcanvas = new TCanvas("BxField","Bx (T)",60,0,1200,800);
   bxcanvas->cd();
   bxHist->Draw("COLZ");
   bxHist->Write(bxHist->GetName(),TObject::kOverwrite);
   TCanvas *bycanvas = new TCanvas("ByField","By (T)",60,0,1200,800);
   bycanvas->cd();
   byHist->Draw("COLZ");
   byHist->Write(byHist->GetName(),TObject::kOverwrite);
   TCanvas *bzcanvas = new TCanvas("BzFields","Bz (T)",60,0,1200,800);
   bzcanvas->cd();
   bzHist->Draw("COLZ");
   bzHist->Write(bzHist->GetName(),TObject::kOverwrite);
   return;
}

//_____________________________________________________________________________
void PlotParticleHistories(TDirectory* const histDir, const vector<TDirectory*> stateDirs, TGeoManager* geoManager)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   Char_t histname[40];
   // -- Define name of combined states
   string stateName;
   vector<TDirectory*>::const_iterator dirIter;
   for (dirIter = stateDirs.begin(); dirIter != stateDirs.end(); dirIter++) {
      if (stateName.empty() == false) stateName += "/";
      stateName += (*dirIter)->GetName();
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounces
   sprintf(histname,"%s:Source",stateName.c_str());
   TH1F* timeInSourceHist = new TH1F(histname,"Source", 100, 0.0, 1.);
   timeInSourceHist->SetXTitle("Percent Time spent in Source");
   timeInSourceHist->SetYTitle("Percentage of total Neutrons");
   sprintf(histname,"%s:TransferSection",stateName.c_str());
   TH1F* timeInTransferSecHist = new TH1F(histname,"TransferSection", 100, 0.0, 1.);
   timeInTransferSecHist->SetXTitle("Percent Time spent in Transfer Section");
   timeInTransferSecHist->SetYTitle("Percentage of total Neutrons");
   sprintf(histname,"%s:RamseyCell",stateName.c_str());
   TH1F* timeInRamseyCellHist = new TH1F(histname,"RamseyCell", 100, 0.0, 1.);
   timeInRamseyCellHist->SetXTitle("Percent Time spent in Ramsey Cell");
   timeInRamseyCellHist->SetYTitle("Percentage of total Neutrons");
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
               if (cl->InheritsFrom("Track")) {
                  Track* track = dynamic_cast<Track*>(objKey->ReadObj());
                  vector<Double_t> times = CalculateParticleHistory(*track, geoManager);
                  const Point& lastPoint = track->GetPoint(track->TotalPoints());
                  Double_t totalTime = lastPoint.T();
                  delete track;
                  Double_t sourcePercent = times[0]/totalTime;
                  Double_t transferPercent = times[1]/totalTime;
                  Double_t ramseyPercent = times[2]/totalTime;
                  timeInSourceHist->Fill(sourcePercent);
                  timeInTransferSecHist->Fill(transferPercent);
                  timeInRamseyCellHist->Fill(ramseyPercent);
               }
            }
         }
      }
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd back into Histogram's dir
   histDir->cd();
   TCanvas *historycanvas = new TCanvas("History","Histories",60,0,1200,800);
   historycanvas->Divide(3,1);
   historycanvas->cd(1);
//   timeInSourceHist->Scale(1.0/stateDir->GetNkeys());
   timeInSourceHist->SetMaximum(1.0);
   timeInSourceHist->Draw();
   timeInSourceHist->Write(timeInSourceHist->GetName(),TObject::kOverwrite);
   historycanvas->cd(2);
//   timeInTransferSecHist->Scale(1.0/stateDir->GetNkeys());
   timeInTransferSecHist->SetMaximum(1.0);
   timeInTransferSecHist->Draw();
   timeInTransferSecHist->Write(timeInTransferSecHist->GetName(),TObject::kOverwrite);
   historycanvas->cd(3);
//   timeInRamseyCellHist->Scale(1.0/stateDir->GetNkeys());
   timeInRamseyCellHist->SetMaximum(1.0);
   timeInRamseyCellHist->Draw();
   timeInRamseyCellHist->Write(timeInRamseyCellHist->GetName(),TObject::kOverwrite);   
}

//_____________________________________________________________________________
vector<Double_t> CalculateParticleHistory(const Track& track, TGeoManager* geoManager) {
   // -- Walk though provided track and determine at each step which volume the particle is in,
   // -- and use this to calculate the percentage of time the particle spends in each User-defined
   // -- 'region of interest' in the experiment
   const string source = "Source";
   const string transferSection = "TransferSection";
   const string ramseyCell = "RamseyCell";
   
   map<string, string> regionList;
   regionList.insert(pair<string,string>("SourceSeg",source));
   regionList.insert(pair<string,string>("ValveVolEntrance",source));
   regionList.insert(pair<string,string>("ValveVolFront",source));
   regionList.insert(pair<string,string>("ValveVolBack",source));
   regionList.insert(pair<string,string>("BendEntrance",source));
   regionList.insert(pair<string,string>("ValveVol",source));
   
   regionList.insert(pair<string,string>("CircleBend",transferSection));
   regionList.insert(pair<string,string>("BendBox",transferSection));
   regionList.insert(pair<string,string>("BendVol",transferSection));
   regionList.insert(pair<string,string>("DetectorValveVol",transferSection));
   regionList.insert(pair<string,string>("DetectorTubeTop",transferSection));
   regionList.insert(pair<string,string>("DetectorTube",transferSection));
   regionList.insert(pair<string,string>("Detector",transferSection));
   regionList.insert(pair<string,string>("GuideSeg",transferSection));
   regionList.insert(pair<string,string>("PreVolumeBox",transferSection));
   
   regionList.insert(pair<string,string>("NeutralElectrode",ramseyCell));
   regionList.insert(pair<string,string>("NeutralElectrodeHole1",ramseyCell));
   regionList.insert(pair<string,string>("NeutralElectrodeHole2",ramseyCell));
   regionList.insert(pair<string,string>("NeutralElectrodeHole3",ramseyCell));
   regionList.insert(pair<string,string>("NeutralElectrodeHole4",ramseyCell));
   regionList.insert(pair<string,string>("NeutralCell",ramseyCell));
   regionList.insert(pair<string,string>("CellConnector",ramseyCell));
   regionList.insert(pair<string,string>("CentralElectrode",ramseyCell));
   regionList.insert(pair<string,string>("CentralElectrodeHole",ramseyCell));
   regionList.insert(pair<string,string>("HVCell",ramseyCell));
   regionList.insert(pair<string,string>("HVElectrode",ramseyCell));
   
   Double_t previousTime = 0.;
   Double_t nextTime = 0.;
   Double_t timeInSource = 0.;
   Double_t timeInTransferSec = 0.;
   Double_t timeInRamseyCell = 0.;
   
   for (UInt_t pointNum = 0; pointNum < track.TotalPoints(); pointNum++) {
      const Point& point = track.GetPoint(pointNum);
      TGeoNode* node = geoManager->FindNode(point.X(), point.Y(), point.Z());
      TGeoVolume* volume = node->GetVolume();
      map<string, string>::iterator key = regionList.find(volume->GetName());
      if (key == regionList.end()) {
         continue;
      }
      nextTime = point.T();
      Double_t deltaT = nextTime - previousTime;
      if (key->second == source) {
         timeInSource += deltaT;
      } else if (key->second == transferSection) {
         timeInTransferSec += deltaT;
      } else if (key->second == ramseyCell) {
         timeInRamseyCell += deltaT;
      } else {
         cout << "Error:" << key->second << endl;
      }
      previousTime = nextTime;
   }
   vector<Double_t> times;
   times.push_back(timeInSource);
   times.push_back(timeInTransferSec);
   times.push_back(timeInRamseyCell);
   return times;
}

//_____________________________________________________________________________
Double_t SpinPrecession(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Sin((par[1]*t + par[2]*(TMath::Pi()/2.0)))*TMath::Sin((par[1]*t + par[2]*(TMath::Pi()/2.0)));
   return f;
}

//_____________________________________________________________________________
bool ValidateRootFile(const string filename)
{
   // -- Check that the filename supplied has a .root extension
   size_t found = filename.find_last_of(".");
   if (found == string::npos) return false;
   if (filename.substr(found) == ".root") return true;
   return false;
}

//_____________________________________________________________________________
bool ValidateStateNames(const vector<string>& statenames)
{
   // -- Check that each statename in list is a valid state as defined in DataFileHierarchy lvl 3
   // -- and is unique
   vector<string>::const_iterator iter;
   for (iter = statenames.begin(); iter != statenames.end(); iter++) {
      // Check state-name
      if (*iter != Folders::propagating &&
          *iter != Folders::absorbed &&
          *iter != Folders::lost &&
          *iter != Folders::decayed &&
          *iter != Folders::detected &&
          *iter != Folders::anomalous) {
         cerr << "Argument, " << *iter << " is not a valid statename" << endl;
         return false;
      }
      // Check for duplicates
      vector<string>::const_iterator second_iter;
      for (second_iter = iter+1; second_iter != statenames.end(); second_iter++) {
         if (*second_iter == *iter) {
            cerr << "Duplicate statenames given: " << *iter << endl;
            return false;
         }
      }
   }
   return true;
}

//_____________________________________________________________________________
void CountParticles(TDirectory * const particleDir)
{
   // Given the particle state directory, count the number of particles in each state subfolder
   if (particleDir->cd(Folders::initial.c_str()) == false) return;
   Int_t initial = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::propagating.c_str()) == false) return;
   Int_t propagating = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::absorbed.c_str()) == false) return;
   Int_t absorbed = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::detected.c_str()) == false) return;
   Int_t detected = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::decayed.c_str()) == false) return;
   Int_t decayed = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::lost.c_str()) == false) return;
   Int_t lost = gDirectory->GetNkeys();
   if (particleDir->cd(Folders::anomalous.c_str()) == false) return;
   Int_t anomalous = gDirectory->GetNkeys();
   cout << "Initial Particles: " << initial << endl;
   cout << "Total Particles: " << propagating+detected+absorbed+decayed+lost+anomalous << endl;
   cout << "Propagating: " << propagating << endl;
   cout << "Detected: " << detected << endl;
   cout << "Absorbed: " << absorbed << endl;
   cout << "Decayed: " << decayed << endl;
   cout << "Lost: " << lost << endl;
   cout << "Anomalous: " << anomalous << endl;
}

