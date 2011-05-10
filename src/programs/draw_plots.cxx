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
#include "TClass.h"

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
#include "Algorithms.h"
#include "DataAnalysis.h"

using namespace std;

void DrawFinalStates(TDirectory* const histDir, TGeoManager* geoManager);
void DrawSpinPolarisation(TDirectory* const histDir);
void DrawBounceCounters(TDirectory* const histDir);
void DrawField(TDirectory* const histDir);

//_____________________________________________________________________________
Int_t main(int argc, char **argv)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   // Plot data needs a minimum of 2 arguments - a data file, and the name of the particle
   // 'state' you wish to include in your histograms.
   if (argc < 2) {
      cerr << "Usage:" << endl;
      cerr << "plot_data <data.root>" << endl;
      return EXIT_FAILURE;
   }
   // Read in Filename and check that it is a .root file
   string filename = argv[1];
   if (Analysis::DataFile::ValidateRootFile(filename) == false) {
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
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Loaded Data File: " << filename << endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Navigate to Histogram folder
   TDirectory * const topDir = gDirectory;
   if (topDir->cd(Folders::histograms.c_str()) == false) {
      cerr << "No Folder named: " << Folders::histograms << " in data file" << endl;
      return EXIT_FAILURE;
   }
   TDirectory * const histDir = gDirectory;
   histDir->ls();
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Load the Geometry
   if (topDir->cd(Folders::geometry.c_str()) == false) {return EXIT_FAILURE;}
   TDirectory* geomDir = gDirectory;
   TKey *geomKey;
   TIter geomIter(geomDir->GetListOfKeys());
   while ((geomKey = dynamic_cast<TKey*>(geomIter.Next()))) {
      const char *classname = geomKey->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TGeoManager")) {
         gGeoManager = dynamic_cast<TGeoManager*>(geomKey->ReadObj());
         break;
      }
   }
   TGeoManager* geoManager = gGeoManager;
   if (geoManager == NULL) return EXIT_FAILURE;
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Loaded Geometry" << endl;
   cout << "-------------------------------------------" << endl;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Particle final state
   DrawFinalStates(histDir, geoManager);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Polarisation
   DrawSpinPolarisation(histDir);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounce Data
   DrawBounceCounters(histDir);
   //////////////////////////////////////////////////////////////////////////////////////
   DrawField(histDir);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Clean up and Finish
   cout << "Finished" << endl;
   theApp->Run();
   return EXIT_SUCCESS;
}

