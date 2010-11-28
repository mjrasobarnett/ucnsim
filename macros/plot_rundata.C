#include <iostream>
#include <map>
#include <iterator>

#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"

#include "include/DataFileHierarchy.h"
#include "include/Constants.h"
#include "include/Units.h"

Int_t plot_rundata(const char* configFileName, const char* treeName, const char* runName="Run1");
Double_t ExponentialDecay(Double_t *x, Double_t *par);
Double_t SpinPrecession(Double_t *x, Double_t *par);

using namespace std;

// -------------------------------------------------------------------------------------- 
Int_t plot_rundata(const char* configFileName, const char* statename, const char* runName) {
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   TUCNConfigFile configFile(configFileName);
   const string initialConfigFileName = configFile.GetString("Config","Initialisation");
   const string runConfigFile = configFile.GetString("Config",runName);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Read in Initial Configuration from file.
   TUCNInitialConfig initialConfig(initialConfigFileName);
   TUCNRunConfig runConfig(runConfigFile);
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
   TPolyMarker3D* points = new TPolyMarker3D(neutrons, 1); // 1 is marker style
   sprintf(histname,"%s:NeutronPositions",statename);
   points->SetName(histname);
   points->SetMarkerColor(2);
   points->SetMarkerStyle(6);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Angular Distribution
   sprintf(histname,"%s:Theta",statename);
   TH1F* thetaHist = new TH1F(histname,"Direction: Theta, Units of Pi", 50, 0.0, 1.0);
   sprintf(histname,"%s:Phi",statename);
   TH1F* phiHist = new TH1F(histname,"Direction: Phi, Units of Pi", 50, 0.0, 2.0);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Energy/Momentum
   const Double_t maximumVelocity = initialConfig.InitialMaxVelocity();
   const Double_t maximumMomentum = 20*Units::eV;
   const Int_t nbins = 50;   
   
   sprintf(histname,"%s:Velocity",statename);
   TH1F* energyHist = new TH1F(histname,"Velocity: Units of m/s", nbins, 0.0, maximumVelocity);      
   energyHist->SetXTitle("Velocity (m/s)");
   energyHist->SetYTitle("Neutrons");
   
   sprintf(histname,"%s:Px",statename);
   TH1F* pxHist = new TH1F(histname,"Px: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   pxHist->SetXTitle("Px (eV)");
   pxHist->SetYTitle("Neutrons");
   
   sprintf(histname,"%s:Py",statename);
   TH1F* pyHist = new TH1F(histname,"Py: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   pyHist->SetXTitle("Py (eV)");
   pyHist->SetYTitle("Neutrons");
   
   sprintf(histname,"%s:Pz",statename);
   TH1F* pzHist = new TH1F(histname,"Pz: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   pzHist->SetXTitle("Pz (eV)");
   pzHist->SetYTitle("Neutrons");
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Run Time
   const Double_t runTime = runConfig.RunTime();
   const Double_t maximumDistance = 10000*Units::m;
   
   sprintf(histname,"%s:Time",statename);
   TH1F* timeHist = new TH1F(histname,"Time: Units of s", nbins, 0.0, runTime+10);
   timeHist->SetXTitle("Time (s)");
   timeHist->SetYTitle("Neutrons");
   
   sprintf(histname,"%s:Dist",statename);
   TH1F* distHist = new TH1F(histname,"Distance: Units of m", nbins, 0.0, maximumDistance);
   distHist->SetXTitle("Distance (m)");
   distHist->SetYTitle("Neutrons");
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Bounces
   sprintf(histname,"%s:Bounces",statename);
   TH1F* bounceHist = new TH1F(histname,"Bounces", nbins, 0.0, 50000);
   sprintf(histname,"%s:Specular",statename);
   TH1F* specHist = new TH1F(histname,"Specular", nbins, 0.0, 50000);
   sprintf(histname,"%s:Diffuse",statename);
   TH1F* diffHist = new TH1F(histname,"Diffuse", nbins, 0.0, 50000);
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Spin Polarisation
   sprintf(histname,"%s:SpinUp",statename);
   TH1F* spinUpHist = new TH1F(histname,"SpinUp", 500, 0.0, runTime);      
   spinUpHist->SetXTitle("Time (s)");
   spinUpHist->SetYTitle("Spin Up Neutrons");
   //spinUpHist->SetLineColor(kBlue);
   spinUpHist->SetFillStyle(1001);
   spinUpHist->SetFillColor(kBlue-7);
   
   sprintf(histname,"%s:SpinDown",statename);
   TH1F* spinDownHist = new TH1F(histname,"SpinDown", 500, 0.0, runTime);      
   spinDownHist->SetXTitle("Time (s)");
   spinDownHist->SetYTitle("Spin Down Neutrons");
   //spinDownHist->SetLineColor(kRed);
   spinDownHist->SetFillStyle(3001);
   spinDownHist->SetFillColor(kRed-7);
   
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
   // -- Loop over particles in state folder
   //////////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////
   TKey *folderKey;
   TIter folderIter(stateDir->GetListOfKeys());
   while ((folderKey = static_cast<TKey*>(folderIter.Next()))) {
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
            classname = objKey->GetClassName();
            cl = gROOT->GetClass(classname);
            if (!cl) continue;
            // -- Extract Final Particle State Data
            if (cl->InheritsFrom("TUCNParticle")) {
               TUCNParticle* particle = dynamic_cast<TUCNParticle*>(objKey->ReadObj());
               // -- Fill Histograms
               points->SetPoint(particle->Id(), particle->X(), particle->Y(), particle->Z());
               thetaHist->Fill(particle->Theta()/TMath::Pi());
               phiHist->Fill(particle->Phi()/TMath::Pi());
               energyHist->Fill(particle->V());
               pxHist->Fill(particle->Px()/Units::eV);
               pyHist->Fill(particle->Py()/Units::eV);
               pzHist->Fill(particle->Pz()/Units::eV);
               timeHist->Fill(particle->T()/Units::s);
               distHist->Fill(particle->Distance()/Units::m);
               bounceHist->Fill(particle->Bounces());
               specHist->Fill(particle->SpecularBounces());
               diffHist->Fill(particle->DiffuseBounces());
               delete particle;
            }
            // -- Extract Spin Observer Data if recorded
            if (cl->InheritsFrom("TUCNSpinObservables")) {
               TUCNSpinObservables* spin = dynamic_cast<TUCNSpinObservables*>(objKey->ReadObj());
               // Loop over spin data recorded for particle
               TUCNSpinObservables::iterator dataIter;
               for (dataIter = spin->begin(); dataIter != spin->end(); dataIter++) {
                  if (dataIter->second == kTRUE) {
                     // If spin up, bin the time
                     spinUpHist->Fill(dataIter->first);
                  } else {
                     // If spin down, bin the time
                     spinDownHist->Fill(dataIter->first);
                  }
               }
               delete spin;
            }
         }
      }
   }
   file->ls();
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
   points->Draw();
   // -- Get the GLViewer so we can manipulate the camera
   TGLViewer * glViewer = dynamic_cast<TGLViewer*>(gPad->GetViewer3D());
   // -- Select Draw style 
   glViewer->SetStyle(TGLRnrCtx::kFill);
   // -- Set Background colour
   glViewer->SetClearColor(TColor::kWhite);
   // -- Set Camera type
   TGLViewer::ECameraType camera = 2;
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
   thetaHist->Draw();
   anglecanvas->cd(2);
   phiHist->Draw();
   
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
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Momentum Distribution
   TCanvas *momcanvas = new TCanvas("PhaseSpace","Phase Space",60,0,1200,800);
   momcanvas->Divide(3,2);
   momcanvas->cd(1);
   energyHist->Draw();
   momcanvas->Update();   
   momcanvas->cd(2);
   pxHist->Draw();
   momcanvas->cd(3);
   pyHist->Draw();
   momcanvas->cd(4);
   pzHist->Draw();
   momcanvas->cd(5);
   distHist->Draw();
   momcanvas->cd(6);
   timeHist->Draw();
   // Fit Final Time to Exponential - determine emptying time
   Char_t linename[40];
   sprintf(linename,"%s:Expo",statename);
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
   
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Spin Precession Plots
   // -- Down
   TCanvas *downCanvas = new TCanvas("Spin Down","Spin Down Polarisation",60,0,1000,800);
   TF1 *downprecession = new TF1("DownSpinPrecessionFunc", SpinPrecession, 0.0, 20.0, 3);
   downprecession->SetParameters(150.0,1.0,0.0);
   downprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
   spinDownHist->Fit(downprecession, "R");
   Double_t downamplitude = downprecession->GetParameter(0);
   Double_t downomega = downprecession->GetParameter(1);
   Double_t downphase = downprecession->GetParameter(2);
   cout << "Amplitude: " << downamplitude << "\t" << "Omega: " << downomega << "\t" << "Phase: " << downphase << endl;   
   spinDownHist->GetYaxis()->SetRangeUser(0.0,200.0);
   TPaveText *downinfo = new TPaveText(0.15,0.71,0.6,0.79,"brNDC");
   downinfo->SetFillColor(0);
   downinfo->SetTextAlign(12);
   downinfo->SetTextFont(62);
   // -- Using string streams to write parameter values into text
   ostringstream down_omega;
   down_omega << "Expected Omega: 29.16 Hz/uT * 0.01uT * Pi = 0.916";
   downinfo->AddText(down_omega.str().c_str());
   downinfo->Draw();
   // -- Up
   TCanvas *upCanvas = new TCanvas("Spin Up","Spin Up Polarisation",60,0,1000,800);
   TF1 *upprecession = new TF1("UpSpinPrecessionFunc", SpinPrecession, 0.0, 20.0, 3);
   upprecession->SetParameters(150.0,1.0,0.0);
   upprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
   spinUpHist->Fit(upprecession, "R");
   Double_t upamplitude = upprecession->GetParameter(0);
   Double_t upomega = upprecession->GetParameter(1);
   Double_t upphase = upprecession->GetParameter(2);
   cout << "Amplitude: " << upamplitude << "\t" << "Omega: " << upomega << "\t" << "Phase: " << upphase << endl;
   spinUpHist->GetYaxis()->SetRangeUser(0.0,200.0);
   downinfo->Draw();
   // -- Combined
   TCanvas *combCanvas = new TCanvas("Spin","Spin Polarisation",60,0,1000,800);
   spinUpHist->Draw();
   spinDownHist->Draw("SAME");
   TLegend* leg = new TLegend(0.7,0.91,0.9,0.99); //coordinates are fractions of pad dimensions
   leg->SetFillColor(0);
   leg->AddEntry(spinUpHist,"Spin Up"); // l means line, p shows marker
   leg->AddEntry(spinDownHist,"Spin Down");
   leg->Draw();
   
   return 0;
}

/*
// -------------------------------------------------------------------------------------- 
void PlotPositions(TGeoManager* geoManager, TUCNRun* run, const TString& treeName) 
{
// -- Create a TPolyMarker3D object to store the final positions
// -- of the neutrons and write this to file. 
   cout << "-------------------------------------------" << endl;
   cout << "PlotPositions" <<  endl;
   cout << "-------------------------------------------" << endl;
   // -- Fetch Tree
   TTree* tree = run->GetData()->FetchTree(treeName);
   if (tree == NULL) return kFALSE;
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Create the points
   Int_t neutrons = tree->GetEntriesFast();
   TPolyMarker3D* points = new TPolyMarker3D(neutrons, 1); // 1 is marker style
   for (Int_t i = 0; i < neutrons; i++) {
      TUCNParticle* particle = run->GetData()->GetParticleState(tree, i);
      if (particle == NULL) continue;
      points->SetPoint(i, particle->X(), particle->Y(), particle->Z());
   }
   cout << "Created Initial Points" << endl;
   points->SetMarkerColor(2);
   points->SetMarkerStyle(6);
   // -- Write the points to the File
   Char_t name[20];
   sprintf(name,"%s:NeutronPositions",tree->GetName());
   points->SetName(name);
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Clean Up
   TCanvas *canvas = new TCanvas("Positions","Neutron Positions",60,0,800,800);
   canvas->cd();
   geoManager->GetTopVolume()->Draw("ogl");
   geoManager->SetVisLevel(4);
   geoManager->SetVisOption(0);
   points->Draw();
   
   // -- Get the GLViewer so we can manipulate the camera
   TGLViewer * glViewer = dynamic_cast<TGLViewer*>(gPad->GetViewer3D());
   // -- Select Draw style 
   glViewer->SetStyle(TGLRnrCtx::kFill);
   // -- Set Background colour
   glViewer->SetClearColor(TColor::kWhite);
   // -- Set Camera type
   TGLViewer::ECameraType camera = 2;
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
   
   return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotAngularDistribution(TUCNRun* run, const TString& treeName) 
{
// -- Create a Histogram object to store the angular distribution 
// -- (as in, their initial and final directions about the origin). 
   cout << "-------------------------------------------" << endl;
   cout << "PlotInitialAndFinalAngularDistribution" <<  endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // Get Tree
   TTree* tree = run->GetData()->FetchTree(treeName);
   if (tree == NULL) return kFALSE;
   // -- Plot the Initial and Final Directions
   Int_t nbins = 50;
   Char_t name[20];
   sprintf(name,"%s:Theta",tree->GetName());
   TH1F* thetaHist = new TH1F(name,"Direction: Theta, Units of Pi", nbins, 0.0, 1.0);
   sprintf(name,"%s:Phi",tree->GetName());
   TH1F* phiHist = new TH1F(name,"Direction: Phi, Units of Pi", nbins, 0.0, 2.0);
   
   for (Int_t i = 0; i < tree->GetEntriesFast(); i++) {
      TUCNParticle* particle = run->GetData()->GetParticleState(tree, i);
      if (particle == NULL) continue;
      thetaHist->Fill(particle->Theta()/TMath::Pi());
      phiHist->Fill(particle->Phi()/TMath::Pi());
   }
   // -- Write the points to the File
   TCanvas *canvas = new TCanvas("Directions","Neutron Directions",60,0,800,800);
   canvas->Divide(2,1);
   canvas->cd(1);
   thetaHist->Draw();
   canvas->cd(2);
   phiHist->Draw();
   return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotBounces(TUCNRun* run, const TString& treeName) 
{
// -- Create a Histogram object to store the angular distribution 
// -- (as in, their initial and final directions about the origin). 
   cout << "-------------------------------------------" << endl;
   cout << "PlotBounces" <<  endl;
   cout << "-------------------------------------------" << endl;
   ///////////////////////////////////////////////////////////////////////////////////////
   // Get Tree
   TTree* tree = run->GetData()->FetchTree(treeName);
   if (tree == NULL) return kFALSE;
   // -- Plot the Initial and Final Directions
   Int_t nbins = 50;
   Char_t name[20];
   sprintf(name,"%s:Bounces",tree->GetName());
   TH1F* bounceHist = new TH1F(name,"Bounces", nbins, 0.0, 50000);
   sprintf(name,"%s:Specular",tree->GetName());
   TH1F* specHist = new TH1F(name,"Specular", nbins, 0.0, 50000);
   sprintf(name,"%s:Diffuse",tree->GetName());
   TH1F* diffHist = new TH1F(name,"Diffuse", nbins, 0.0, 50000);
   
   for (Int_t i = 0; i < tree->GetEntriesFast(); i++) {
      TUCNParticle* particle = run->GetData()->GetParticleState(tree, i);
      if (particle == NULL) continue;
      bounceHist->Fill(particle->Bounces());
      specHist->Fill(particle->SpecularBounces());
      diffHist->Fill(particle->DiffuseBounces());
   //   cout << particle->Id() << "\t" << particle->Bounces() << "\t" << particle->SpecularBounces() << "\t" << particle->DiffuseBounces() << endl;
   }
   // -- Write the points to the File
   TCanvas *canvas = new TCanvas("Directions","Neutron Directions",60,0,800,800);
   canvas->Divide(3,1);
   canvas->cd(1);
   bounceHist->Draw();
   canvas->cd(2);
   specHist->Draw();
   canvas->cd(3);
   diffHist->Draw();
   return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotPhaseSpace(TUCNRun* run, const TString& treeName) 
{
// -- Create a Histogram object to store the angular distribution (as in, their initial and final directions about the origin). 
   cout << "-------------------------------------------" << endl;
   cout << "PlotInitialAndFinalPhaseSpace" <<  endl;
   cout << "-------------------------------------------" << endl;
   // Get Tree
   TTree* tree = run->GetData()->FetchTree(treeName);
   if (tree == NULL) return kFALSE;
   
   Double_t maximumVelocity = 8.0;
   Double_t maximumMomentum = 20*Units::eV;
   Double_t runTime = 800*Units::s;
   Double_t maximumDistance = 10000*Units::m;
   // -- Set up the histograms
   Int_t nbins = 50;
   Char_t name[20];
   
   sprintf(name,"%s:Velocity",tree->GetName());
   TH1F* energyHist = new TH1F(name,"Velocity: Units of m/s", nbins, 0.0, maximumVelocity);      
   energyHist->SetXTitle("Velocity (m/s)");
   energyHist->SetYTitle("Neutrons");
   
   sprintf(name,"%s:Px",tree->GetName());
   TH1F* pxHist = new TH1F(name,"Px: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   pxHist->SetXTitle("Px (eV)");
   pxHist->SetYTitle("Neutrons");
   
   sprintf(name,"%s:Py",tree->GetName());
   TH1F* pyHist = new TH1F(name,"Py: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   pyHist->SetXTitle("Py (eV)");
   pyHist->SetYTitle("Neutrons");
   
   sprintf(name,"%s:Pz",tree->GetName());
   TH1F* pzHist = new TH1F(name,"Pz: Units of eV", nbins, -maximumMomentum, maximumMomentum);
   pzHist->SetXTitle("Pz (eV)");
   pzHist->SetYTitle("Neutrons");
   
   sprintf(name,"%s:Time",tree->GetName());
   TH1F* timeHist = new TH1F(name,"Time: Units of s", nbins, 0.0, runTime+10);
   timeHist->SetXTitle("Time (s)");
   timeHist->SetYTitle("Neutrons");
   
   sprintf(name,"%s:Dist",tree->GetName());
   TH1F* distHist = new TH1F(name,"Distance: Units of m", nbins, 0.0, maximumDistance);
   distHist->SetXTitle("Distance (m)");
   distHist->SetYTitle("Neutrons");
   // Fill Histograms
   for (Int_t i = 0; i < tree->GetEntriesFast(); i++) {
      TUCNParticle* particle = run->GetData()->GetParticleState(tree, i);
      if (particle == NULL) continue;
      energyHist->Fill(particle->V());
      pxHist->Fill(particle->Px()/Units::eV);
      pyHist->Fill(particle->Py()/Units::eV);
      pzHist->Fill(particle->Pz()/Units::eV);
      timeHist->Fill(particle->T()/Units::s);
      distHist->Fill(particle->Distance()/Units::m);
   }
   
   // -- Draw histograms
   TCanvas *canvas = new TCanvas("PhaseSpace","Phase Space",60,0,1000,800);
   canvas->Divide(3,2);
   canvas->cd(1);
   energyHist->Draw();
   canvas->Update();   
   canvas->cd(2);
   pxHist->Draw();
   canvas->cd(3);
   pyHist->Draw();
   canvas->cd(4);
   pzHist->Draw();
   canvas->cd(5);
   distHist->Draw();
   canvas->cd(6);
   timeHist->Draw();
   // Fit Final Time to Exponential - determine emptying time
   sprintf(name,"%s:Expo",tree->GetName());
   TF1 *expo = new TF1(name, ExponentialDecay, 100., run->RunTime(), 2);
   expo->SetParameters(100.0,10.0);
   expo->SetParNames("Initial Number","Emptying Time(s)");
   timeHist->Fit(expo, "R");
   Double_t decayConstant = expo->GetParameter(1);
   Double_t decayConstantError = expo->GetParError(1);
   cout << "DecayConstant: " << decayConstant << "\t" << "Error: " << decayConstantError << endl;
   
   
   Double_t emptyingTime = 1.0/decayConstant;
   Double_t emptyingTimeError = decayConstantError/(decayConstant*decayConstant);
   
   cout << "EmptyingTime: " << emptyingTime << "\t" << "Error: " << emptyingTimeError << endl;
   
   return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotObservers(TUCNRun* run, const TString& treeName)
{
   // -- Get Observers to create their plots of what they have stored
   // -- including data only from the particles in the specified tree
   TTree* tree = run->GetData()->FetchTree(treeName);
   if (tree == NULL) return kFALSE;
   run->GetData()->PlotObservers(tree);
}


void TUCNSpinObserver::Plot(TUCNData* data, TTree* tree)
{
   TH1F* spinUpHist = new TH1F("SpinUpHist","SpinUpHist", 500, 0.0, 20.0);      
   spinUpHist->SetXTitle("Time (s)");
   spinUpHist->SetYTitle("Spin Up Neutrons");
   spinUpHist->SetTitle("Volume: HV-Ramsey-Cell. BField: 0.01uT along z-axis. UCN initially polarised Spin Up along X-Axis.");
   //spinUpHist->SetLineColor(kBlue);
   spinUpHist->SetFillStyle(1001);
   spinUpHist->SetFillColor(kBlue-7);
   
   TH1F* spinDownHist = new TH1F("SpinDownHist","", 500, 0.0, 20.0);      
   spinDownHist->SetXTitle("Time (s)");
   spinDownHist->SetYTitle("Spin Down Neutrons");
   spinDownHist->SetTitle("Volume: HV-Ramsey-Cell. BField: 0.01uT along z-axis. UCN initially polarised Spin Up along X-Axis.");
   //spinDownHist->SetLineColor(kRed);
   spinDownHist->SetFillStyle(3001);
   spinDownHist->SetFillColor(kRed-7);
   
   // Fill Histograms
   for (Int_t i = 0; i < tree->GetEntriesFast(); i++) {
      TUCNParticle* particle = data->GetParticleState(tree, i);
      if (particle == NULL) continue;
      // First look through the data for an entry matching the particle's ID number 
      map<Int_t, TUCNSpinObservables*>::iterator partIter = fParticleData.find(particle->Id());
      if (partIter != fParticleData.end()) {
         // If particle is found, walk through the datapoints 
         TUCNSpinObservables* datapoints = partIter->second;
         map<Double_t, Bool_t>::iterator dataIter;
         for (dataIter = datapoints->begin(); dataIter != datapoints->end(); dataIter++) {
            if (dataIter->second == kTRUE) {
               // If spin up, bin the time
               spinUpHist->Fill(dataIter->first);
            } else {
               // If spin down, bin the time
               spinDownHist->Fill(dataIter->first);
            }
         }
      }
   }
   
   TCanvas *downCanvas = new TCanvas("Spin Down","Spin Down Polarisation",60,0,1000,800);
   TF1 *downprecession = new TF1("DownSpinPrecessionFunc", SpinPrecession, 0.0, 20.0, 3);
   downprecession->SetParameters(150.0,1.0,0.0);
   downprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
   spinDownHist->Fit(downprecession, "R");
   Double_t downamplitude = downprecession->GetParameter(0);
   Double_t downomega = downprecession->GetParameter(1);
   Double_t downphase = downprecession->GetParameter(2);
   cout << "Amplitude: " << downamplitude << "\t" << "Omega: " << downomega << "\t" << "Phase: " << downphase << endl;
   
   spinDownHist->GetYaxis()->SetRangeUser(0.0,200.0);
   TPaveText *downinfo = new TPaveText(0.15,0.71,0.6,0.79,"brNDC");
   downinfo->SetFillColor(0);
   downinfo->SetTextAlign(12);
   downinfo->SetTextFont(62);
   // -- Using string streams to write parameter values into text
   ostringstream down_omega;
   down_omega << "Expected Omega: 29.16 Hz/uT * 0.01uT * Pi = 0.916";
   downinfo->AddText(down_omega.str().c_str());
   downinfo->Draw();
   
   
   //
   TCanvas *upCanvas = new TCanvas("Spin Up","Spin Up Polarisation",60,0,1000,800);
   TF1 *upprecession = new TF1("UpSpinPrecessionFunc", SpinPrecession, 0.0, 20.0, 3);
   upprecession->SetParameters(150.0,1.0,0.0);
   upprecession->SetParNames("Amplitude","Omega (rad/s)","Phase (Pi/2)");
   spinUpHist->Fit(upprecession, "R");
   Double_t upamplitude = upprecession->GetParameter(0);
   Double_t upomega = upprecession->GetParameter(1);
   Double_t upphase = upprecession->GetParameter(2);
   cout << "Amplitude: " << upamplitude << "\t" << "Omega: " << upomega << "\t" << "Phase: " << upphase << endl;
   
   spinUpHist->GetYaxis()->SetRangeUser(0.0,200.0);
   downinfo->Draw();
   
   
   TCanvas *combCanvas = new TCanvas("Spin","Spin Polarisation",60,0,1000,800);
   spinUpHist->Draw();
   spinDownHist->Draw("SAME");
   TLegend* leg = new TLegend(0.7,0.91,0.9,0.99); //coordinates are fractions of pad dimensions
   leg->SetFillColor(0);
   leg->AddEntry(spinUpHist,"Spin Up"); // l means line, p shows marker
   leg->AddEntry(spinDownHist,"Spin Down");
   leg->Draw();
   
}
*/

// -------------------------------------------------------------------------------------- 
Double_t ExponentialDecay(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Exp(-t/par[1]);
   return f;
}

// -------------------------------------------------------------------------------------- 
Double_t SpinPrecession(Double_t *x, Double_t *par)
{
   Double_t t = x[0];
   Double_t f = par[0]*TMath::Sin((par[1]*t + par[2]*(TMath::Pi()/2.0)))*TMath::Sin((par[1]*t + par[2]*(TMath::Pi()/2.0)));
   return f;
}

