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
#include "ValidStates.h"
#include "Algorithms.h"
#include "DataAnalysis.h"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

bool draw_plots(const string& filename, const string& state);
bool AttemptDrawingPositions(TFile& file, const string& stateName);
bool AttemptDrawingVelocity(TFile& file, const string& stateName);
bool AttemptDrawingPolarisation(TFile& file, const string& stateName);
bool AttemptDrawingBounces(TFile& file, const string& stateName);
bool AttemptDrawingFields(TFile& file, const string& stateName);

//_____________________________________________________________________________
int main(int argc, char **argv)
{
   try {
      // -- Create a description for all command-line options
      po::options_description description("Allowed options");
      description.add_options()
        ("help", "produce help message")
        ("file", po::value<string>(), "filename of the datafile to be examined")
        ("state", po::value<string>(), "state to be searched for in histogram folder");
      ;
      
      // -- Create a description for all command-line options
      po::variables_map variables;
      po::store(po::parse_command_line(argc, argv, description), variables);
      po::notify(variables);
      
      // -- If user requests help, print the options description
      if (variables.count("help")) {
         cout << description << "\n";
         return 1;
      }
      
      // -- Check whether a datafile was given. If not, exit with a warning
      if (variables.count("file")) {
         cout << "DataFile name was set to: "
              << variables["file"].as<string>() << "\n";
      } else {
         cout << "DataFile name was not set.\n";
         return EXIT_FAILURE;
      }
      
      // -- Check whether a list of states was given. If not, exit with a warning
      if (variables.count("state")) {
         cout << "State to be searched for is: " 
              << variables["state"].as<string>() << "\n";
      } else {
         cout << "No states have been selected.\n";
         return EXIT_FAILURE;
      }
      // -- Call draw_plots with filename and statename as args
      draw_plots(variables["file"].as<string>(), variables["state"].as<string>());
   }
   catch(exception& e) {
      cerr << "error: " << e.what() << "\n";
      return 1;
   }
   catch(...) {
      cerr << "Exception of unknown type!\n";
   }
   return EXIT_SUCCESS;
}

//_____________________________________________________________________________
bool draw_plots(const string& filename, const string& state)
{
   // Start an interactive root session so we can view the plots as they are made
   TRint *theApp = new TRint("FittingApp", NULL, NULL);
   // Read in Filename and check that it is a .root file
   if (Analysis::DataFile::IsRootFile(filename) == false) {return false;}
   // Read in list of states to be included in histogram and check that they are
   // valid state names
   if (Analysis::DataFile::IsValidStateName(state) == false) {return false;}
   string stateName = boost::to_lower_copy(state);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Open Data File
   TFile* file = Analysis::DataFile::OpenRootFile(filename,"UPDATE");
   ///////////////////////////////////////////////////////////////////////////////////////
   AttemptDrawingPositions(*file, stateName);
   AttemptDrawingVelocity(*file, stateName);
   AttemptDrawingPolarisation(*file, stateName);
   AttemptDrawingBounces(*file, stateName);
   AttemptDrawingFields(*file, stateName);
   //////////////////////////////////////////////////////////////////////////////////////
   theApp->Run();
   return true;
}

//_____________________________________________________________________________
bool AttemptDrawingPositions(TFile& file, const string& stateName) 
{
   // -- Attempt to navigate to Histogram Directory
   TDirectory* histDir = Analysis::DataFile::NavigateToHistDir(file);
   // -- Loop over all objects in histogram folder
   TPolyMarker3D* positions = NULL;
   TKey *key;
   TIter iter(histDir->GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(iter.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TPolyMarker3D")) {
         // If we found a TPolyMarker3D object, check its name to see if it
         // belongs to the requested state
         string objectName = key->GetName();
         vector<string> stringTokens = Algorithms::String::FactorString(objectName, ':');
         if (stringTokens.empty() == false) {
            if (stringTokens[0] == stateName) {
               positions = dynamic_cast<TPolyMarker3D*>(key->ReadObj());
               break;
            }
         }
      }
   }
   // -- Check whether we retrieved anything for the requested state
   if (positions == NULL) {
      cerr << "No Positions saved for state: " << stateName << endl;
      return false;
   }
   // -- Fetch the geometry
   TGeoManager& geoManager = Analysis::DataFile::LoadGeometry(file);
   TCanvas* canvas = new TCanvas("Positions","Neutron Positions",10,10,10,10);
   double camera[3] = {0.0,0.0,0.0};
   Analysis::Geometry::DrawGeometry(*canvas, geoManager, camera);
   positions->Draw();
   return true;
}