//_____________________________________________________________________________
void DrawFinalStates(TDirectory* const histDir, TGeoManager* geoManager)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into the Histogram's directory
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Final Positions
   TPolyMarker3D* points = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Angular Distribution
   TH1F* thetaHist = NULL;
   TH1F* phiHist = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Energy/Momentum
   TH1F* vHist = NULL;
   TH1F* vxHist = NULL;
   TH1F* vyHist = NULL;
   TH1F* vzHist = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Run Time
   TH1F* timeHist = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all histograms in folder
   TKey *key;
   TIter iter(histDir->GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(iter.Next()))) {
      string histName = key->GetName();
      size_t position = histName.find_last_of(":");
      if (position != string::npos) {
         if (histName.substr(position+1) == "Time") {
            timeHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "NeutronPositions") {
            points = dynamic_cast<TPolyMarker3D*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Theta") {
            thetaHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Phi") {
            phiHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Velocity") {
            vHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Vx") {
            vxHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Vy") {
            vyHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Vz") {
            vzHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else {
            continue;
         }
      }   
   }
   // -- check that all histograms were extracted
   if (timeHist == NULL || points == NULL || thetaHist == NULL || phiHist == NULL ||
       vHist == NULL || vxHist == NULL || vyHist == NULL || vzHist == NULL) {
      cout << "-------------------------------------------" << endl;
      cout << "Error: Could not extract Final States Histograms" << endl;
      cout << "-------------------------------------------" << endl;
      return;
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
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Velocity Distribution
   TCanvas *velcanvas = new TCanvas("Velocity","Velocity Space",60,0,1200,800);
   velcanvas->Divide(3,2);
   velcanvas->cd(1);
   vHist->Draw();
   velcanvas->Update();
   velcanvas->cd(2);
   thetaHist->Draw();
   velcanvas->cd(3);
   phiHist->Draw();
   velcanvas->cd(4);
   vxHist->Draw();
   velcanvas->cd(5);
   vyHist->Draw();
   velcanvas->cd(6);
   vzHist->Draw();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Final Positions
   TCanvas *poscanvas = new TCanvas("Positions","Neutron Positions",10,10,50,50);
   poscanvas->cd();
   geoManager->GetTopVolume()->Draw("ogl");
   geoManager->SetVisLevel(4);
   geoManager->SetVisOption(0);
   points->Draw();
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
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Fetched Final State Histograms" << endl;
   cout << "-------------------------------------------" << endl;
   return;
}

//_____________________________________________________________________________
void DrawSpinPolarisation(TDirectory* const histDir)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- X Axis Spin Polarisation
   TH1F* spinUpAlongXHist = NULL;
   TH1F* spinDownAlongXHist = NULL;
   TH1F* spinUpDownAlongXHist = NULL;
   TGraph* spinAlphaX = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Y Axis Spin Polarisation
   TH1F* spinUpAlongYHist = NULL;
   TH1F* spinDownAlongYHist = NULL;
   TH1F* spinUpDownAlongYHist = NULL;
   TGraph* spinAlphaY = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Z Axis Spin Polarisation
   TH1F* spinUpAlongZHist = NULL;
   TH1F* spinDownAlongZHist = NULL;
   TH1F* spinUpDownAlongZHist = NULL;
   TGraph* spinAlphaZ = NULL;
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all histograms in folder
   TKey *key;
   TIter iter(histDir->GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(iter.Next()))) {
      string histName = key->GetName();
      size_t position = histName.find_last_of(":");
      if (position != string::npos) {
         if (histName.substr(position+1) == "SpinUp Along X") {
            spinUpAlongXHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "SpinDown Along X") {
            spinDownAlongXHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "SpinUp+Down Along X") {
            spinUpDownAlongXHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "SpinUp Along Y") {
            spinUpAlongYHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "SpinDown Along Y") {
            spinDownAlongYHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "SpinUp+Down Along Y") {
            spinUpDownAlongYHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "SpinUp Along Z") {
            spinUpAlongZHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "SpinDown Along Z") {
            spinDownAlongZHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "SpinUp+Down Along Z") {
            spinUpDownAlongZHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Polarisation Along X") {
            spinAlphaX = dynamic_cast<TGraph*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Polarisation Along Y") {
            spinAlphaY = dynamic_cast<TGraph*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Polarisation Along Z") {
            spinAlphaZ = dynamic_cast<TGraph*>(key->ReadObj());
         } else {
            continue;
         }
      }   
   }
   
   if (spinUpAlongXHist == NULL ||
       spinDownAlongXHist == NULL ||
       spinUpDownAlongXHist == NULL ||
       spinAlphaX == NULL ||
       spinUpAlongYHist == NULL ||
       spinDownAlongYHist == NULL ||
       spinUpDownAlongYHist == NULL ||
       spinAlphaY == NULL ||
       spinUpAlongZHist == NULL ||
       spinDownAlongZHist == NULL ||
       spinUpDownAlongZHist == NULL ||
       spinAlphaZ == NULL) {
      cout << "-------------------------------------------" << endl;
      cout << "Error: Could not extract Spin Polarisation Histograms" << endl;
      cout << "-------------------------------------------" << endl;
      return;
   }
       
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd back into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Spin Precession Plots
   TCanvas *spinXcanvas = new TCanvas("SpinAlongX","Spin Polarisation Along X",60,0,1200,800);
   spinXcanvas->Divide(2,2);
   spinXcanvas->cd(1);
   spinUpAlongXHist->Draw();
   spinXcanvas->cd(2);
   spinDownAlongXHist->Draw();
   spinXcanvas->cd(3);
   spinUpDownAlongXHist->Draw();
   spinXcanvas->cd(4);
   spinAlphaX->Draw("AP");
   
   TCanvas *spinYcanvas = new TCanvas("SpinAlongY","Spin Polarisation Along Y",60,0,1200,800);
   spinYcanvas->Divide(2,2);
   spinYcanvas->cd(1);
   spinUpAlongYHist->Draw();
   spinYcanvas->cd(2);
   spinDownAlongYHist->Draw();
   spinYcanvas->cd(3);
   spinUpDownAlongYHist->Draw();
   spinYcanvas->cd(4);
   spinAlphaY->Draw("AP");
   
   TCanvas *spinZcanvas = new TCanvas("SpinAlongZ","Spin Polarisation Along Z",60,0,1200,800);
   spinZcanvas->Divide(2,2);
   spinZcanvas->cd(1);
   spinUpAlongZHist->Draw();
   spinZcanvas->cd(2);
   spinDownAlongZHist->Draw();
   spinZcanvas->cd(3);
   spinUpDownAlongZHist->Draw();
   spinZcanvas->cd(4);
   spinAlphaZ->Draw("AP");
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Fetched Spin Histograms" << endl;
   cout << "-------------------------------------------" << endl;
   return;
}


