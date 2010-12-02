#include <iostream>
#include <sstream>

#include "TGeoManager.h"
#include "TH1F.h"
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

#include "Particle.h"
#include "ConfigFile.h"
#include "InitialConfig.h"
#include "RunConfig.h"
#include "Observables.h"

#include "Constants.h"
#include "Units.h"
#include "DataFileHierarchy.h"

using namespace std;

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
   
   TH1F* spinUpHist = NULL;
   TH1F* spinDownHist = NULL;
   
   TH1F* spinUpDownHist = NULL;
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
   const char* runName="Run1";
   
   // Start 'the app' -- this is so we are able to enter into a ROOT session
   // after the program has run, instead of just quitting.
   TRint *theApp = new TRint("FittingApp", &argc, argv);
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
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
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   //////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////
   Char_t histname[40];
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Final Positions
   const Int_t neutrons = initialConfig.InitialParticles();
   Plot::points = new TPolyMarker3D(neutrons, 1); // 1 is marker style
   sprintf(histname,"%s:NeutronPositions",statename.Data());
   Plot::points->SetName(histname);
   Plot::points->SetMarkerColor(2);
   Plot::points->SetMarkerStyle(6);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Angular Distribution
   sprintf(histname,"%s:Theta",statename.Data());
   Plot::thetaHist = new TH1F(histname,"Direction: Theta, Units of Pi", 50, 0.0, 1.0);
   sprintf(histname,"%s:Phi",statename.Data());
   Plot::phiHist = new TH1F(histname,"Direction: Phi, Units of Pi", 50, -1.0, 1.0);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Energy/Momentum
   const Double_t maximumVelocity = initialConfig.InitialMaxVelocity();
   const Double_t maximumMomentum = 20*Units::eV;
   const Int_t nbins = 50;   
   
   sprintf(histname,"%s:Velocity",statename.Data());
   Plot::energyHist = new TH1F(histname,"Velocity: Units of m/s", nbins, 0.0, maximumVelocity);      
   Plot::energyHist->SetXTitle("Velocity (m/s)");
   Plot::energyHist->SetYTitle("Neutrons");
   
   sprintf(histname,"%s:Px",statename.Data());
   Plot::pxHist = new TH1F(histname,"Px: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   Plot::pxHist->SetXTitle("Px (eV)");
   Plot::pxHist->SetYTitle("Neutrons");
   
   sprintf(histname,"%s:Py",statename.Data());
   Plot::pyHist = new TH1F(histname,"Py: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   Plot::pyHist->SetXTitle("Py (eV)");
   Plot::pyHist->SetYTitle("Neutrons");
   
   sprintf(histname,"%s:Pz",statename.Data());
   Plot::pzHist = new TH1F(histname,"Pz: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   Plot::pzHist->SetXTitle("Pz (eV)");
   Plot::pzHist->SetYTitle("Neutrons");
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Run Time
   const Double_t runTime = runConfig.RunTime();
   const Double_t maximumDistance = 10000*Units::m;
   
   sprintf(histname,"%s:Time",statename.Data());
   Plot::timeHist = new TH1F(histname,"Time: Units of s", nbins, 0.0, runTime+10);
   Plot::timeHist->SetXTitle("Time (s)");
   Plot::timeHist->SetYTitle("Neutrons");
   
   sprintf(histname,"%s:Dist",statename.Data());
   Plot::distHist = new TH1F(histname,"Distance: Units of m", nbins, 0.0, maximumDistance);
   Plot::distHist->SetXTitle("Distance (m)");
   Plot::distHist->SetYTitle("Neutrons");
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounces
   if (runConfig.ObserveBounces() == kTRUE) {
      sprintf(histname,"%s:Bounces",statename.Data());
      Plot::bounceHist = new TH1F(histname,"Bounces", 200, 0.0, 20000);
      sprintf(histname,"%s:Specular",statename.Data());
      Plot::specHist = new TH1F(histname,"Specular", 200, 0.0, 20000);
      sprintf(histname,"%s:Diffuse",statename.Data());
      Plot::diffHist = new TH1F(histname,"Diffuse", 200, 0.0, 20000);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Spin Polarisation
   if (runConfig.ObservePolarisation() == kTRUE) {
      sprintf(histname,"%s:SpinUp",statename.Data());
      Plot::spinUpHist = new TH1F(histname,"SpinUp", 500, 0.0, runTime);      
      Plot::spinUpHist->SetXTitle("Time (s)");
      Plot::spinUpHist->SetYTitle("Spin Up Neutrons");
      //spinUpHist->SetLineColor(kBlue);
      Plot::spinUpHist->SetFillStyle(1001);
      Plot::spinUpHist->SetFillColor(kBlue-7);
   
      sprintf(histname,"%s:SpinDown",statename.Data());
      Plot::spinDownHist = new TH1F(histname,"SpinDown", 500, 0.0, runTime);      
      Plot::spinDownHist->SetXTitle("Time (s)");
      Plot::spinDownHist->SetYTitle("Spin Down Neutrons");
      //spinDownHist->SetLineColor(kRed);
      Plot::spinDownHist->SetFillStyle(3001);
      Plot::spinDownHist->SetFillColor(kRed-7);
      
      sprintf(histname,"%s:SpinUp+Down",statename.Data());
      Plot::spinUpDownHist  = new TH1F(histname,"SpinUp+Down", 500, 0.0, runTime);   
      Plot::spinUpDownHist->SetXTitle("Time (s)");
      Plot::spinUpDownHist->SetYTitle("Spin Up+Down Neutrons");
      //spinDownHist->SetLineColor(kRed);
      Plot::spinUpDownHist->SetFillStyle(3001);
      Plot::spinUpDownHist->SetFillColor(kBlack);
   }
   //////////////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
   //////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////
   TFile *file = 0;
   file = TFile::Open(dataFileName, "update");
   if (!file || file->IsZombie()) {
      cerr << "Cannot open file: " << dataFileName << endl;
      return 0;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Load the Geometry
   TString geomFileName = runConfig.GeomVisFileName();
   if (geomFileName == "") { 
      cout << "No File holding the geometry can be found" << endl;
      return kFALSE;
   }
   TGeoManager* geoManager = TGeoManager::Import(geomFileName); 
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Navigate to folder for selected state
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
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all particle folders in the current state's folder
   //////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////
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
            } else if (cl->InheritsFrom("SpinObservables")) {
               // -- Extract Spin Observer Data if recorded
               SpinObservables* data = dynamic_cast<SpinObservables*>(objKey->ReadObj());
               // Loop over spin data recorded for particle
               SpinObservables::iterator dataIter;
               for (dataIter = data->begin(); dataIter != data->end(); dataIter++) {
                  if (dataIter->second == kTRUE) {
                     // If spin up, bin the time
                     if (Plot::spinUpHist) Plot::spinUpHist->Fill(dataIter->first);
                  } else {
                     // If spin down, bin the time
                     if (Plot::spinUpHist) Plot::spinDownHist->Fill(dataIter->first);
                  }
                  Plot::spinUpDownHist->Fill(dataIter->first);
               }
               delete data;
            } else if (cl->InheritsFrom("BounceObservables")) {
               // -- Extract Bounce Observer Data if recorded
               BounceObservables* data =dynamic_cast<BounceObservables*>(objKey->ReadObj());
               Plot::bounceHist->Fill(data->CountTotal());
               Plot::specHist->Fill(data->CountSpecular());
               Plot::diffHist->Fill(data->CountDiffuse());
               delete data;
            }
         }
      }
   }
   file->Close();
   //////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Draw Histograms
   //////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////
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
   sprintf(linename,"%s:Expo",statename.Data());
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
   // -- Bounce Counters
   if (Plot::bounceHist) {
      TCanvas *bouncecanvas = new TCanvas("Bounces","Bounce counters",60,0,1200,800);
      bouncecanvas->Divide(3,1);
      bouncecanvas->cd(1);
      Plot::bounceHist->Draw();
      bouncecanvas->cd(2);
      Plot::specHist->Draw();
      bouncecanvas->cd(3);
      Plot::diffHist->Draw();
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Spin Precession Plots
   // -- Down
   if (Plot::spinUpHist) {
      TCanvas *spincanvas = new TCanvas("Spin","Spin Polarisation",60,0,1200,800);
      spincanvas->Divide(3,1);
      spincanvas->cd(1);
      Plot::spinDownHist->Draw();
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
   */   // -- Up
      spincanvas->cd(2);
   //   spinUpHist->GetYaxis()->SetRangeUser(0.0,200.0);
      Plot::spinUpHist->Draw();
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
   */   // -- Combined
      spincanvas->cd(3);
      Plot::spinUpDownHist->Draw();
   /*
      
      TLegend* leg = new TLegend(0.7,0.91,0.9,0.99); //coordinates are fractions of pad dimensions
      leg->SetFillColor(0);
      leg->AddEntry(spinUpHist,"Spin Up"); // l means line, p shows marker
      leg->AddEntry(spinDownHist,"Spin Down");
      leg->Draw();
   */ 
      
   }
   
   
   cout << "Finished" << endl;
   theApp->Run();
   return 0;
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
