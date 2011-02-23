#include <iostream>
#include <string>
#include <memory>

#include "gsl/gsl_poly.h"

#include "ConfigFile.h"
#include "Run.h"
#include "Particle.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TRint.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "UniformMagField.h"
#include "MagFieldManager.h"
#include "Data.h"
#include "FieldMap.h"
#include "FileParser.h"
#include "Tube.h"
#include "Polynomial.h"

#include "Constants.h"
#include "Units.h"
#include "GeomParameters.h"

using namespace std;
using namespace GeomParameters;

int TestFieldManager();
bool TestFieldMap();

//__________________________________________________________________________
Int_t main(Int_t /*argc*/,Char_t ** /*argv*/)
{
 
   return EXIT_SUCCESS;
}

//__________________________________________________________________________
void TestPolynomial()
{
//   Double_t params[5] = {1.0, 6.0, 10.0, 1.0, 1.0};
   Double_t params[5] = {1.0, 4.0, 3.0, 4.0, 1.0};
   Double_t roots[4] = {0.,0.,0.,0.};
   Polynomial::Instance()->QuarticRootFinder(params, roots);

//   Double_t params[4] = {1.0, 4.0, 3.0, 1.0};
//   Double_t roots[3] = {0.,0.,0.};
//   Polynomial::Instance()->CubicRootFinder(params, roots);

}

//__________________________________________________________________________
bool TestFieldMap()
{
   // Create a Uniform Magnetic field and write it to file
   
   TGeoShape* fieldShape = new Tube("SolenoidFieldShape",hvCellRMin, hvCellRMax, hvCellHalfZ);
   TGeoRotation hvCellRot("HVCellRot", 0., 0., 0.);
   TGeoTranslation hvCellTra("HVCellTra", 0., 0., 0.);
   TGeoCombiTrans hvCellCom(hvCellTra,hvCellRot);
   TGeoHMatrix hvCellMat = hvCellCom;   
   TGeoMatrix* matrix = new TGeoHMatrix(hvCellMat);
   
   
   string filename = "runs/spins/ramseycell_fieldmap_raw.txt";
   MagFieldMap* field = new MagFieldMap("FieldMap", fieldShape, matrix);
   if (field->BuildMap(filename) == kFALSE) {
      Error("BuildFieldMap","Cannot open file: %s", filename.c_str());
      return kFALSE;
   }
   
   // Add field to magfield manager
   MagFieldManager* magFieldManager = new MagFieldManager();
   magFieldManager->AddField(field);
   
   // -- Write magfieldmanager to geometry file
   const char *magFileName = "$(UCN_DIR)/geom/ramseycell_fields.root";
   TFile *f = TFile::Open(magFileName,"recreate");
   if (!f || f->IsZombie()) {
     Error("BuildFieldMap","Cannot open file: %s", magFileName);
     return kFALSE;
   }
   cout << sizeof(*magFieldManager) << endl;
   magFieldManager->Write(magFieldManager->GetName());
   f->ls();
   f->Close();
   if (magFieldManager) delete magFieldManager;
   magFieldManager = NULL;
   f = NULL;
   
   f = TFile::Open(magFileName,"read");
   f->GetObject("MagFieldManager",magFieldManager);
   f->ls();
   f->Close();
   cout << sizeof(*magFieldManager) << endl;
   
   TVector3 point(0.005,-0.005,0.001);
//   const MagField* magfield = magFieldManager->GetMagField(point," ");
//   const MagFieldMap* map = dynamic_cast<const MagFieldMap*>(magfield);
   
//   TVector3 avgField = map->Interpolate(point, 6);
//   cout << "Final Field (T): " << avgField.X() << "\t" << avgField.Y() << "\t" << avgField.Z() << endl;
   return kTRUE;
}

//__________________________________________________________________________
int TestFieldManager()
{
   // -- Simple function tests putting a magfield into MagFieldManager object, and then writing
   // the manager out to file and succesfully reading it back in. We should find that root makes a
   // new copy of the fields stored by the manager when this is read back in from file.
   TVector3 vec(0.,0.,0.);
   MagField* magfield1 = new UniformMagField("1", vec,0, 0);
   MagField* magfield2 = new UniformMagField("2", vec,0, 0);
   
   MagFieldManager* manager1(new MagFieldManager());
   manager1->AddField(magfield1);
   manager1->AddField(magfield2);
   
   TFile *f = TFile::Open("test.root","recreate");
   if (!f || f->IsZombie()) {
      Error("Export","Cannot open file");
      return EXIT_FAILURE;
   }
   manager1->Write(manager1->GetName());
   f->ls();
   cout << "-------------------------------------------" << endl;
   // -- Clean up
   delete f;
   f = 0;
   
   
   // -- Open File
   f = TFile::Open("test.root", "read");
   if (!f || f->IsZombie()) {
      Error("Export","Cannot open file");
      return EXIT_FAILURE;
   }
   
   // -- Extract Object
   cout << "Fetching manaer" << endl;
   MagFieldManager* manager2(new MagFieldManager());
   f->GetObject("MagFieldManager", manager2);
   if (manager2 == NULL) {
      Error("Export","Cannot extract manager from file");
      return EXIT_FAILURE;
   }
   cout << "Successfully Loaded Manager" << endl;
   
   // Look to see if addresses of fields held are different. If yes, then these objects were copied
   // when read in from file.
   // These methods need to be reimplemented in Manager when testing as I am not committing them.
//   manager1->List(); // List addresses of fields held
//   manager2->List(); // List addresses of fields held
   
   delete manager1;
   manager1 = 0;
   delete manager2;
   manager2 = 0;
   delete f;
   f = 0;
   
   return EXIT_SUCCESS;
}


