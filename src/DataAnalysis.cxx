#include <iostream>
#include <iomanip>
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <math.h>

#include "DataAnalysis.h"

#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TClass.h"
#include "TGeoManager.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TPolyMarker3D.h"
#include "TMath.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"
#include "TLine.h"
#include "TTree.h"

#include "Particle.h"
#include "ConfigFile.h"
#include "RunConfig.h"
#include "SpinData.h"
#include "BounceData.h"
#include "FieldData.h"
#include "ParticleManifest.h"

#include "Algorithms.h"
#include "ValidStates.h"
#include "Constants.h"
#include "Units.h"

#include <boost/foreach.hpp>

using namespace Analysis;
using namespace std;

//_____________________________________________________________________________
TFile* DataFile::OpenRootFile(const std::string filename, const std::string option)
{
   // -- Simply open a Root file with the given name and option and return pointer to it.
   cout << "-------------------------------------------" << endl;
   TFile *file = NULL;
   file = TFile::Open(filename.c_str(), option.c_str());
   if (!file || file->IsZombie()) {
      cerr << "Error: Cannot open file - " << filename << endl;
      return NULL;
   }
   file->cd();
   cout << "Successfully Loaded Data File: " << filename << endl;
   cout << "-------------------------------------------" << endl;
   return file;
}

//_____________________________________________________________________________
const RunConfig& DataFile::LoadRunConfig(TFile& file)
{
   // -- Attempt to read in the runconfig from the top level directory
   cout << "Attempting to load the RunConfig" << endl;
   // -- Loop over all objects in folder and extract latest RunConfig
   RunConfig* ptr_config = NULL;
   TKey *key;
   TIter folderIter(file.GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(folderIter.Next()))) {
      // Check if current item is of class RunConfig
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("RunConfig")) {
         // Read RunConfig into memory when found
         ptr_config = dynamic_cast<RunConfig*>(key->ReadObj());
         break;
      }
   }
   // Throw exception if we failed to find any RunConfig in this folder
   if (ptr_config == NULL) {
      throw runtime_error("Unable to load RunConfig from config folder");
   }
   cout << "Successfully Loaded RunConfig" << endl;
   cout << "-------------------------------------------" << endl;
   return *ptr_config;
}

//_____________________________________________________________________________
TGeoManager& DataFile::LoadGeometry(TFile& file)
{
   // -- Attempt to read in the Geometry from the top level directory
   cout << "Attempting to load the Geometry" << endl;
   TGeoManager* geoManager = NULL;
   TKey *key;
   TIter folderIter(file.GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(folderIter.Next()))) {
      // Check if current item is of Class TGeomanager
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TGeoManager")) {
         // Read TGeoManager into memory when found
         geoManager = dynamic_cast<TGeoManager*>(key->ReadObj());
         break;
      }
   }
   // Throw exception if we failed to find any TGeoManager in this folder
   if (geoManager == NULL) {
      throw runtime_error("Unable to load GeoManager from file");
   }
   cout << "Successfully Loaded Geometry" << endl;
   cout << "-------------------------------------------" << endl;
   return *geoManager;
}

//_____________________________________________________________________________
const ParticleManifest& DataFile::LoadParticleManifest(TFile& file)
{
   // -- Attempt to read in the ParticleManifest from the top level directory
   cout << "Attempting to load the ParticleManifest" << endl;
   ParticleManifest* manifest = NULL;
   TKey *key;
   TIter folderIter(file.GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(folderIter.Next()))) {
      // Check if current item is of Class TGeomanager
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("ParticleManifest")) {
         // Read TGeoManager into memory when found
         manifest = dynamic_cast<ParticleManifest*>(key->ReadObj());
         break;
      }
   }
   // Throw exception if we failed to find any TGeoManager in this folder
   if (manifest == NULL) {
      throw runtime_error("Unable to load ParticleManifest from file");
   }
   cout << "Successfully Loaded ParticleManifest" << endl;
   cout << "-------------------------------------------" << endl;
   return *manifest;
}

//_____________________________________________________________________________
TTree* DataFile::LoadParticleDataTree(TFile& file)
{
   // -- Attempt to read in the ParticleManifest from the top level directory
   cout << "Attempting to load the Particle Data Tree" << endl;
   TTree* tree = NULL;
   TKey *key;
   TIter folderIter(file.GetListOfKeys());
   while ((key = dynamic_cast<TKey*>(folderIter.Next()))) {
      // Check if current item is of Class TGeomanager
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TTree")) {
         // Read TGeoManager into memory when found
         tree = dynamic_cast<TTree*>(key->ReadObj());
         break;
      }
   }
   // Throw exception if we failed to find any TGeoManager in this folder
   if (tree == NULL) {
      throw runtime_error("Unable to load Particle Data Tree from file");
   }
   cout << "Successfully Loaded Particle Data Tree" << endl;
   cout << "-------------------------------------------" << endl;
   return tree;
}

