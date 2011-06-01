#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "TDirectory.h"
#include "TClass.h"
#include "TGeoManager.h"
#include "TH1F.h"
#include "TF1.h"
#include "TRint.h"
#include "TCanvas.h"
#include "TPolyMarker3D.h"
#include "TPointSet3D.h"
#include "TPolyLine3D.h"
#include "TFile.h"
#include "TMath.h"
#include "TLegend.h"
#include "TKey.h"
#include "TPaveText.h"
#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"
#include "TTree.h"
#include "TBranch.h"

#include "Particle.h"
#include "ConfigFile.h"
#include "InitialConfig.h"
#include "RunConfig.h"
#include "Track.h"
#include "ParticleManifest.h"

#include "Constants.h"
#include "Units.h"
#include "ValidStates.h"
#include "Algorithms.h"
#include "DataAnalysis.h"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

bool SelectTrackToDraw(const string& filename, const string& state);
void DrawTrack(Track* track, TPolyLine3D* line, TPolyMarker3D* startMarker, TPolyMarker3D* endMarker);

//_____________________________________________________________________________
// A helper function to simplify the printing of command line options.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, "\n")); 
    return os;
}

//_____________________________________________________________________________
Int_t main(Int_t argc,Char_t **argv)
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
      SelectTrackToDraw(variables["file"].as<string>(), variables["state"].as<string>());
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
bool SelectTrackToDraw(const string& filename, const string& state)
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
   // Build the ConfigFile
   const RunConfig& runConfig = Analysis::DataFile::LoadRunConfig(*file);
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Load the Geometry
   TGeoManager& geoManager = Analysis::DataFile::LoadGeometry(*file);
   //////////////////////////////////////////////////////////////////////////////////////
   // Load the Particle Manifest
   const ParticleManifest& manifest = Analysis::DataFile::LoadParticleManifest(*file);
   manifest.Print();
   vector<int> particleIndexes = manifest.GetListing(stateName).GetTreeIndexes();
   //////////////////////////////////////////////////////////////////////////////////////
   // Load the Data Tree
   TTree* dataTree = Analysis::DataFile::LoadParticleDataTree(*file);
   // Ask User to choose which Particle to draw
   cout << "Select which particle's track to draw: " << endl;
   cout << "Options: 'l' -- List all particles in directory" << endl;
   cout.width(9);
   cout << " " << "'q' -- Quit" << endl; 
   cout.width(15);
   
   string userInput;
   
   Track* track = new Track();
   TCanvas* canvas = NULL;
   TPolyLine3D* line = NULL;
   TPointSet3D* endMarker = NULL;
   TPointSet3D* startMarker = NULL;

   while (true) {
      cout << "   -- Enter Particle's ID (or 'q' to quit): ";
      cin >> userInput;
      cout << endl;
      cin.clear();
      // Delete previous resources
      if (track) delete track; track = NULL;
      if (canvas) delete canvas; canvas = NULL;
      if (line) delete line; line = NULL;
      
      if (userInput == "q") {break;}
      if (userInput == "l") {
         cout << "Available particle's: " << endl;
         cout << particleIndexes;
         continue;
      }
      
      int trackIndex = -1;
      if (Algorithms::String::ConvertToInt(userInput, trackIndex) == false) return false;
      
      //////////////////////////////////////////////////////////////////////////////////////
      // Fetch the 'final' state branch from the data tree, and prepare to read particles from it 
      TBranch* trackBranch = dataTree->GetBranch(track->ClassName());
      if (trackBranch == NULL) {
         cerr << "Error - Could not find branch: " << track->ClassName() << " in input tree" << endl;
         throw runtime_error("Failed to find track branch in data tree");
      }
      dataTree->SetBranchAddress(trackBranch->GetName(), &track);
      // Extract Final Particle State Data
      int bytesCopied = trackBranch->GetEntry(trackIndex);
      if (bytesCopied <= 0) return false;
      
      // Check if we found a track for the request index
      if (track == NULL) {
         cout << endl;
         cout << "No track for this particle ID could be found.";
         cout << " Please try again, Or input 'q' to quit." << endl;
      } else {
         canvas = new TCanvas("Positions","Neutron Positions",60,30,400,400);
         canvas->cd();
         geoManager.GetTopVolume()->Draw("ogl");
         geoManager.SetVisLevel(4);
         geoManager.SetVisOption(0);
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
         DrawTrack(track, line, startMarker, endMarker);
         theApp->Run(kTRUE);
         // Reset user input
         userInput = "";
         continue;
      }
   }
   // Clean up
   if (canvas) delete canvas; canvas = NULL;
   if (line) delete line; line = NULL;
   if (track) delete track; track = NULL;
   if (endMarker) delete endMarker; endMarker = NULL;
   if (startMarker) delete startMarker; startMarker = NULL;

   file->Close();
   cout << "Finished" << endl;
   return 0;
}

