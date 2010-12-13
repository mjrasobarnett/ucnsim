#include <iostream>
#include <sstream>
#include <string>

#include "TGeoManager.h"
#include "TH1F.h"
#include "TF1.h"
#include "TRint.h"
#include "TCanvas.h"
#include "TPolyMarker3D.h"
#include "TPointSet3D.h"
#include "TFile.h"
#include "TMath.h"
#include "TLegend.h"
#include "TKey.h"
#include "TPaveText.h"
#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"

#include "TUCNParticle.h"
#include "TUCNConfigFile.h"
#include "TUCNInitialConfig.h"
#include "TUCNRunConfig.h"
#include "TUCNObservables.h"

#include "Constants.h"
#include "Units.h"
#include "DataFileHierarchy.h"

using namespace std;

namespace Plot {
   TUCNTrackObservables* track = NULL;
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
      cerr << "draw_tracks <configfile.cfg> <treename>" << endl;
      return -1;
   }
   const char* runName="Run1";
   
   // Start 'the app' -- this is so we are able to enter into a ROOT session
   // after the program has run, instead of just quitting.
   TRint *theApp = new TRint("FittingApp", &argc, argv);
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   TUCNConfigFile configFile(configFileName.Data());
   const string initialConfigFileName = configFile.GetString("Config","Initialisation");
   const string runConfigFile = configFile.GetString("Config",runName);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Read in Initial Configuration from file.
   TUCNInitialConfig initialConfig(initialConfigFileName);
   TUCNRunConfig runConfig(runConfigFile);
   // Read the outputfile name
   TString dataFileName = runConfig.OutputFileName();
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
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
   // Ask User to choose which Particle to draw
   cout << "Select which particle's track to draw: " << endl;
   cout << "Options: 'l' -- List all particles in directory" << endl;
   cout.width(9);
   cout << " " << "'q' -- Quit" << endl; 
   cout.width(15);
   string userInput;   
   while (userInput != "q") {
      cout << "   -- Enter Particle's ID: ";
      cin >> userInput;
      cout << endl;
      if (userInput == "l") {stateDir->ls(); continue;}
      // Loop over particle folders in stateDir for a folder that matches the provided ID
      TKey *folderKey;
      TIter folderIter(stateDir->GetListOfKeys());
      while ((folderKey = dynamic_cast<TKey*>(folderIter.Next()))) {
         cout << folderKey->GetName() << endl;
         cout << userInput.c_str() << endl;
         istringstream s_ID(folderKey->GetName());
         istringstream s_User(userInput);
         int p_ID = 0;
         int user = 0;
         s_ID >> p_ID;
         s_User >> user;
         if (p_ID == user) {
            // Loop over all objects in particle dir
            stateDir->cd(folderKey->GetName());
            TDirectory* particleDir = gDirectory;
            TKey *objKey;
            TIter objIter(particleDir->GetListOfKeys());
            while ((objKey = static_cast<TKey*>(objIter.Next()))) {
               // For Each object in the particle's directory, check its class name and what it
               // inherits from to determine what to do.
               const char *classname = objKey->GetClassName();
               TClass *cl = gROOT->GetClass(classname);
               if (!cl) continue;
               if (cl->InheritsFrom("TUCNTrackObservables")) {
                  Plot::track = dynamic_cast<TUCNTrackObservables*>(objKey->ReadObj());
                  break;
               }
            }
            break;
         }
      }
      // Check if we found a track for thi
      if (Plot::track == NULL) {
         cout << endl;
         cout << "No track for this particle ID could be found.";
         cout << " Please try again, Or input 'q' to quit." << endl;
      } else {
         break;
      }
   }
   file->Close();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Draw Track
   //////////////////////////////////////////////////////////////////////////////////////
   if (Plot::track != NULL) {
      
      TCanvas *poscanvas = new TCanvas("Positions","Neutron Positions",60,30,400,400);
      poscanvas->cd();
      geoManager->GetTopVolume()->Draw("ogl");
      geoManager->SetVisLevel(4);
      geoManager->SetVisOption(0);
      
      Plot::track->SetLineColor(kRed);
      Plot::track->Draw();
      
      TPointSet3D* endMarker = new TPointSet3D(1,24);
      TPointSet3D* startMarker = new TPointSet3D(1,24);      
      startMarker->SetMarkerColor(kBlue);
      endMarker->SetMarkerColor(kGreen-3);
      
      Int_t end = Plot::track->GetN();
      Float_t* points = Plot::track->GetP();
      startMarker->SetPoint(0, points[0], points[1], points[2]);
      endMarker->SetPoint(0, points[end+0], points[end+1], points[end+2]);
      
      startMarker->Draw();
      endMarker->Draw();
      
      cout << "Start: " << points[0] << "\t" << points[1] << "\t" << points[2] << endl; 
      cout << "End: " << points[end+0] << "\t" << points[end+1] << "\t" << points[end+2] << endl;       
      
      
      // -- Get the GLViewer so we can manipulate the camera
      TGLViewer * glViewer = dynamic_cast<TGLViewer*>(gPad->GetViewer3D());
      // -- Select Draw style 
      glViewer->SetStyle(TGLRnrCtx::kFill);
      // -- Set Background colour
      glViewer->SetClearColor(kBlack);
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
   }
   cout << "Finished" << endl;
   theApp->Run();
   return 0;
}