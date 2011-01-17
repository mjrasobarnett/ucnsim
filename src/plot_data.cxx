#include <iostream>
#include <sstream>

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

void PlotFinalStates(TDirectory* const histDir, TDirectory* const stateDir, const InitialConfig& initialConfig, const RunConfig& runConfig, TGeoManager* geoManager);
void PlotSpinPolarisation(TDirectory* const histDir, TDirectory* const stateDir, const RunConfig& runConfig);
void PlotBounceCounters(TDirectory* const histDir, TDirectory* const stateDir, const RunConfig& runConfig);
void PlotField(TDirectory* const histDir, TDirectory* const stateDir, const RunConfig& runConfig);
void PlotParticleHistories(TDirectory* const histDir, TDirectory* const stateDir, const RunConfig& runConfig, TGeoManager* geoManager);
vector<Double_t> CalculateParticleHistory(const Track& track, TGeoManager* geoManager);

Double_t ExponentialDecay(Double_t *x, Double_t *par);
Double_t SpinPrecession(Double_t *x, Double_t *par);

namespace Plot {
   TPolyMarker3D* points = NULL;
   
   TH1F* thetaHist = NULL;
   TH1F* phiHist = NULL;
   
   TH1F* energyHist = NULL;
   TH1F* pxHist = NULL;
   TH1F* pyHist = NULL;
   TH1F* pzHist = NULL;
   
   TH1F* timeHist = NULL;
   TH1F* distHist = NULL;
   
   TH1F* bounceHist = NULL;
   TH1F* specHist = NULL;
   TH1F* diffHist = NULL;
   
   TH1F* spinUpAlongXHist = NULL;
   TH1F* spinDownAlongXHist = NULL;
   TH1F* spinUpDownAlongXHist = NULL;
   TH1F* spinUpAlongYHist = NULL;
   TH1F* spinDownAlongYHist = NULL;
   TH1F* spinUpDownAlongYHist = NULL;
   TH1F* spinUpAlongZHist = NULL;
   TH1F* spinDownAlongZHist = NULL;
   TH1F* spinUpDownAlongZHist = NULL;
   
   TGraph* spinAlphaX = NULL;
   TGraph* spinAlphaY = NULL;
   TGraph* spinAlphaZ = NULL;
   
   TH2F* bxHist = NULL;
   TH2F* byHist = NULL;
   TH2F* bzHist = NULL;
}