//_____________________________________________________________________________
bool DataFile::IsRootFile(const string filename)
{
   // -- Check that the filename supplied has a .root extension
   size_t found = filename.find_last_of(".");
   if (found == string::npos) {
      cout << "Error - File: " << filename << "is not a valid Root file" << endl;
      return false;
   }
   if (filename.substr(found) == ".root") return true;
   cout << "Error - File: " << filename << "is not a valid Root file" << endl;
   return false;
}
//_____________________________________________________________________________
bool DataFile::IsValidStateName(const vector<string>& statenames)
{
   // -- Check that each statename in list is a valid state as defined
   // -- in DataFileHierarchy lvl 3 and is unique
   vector<string>::const_iterator iter;
   for (iter = statenames.begin(); iter != statenames.end(); iter++) {
      // Check state-name
      if (*iter != States::initial &&
          *iter != States::propagating &&
          *iter != States::absorbed &&
          *iter != States::lost &&
          *iter != States::decayed &&
          *iter != States::detected &&
          *iter != States::anomalous) {
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
bool DataFile::IsValidStateName(const string statename)
{
   // -- Check that each statename in list is a valid state as defined
   // -- in DataFileHierarchy lvl 3 and is unique
   // Check state-name
   if (statename != States::initial &&
         statename != States::propagating &&
         statename != States::absorbed &&
         statename != States::lost &&
         statename != States::decayed &&
         statename != States::detected &&
         statename != States::anomalous) {
      cerr << "Argument, " << statename << " is not a valid statename" << endl;
      return false;
   }
   return true;
}

//_____________________________________________________________________________
string DataFile::ConcatenateStateNames(const vector<string>& states)
{
   // -- Return a string made up of the names of each state in the supplied list
   // -- separated by a '+'
   string result;
   BOOST_FOREACH(string name, states) {
      if (result.empty() == false) result.append("+");
      result.append(name);
   }
   return result;
}

//_____________________________________________________________________________
TDirectory* DataFile::NavigateToHistDir(TFile& file)
{
   // -- Attempt to enter a folder called 'histograms' in the top level directory
   // -- of the given file. If it doesn't exist, create it, and return folder
   TDirectory* histDir = NULL;
   if (file.cd("histograms") == false) {
      histDir = file.mkdir("histograms");
      histDir->cd();
   } else {
      histDir = gDirectory;
   }
   return histDir;
}

//_____________________________________________________________________________
void DataFile::CopyDirectory(TDirectory * const sourceDir, TDirectory * const outputDir) {
   // Copy the source directory and all its subdirectories into the supplied output directory
   // as a new subdirectory  
   TDirectory* copiedDir = outputDir->mkdir(sourceDir->GetName());
   copiedDir->cd();
   // Loop on all entries of this directory
   TKey *key;
   TIter nextkey(sourceDir->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TDirectory")) {
         // Copy subdirectory to a new subdirectoy in copiedDir
         sourceDir->cd(key->GetName());
         TDirectory *subdir = gDirectory;
         copiedDir->cd();
         CopyDirectory(subdir, copiedDir);
         copiedDir->cd();
      } else {
         // Copy Object
         sourceDir->cd();
         TObject *obj = key->ReadObj();
         copiedDir->cd();
         obj->Write();
         delete obj;
     }
  }
  copiedDir->SaveSelf(kTRUE);
  outputDir->cd();
}

//_____________________________________________________________________________
void DataFile::CopyDirectoryContents(TDirectory * const sourceDir, TDirectory * const outputDir) {
   // -- Similar to CopyDirectory but here we only copy all the objects and subdirs of the source
   // -- directory into the supplied output directory, not the source directory itself
   outputDir->cd();
   // Loop on all entries of this directory
   TKey *key;
   TIter nextkey(sourceDir->GetListOfKeys());
   Int_t counter = 0;
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TDirectory")) {
         // Copy subdirectory to a new subdirectoy in outputDir
         sourceDir->cd(key->GetName());
         TDirectory *subdir = gDirectory;
         outputDir->cd();
         TDirectory* outputSubDir = outputDir->mkdir(subdir->GetName());
         CopyDirectoryContents(subdir, outputSubDir);
         outputDir->cd();
      } else {
         // Copy Object
         sourceDir->cd();
         TObject *obj = key->ReadObj();
         outputDir->cd();
         obj->Write();
         delete obj;
      }
      ++counter;
      Algorithms::ProgressBar::PrintProgress(counter,sourceDir->GetNkeys(),1);
   }
   outputDir->SaveSelf(kTRUE);
   outputDir->cd();
}

//_____________________________________________________________________________
double FitFunctions::SpinPrecession(double *x, double *par)
{
   double t = x[0];
   double f = par[0]*TMath::Sin((par[1]*t + par[2]*(TMath::Pi()/2.0)))*TMath::Sin((par[1]*t + par[2]*(TMath::Pi()/2.0)));
   return f;
}

//_____________________________________________________________________________
double FitFunctions::ExponentialDecay(double *x, double *par)
{
   double t = x[0];
   double f = par[0]*TMath::Exp(-t/par[1]);
   return f;
}

//_____________________________________________________________________________
void FinalStates::PlotFinalState(const std::string state, const std::vector<int> particleIndexes, TTree* dataTree, const RunConfig& runConfig)
{
   //////////////////////////////////////////////////////////////////////////////////////
   cout << "Preparing to draw histograms for the final particle state..." << endl;
   // -- Angular Distribution
   Char_t histname[40];
   sprintf(histname,"%s:Theta",state.c_str());
   TH1F* thetaHist = new TH1F(histname,"Direction: Theta, Degrees", 50, 0., 180.);
   thetaHist->SetXTitle("Degrees");
   thetaHist->SetYTitle("Neutrons");
   thetaHist->SetLineColor(kRed);
   thetaHist->SetFillStyle(3001);
   thetaHist->SetFillColor(kRed);
   sprintf(histname,"%s:Phi",state.c_str());
   TH1F* phiHist = new TH1F(histname,"Direction: Phi, Degrees", 50, -180.0, 180.0);
   phiHist->SetXTitle("Degrees");
   phiHist->SetYTitle("Neutrons");
   phiHist->SetLineColor(kRed);
   phiHist->SetFillStyle(3001);
   phiHist->SetFillColor(kRed);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Energy/Momentum
   const double maximumVelocity = 8.0;
   const int nbins = 50;
   sprintf(histname,"%s:Velocity",state.c_str());
   TH1F* energyHist = new TH1F(histname,"Velocity: Units of m/s", nbins, 0.0, maximumVelocity);
   energyHist->SetXTitle("Velocity (m/s)");
   energyHist->SetYTitle("Neutrons");
   energyHist->SetLineColor(kBlack);
   energyHist->SetFillStyle(3001);
   energyHist->SetFillColor(kBlack);
   sprintf(histname,"%s:Vx",state.c_str());
   TH1F* vxHist = new TH1F(histname,"Vx (m/s)", nbins, -maximumVelocity, maximumVelocity);
   vxHist->SetXTitle("Vx (m/s)");
   vxHist->SetYTitle("Neutrons");
   vxHist->SetLineColor(kBlue);
   vxHist->SetFillStyle(3001);
   vxHist->SetFillColor(kBlue);
   sprintf(histname,"%s:Vy",state.c_str());
   TH1F* vyHist = new TH1F(histname,"Vy (m/s)", nbins, -maximumVelocity, maximumVelocity);
   vyHist->SetXTitle("Vy (m/s)");
   vyHist->SetYTitle("Neutrons");
   vyHist->SetLineColor(kBlue);
   vyHist->SetFillStyle(3001);
   vyHist->SetFillColor(kBlue);
   sprintf(histname,"%s:Vz",state.c_str());
   TH1F* vzHist = new TH1F(histname,"Vz (m/s)", nbins, -maximumVelocity, maximumVelocity);
   vzHist->SetXTitle("Vz (m/s)");
   vzHist->SetYTitle("Neutrons");
   vzHist->SetLineColor(kBlue);
   vzHist->SetFillStyle(3001);
   vzHist->SetFillColor(kBlue);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Run Time
   const double runTime = runConfig.RunTime();
   sprintf(histname,"%s:Time",state.c_str());
   TH1F* timeHist = new TH1F(histname,"Time: Units of s", ((int)runTime), 0.0, runTime+1);
   timeHist->SetXTitle("Time (s)");
   timeHist->SetYTitle("Neutrons");
   //////////////////////////////////////////////////////////////////////////////////////
   // Fetch the 'final' state branch from the data tree, and prepare to read particles from it 
   Particle* particle = new Particle();
   TBranch* particleBranch = dataTree->GetBranch(States::final.c_str());
   if (particleBranch == NULL) {
      cerr << "Error - Could not find branch: " << States::final << " in input tree" << endl;
      throw runtime_error("Failed to find particle branch in input tree");
   }
   dataTree->SetBranchAddress(particleBranch->GetName(), &particle);
   // Loop over all selected particles 
   BOOST_FOREACH(int particleIndex, particleIndexes) {
      // Extract Final Particle State Data
      particleBranch->GetEntry(particleIndex);
      // Fill Histograms
      thetaHist->Fill((particle->Theta()*180.0)/TMath::Pi());
      phiHist->Fill((particle->Phi()*180.0)/TMath::Pi());
      energyHist->Fill(particle->V());
      vxHist->Fill(particle->Vx());
      vyHist->Fill(particle->Vy());
      vzHist->Fill(particle->Vz());
      timeHist->Fill(particle->T());
   }
   delete particle; particle = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Draw Histograms
   // Time Distribution
   TCanvas *timecanvas = new TCanvas("Times","Final Time (s)",60,0,1200,800);
   timecanvas->cd();
   timeHist->Draw();
   timeHist->Write(timeHist->GetName(),TObject::kOverwrite);
   //////////////////////////////////////////////////////////////////////////////////////
   // Velocity Distribution
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
   cout << "Successfully created histograms for the final particle state" << endl;
   cout << "-------------------------------------------" << endl;
   return;
}