//_____________________________________________________________________________
void DrawBounceCounters(TDirectory* const histDir)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounces
   TH1F* bounceHist = NULL;
   TH1F* specHist = NULL;
   TH1F* diffHist = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all histograms in folder
   TKey *key;
   TIter iter(histDir->GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(iter.Next()))) {
      string histName = key->GetName();
      size_t position = histName.find_last_of(":");
      if (position != string::npos) {
         if (histName.substr(position+1) == "Bounces") {
            bounceHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Specular") {
            specHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Diffuse") {
            diffHist = dynamic_cast<TH1F*>(key->ReadObj());
         } else {
            continue;
         }
      }   
   }
   
   if (bounceHist == NULL || specHist == NULL || diffHist == NULL) {
      cout << "-------------------------------------------" << endl;
      cout << "Error: Could not extract Bounce Histograms" << endl;
      cout << "-------------------------------------------" << endl;
      return;
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
   bouncecanvas->cd(2);
   specHist->Draw();
   bouncecanvas->cd(3);
   diffHist->Draw();
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Fetched Bounce Histograms" << endl;
   cout << "-------------------------------------------" << endl;
   return;
}

//_____________________________________________________________________________
void DrawField(TDirectory* const histDir)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd into Histogram's dir
   histDir->cd();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- B Field
   TH2F* bxHist = NULL;
   TH2F* byHist = NULL;
   TH2F* bzHist = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all histograms in folder
   TKey *key;
   TIter iter(histDir->GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(iter.Next()))) {
      string histName = key->GetName();
      size_t position = histName.find_last_of(":");
      if (position != string::npos) {
         if (histName.substr(position+1) == "Field Bx") {
            bxHist = dynamic_cast<TH2F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Field By") {
            byHist = dynamic_cast<TH2F*>(key->ReadObj());
         } else if (histName.substr(position+1) == "Field Bz") {
            bzHist = dynamic_cast<TH2F*>(key->ReadObj());
         } else {
            continue;
         }
      }   
   }
   if (bxHist == NULL || byHist == NULL || bzHist == NULL) {
      cout << "-------------------------------------------" << endl;
      cout << "Error: Could not extract Field Histograms" << endl;
      cout << "-------------------------------------------" << endl;
      return;
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd back into Histogram's dir
   histDir->cd();
   TCanvas *bxcanvas = new TCanvas("BxField","Bx (T)",60,0,1200,800);
   bxcanvas->cd();
   bxHist->Draw("COLZ");
   TCanvas *bycanvas = new TCanvas("ByField","By (T)",60,0,1200,800);
   bycanvas->cd();
   byHist->Draw("COLZ");
   TCanvas *bzcanvas = new TCanvas("BzFields","Bz (T)",60,0,1200,800);
   bzcanvas->cd();
   bzHist->Draw("COLZ");
   cout << "-------------------------------------------" << endl;
   cout << "Successfully Fetched Field Histograms" << endl;
   cout << "-------------------------------------------" << endl;
   return;
}