Int_t main(Int_t argc,Char_t **argv)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   TString configFileName, statename;
   if (argc == 3) {
      configFileName = argv[1];
      statename = argv[2];
   } else {
      cerr << "Usage:" << endl;
      cerr << "plot_data <configfile.cfg> <treename>" << endl;
      return -1;
   }
   // Start 'the app' -- this is so we are able to enter into a ROOT session
   // after the program has run, instead of just quitting.
   TRint *theApp = new TRint("FittingApp", &argc, argv);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   const char* runName="Run1";   
   ConfigFile configFile(configFileName.Data());
   const string initialConfigFileName = configFile.GetString("Config","Initialisation");
   const string runConfigFile = configFile.GetString("Config",runName);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Read in Initial Configuration from file.
   InitialConfig initialConfig(initialConfigFileName);
   RunConfig runConfig(runConfigFile);
   // Read the outputfile name
   TString dataFileName = runConfig.OutputFileName();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Store the top level directory
   //////////////////////////////////////////////////////////////////////////////////////
   TDirectory * const histDir = gDirectory;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
   //////////////////////////////////////////////////////////////////////////////////////
   TFile *file = 0;
   file = TFile::Open(dataFileName, "update");
   if (!file || file->IsZombie()) {
      cerr << "Cannot open file: " << dataFileName << endl;
      return -1;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Load the Geometry
   TString geomFileName = runConfig.GeomVisFileName();
   if (geomFileName == "") { 
      cout << "No File holding the geometry can be found" << endl;
      return -1;
   }
   TGeoManager* geoManager = TGeoManager::Import(geomFileName);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Navigate to and store folder for selected state
   //////////////////////////////////////////////////////////////////////////////////////
   file->cd();
   gDirectory->cd(Folders::particles.c_str());
   if (statename == Folders::initialstates) {
      gDirectory->cd(statename);
   } else {
      gDirectory->cd(Folders::finalstates.c_str());
      if (gDirectory->cd(statename) == kFALSE) {
         cout << "State name provided is not found in the under /particles/finalstates" << endl;
         return -1;
      }
   }
   TDirectory* const stateDir = gDirectory;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Particle final state
   PlotFinalStates(histDir, stateDir, initialConfig, runConfig, geoManager);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Polarisation
   if (runConfig.ObservePolarisation() == kTRUE) {
      PlotSpinPolarisation(histDir, stateDir, runConfig);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounce Data
   if (runConfig.ObserveBounces() == kTRUE) {
      PlotBounceCounters(histDir, stateDir, runConfig);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Track History
   if (runConfig.ObserveTracks() == kTRUE) {
      PlotParticleHistories(histDir, stateDir, runConfig, geoManager);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Track History
   if (runConfig.ObserveField() == kTRUE) {
      PlotField(histDir, stateDir, runConfig);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Clean up and Finish
   file->Close();
   cout << "Finished" << endl;
   theApp->Run();
   return 0;
}

//_____________________________________________________________________________
void PlotFinalStates(TDirectory* const histDir, TDirectory* const stateDir, const InitialConfig& initialConfig, const RunConfig& runConfig, TGeoManager* geoManager)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into the Histogram's directory
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   Char_t histname[40];
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Final Positions
   const Int_t neutrons = initialConfig.InitialParticles();
   Plot::points = new TPolyMarker3D(neutrons, 1); // 1 is marker style
   sprintf(histname,"%s:NeutronPositions",stateDir->GetName());
   Plot::points->SetName(histname);
   Plot::points->SetMarkerColor(2);
   Plot::points->SetMarkerStyle(6);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Angular Distribution
   sprintf(histname,"%s:Theta",stateDir->GetName());
   Plot::thetaHist = new TH1F(histname,"Direction: Theta, Units of Pi", 50, 0.0, 1.0);
   sprintf(histname,"%s:Phi",stateDir->GetName());
   Plot::phiHist = new TH1F(histname,"Direction: Phi, Units of Pi", 50, -1.0, 1.0);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Energy/Momentum
   const Double_t maximumVelocity = initialConfig.InitialMaxVelocity();
   const Double_t maximumMomentum = 20*Units::eV;
   const Int_t nbins = 50;   
   sprintf(histname,"%s:Velocity",stateDir->GetName());
   Plot::energyHist = new TH1F(histname,"Velocity: Units of m/s", nbins, 0.0, maximumVelocity);      
   Plot::energyHist->SetXTitle("Velocity (m/s)");
   Plot::energyHist->SetYTitle("Neutrons");
   sprintf(histname,"%s:Px",stateDir->GetName());
   Plot::pxHist = new TH1F(histname,"Px: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   Plot::pxHist->SetXTitle("Px (eV)");
   Plot::pxHist->SetYTitle("Neutrons");
   sprintf(histname,"%s:Py",stateDir->GetName());
   Plot::pyHist = new TH1F(histname,"Py: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   Plot::pyHist->SetXTitle("Py (eV)");
   Plot::pyHist->SetYTitle("Neutrons");
   sprintf(histname,"%s:Pz",stateDir->GetName());
   Plot::pzHist = new TH1F(histname,"Pz: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   Plot::pzHist->SetXTitle("Pz (eV)");
   Plot::pzHist->SetYTitle("Neutrons");
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Run Time
   const Double_t runTime = runConfig.RunTime();
   const Double_t maximumDistance = 10000*Units::m; 
   sprintf(histname,"%s:Time",stateDir->GetName());
   Plot::timeHist = new TH1F(histname,"Time: Units of s", nbins, 0.0, runTime+10);
   Plot::timeHist->SetXTitle("Time (s)");
   Plot::timeHist->SetYTitle("Neutrons");
   sprintf(histname,"%s:Dist",stateDir->GetName());
   Plot::distHist = new TH1F(histname,"Distance: Units of m", nbins, 0.0, maximumDistance);
   Plot::distHist->SetXTitle("Distance (m)");
   Plot::distHist->SetYTitle("Neutrons");
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into the State's folder
   stateDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all particle folders in the current state's folder
   TKey *folderKey;
   TIter folderIter(stateDir->GetListOfKeys());
   while ((folderKey = dynamic_cast<TKey*>(folderIter.Next()))) {
      const char *classname = folderKey->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TDirectory")) {
         // Loop over all objects in particle dir
         stateDir->cd(folderKey->GetName());
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
               Plot::points->SetPoint(particle->Id(), particle->X(), particle->Y(), particle->Z());
               Plot::thetaHist->Fill(particle->Theta()/TMath::Pi());
               Plot::phiHist->Fill(particle->Phi()/TMath::Pi());
               Plot::energyHist->Fill(particle->V());
               Plot::pxHist->Fill(particle->Px()/Units::eV);
               Plot::pyHist->Fill(particle->Py()/Units::eV);
               Plot::pzHist->Fill(particle->Pz()/Units::eV);
               Plot::timeHist->Fill(particle->T()/Units::s);
               Plot::distHist->Fill(particle->Distance()/Units::m);
               delete particle;
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
   // -- Angular Distribution
   TCanvas *anglecanvas = new TCanvas("Directions","Neutron Directions",60,0,1200,800);
   anglecanvas->Divide(2,1);
   anglecanvas->cd(1);
   Plot::thetaHist->Draw();
   anglecanvas->cd(2);
   Plot::phiHist->Draw();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Momentum Distribution
   TCanvas *momcanvas = new TCanvas("PhaseSpace","Phase Space",60,0,1200,800);
   momcanvas->Divide(3,2);
   momcanvas->cd(1);
   Plot::energyHist->Draw();
   momcanvas->Update();   
   momcanvas->cd(2);
   Plot::pxHist->Draw();
   momcanvas->cd(3);
   Plot::pyHist->Draw();
   momcanvas->cd(4);
   Plot::pzHist->Draw();
   momcanvas->cd(5);
   Plot::distHist->Draw();
   momcanvas->cd(6);
   Plot::timeHist->Draw();
   // Fit Final Time to Exponential - determine emptying time
/*   Char_t linename[40];
   sprintf(linename,"%s:Expo",stateDir->GetName());
   TF1 *expo = new TF1(linename, ExponentialDecay, 100., runTime, 2);
   expo->SetParameters(100.0,10.0);
   expo->SetParNames("Initial Number","Emptying Time(s)");
   timeHist->Fit(expo, "R");
   Double_t decayConstant = expo->GetParameter(1);
   Double_t decayConstantError = expo->GetParError(1);
   cout << "DecayConstant: " << decayConstant << "\t" << "Error: " << decayConstantError << endl;
   Double_t emptyingTime = 1.0/decayConstant;
   Double_t emptyingTimeError = decayConstantError/(decayConstant*decayConstant);
   cout << "EmptyingTime: " << emptyingTime << "\t" << "Error: " << emptyingTimeError << endl;
*/
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Final Positions
   TCanvas *poscanvas = new TCanvas("Positions","Neutron Positions",60,30,400,400);
   poscanvas->cd();
   geoManager->GetTopVolume()->Draw("ogl");
   geoManager->SetVisLevel(4);
   geoManager->SetVisOption(0);
   Plot::points->Draw();
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
void PlotSpinPolarisation(TDirectory* const histDir, TDirectory* const stateDir, const RunConfig& runConfig) 
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   Char_t histname[40];
   const Double_t runTime = runConfig.RunTime();
   const TVector3 xAxis(1.0,0.0,0.0);
   const TVector3 yAxis(0.0,1.0,0.0);
   const TVector3 zAxis(0.0,0.0,1.0);
   const int nbins = 500;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- X Axis Spin Polarisation
   sprintf(histname,"%s:SpinUp AlongX",stateDir->GetName());
   Plot::spinUpAlongXHist = new TH1F(histname,"SpinUp AlongX", nbins, 0.0, runTime);      
   Plot::spinUpAlongXHist->SetXTitle("Time (s)");
   Plot::spinUpAlongXHist->SetYTitle("Spin Up Neutrons");
   //spinUpAlongXHist->SetLineColor(kBlue);
   Plot::spinUpAlongXHist->SetFillStyle(1001);
   Plot::spinUpAlongXHist->SetFillColor(kBlue-7);

   sprintf(histname,"%s:SpinDown AlongX",stateDir->GetName());
   Plot::spinDownAlongXHist = new TH1F(histname,"SpinDown AlongX", nbins, 0.0, runTime);      
   Plot::spinDownAlongXHist->SetXTitle("Time (s)");
   Plot::spinDownAlongXHist->SetYTitle("Spin Down Neutrons");
   //spinDownAlongXHist->SetLineColor(kRed);
   Plot::spinDownAlongXHist->SetFillStyle(3001);
   Plot::spinDownAlongXHist->SetFillColor(kRed-7);
   
   sprintf(histname,"%s:SpinUp+Down Along X",stateDir->GetName());
   Plot::spinUpDownAlongXHist  = new TH1F(histname,"SpinUp+Down Along X", nbins, 0.0, runTime);   
   Plot::spinUpDownAlongXHist->SetXTitle("Time (s)");
   Plot::spinUpDownAlongXHist->SetYTitle("Spin Up+Down Neutrons");
   //spinDownHist->SetLineColor(kRed);
   Plot::spinUpDownAlongXHist->SetFillStyle(3001);
   Plot::spinUpDownAlongXHist->SetFillColor(kBlack);
   
   sprintf(histname,"%s:Polarisation Along X",stateDir->GetName());
   Plot::spinAlphaX = new TGraph(nbins);
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Y Axis Spin Polarisation
   sprintf(histname,"%s:SpinUp Along Y",stateDir->GetName());
   Plot::spinUpAlongYHist = new TH1F(histname,"SpinUp Along Y", nbins, 0.0, runTime);      
   Plot::spinUpAlongYHist->SetXTitle("Time (s)");
   Plot::spinUpAlongYHist->SetYTitle("Spin Up Neutrons");
   //spinUpAlongYHist->SetLineColor(kBlue);
   Plot::spinUpAlongYHist->SetFillStyle(1001);
   Plot::spinUpAlongYHist->SetFillColor(kBlue-7);

   sprintf(histname,"%s:SpinDown Along Y",stateDir->GetName());
   Plot::spinDownAlongYHist = new TH1F(histname,"SpinDown Along Y", nbins, 0.0, runTime);      
   Plot::spinDownAlongYHist->SetXTitle("Time (s)");
   Plot::spinDownAlongYHist->SetYTitle("Spin Down Neutrons");
   //spinDownAlongYHist->SetLineColor(kRed);
   Plot::spinDownAlongYHist->SetFillStyle(3001);
   Plot::spinDownAlongYHist->SetFillColor(kRed-7);
   
   sprintf(histname,"%s:SpinUp+Down Along Y",stateDir->GetName());
   Plot::spinUpDownAlongYHist  = new TH1F(histname,"SpinUp+Down Along Y", nbins, 0.0, runTime);   
   Plot::spinUpDownAlongYHist->SetXTitle("Time (s)");
   Plot::spinUpDownAlongYHist->SetYTitle("Spin Up+Down Neutrons");
   //spinDownHist->SetLineColor(kRed);
   Plot::spinUpDownAlongYHist->SetFillStyle(3001);
   Plot::spinUpDownAlongYHist->SetFillColor(kBlack);
   
   sprintf(histname,"%s:Polarisation Along Y",stateDir->GetName());
   Plot::spinAlphaY = new TGraph(nbins);
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Z Axis Spin Polarisation
   sprintf(histname,"%s:SpinUp Along Z",stateDir->GetName());
   Plot::spinUpAlongZHist = new TH1F(histname,"SpinUp Along Z", nbins, 0.0, runTime);      
   Plot::spinUpAlongZHist->SetXTitle("Time (s)");
   Plot::spinUpAlongZHist->SetYTitle("Spin Up Neutrons");
   //spinUpAlongZHist->SetLineColor(kBlue);
   Plot::spinUpAlongZHist->SetFillStyle(1001);
   Plot::spinUpAlongZHist->SetFillColor(kBlue-7);

   sprintf(histname,"%s:SpinDown Along Z",stateDir->GetName());
   Plot::spinDownAlongZHist = new TH1F(histname,"SpinDown Along Z", nbins, 0.0, runTime);      
   Plot::spinDownAlongZHist->SetXTitle("Time (s)");
   Plot::spinDownAlongZHist->SetYTitle("Spin Down Neutrons");
   //spinDownAlongZHist->SetLineColor(kRed);
   Plot::spinDownAlongZHist->SetFillStyle(3001);
   Plot::spinDownAlongZHist->SetFillColor(kRed-7);
   
   sprintf(histname,"%s:SpinUp+Down Along Z",stateDir->GetName());
   Plot::spinUpDownAlongZHist  = new TH1F(histname,"SpinUp+Down Along Z", nbins, 0.0, runTime);   
   Plot::spinUpDownAlongZHist->SetXTitle("Time (s)");
   Plot::spinUpDownAlongZHist->SetYTitle("Spin Up+Down Neutrons");
   //spinDownHist->SetLineColor(kRed);
   Plot::spinUpDownAlongZHist->SetFillStyle(3001);
   Plot::spinUpDownAlongZHist->SetFillColor(kBlack);
   
   sprintf(histname,"%s:Polarisation Along Z",stateDir->GetName());
   Plot::spinAlphaZ = new TGraph(nbins);
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into the State's folder
   stateDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all particle folders in the current state's folder
   TKey *folderKey;
   TIter folderIter(stateDir->GetListOfKeys());
   while ((folderKey = dynamic_cast<TKey*>(folderIter.Next()))) {
      const char *classname = folderKey->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TDirectory")) {
         // Loop over all objects in particle dir
         stateDir->cd(folderKey->GetName());
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
                  Plot::spinUpDownAlongXHist->Fill(dataIter->first);
                  Plot::spinUpDownAlongYHist->Fill(dataIter->first);
                  Plot::spinUpDownAlongZHist->Fill(dataIter->first);
                  // For each data point, record the spin polarisation along each axis
                  const Spin* spin = dataIter->second;
                  // Measure polarisation along X
                  if (spin->IsSpinUp(xAxis)) {
                     // If spin up, bin the time
                     if (Plot::spinUpAlongXHist) Plot::spinUpAlongXHist->Fill(dataIter->first);
                  } else {
                     // If spin down, bin the time
                     if (Plot::spinUpAlongXHist) Plot::spinDownAlongXHist->Fill(dataIter->first);
                  }
                  // Measure polarisation along Y
                  if (spin->IsSpinUp(yAxis)) {
                     // If spin up, bin the time
                     if (Plot::spinUpAlongYHist) Plot::spinUpAlongYHist->Fill(dataIter->first);
                  } else {
                     // If spin down, bin the time
                     if (Plot::spinUpAlongYHist) Plot::spinDownAlongYHist->Fill(dataIter->first);
                  }
                  // Measure polarisation along Y
                  if (spin->IsSpinUp(zAxis)) {
                     // If spin up, bin the time
                     if (Plot::spinUpAlongZHist) Plot::spinUpAlongZHist->Fill(dataIter->first);
                  } else {
                     // If spin down, bin the time
                     if (Plot::spinUpAlongZHist) Plot::spinDownAlongZHist->Fill(dataIter->first);
                  }
               }
               delete data;
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
   Plot::spinUpAlongXHist->Draw();
   //   spinUpHist->GetYaxis()->SetRangeUser(0.0,200.0);
   /*   TF1 *upprecession = new TF1("UpSpinPrecessionFunc", SpinPrecession, 0.0, 20., 3);
      upprecession->SetParameters(150.0,1.0,0.0);
      upprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
      spinUpHist->Fit(upprecession, "RQ");
      Double_t upamplitude = upprecession->GetParameter(0);
      Double_t upomega = upprecession->GetParameter(1);
      Double_t upphase = upprecession->GetParameter(2);
      cout << "------------------------------------" << endl;
      cout << "Spin Up:" << endl;
      cout << "Amplitude: " << upamplitude << "\t" << "Omega: " << upomega;
      cout << "\t" << "Phase: " << upphase << endl;
      cout << "------------------------------------" << endl;
   */
   spinXcanvas->cd(2);
   Plot::spinDownAlongXHist->Draw();
   //   spinDownHist->GetYaxis()->SetRangeUser(0.0,200.0);
   /*   TF1 *downprecession = new TF1("DownSpinPrecessionFunc", SpinPrecession, 0.0, 20., 3);
      downprecession->SetParameters(150.0,1.0,0.0);
      downprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
      spinDownHist->Fit(downprecession, "RQ");
      Double_t downamplitude = downprecession->GetParameter(0);
      Double_t downomega = downprecession->GetParameter(1);
      Double_t downphase = downprecession->GetParameter(2);
      cout << "------------------------------------" << endl;
      cout << "Spin Down:" << endl;
      cout << "Amplitude: " << downamplitude << "\t" << "Omega: " << downomega;
      cout << "\t" << "Phase: " << downphase << endl;   
      cout << "------------------------------------" << endl;
   */ 
   spinXcanvas->cd(3);
   Plot::spinUpDownAlongXHist->Draw();
   /*
      TLegend* leg = new TLegend(0.7,0.91,0.9,0.99); //coordinates are fractions of pad dimensions
      leg->SetFillColor(0);
      leg->AddEntry(spinUpHist,"Spin Up"); // l means line, p shows marker
      leg->AddEntry(spinDownHist,"Spin Down");
      leg->Draw();
   */
   spinXcanvas->cd(4);
   // Calculate polarisation
   assert(Plot::spinUpAlongXHist->GetNbinsX() == nbins);
   for (int i = 0; i < Plot::spinUpAlongXHist->GetNbinsX(); i++) {
      Double_t binCentre = Plot::spinUpAlongXHist->GetBinCenter(i);
      Double_t upCounts = Plot::spinUpAlongXHist->GetBinContent(i);
      Double_t downCounts = Plot::spinDownAlongXHist->GetBinContent(i);
      Double_t totalCounts = upCounts + downCounts;
      Double_t alpha = totalCounts == 0 ? 0.0 : (upCounts - downCounts) / totalCounts;
//      cout << "Time: " << binCentre << "\t" << "Up: " << upCounts << "\t";
//      cout << "Down: " << downCounts << "\t" << "Alpha: " << alpha << endl;
      Plot::spinAlphaX->SetPoint(i, binCentre, alpha);
   }
   Plot::spinAlphaX->SetMarkerStyle(7);
   Plot::spinAlphaX->Draw("ALP");
   Plot::spinAlphaX->GetXaxis()->SetTitle("Time (s)");
   Plot::spinAlphaX->GetXaxis()->SetRangeUser(0.0,runTime);
   Plot::spinAlphaX->GetYaxis()->SetTitle("Alpha");
   Plot::spinAlphaX->GetYaxis()->SetRangeUser(-1.0,1.0);
   Plot::spinAlphaX->SetTitle("Polarisation along X");
   
   TCanvas *spinYcanvas = new TCanvas("SpinAlongY","Spin Polarisation Along Y",60,0,1200,800);
   spinYcanvas->Divide(2,2);
   spinYcanvas->cd(1);
   Plot::spinUpAlongYHist->Draw();
   spinYcanvas->cd(2);
   Plot::spinDownAlongYHist->Draw();
   spinYcanvas->cd(3);
   Plot::spinUpDownAlongYHist->Draw();
   spinYcanvas->cd(4);
   // Calculate polarisation
   assert(Plot::spinUpAlongYHist->GetNbinsX() == nbins);
   for (int i = 0; i < Plot::spinUpAlongYHist->GetNbinsX(); i++) {
      Double_t binCentre = Plot::spinUpAlongYHist->GetBinCenter(i);
      Double_t upCounts = Plot::spinUpAlongYHist->GetBinContent(i);
      Double_t downCounts = Plot::spinDownAlongYHist->GetBinContent(i);
      Double_t totalCounts = upCounts + downCounts;
      Double_t alpha = totalCounts == 0 ? 0.0 : (upCounts - downCounts) / totalCounts;
//      cout << "Time: " << binCentre << "\t" << "Up: " << upCounts << "\t";
//      cout << "Down: " << downCounts << "\t" << "Alpha: " << alpha << endl;
      Plot::spinAlphaY->SetPoint(i, binCentre, alpha);
   }
   Plot::spinAlphaY->SetMarkerStyle(7);
   Plot::spinAlphaY->Draw("ALP");
   Plot::spinAlphaY->GetXaxis()->SetTitle("Time (s)");
   Plot::spinAlphaY->GetXaxis()->SetRangeUser(0.0,runTime);
   Plot::spinAlphaY->GetYaxis()->SetTitle("Alpha");
   Plot::spinAlphaY->GetYaxis()->SetRangeUser(-1.0,1.0);
   Plot::spinAlphaY->SetTitle("Polarisation along Y");
   
   TCanvas *spinZcanvas = new TCanvas("SpinAlongZ","Spin Polarisation Along Z",60,0,1200,800);
   spinZcanvas->Divide(2,2);
   spinZcanvas->cd(1);
   Plot::spinUpAlongZHist->Draw();
   spinZcanvas->cd(2);
   Plot::spinDownAlongZHist->Draw();
   spinZcanvas->cd(3);
   Plot::spinUpDownAlongZHist->Draw();
   spinZcanvas->cd(4);
   // Calculate polarisation
   assert(Plot::spinUpAlongZHist->GetNbinsX() == nbins);
   for (int i = 0; i < Plot::spinUpAlongZHist->GetNbinsX(); i++) {
      Double_t binCentre = Plot::spinUpAlongZHist->GetBinCenter(i);
      Double_t upCounts = Plot::spinUpAlongZHist->GetBinContent(i);
      Double_t downCounts = Plot::spinDownAlongZHist->GetBinContent(i);
      Double_t totalCounts = upCounts + downCounts;
      Double_t alpha = totalCounts == 0 ? 0.0 : (upCounts - downCounts) / totalCounts;
//      cout << "Time: " << binCentre << "\t" << "Up: " << upCounts << "\t";
//      cout << "Down: " << downCounts << "\t" << "Alpha: " << alpha << endl;
      Plot::spinAlphaZ->SetPoint(i, binCentre, alpha);
   }
   Plot::spinAlphaZ->SetMarkerStyle(7);
   Plot::spinAlphaZ->Draw("ALP");
   Plot::spinAlphaZ->GetXaxis()->SetTitle("Time (s)");
   Plot::spinAlphaZ->GetXaxis()->SetRangeUser(0.0,runTime);
   Plot::spinAlphaZ->GetYaxis()->SetTitle("Alpha");
   Plot::spinAlphaZ->GetYaxis()->SetRangeUser(-1.0,1.0);
   Plot::spinAlphaZ->SetTitle("Polarisation along Z");
   
   return;
}

//_____________________________________________________________________________
void PlotBounceCounters(TDirectory* const histDir, TDirectory* const stateDir, const RunConfig& /*runConfig*/)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   Char_t histname[40];
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounces
   sprintf(histname,"%s:Bounces",stateDir->GetName());
   Plot::bounceHist = new TH1F(histname,"Bounces", 200, 0.0, 20000);
   sprintf(histname,"%s:Specular",stateDir->GetName());
   Plot::specHist = new TH1F(histname,"Specular", 200, 0.0, 20000);
   sprintf(histname,"%s:Diffuse",stateDir->GetName());
   Plot::diffHist = new TH1F(histname,"Diffuse", 200, 0.0, 20000);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into the State's folder
   stateDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all particle folders in the current state's folder
   TKey *folderKey;
   TIter folderIter(stateDir->GetListOfKeys());
   while ((folderKey = dynamic_cast<TKey*>(folderIter.Next()))) {
      const char *classname = folderKey->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TDirectory")) {
         // Loop over all objects in particle dir
         stateDir->cd(folderKey->GetName());
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
               Plot::bounceHist->Fill(data->CountTotal());
               Plot::specHist->Fill(data->CountSpecular());
               Plot::diffHist->Fill(data->CountDiffuse());
               delete data;
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
   Plot::bounceHist->Draw();
   bouncecanvas->cd(2);
   Plot::specHist->Draw();
   bouncecanvas->cd(3);
   Plot::diffHist->Draw();
   return;
}

//_____________________________________________________________________________
void PlotField(TDirectory* const histDir, TDirectory* const stateDir, const RunConfig& runConfig)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   Char_t histname[40];
   const Double_t runTime = runConfig.RunTime();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bx
   sprintf(histname,"%s:Field Bx",stateDir->GetName());
   Plot::bxHist = new TH2F(histname,"Bx", 500, -0.1*Units::uT, 0.1*Units::uT, 500, 0.0, runTime);
   Plot::bxHist->SetXTitle("Field measured (T)");
   Plot::bxHist->SetYTitle("Time (s)");
   Plot::bxHist->SetZTitle("Neutrons");
   // -- Bz
   sprintf(histname,"%s:Field By",stateDir->GetName());
   Plot::byHist = new TH2F(histname,"By", 500, -0.1*Units::uT, 0.1*Units::uT, 500, 0.0, runTime);
   Plot::byHist->SetXTitle("Field measured (T)");
   Plot::byHist->SetYTitle("Time (s)");
   Plot::byHist->SetZTitle("Neutrons");
   // -- Bx
   sprintf(histname,"%s:Field Bz",stateDir->GetName());
   Plot::bzHist = new TH2F(histname,"Bz", 500, 4.8*Units::uT, 5.2*Units::uT, 500, 0.0, runTime);
   Plot::bzHist->SetXTitle("Field measured (T)");
   Plot::bzHist->SetYTitle("Time (s)");
   Plot::bzHist->SetZTitle("Neutrons");
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into the State's folder
   stateDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all particle folders in the current state's folder
   TKey *folderKey;
   TIter folderIter(stateDir->GetListOfKeys());
   while ((folderKey = dynamic_cast<TKey*>(folderIter.Next()))) {
      const char *classname = folderKey->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TDirectory")) {
         // Loop over all objects in particle dir
         stateDir->cd(folderKey->GetName());
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
                  Plot::bxHist->Fill((*dataIter)->Bx(),(*dataIter)->T());
                  Plot::byHist->Fill((*dataIter)->By(),(*dataIter)->T());
                  Plot::bzHist->Fill((*dataIter)->Bz(),(*dataIter)->T());
               }
               delete fieldData;
            }
         }
      }
   }
   TCanvas *bxcanvas = new TCanvas("BxField","Bx (T)",60,0,1200,800);
   bxcanvas->cd();
   Plot::bxHist->Draw("COLZ");
   TCanvas *bycanvas = new TCanvas("ByField","By (T)",60,0,1200,800);
   bycanvas->cd();
   Plot::byHist->Draw("COLZ");
   TCanvas *bzcanvas = new TCanvas("BzFields","Bz (T)",60,0,1200,800);
   bzcanvas->cd();
   Plot::bzHist->Draw("COLZ");
   return;
}

//_____________________________________________________________________________
void PlotParticleHistories(TDirectory* const histDir, TDirectory* const stateDir, const RunConfig& /*runConfig*/, TGeoManager* geoManager)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   Char_t histname[40];
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounces
   sprintf(histname,"%s:Source",stateDir->GetName());
   TH1F* timeInSourceHist = new TH1F(histname,"Source", 100, 0.0, 1.);
   timeInSourceHist->SetXTitle("Percent Time spent in Source");
   timeInSourceHist->SetYTitle("Percentage of total Neutrons");
   sprintf(histname,"%s:TransferSection",stateDir->GetName());
   TH1F* timeInTransferSecHist = new TH1F(histname,"TransferSection", 100, 0.0, 1.);
   timeInTransferSecHist->SetXTitle("Percent Time spent in Transfer Section");
   timeInTransferSecHist->SetYTitle("Percentage of total Neutrons");
   sprintf(histname,"%s:RamseyCell",stateDir->GetName());
   TH1F* timeInRamseyCellHist = new TH1F(histname,"RamseyCell", 100, 0.0, 1.);
   timeInRamseyCellHist->SetXTitle("Percent Time spent in Ramsey Cell");
   timeInRamseyCellHist->SetYTitle("Percentage of total Neutrons");
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into the State's folder
   stateDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all particle folders in the current state's folder
   TKey *folderKey;
   TIter folderIter(stateDir->GetListOfKeys());
   while ((folderKey = dynamic_cast<TKey*>(folderIter.Next()))) {
      const char *classname = folderKey->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TDirectory")) {
         // Loop over all objects in particle dir
         stateDir->cd(folderKey->GetName());
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
   
   TCanvas *historycanvas = new TCanvas("History","Histories",60,0,1200,800);
   historycanvas->Divide(3,1);
   historycanvas->cd(1);
   timeInSourceHist->Scale(1.0/stateDir->GetNkeys());
   timeInSourceHist->SetMaximum(1.0);
   timeInSourceHist->Draw();
   historycanvas->cd(2);
   timeInTransferSecHist->Scale(1.0/stateDir->GetNkeys());
   timeInTransferSecHist->SetMaximum(1.0);
   timeInTransferSecHist->Draw();
   historycanvas->cd(3);
   timeInRamseyCellHist->Scale(1.0/stateDir->GetNkeys());
   timeInRamseyCellHist->SetMaximum(1.0);
   timeInRamseyCellHist->Draw();
   
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
Double_t ExponentialDecay(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Exp(-t/par[1]);
   return f;
}

//_____________________________________________________________________________
Double_t SpinPrecession(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Sin((par[1]*t + par[2]*(TMath::Pi()/2.0)))*TMath::Sin((par[1]*t + par[2]*(TMath::Pi()/2.0)));
   return f;
}