//_____________________________________________________________________________
void FinalStates::DrawFinalPositions(const std::string state, const std::vector<int> particleIndexes, TTree* dataTree, TGeoManager& geoManager, double* cameraCentre)
{
   //////////////////////////////////////////////////////////////////////////////////////
   cout << "Preparing to draw particle positions in the geometry..." << endl;
   // -- Count total Neutrons, and define name of combined states
   size_t totalNeutrons = particleIndexes.size();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Final Positions
   TPolyMarker3D* points = new TPolyMarker3D(totalNeutrons, 1); // 1 is marker style
   Char_t histname[40];
   sprintf(histname,"%s:NeutronPositions",state.c_str());
   points->SetName(histname);
   points->SetMarkerColor(2);
   points->SetMarkerStyle(6);
   //////////////////////////////////////////////////////////////////////////////////////
   // Fetch the 'final' state branch from the data tree, and prepare to read particles from it 
   Particle* particle = new Particle();
   TBranch* particleBranch = dataTree->GetBranch(States::final.c_str());
   if (particleBranch == NULL) {
      cerr << "Error - Could not find branch: " << States::final << " in input tree" << endl;
      throw runtime_error("Failed to find particle branch in input tree");
   }
   dataTree->SetBranchAddress(particleBranch->GetName(), &particle);
   // Loop over all selected particles 
   BOOST_FOREACH(int particleIndex, particleIndexes) {
      // Extract Final Particle State Data
      particleBranch->GetEntry(particleIndex);
      // Fill Point array
      static int pointNum = 0;
      points->SetPoint(pointNum, particle->X(), particle->Y(), particle->Z());
      pointNum++;
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // Final Positions
   TCanvas *poscanvas = new TCanvas("Positions","Neutron Positions",10,10,10,10);
   poscanvas->cd();
   geoManager.GetTopVolume()->Draw("ogl");
   geoManager.SetVisLevel(4);
   geoManager.SetVisOption(0);
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
   glViewer->SetPerspectiveCamera(camera,4,100, cameraCentre,0,0);
   // -- Draw Reference Point, Axes
   // -- Options for SetGuideState:
   // -- {int axesType = 0(Off), 1(EDGE), 2(ORIGIN)}, {Bool_t axesDepthTest}, {Bool_t referenceOn}, {const double referencePos[3]}
   double refPoint[3] = {0.,0.,0.};
   glViewer->SetGuideState(2, kFALSE, kFALSE, refPoint);
   glViewer->UpdateScene();
   glViewer = 0;
   cout << "Successfully drawn particle positions in the geometry" << endl;
   cout << "-------------------------------------------" << endl;
   return;
}

//_____________________________________________________________________________
bool FinalStates::PlotEmptyingTime(const vector<TDirectory*> stateDirs, const RunConfig& runConfig, const double lLimit, const double uLimit)
{
   //////////////////////////////////////////////////////////////////////////////////////
   Char_t histname[40];
   // -- Count total Neutrons, and define name of combined states
   string stateName = "";//DataFile::ConcatenateStateNames(stateDirs);
   int total_neutrons = 0;
   vector<TDirectory*>::const_iterator dirIter;
   for (dirIter = stateDirs.begin(); dirIter != stateDirs.end(); dirIter++) {
      total_neutrons += (*dirIter)->GetNkeys();
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Run Time
   const double runTime = runConfig.RunTime();
   sprintf(histname,"%s:Time",stateName.c_str());
   TH1F* timeHist = new TH1F(histname,"Time: Units of s", ((int)runTime), lLimit, uLimit);
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
                  timeHist->Fill(particle->T());
                  delete particle;
               }
            }
         }
      }
   }
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Draw Histograms
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Time Distribution
   TCanvas *timecanvas = new TCanvas("Times","Final Time (s)",60,0,600,600);
   timecanvas->cd();
   timeHist->Draw("PE1 X0");
   
   // Fit exponential to Graph
   int numParams = 2;
   TF1* expo = new TF1("Exponential", FitFunctions::ExponentialDecay, lLimit, uLimit, numParams);
   expo->SetParNames("Amplitude","Decay lifetime");
   expo->SetParameters(1.0,1.0);
   timeHist->Fit(expo, "R 0");
   expo->Draw("SAME");
   // Extract Emptying Time
   double t2 = expo->GetParameter(1);
   double t2error = expo->GetParError(1);
   
   cout << "Emptying Lifetime: " << t2 << "\t Error: " << t2error << endl;
   cout << "Chi-Sq: " << expo->GetChisquare() << "\t NDF: " << expo->GetNDF() << endl;
   timecanvas->SaveAs("lifetime.root");
   return true;
}