//_____________________________________________________________________________
void DrawTrack(Track* track, TPolyLine3D* line, TPolyMarker3D* startMarker, TPolyMarker3D* endMarker)
{
   double startTime = -1.0, endTime = -1.0;
   string useWholeTrack;
   int totalPoints = 0;
   vector<Double_t> trackpoints;
   
   Point startPoint;
   Point endPoint;
   while (true) {
      cout << "   -- Do you want to draw the whole track? (y/n): ";
      cin >> useWholeTrack;
      cout << endl;
      cin.clear();
      if (useWholeTrack == "y") {
         trackpoints = track->OutputPointsArray();
         totalPoints = track->TotalPoints();
         startPoint = track->GetPoint(0);
         endPoint = track->GetPoint(track->TotalPoints());
         break;
      } else if (useWholeTrack == "n") {
         cout << "   -- Enter Start time, for where you would like to view the track from: ";
         cin >> startTime;
         cout << endl;
         cin.clear();
         cout << "   -- Enter End Time, for where you would like to view the track up to: ";
         cin >> endTime;
         cout << endl;
         cin.clear();
         // Take segment of track
         cout << "Creating track segment" << endl;
         Track newtrack = track->GetTrackSegment(startTime, endTime);
         trackpoints = newtrack.OutputPointsArray();
         totalPoints = newtrack.TotalPoints();
         startPoint = newtrack.GetPoint(0);
         endPoint = newtrack.GetPoint(newtrack.TotalPoints());
         break;
      } else {
         cout << "Please input 'y' or 'n'" << endl;
      }
   }
   cout << "Number of Points: " << totalPoints << endl;
   line = new TPolyLine3D(totalPoints ,&trackpoints[0]);
   line->SetLineColor(kRed);
   line->SetLineWidth(3);
   line->Draw();
   
   // -- Draw Start and Finish points of track
   if (endMarker) delete endMarker; endMarker = NULL;
   endMarker = new TPointSet3D(1,24);
   if (startMarker) delete startMarker; startMarker = NULL;
   startMarker = new TPointSet3D(1,24);
   startMarker->SetPoint(0, startPoint.X(), startPoint.Y(), startPoint.Z());
   endMarker->SetPoint(0, endPoint.X(), endPoint.Y(), endPoint.Z());
   // Start is BLUE
   startMarker->SetMarkerColor(kBlue);
   // End is GREEN
   endMarker->SetMarkerColor(kGreen-3);
   startMarker->Draw();
   endMarker->Draw();
   cout << "Drawing Start and End Points of Track -- " << endl;
   cout << left << setw(25) << "Start (Blue Marker) -- " << setw(4) << "X: " << setw(10) << startPoint.X() << "\t";
   cout << setw(4) << "Y: " << setw(10) << startPoint.Y() << "\t";
   cout << setw(4) << "Z: " << setw(10) << startPoint.Z() << endl;
   cout << setw(25) << "End (Green Marker) -- " << setw(4) << "X: " << setw(10) << endPoint.X() << "\t";
   cout << setw(4) << "Y: " << setw(10) << endPoint.Y() << "\t";
   cout << setw(4) << "Z: " << setw(10) << endPoint.Z() << endl;
   
   // -- Get the GLViewer so we can manipulate the camera
   TGLViewer* glViewer = dynamic_cast<TGLViewer*>(gPad->GetViewer3D());
   glViewer->UpdateScene();
   return;
}

