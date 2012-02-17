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

#include "Particle.hpp"
#include "ConfigFile.hpp"
#include "InitialConfig.hpp"
#include "RunConfig.hpp"
#include "Track.hpp"
#include "ParticleManifest.hpp"

#include "Constants.hpp"
#include "Units.hpp"
#include "ValidStates.hpp"
#include "Algorithms.hpp"
#include "DataAnalysis.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

bool SelectTrackToDraw(const string& filename, const string& state);
void UpdateLine(Track* track, TPolyLine3D& line);
void DetermineStartAndEndPoints(Track* track, TPointSet3D& startMarker, TPointSet3D& endMarker);
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
   if (file == NULL) return EXIT_FAILURE;
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
   // Draw Geometry
   TCanvas canvas("Positions","Neutron Positions",60,30,400,400);
   Double_t cameraCentre[3] = {0,0,0};
   Analysis::Geometry::DrawGeometry(canvas, geoManager, cameraCentre);
   
   TPolyLine3D line;
   line.SetLineColor(kRed);
   line.SetLineWidth(3);

   // Start is BLUE
   TPointSet3D startMarker(1,24);
   startMarker.SetMarkerColor(kBlue+2);
   startMarker.SetMarkerSize(3);
   // End is ORANGE
   TPointSet3D endMarker(1,24);
   endMarker.SetMarkerColor(kOrange-3);
   endMarker.SetMarkerSize(3);
   
   // Draw the line, and start/end markers on current pad
   line.Draw();
   startMarker.Draw();
   endMarker.Draw();
   while (true) {
      cout << "   -- Enter Particle's ID (or 'q' to quit): ";
      cin >> userInput;
      cout << endl;
      cin.clear();
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
         // Work out how much of track to draw
         DetermineStartAndEndPoints(track, startMarker, endMarker);
         // Draw Track
         UpdateLine(track, line);
         // -- Update scene
         TGLViewer* glViewer = dynamic_cast<TGLViewer*>(gPad->GetViewer3D());
         glViewer->UpdateScene();
         // -- Enter interactive session
         theApp->Run(kTRUE);
         // Reset user input
         userInput = "";
         continue;
      }
   }
   cout << "Finished" << endl;
   // Clean up
   if (track) delete track; track = NULL;
   file->Close();
   return 0;
}

//_____________________________________________________________________________
void DetermineStartAndEndPoints(Track* track, TPointSet3D& startMarker, TPointSet3D& endMarker)
{
   double startTime = -1.0, endTime = -1.0;
   string useWholeTrack;
   while (true) {
      cout << "   -- Do you want to draw the whole track? (y/n): ";
      cin >> useWholeTrack;
      cout << endl;
      cin.clear();
      if (useWholeTrack == "y") {
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
         if (track->Truncate(startTime, endTime) == false) {
            cerr << "Invalid times given" << endl;
            continue;
         };
         break;
      } else {
         cout << "Please input 'y' or 'n'" << endl;
      }
   }
   // -- Define markers for the Start and Finish points of track
   Point startPoint = track->GetPoint(0);
   Point endPoint = track->GetPoint(track->TotalPoints());
   startMarker.SetPoint(0, startPoint.X(), startPoint.Y(), startPoint.Z());
   endMarker.SetPoint(0, endPoint.X(), endPoint.Y(), endPoint.Z());
   cout << "Start and End Points of Track -- " << endl;
   cout << left << setw(25) << "Start (Blue Marker) -- " << setw(4) << "X: " << setw(10) << startMarker.GetP()[0] << "\t";
   cout << setw(4) << "Y: " << setw(10) << startMarker.GetP()[1] << "\t";
   cout << setw(4) << "Z: " << setw(10) << startMarker.GetP()[2] << endl;
   cout << setw(25) << "End (Green Marker) -- " << setw(4) << "X: " << setw(10) << endMarker.GetP()[0] << "\t";
   cout << setw(4) << "Y: " << setw(10) << endMarker.GetP()[1] << "\t";
   cout << setw(4) << "Z: " << setw(10) << endMarker.GetP()[2] << endl;
   return;
}

//_____________________________________________________________________________
void UpdateLine(Track* track, TPolyLine3D& line)
{
   vector<Double_t> trackpoints = track->OutputPointsArray();
   cout << "Number of Points: " << track->TotalPoints() << endl;
   line.SetPolyLine(track->TotalPoints(), &trackpoints[0]);
   return;
}