//_____________________________________________________________________________
void Polarisation::PlotSpinPolarisation(const std::string state, const std::vector<int> particleIndexes, TTree* dataTree, const RunConfig& runConfig) 
{
   //////////////////////////////////////////////////////////////////////////////////////
   cout << "Preparing to draw particle spin polarisation over time..." << endl;
   // -- Define Histograms
   const double runTime = runConfig.RunTime();
   const double spinMeasInterval = runConfig.SpinMeasureInterval();
   const int nbins = ceil(runTime/spinMeasInterval);
   const TVector3 xAxis(1.0,0.0,0.0);
   const TVector3 yAxis(0.0,1.0,0.0);
   const TVector3 zAxis(0.0,0.0,1.0);
   // -- Define name of combined states

   //////////////////////////////////////////////////////////////////////////////////////
   // -- X Axis Spin Polarisation
   Char_t histname[40];
   sprintf(histname,"%s:SpinUp Along X",state.c_str());
   TH1F* spinUpAlongXHist = new TH1F(histname,"SpinUp Along X", nbins, 0.0, runTime);      
   spinUpAlongXHist->SetXTitle("Time (s)");
   spinUpAlongXHist->SetYTitle("Spin Up Neutrons");
   spinUpAlongXHist->SetFillStyle(1001);
   spinUpAlongXHist->SetFillColor(kBlue-7);
   spinUpAlongXHist->SetLineColor(kBlue-7);

   sprintf(histname,"%s:SpinDown Along X",state.c_str());
   TH1F* spinDownAlongXHist = new TH1F(histname,"SpinDown Along X", nbins, 0.0, runTime);      
   spinDownAlongXHist->SetXTitle("Time (s)");
   spinDownAlongXHist->SetYTitle("Spin Down Neutrons");
   spinDownAlongXHist->SetFillStyle(3001);
   spinDownAlongXHist->SetFillColor(kRed-7);
   spinDownAlongXHist->SetLineColor(kRed-7);
   
   sprintf(histname,"%s:SpinUp+Down Along X",state.c_str());
   TH1F* spinUpDownAlongXHist  = new TH1F(histname,"SpinUp+Down Along X", nbins, 0.0, runTime);   
   spinUpDownAlongXHist->SetXTitle("Time (s)");
   spinUpDownAlongXHist->SetYTitle("Spin Up+Down Neutrons");
   spinUpDownAlongXHist->SetFillStyle(3001);
   spinUpDownAlongXHist->SetFillColor(kBlack);
   spinUpDownAlongXHist->SetLineColor(kBlack);
   
   sprintf(histname,"%s:Polarisation Along X",state.c_str());
   TGraph* spinAlphaX = new TGraph(nbins);
   spinAlphaX->SetName(histname);
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Y Axis Spin Polarisation
   sprintf(histname,"%s:SpinUp Along Y",state.c_str());
   TH1F* spinUpAlongYHist = new TH1F(histname,"SpinUp Along Y", nbins, 0.0, runTime);      
   spinUpAlongYHist->SetXTitle("Time (s)");
   spinUpAlongYHist->SetYTitle("Spin Up Neutrons");
   spinUpAlongYHist->SetLineColor(kBlue-7);
   spinUpAlongYHist->SetFillStyle(1001);
   spinUpAlongYHist->SetFillColor(kBlue-7);

   sprintf(histname,"%s:SpinDown Along Y",state.c_str());
   TH1F* spinDownAlongYHist = new TH1F(histname,"SpinDown Along Y", nbins, 0.0, runTime);      
   spinDownAlongYHist->SetXTitle("Time (s)");
   spinDownAlongYHist->SetYTitle("Spin Down Neutrons");
   spinDownAlongYHist->SetLineColor(kRed-7);
   spinDownAlongYHist->SetFillStyle(3001);
   spinDownAlongYHist->SetFillColor(kRed-7);
   
   sprintf(histname,"%s:SpinUp+Down Along Y",state.c_str());
   TH1F* spinUpDownAlongYHist  = new TH1F(histname,"SpinUp+Down Along Y", nbins, 0.0, runTime);   
   spinUpDownAlongYHist->SetXTitle("Time (s)");
   spinUpDownAlongYHist->SetYTitle("Spin Up+Down Neutrons");
   spinUpDownAlongYHist->SetLineColor(kBlack);
   spinUpDownAlongYHist->SetFillStyle(3001);
   spinUpDownAlongYHist->SetFillColor(kBlack);
   
   sprintf(histname,"%s:Polarisation Along Y",state.c_str());
   TGraph* spinAlphaY = new TGraph(nbins);
   spinAlphaY->SetName(histname);
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Z Axis Spin Polarisation
   sprintf(histname,"%s:SpinUp Along Z",state.c_str());
   TH1F* spinUpAlongZHist = new TH1F(histname,"SpinUp Along Z", nbins, 0.0, runTime);      
   spinUpAlongZHist->SetXTitle("Time (s)");
   spinUpAlongZHist->SetYTitle("Spin Up Neutrons");
   spinUpAlongZHist->SetLineColor(kBlue-7);
   spinUpAlongZHist->SetFillStyle(1001);
   spinUpAlongZHist->SetFillColor(kBlue-7);

   sprintf(histname,"%s:SpinDown Along Z",state.c_str());
   TH1F* spinDownAlongZHist = new TH1F(histname,"SpinDown Along Z", nbins, 0.0, runTime);      
   spinDownAlongZHist->SetXTitle("Time (s)");
   spinDownAlongZHist->SetYTitle("Spin Down Neutrons");
   spinDownAlongZHist->SetLineColor(kRed-7);
   spinDownAlongZHist->SetFillStyle(3001);
   spinDownAlongZHist->SetFillColor(kRed-7);
   
   sprintf(histname,"%s:SpinUp+Down Along Z",state.c_str());
   TH1F* spinUpDownAlongZHist  = new TH1F(histname,"SpinUp+Down Along Z", nbins, 0.0, runTime);   
   spinUpDownAlongZHist->SetXTitle("Time (s)");
   spinUpDownAlongZHist->SetYTitle("Spin Up+Down Neutrons");
   spinUpDownAlongZHist->SetLineColor(kBlack);
   spinUpDownAlongZHist->SetFillStyle(3001);
   spinUpDownAlongZHist->SetFillColor(kBlack);
   
   sprintf(histname,"%s:Polarisation Along Z",state.c_str());
   TGraph* spinAlphaZ = new TGraph(nbins);
   spinAlphaZ->SetName(histname);
   
   //////////////////////////////////////////////////////////////////////////////////////
   // Fetch the 'final' state branch from the data tree, and prepare to read particles from it 
   SpinData* data = new SpinData();
   TBranch* spinBranch = dataTree->GetBranch(data->ClassName());
   if (spinBranch == NULL) {
      cerr << "Error - Could not find branch: " << data->ClassName() << " in input tree" << endl;
      throw runtime_error("Failed to find branch in input tree");
   }
   dataTree->SetBranchAddress(spinBranch->GetName(), &data);
   // Loop over all selected particles 
   BOOST_FOREACH(int particleIndex, particleIndexes) {
      // Extract Final Particle State Data
      spinBranch->GetEntry(particleIndex);
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
   }
   delete data; data = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Spin Precession Plots
   // -- X
   TCanvas *spinXcanvas = new TCanvas("SpinAlongX","Spin Polarisation Along X",60,0,1200,800);
   spinXcanvas->Divide(2,2);
   spinXcanvas->cd(1);
   spinUpAlongXHist->Draw();
   spinUpAlongXHist->Write(spinUpAlongXHist->GetName(),TObject::kOverwrite);
   spinXcanvas->cd(2);
   spinDownAlongXHist->Draw();
   spinDownAlongXHist->Write(spinDownAlongXHist->GetName(),TObject::kOverwrite);
   spinXcanvas->cd(3);
   spinUpDownAlongXHist->Draw();
   spinUpDownAlongXHist->Write(spinUpDownAlongXHist->GetName(),TObject::kOverwrite);
   spinXcanvas->cd(4);
   // Calculate polarisation
   assert(spinUpAlongXHist->GetNbinsX() == nbins);
   for (int i = 0; i < spinUpAlongXHist->GetNbinsX(); i++) {
      double binCentre = spinUpAlongXHist->GetBinLowEdge(i);
      double upCounts = spinUpAlongXHist->GetBinContent(i);
      double downCounts = spinDownAlongXHist->GetBinContent(i);
      double totalCounts = upCounts + downCounts;
      double alpha = totalCounts == 0 ? 0.0 : (upCounts - downCounts) / totalCounts;
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
   // -- Y
   TCanvas *spinYcanvas = new TCanvas("SpinAlongY","Spin Polarisation Along Y",60,0,1200,800);
   spinYcanvas->Divide(2,2);
   spinYcanvas->cd(1);
   spinUpAlongYHist->Draw();
//   spinUpAlongYHist->GetXaxis()->SetRangeUser(0.0,0.05);
//   spinUpAlongYHist->GetYaxis()->SetRangeUser(0.0,1000.0);
//   TF1 *upprecession = new TF1("UpSpinPrecessionFunc", FitFunctions::SpinPrecession, 0.0, 0.05, 3);
//   upprecession->SetParameters(1000.0,400.0,1.0);
//   upprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
//   spinUpAlongYHist->Fit(upprecession, "R 0");
//   upprecession->Draw("SAME");
//   double upamplitude = upprecession->GetParameter(0);
//   double upomega = upprecession->GetParameter(1);
//   double upphase = upprecession->GetParameter(2);
//   cout << "------------------------------------" << endl;
//   cout << "Spin Up:" << endl;
//   cout << "Amplitude: " << upamplitude << "\t" << "Omega: " << upomega;
//   cout << "\t" << "Phase: " << upphase << endl;
//   cout << "------------------------------------" << endl;
   spinUpAlongYHist->Write(spinUpAlongYHist->GetName(),TObject::kOverwrite);
   spinYcanvas->cd(2);
   spinDownAlongYHist->Draw();
//   spinDownAlongYHist->GetXaxis()->SetRangeUser(0.0,0.05);
//   spinDownAlongYHist->GetYaxis()->SetRangeUser(0.0,1000.0);
//   TF1 *downprecession = new TF1("DownSpinPrecessionFunc", FitFunctions::SpinPrecession, 0.0, 0.05, 3);
//   downprecession->SetParameters(1000.0,400.0,0.0);
//   downprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
//   spinDownAlongYHist->Fit(downprecession, "R 0");
//   downprecession->Draw("SAME");
//   double downamplitude = downprecession->GetParameter(0);
//   double downomega = downprecession->GetParameter(1);
//   double downphase = downprecession->GetParameter(2);
//   cout << "------------------------------------" << endl;
//   cout << "Spin Down:" << endl;
//   cout << "Amplitude: " << downamplitude << "\t" << "Omega: " << downomega;
//   cout << "\t" << "Phase: " << downphase << endl;   
//   cout << "------------------------------------" << endl;
//   TLegend* leg = new TLegend(0.7,0.91,0.9,0.99); //coordinates are fractions of pad dimensions
//  leg->SetFillColor(0);
//   leg->AddEntry(spinUpAlongYHist,"Spin Up"); // l means line, p shows marker
//   leg->AddEntry(spinDownAlongYHist,"Spin Down");
//   leg->Draw("SAME");
   spinDownAlongYHist->Write(spinDownAlongYHist->GetName(),TObject::kOverwrite);
   spinYcanvas->cd(3);
   spinUpDownAlongYHist->Draw();
   spinUpDownAlongYHist->Write(spinUpDownAlongYHist->GetName(),TObject::kOverwrite);
   spinYcanvas->cd(4);
   // Calculate polarisation
   assert(spinUpAlongYHist->GetNbinsX() == nbins);
   for (int i = 0; i < spinUpAlongYHist->GetNbinsX(); i++) {
      double binCentre = spinUpAlongYHist->GetBinLowEdge(i);
      double upCounts = spinUpAlongYHist->GetBinContent(i);
      double downCounts = spinDownAlongYHist->GetBinContent(i);
      double totalCounts = upCounts + downCounts;
      double alpha = totalCounts == 0 ? 0.0 : (upCounts - downCounts) / totalCounts;
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
   // -- Z
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
      double binCentre = spinUpAlongZHist->GetBinLowEdge(i);
      double upCounts = spinUpAlongZHist->GetBinContent(i);
      double downCounts = spinDownAlongZHist->GetBinContent(i);
      double totalCounts = upCounts + downCounts;
      double alpha = totalCounts == 0 ? 0.0 : (upCounts - downCounts) / totalCounts;
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
   cout << "Successfully drawn particle spin polarisation over time" << endl;
   cout << "-------------------------------------------" << endl;
   return;
}

//_____________________________________________________________________________
void Polarisation::PlotField(const std::string state, const std::vector<int> particleIndexes, TTree* dataTree, const RunConfig& runConfig)
{
   cout << "Preparing to draw particle field measurements over time..." << endl;
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   const double runTime = runConfig.RunTime();
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bx
   Char_t histname[40];
   sprintf(histname,"%s:Field Bx",state.c_str());
   TH2F* bxHist = new TH2F(histname,"Bx", 2000, -1000*Units::uT, 1000*Units::uT, 500, 0.0, runTime);
   bxHist->SetXTitle("Field measured (T)");
   bxHist->SetYTitle("Time (s)");
   bxHist->SetZTitle("Neutrons");
   // -- By
   sprintf(histname,"%s:Field By",state.c_str());
   TH2F* byHist = new TH2F(histname,"By", 2000, -100*Units::uT, 100*Units::uT, 500, 0.0, runTime);
   byHist->SetXTitle("Field measured (T)");
   byHist->SetYTitle("Time (s)");
   byHist->SetZTitle("Neutrons");
   // -- B`
   sprintf(histname,"%s:Field Bz",state.c_str());
   TH2F* bzHist = new TH2F(histname,"Bz", 2000, -100*Units::uT, 100*Units::uT, 500, 0.0, runTime);
   bzHist->SetXTitle("Field measured (T)");
   bzHist->SetYTitle("Time (s)");
   bzHist->SetZTitle("Neutrons");
   //////////////////////////////////////////////////////////////////////////////////////
   // Fetch the 'final' state branch from the data tree, and prepare to read particles from it 
   FieldData* data = new FieldData();
   TBranch* fieldBranch = dataTree->GetBranch(data->ClassName());
   if (fieldBranch == NULL) {
      cerr << "Error - Could not find branch: " << data->ClassName() << " in input tree" << endl;
      throw runtime_error("Failed to find branch in input tree");
   }
   dataTree->SetBranchAddress(fieldBranch->GetName(), &data);
   // Loop over all selected particles 
   BOOST_FOREACH(int particleIndex, particleIndexes) {
      // Extract Final Particle State Data
      fieldBranch->GetEntry(particleIndex);
      FieldData::const_iterator dataIter;
      for (dataIter = data->begin(); dataIter != data->end(); dataIter++) {
         bxHist->Fill((*dataIter)->Fx(),(*dataIter)->T());
         byHist->Fill((*dataIter)->Fy(),(*dataIter)->T());
         bzHist->Fill((*dataIter)->Fz(),(*dataIter)->T());
      }
   }
   delete data; data = NULL;
   //////////////////////////////////////////////////////////////////////////////////////
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
   cout << "Successfully drawn particle field measurements over time" << endl;
   cout << "-------------------------------------------" << endl;
   return;
}

//_____________________________________________________________________________
double Polarisation::CalculateMeanPhase(vector<vector<Polarisation::Coords> >& phase_data, const unsigned int intervalNum)
{
   // -- Calculate the mean phase of the particles alpha.
   // -- Because the angles are a circular quantity, distibuted usually from (-Pi, +Pi]
   // -- we cannot just take the arithmetic mean of each particles angle. Instead to get a
   // -- correct measure of the mean we first take the mean of the points on the unit-sphere
   // -- eg: {cos(theta),sin(theta)}. Then the mean angle is just
   // --          mean{theta} = atan2(mean{cos(theta)}/mean{sin(theta)})
   double sumCosTheta = 0., sumSinTheta = 0., sumTheta = 0.;
   for (unsigned int particleIndex  = 0; particleIndex < phase_data.size(); particleIndex++) {
      sumCosTheta += phase_data[particleIndex][intervalNum].fCosTheta;
      sumSinTheta += phase_data[particleIndex][intervalNum].fSinTheta;
      sumTheta += phase_data[particleIndex][intervalNum].fTheta;
   }
   double meanCosTheta = sumCosTheta/phase_data.size();
   double meanSinTheta = sumSinTheta/phase_data.size();
   double meanPhase = TMath::ATan2(meanSinTheta, meanCosTheta);
   return meanPhase;
}

//_____________________________________________________________________________
double Polarisation::CalculateAlpha(vector<vector<Polarisation::Coords> >& phase_data, const unsigned int intervalNum, const double meanPhase)
{
   // Calculate the number spin up and down based on phase difference
   int numSpinUp = 0, numSpinDown = 0;
   for (unsigned int particleIndex  = 0; particleIndex < phase_data.size(); particleIndex++) {
      // Calculate the angle between each particle's phase and the mean phase
      double phasediff = TMath::Abs(phase_data[particleIndex][intervalNum].fTheta - meanPhase);
      double probSpinDown = (TMath::Cos(phasediff) + 1.0) / 2.0;
      if (gRandom->Uniform(0.0,1.0) < probSpinDown) {
         numSpinDown++;
      } else {
         numSpinUp++;
      }
//       cout << setw(4) << particleIndex << "\t";
//       cout << setw(12) << "Mean Phase: " << setw(6) << meanPhase*180.0/TMath::Pi() << "\t";
//       cout << setw(12) << "Theta: " << setw(6) << phase_data[particleIndex][intervalNum].fTheta*180.0/TMath::Pi() << "\t";
//       cout << setw(12) << "Phase Diff: " << setw(6) << phasediff*180.0/TMath::Pi() << "\t";
//       cout << setw(12) << "ProbSpinDown: " << setw(6) << probSpinDown << "\t";
//       cout << setw(12) << "Up: " << setw(6) << numSpinUp << "\t";
//       cout << setw(12) << "Down: " << setw(6) << numSpinDown << "\t";
//       cout << setw(12) << "Up+Down: " << setw(6) << numSpinUp + numSpinDown << "\t";
//       cout << setw(12) << "Up-Down: " << setw(6) << numSpinUp - numSpinDown << endl;
   }
   // Calculate polarisation at this time
   double alpha = ((double)(numSpinDown - numSpinUp)) / ((double)(numSpinUp + numSpinDown));
   return alpha;
}

//_____________________________________________________________________________
void Polarisation::PlotPhaseAngleSnapShots(vector<vector<Polarisation::Coords> >& phase_data, const unsigned int intervals)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Plot phase snapshots
   cout << "Plotting Phase Angle Snapshots - Setting ROOT to Batch mode" << endl;
   gROOT->SetBatch(true);
   for (unsigned int intervalNum = 0; intervalNum < intervals; intervalNum++) {
      TCanvas *phaseCanvas = new TCanvas("Phase","Phase",60,0,1200,800);
      phaseCanvas->cd();
      Char_t histname[40];
      sprintf(histname,"Total:Phase Distribution: %03i",intervalNum);
      TH1F* nextHistogram = new TH1F(histname, "Phase Angle Distribution",180,-180.,180.);
      nextHistogram->SetXTitle("Phase Angle");
      nextHistogram->SetYTitle("Neutrons");
      nextHistogram->GetYaxis()->SetRangeUser(0,100);
      nextHistogram->SetLineColor(kBlack);
      nextHistogram->SetFillStyle(3001);
      nextHistogram->SetFillColor(kBlack);
      // Loop over all particles for each interval
      vector<vector<Coords> >::iterator particleIter = phase_data.begin();
      for ( ; particleIter != phase_data.end(); particleIter++) {
         double theta = (*particleIter)[intervalNum].fTheta;
         nextHistogram->Fill(theta*180.0/TMath::Pi());
      }
      nextHistogram->Draw();
      // Draw a line to mark where the mean phase is on each histogram
      double meanPhase = Polarisation::CalculateMeanPhase(phase_data, intervalNum);
      TLine line(meanPhase*180.0/TMath::Pi(), 0.0, meanPhase*180.0/TMath::Pi(), 100);
      line.Draw("SAME");
      // Write histogram to file
      string filepath = "images/";
      string filename = filepath + nextHistogram->GetName();
      filename += ".png";
      phaseCanvas->Print(filename.c_str());
      delete phaseCanvas;
      delete nextHistogram;
   }
   cout << "Finished Phase Angle Snapshots - Ending Batch mode" << endl;
   gROOT->SetBatch(false);
   return;
}

//_____________________________________________________________________________
bool Polarisation::CalculateT2(TFile& dataFile, std::vector<std::string> stateNames, double& t2, double& t2error)
{
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Load the RunConfig into memory and store key parameters required
   const RunConfig& runConfig = DataFile::LoadRunConfig(dataFile);
   const double runTime = runConfig.RunTime();
   const double spinMeasInterval = runConfig.SpinMeasureInterval();
   if (spinMeasInterval <= 0 || runTime <= 0) {
      cerr << "Invalid RunTime or SpinMeasInterval defined in RunConfig" << endl;
      return false;
   }
   // Calculate number of measurement intervals from total runtime and length of spin measurement
   const unsigned int intervals = 1 + runTime/spinMeasInterval;
   cout << "Run Time: " << runTime << "\t";
   cout << "Spin Measurement interval length: " << spinMeasInterval << endl;
   cout << "Measurement intervals: " << intervals << endl;
   // Make a list of the folders for each state to be included in analysis
   vector<TDirectory*> stateDirs;
//   if (DataFile::FetchStateDirectories(dataFile, stateNames, stateDirs) == false) return false;
   
   TGraph* alphaT2 = Polarisation::CreateT2AlphaGraph(stateDirs, runTime, intervals);
   // Draw graph
   TDirectory* histDir = DataFile::NavigateToHistDir(dataFile);
   histDir->cd();
   TCanvas *alphaT2canvas = new TCanvas("Alpha T2","Alpha T2",60,0,1200,800);
   alphaT2canvas->cd();
   alphaT2->SetMarkerStyle(7);
   alphaT2->Draw("AP");
   alphaT2->GetXaxis()->SetTitle("Time (s)");
   alphaT2->GetXaxis()->SetRangeUser(0.0,runTime);
   alphaT2->GetYaxis()->SetTitle("Spin Down Polarisation");
   alphaT2->GetYaxis()->SetRangeUser(-1.0,1.0);
   alphaT2->SetTitle("Spin Polarisation after second ramsey pulse (T2)");
   // Write graph to file
   int bytes = alphaT2->Write(alphaT2->GetName(),TObject::kOverwrite);
   if (bytes == 0) {
      cout << "Error: Failed to Write graph: " << alphaT2->GetName() << " to file" << endl;
   } else {
      cout << "Written graph: " << alphaT2->GetName() << " to file" << endl;
   }
   // Fit exponential to Graph
   int numParams = 2;
   TF1* expo = new TF1("Exponential", FitFunctions::ExponentialDecay, 0.0, runTime, numParams);
   expo->SetParNames("Amplitude","Decay lifetime");
   expo->SetParameters(1.0,1.0);
   alphaT2->Fit(expo, "RQ");
   // Extract T2
   t2 = expo->GetParameter(1);
   t2error = expo->GetParError(1);
   cout << "T2: " << t2 << "\t Error: " << t2error << endl;
   // Clean up
   delete alphaT2;
   delete alphaT2canvas;
   return true;
}

//_____________________________________________________________________________
TGraph* Polarisation::CreateT2AlphaGraph(vector<TDirectory*> stateDirs, double runTime, unsigned int intervals)
{
   // Define name of combined states
   string stateName = "";//DataFile::ConcatenateStateNames(stateDirs);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Define Histograms
   // Define axes of coordinate system
   const TVector3 xAxis(1.0,0.0,0.0);
   const TVector3 yAxis(0.0,1.0,0.0);
   const TVector3 zAxis(0.0,0.0,1.0);      
   //////////////////////////////////////////////////////////////////////////////////////
   TH1F time_data("T2 Time Data","T2 Time Data", intervals, 0.0, runTime);
   vector<vector<Coords> > phase_data;
   TGraph* alphaT2 = new TGraph(intervals);
   Char_t histname[40];
   sprintf(histname,"%s:T2_Polarisation",stateName.c_str());
   alphaT2->SetName(histname);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over each state to be included in histogram
   vector<TDirectory*>::const_iterator dirIter;
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
                  assert(data->size() == intervals);
                  // Create storage for this particle's phase information
                  vector<Coords> phases;
                  // Loop over spin data recorded for particle
                  SpinData::const_iterator dataIter;
                  for (dataIter = data->begin(); dataIter != data->end(); dataIter++) {
                     // Bin the time in the histogram 
                     time_data.Fill(dataIter->first);
                     // -- For a holding Field aligned along the X-Axis, we want to find the
                     // -- phase of the spin in the Y-Z plane. 
                     const Spin* spin = dataIter->second;
                     // Calculate probability of spin up along Y axis
                     double yprob = spin->CalculateProbSpinUp(yAxis);
                     // Calculate probability of spin up along Z axis
                     double zprob = spin->CalculateProbSpinUp(zAxis);
                     // Remap probabilities, [0,1] into a unit vector in the y-z plane [-1,1] 
                     double ycoord = (2.0*yprob - 1.0);
                     double zcoord = (2.0*zprob - 1.0);
                     // Normalize vector
                     double mag = ycoord*ycoord + zcoord*zcoord;
                     assert(Algorithms::Precision::IsNotEqual(mag,0.0));
                     ycoord = ycoord/mag;
                     zcoord = zcoord/mag;
                     // Calculate theta
                     double theta = TMath::ATan2(zcoord,ycoord);
                     // Store the current coordinates on the unit circle in the y-z plane
                     Coords currentCoords;
                     currentCoords.fSinTheta = zcoord;
                     currentCoords.fCosTheta = ycoord;
                     currentCoords.fTheta = theta;
                     // Add set of y-z coords to list
                     phases.push_back(currentCoords);
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
   // -- Plot snapshots of the particles phase distribution over time
   Analysis::Polarisation::PlotPhaseAngleSnapShots(phase_data,intervals);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Calculate the polarisation, alpha, at each measurement interval
   cout << setw(12) << "IntervalNum" << "\t" << setw(12) << "Alpha";
   cout << setw(12) << "Mean Phase" << "\t" << "Old Mean Phase" << endl;
   for (unsigned int intervalNum = 0; intervalNum < intervals; intervalNum++) {
      double meanPhase = Polarisation::CalculateMeanPhase(phase_data, intervalNum);
      double alpha = Polarisation::CalculateAlpha(phase_data, intervalNum, meanPhase);
      double timebin = time_data.GetBinLowEdge(intervalNum);
      // Add point to graph
      cout << setw(4) << intervalNum << "\t";
      cout << setw(12) << "Alpha: " << setw(6) << alpha << "\t";
      cout << setw(12) << "Mean Phase: " << setw(6) << meanPhase*180.0/TMath::Pi() << "\t";
      alphaT2->SetPoint(intervalNum, timebin, alpha);
   }
   return alphaT2;
}

//_____________________________________________________________________________
bool Polarisation::PlotT2_vs_Runs(string configFileName, string statename)
{
   // Read in list of states to be included in histogram and check that they are valid state names
   vector<string> stateNames;
   stateNames.push_back(statename);
   if (Analysis::DataFile::IsValidStateName(stateNames) == false) {
      cerr << "Error: statenames supplied are not valid" << endl;
      return false;
   }
   // Build the ConfigFile
   ConfigFile configFile(configFileName);
   // Fetch the Number of Runs
   const int numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
   if (numberOfRuns < 1) {
      cerr << "Cannot read valid number of runs from ConfigFile: " << numberOfRuns << endl;
      return false;
   }
   // Make T2 graph
   TGraphErrors* graph = new TGraphErrors(numberOfRuns);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Loop over runs specified in ConfigFile
   for (int runNum = 1; runNum <= numberOfRuns; runNum++) {
      // Load Run Config for this run
      RunConfig runConfig(configFile, runNum);
      // Fetch this run's Data file
      const string dataFileName = runConfig.OutputFileName();
      TFile* dataFile = Analysis::DataFile::OpenRootFile(dataFileName, "UPDATE");
      // Calculate T2 for this run
      double t2 = 0., t2error = 0.;
      if (Analysis::Polarisation::CalculateT2(*dataFile, stateNames, t2, t2error) == false) {
         cerr << "Failed to calculate T2 for datafile: " << dataFileName << endl;
         return false;
      }
      // Add T2 to graph
      cout << "T2: " << t2 << "\t" << "Error: " << t2error << endl;
      graph->SetPoint(runNum-1, runNum, t2);
      graph->SetPointError(runNum-1, 0, t2error);
      // Close File
      dataFile->Close();
      delete dataFile;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // Draw Graph
   TCanvas* canvas = new TCanvas("Phase","Phase",60,0,1200,800);
   canvas->cd();
   graph->SetMarkerStyle(8);
   graph->Draw("AP");
   graph->GetXaxis()->SetTitle("Config Num");
   graph->GetYaxis()->SetTitle("T2 (s)");
   graph->GetYaxis()->SetRangeUser(0.0, 20.0);
   graph->SetTitle("T2");
   // Write canvas to file
   canvas->SaveAs("t2.png");
   // Clean up graph
   delete graph;
   delete canvas;
   return true;
}

//_____________________________________________________________________________
void Bounces::PlotBounceCounters(TDirectory* const histDir, const vector<TDirectory*> stateDirs)
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
   TH1F* bounceHist = new TH1F(histname,"Bounces", 100, 0.0, 10000.0);
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
   diffHist->Write(diffHist->GetName(),TObject::kOverwrite);
   return;
}

//_____________________________________________________________________________
void Tracks::PlotParticleHistories(TDirectory* const histDir, const vector<TDirectory*> stateDirs, TGeoManager& geoManager)
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
                  vector<double> times = CalculateParticleHistory(*track, geoManager);
                  const Point& lastPoint = track->GetPoint(track->TotalPoints());
                  double totalTime = lastPoint.T();
                  delete track;
                  double sourcePercent = times[0]/totalTime;
                  double transferPercent = times[1]/totalTime;
                  double ramseyPercent = times[2]/totalTime;
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
vector<double> Tracks::CalculateParticleHistory(const Track& track, TGeoManager& geoManager) {
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
   
   double previousTime = 0.;
   double nextTime = 0.;
   double timeInSource = 0.;
   double timeInTransferSec = 0.;
   double timeInRamseyCell = 0.;
   
   for (unsigned int pointNum = 0; pointNum < track.TotalPoints(); pointNum++) {
      const Point& point = track.GetPoint(pointNum);
      TGeoNode* node = geoManager.FindNode(point.X(), point.Y(), point.Z());
      TGeoVolume* volume = node->GetVolume();
      map<string, string>::iterator key = regionList.find(volume->GetName());
      if (key == regionList.end()) {
         continue;
      }
      nextTime = point.T();
      double deltaT = nextTime - previousTime;
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
   vector<double> times;
   times.push_back(timeInSource);
   times.push_back(timeInTransferSec);
   times.push_back(timeInRamseyCell);
   return times;
}