//_____________________________________________________________________________
bool AttemptDrawingVelocity(TFile& file, const string& stateName) 
{
   // -- Attempt to navigate to Histogram Directory
   TDirectory* histDir = Analysis::DataFile::NavigateToHistDir(file);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Velocity
   TH1F* vHist = NULL;
   TH1F* vxHist = NULL;
   TH1F* vyHist = NULL;
   TH1F* vzHist = NULL;   
   // -- Loop over all objects in histogram folder
   TKey *key;
   TIter iter(histDir->GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(iter.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TH1F")) {
         // If we found a Histogram object, check its name to see if it
         // belongs to the requested state
         string objectName = key->GetName();
         vector<string> stringTokens = Algorithms::String::FactorString(objectName, ':');
         if (stringTokens.size() == 2) {
            if (stringTokens[0] == stateName) {
               if (stringTokens[1] == "Velocity") {
                  vHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "Vx") {
                  vxHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "Vy") {
                  vyHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "Vz") {
                  vzHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else {
                  continue;
               }
            }
         }
      }
   }
   // -- Check whether we retrieved anything for the requested state
   if (vHist == NULL || vxHist == NULL || vyHist == NULL || vzHist == NULL) {
      cerr << "One or more velocity histograms are missing for: " << stateName << endl;
      return false;
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Draw Histograms
   TCanvas* canvas = new TCanvas("Velocity","Velocity of particles",60,0,1200,800);
   canvas->Divide(2,2);
   canvas->cd(1);
   vxHist->Draw();
   canvas->cd(2);
   vyHist->Draw();
   canvas->cd(3);
   vzHist->Draw();
   canvas->cd(4);
   vHist->Draw();
   return true;
}

//_____________________________________________________________________________
bool AttemptDrawingPolarisation(TFile& file, const string& stateName)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Attempt to navigate to Histogram Directory
   TDirectory* histDir = Analysis::DataFile::NavigateToHistDir(file);
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
   // -- Loop over all objects in histogram folder
   TKey *key;
   TIter iter(histDir->GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(iter.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TH1F")) {
         // If we found a Histogram object, check its name to see if it
         // belongs to the requested state
         string objectName = key->GetName();
         vector<string> stringTokens = Algorithms::String::FactorString(objectName, ':');
         if (stringTokens.size() == 2) {
            if (stringTokens[0] == stateName) {
               if (stringTokens[1] == "SpinUp Along X") {
                  spinUpAlongXHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "SpinDown Along X") {
                  spinDownAlongXHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "SpinUp+Down Along X") {
                  spinUpDownAlongXHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "SpinUp Along Y") {
                  spinUpAlongYHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "SpinDown Along Y") {
                  spinDownAlongYHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "SpinUp+Down Along Y") {
                  spinUpDownAlongYHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "SpinUp Along Z") {
                  spinUpAlongZHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "SpinDown Along Z") {
                  spinDownAlongZHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "SpinUp+Down Along Z") {
                  spinUpDownAlongZHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "Polarisation Along X") {
                  spinAlphaX = dynamic_cast<TGraph*>(key->ReadObj());
               } else if (stringTokens[1] == "Polarisation Along Y") {
                  spinAlphaY = dynamic_cast<TGraph*>(key->ReadObj());
               } else if (stringTokens[1] == "Polarisation Along Z") {
                  spinAlphaZ = dynamic_cast<TGraph*>(key->ReadObj());
               } else {
                  continue;
               }
            }
         }
      }
   }
   // -- Check whether we retrieved anything for the requested state
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
       spinAlphaZ == NULL) 
   {
      cerr << "One or more spin histograms are missing for: " << stateName << endl;
      return false;
   }
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
   return true;
}

//_____________________________________________________________________________
bool AttemptDrawingBounces(TFile& file, const string& stateName)
{
   // -- Attempt to navigate to Histogram Directory
   TDirectory* histDir = Analysis::DataFile::NavigateToHistDir(file);
   // -- Bounces
   TH1F* bounceHist = NULL;
   TH1F* specHist = NULL;
   TH1F* diffHist = NULL;
   // -- Loop over all objects in histogram folder
   TKey *key;
   TIter iter(histDir->GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(iter.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TH1F")) {
         // If we found a Histogram object, check its name to see if it
         // belongs to the requested state
         string objectName = key->GetName();
         vector<string> stringTokens = Algorithms::String::FactorString(objectName, ':');
         if (stringTokens.size() == 2) {
            if (stringTokens[0] == stateName) {
               if (stringTokens[1] == "Bounces") {
                  bounceHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "Specular") {
                  specHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else if (stringTokens[1] == "Diffuse") {
                  diffHist = dynamic_cast<TH1F*>(key->ReadObj());
               } else {
                  continue;
               }
            }
         }
      }
   }
   if (bounceHist == NULL || specHist == NULL || diffHist == NULL) {
      cerr << "No Bounce Histograms found for state: " << stateName << endl;
      return false;
   }
   // -- Bounce Counters
   TCanvas *bouncecanvas = new TCanvas("Bounces","Bounce counters",60,0,800,600);
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
   return true;
}

//_____________________________________________________________________________
bool AttemptDrawingFields(TFile& file, const string& stateName)
{
   // -- Attempt to navigate to Histogram Directory
   TDirectory* histDir = Analysis::DataFile::NavigateToHistDir(file);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- B Field
   TH2F* bxHist = NULL;
   TH2F* byHist = NULL;
   TH2F* bzHist = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all objects in histogram folder
   TKey *key;
   TIter iter(histDir->GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(iter.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TH1F")) {
         // If we found a Histogram object, check its name to see if it
         // belongs to the requested state
         string objectName = key->GetName();
         vector<string> stringTokens = Algorithms::String::FactorString(objectName, ':');
         if (stringTokens.size() == 2) {
            if (stringTokens[0] == stateName) {
               if (stringTokens[1] == "Field Bx") {
                  bxHist = dynamic_cast<TH2F*>(key->ReadObj());
               } else if (stringTokens[1] == "Field By") {
                  byHist = dynamic_cast<TH2F*>(key->ReadObj());
               } else if (stringTokens[1] == "Field Bz") {
                  bzHist = dynamic_cast<TH2F*>(key->ReadObj());
               } else {
                  continue;
               }
            }
         }
      }
   }
   if (bxHist == NULL || byHist == NULL || bzHist == NULL) {
      cerr << "No Field Histograms found for state: " << stateName << endl;
      return false;
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- cd back into Histogram's dir
   histDir->cd();
   TCanvas *canvas = new TCanvas("BxField","Bx (T)",60,0,1000,800);
   canvas->Divide(3,1);
   canvas->cd(1);
   bxHist->Draw("COLZ");
   canvas->cd(2);
   byHist->Draw("COLZ");
   canvas->cd(3);
   bzHist->Draw("COLZ");
   return true;
}