/* 
// -------------------------------------------------------------------------------------- 
Double_t ExponentialDecay(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Exp(-t/par[1]);
   return f;
}



{
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   string configFileName, treeName;
   if (argc == 3) {
      configFileName= argv[1];
      treeName = argv[2];
   } else {
      cerr << "Error: No configuration file has been specified." << endl;
      cerr << "Usage, ucnsim <configFile.cfg> <treename>" << endl;
      return EXIT_FAILURE;
   }
   ConfigFile configFile(configFileName);
   
   TRint *theApp = new TRint("FittingApp", &argc, argv);
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Fetch the Number of Runs
   ///////////////////////////////////////////////////////////////////////////////////////
   const Int_t numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
   if (numberOfRuns < 1) {
      cerr << "Cannot read valid number of runs from ConfigFile" << endl;
      return EXIT_FAILURE;
   }
   cout << "Number of Runs: " << numberOfRuns << endl << endl; 
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Create storage for emptying times
   ///////////////////////////////////////////////////////////////////////////////////////
   Double_t emptyingTimes[20];
   Double_t emptyingTimeErrors[20];
   Double_t surfaceRoughness[20] = {0.00, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Loop over all the Runs
   ///////////////////////////////////////////////////////////////////////////////////////
   for (Int_t runNumber = 1; runNumber <= numberOfRuns; runNumber++) {
      // Find the run
      Char_t runName[20];
      sprintf(runName,"Run%d",runNumber); 
      // Read the outputfile name
      TString dataFileName = configFile.GetString("OutputDataFile",runName);
      if (dataFileName == "") { 
         cout << "No File holding the particle data has been specified" << endl;
         return kFALSE;
      }
      // -- Open File
      TFile *file = 0;
      file = TFile::Open(dataFileName, "read");
      if (!file || file->IsZombie()) {
         cerr << "Cannot open file: " << dataFileName << endl;
         return 0;
      }
      // -- Extract Run Object
      cout << "Fetching run: " << runName << endl;
      Run* run = new Run();
      file->GetObject(static_cast<const char*>(runName), run);
      if (run == NULL) {
         cerr << "Could not find run: " << runName << endl;
         return kFALSE;
      }
      cout << "Successfully Loaded Run: " << runName << endl;
      
      // -- Fetch required Tree
      TTree* tree = run->GetData()->FetchTree(treeName);
      if (tree == NULL) return kFALSE;
      
      // Create Time Histogram
      Int_t runTime = configFile.GetInt("RunTime(s)",runName);
      Int_t fillTime = configFile.GetInt("FillingTime",runName);
      Int_t nbins = 50;
      
      Char_t histname[20];
      sprintf(histname,"%s:Time",tree->GetName());
      TH1F* timeHist = new TH1F(histname,"Time: Units of s", nbins, 0.0, runTime+10);
      timeHist->SetXTitle("Time (s)");
      timeHist->SetYTitle("Neutrons");
      
      // Fill Histograms
      for (Int_t i = 0; i < tree->GetEntriesFast(); i++) {
         Particle* particle = run->GetData()->GetParticleState(tree, i);
         if (particle == NULL) continue;
         timeHist->Fill(particle->T()/Units::s);
         particle = 0;
      }
      
      // Fit Final Time to Exponential - determine emptying time
      Char_t funcName[20];
      sprintf(funcName,"%s:Expo",tree->GetName());
      TF1 *expo = new TF1(funcName, ExponentialDecay, fillTime, run->RunTime(), 2);
      expo->SetParameters(100.0,10.0);
      expo->SetParNames("Initial Number","Emptying Time(s)");
      timeHist->Fit(expo, "RN");
      Double_t emptyingTime = expo->GetParameter(1);
      Double_t emptyingTimeError = expo->GetParError(1);
      cout << "EmptyingTime: " << emptyingTime << "\t" << "Error: " << emptyingTimeError << endl;
      
      // Fill emptyingtime arrays for fitting
      emptyingTimes[runNumber-1] = emptyingTime;
      emptyingTimeErrors[runNumber-1] = emptyingTimeError;
      
      file->ls();
      
      delete expo;
      delete timeHist;
      delete run;
      delete file;
   }
   
   cout << "Finished fitting emptying times, preparing plot..." << endl;
   
   
   TGraphErrors* graph  = new TGraphErrors(numberOfRuns, surfaceRoughness, emptyingTimes, 0, emptyingTimeErrors);
   graph->GetXaxis()->SetTitle("Roughness parameter");
   graph->GetYaxis()->SetTitle("Emptying time, (s)");
   graph->GetXaxis()->SetLimits(-0.1,1.1);
//   graph->GetYaxis()->SetRangeUser(0.,18.);
   graph->SetMarkerColor(4);
   graph->SetMarkerStyle(21);
   cout << "Created graph" << endl;
   graph->Draw("AR");
   
   for (Int_t i = 0; i < 20; i++) {
      cout << emptyingTimes[i] << ", "; 
   }
   cout << endl;
   
   for (Int_t i = 0; i < 20; i++) {
      cout << emptyingTimeErrors[i] << ", "; 
   }
   cout << endl;
   
   for (Int_t i = 0; i < 20; i++) {
      cout << surfaceRoughness[i] << ", "; 
   }
   cout << endl;
      
   Double_t a;
   cout << "Wait for user input" << endl;
   cin >> a;
   
   theApp->Run();
}   
*/


