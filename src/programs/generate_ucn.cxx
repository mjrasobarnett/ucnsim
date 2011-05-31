#include <iostream>
#include <string>
#include <cassert>

#include "ConfigFile.h"
#include "InitialConfig.h"
#include "Data.h"
#include "Particle.h"
#include "Box.h"
#include "Tube.h"
#include "Volume.h"
#include "ParticleManifest.h"

#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TRint.h"
#include "TBenchmark.h"
#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"
#include "TPolyMarker3D.h"
#include "TTree.h"
#include "TBranch.h"

#include "Materials.h"
#include "Constants.h"
#include "Units.h"
#include "ValidStates.h"
#include "GeomParameters.h"
#include "Algorithms.h"
#include "DataAnalysis.h"

using std::cin;
using std::cout;
using std::endl;
using std::cerr;
using std::string;

using namespace GeomParameters;

// Function Declarations
void GenerateBeam(const InitialConfig& initialConfig);
Bool_t GenerateParticles(const InitialConfig& initialConfig, const TGeoVolume* beamVolume, const TGeoMatrix* beamMatrix);
Bool_t CreateRandomParticle(Particle* particle, const Double_t fillTime, const TGeoVolume* beamVolume, const TGeoMatrix* beamMatrix); 
Bool_t DetermineParticleVelocity(Particle* particle, const Double_t maxVelocity, const Double_t minTheta, const Double_t maxTheta, const Double_t minPhi, const Double_t maxPhi);
void DefinePolarisation(Particle* particle, const Double_t percentPolarised, const TVector3& spinAxis, const Bool_t spinUp);

//__________________________________________________________________________
Int_t main(Int_t argc,Char_t **argv)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Set up benchmark
   TBenchmark benchmark;
   benchmark.SetName("UCNSIM");
   benchmark.Start("UCNSIM");
   ///////////////////////////////////////////////////////////////////////////////////////
   // Build the ConfigFile
   ///////////////////////////////////////////////////////////////////////////////////////
   string configFileName;
   if (argc == 2) {
      configFileName= argv[1];
   } else {
      cerr << "Error: No configuration file has been specified." << endl;
      cerr << "Usage, ucnsim <configFile.cfg>" << endl;
      return EXIT_FAILURE;
   }
   // Start 'the app' -- this is so we are able to enter into a ROOT session
   // after the program has run, instead of just quitting.
   TRint *theApp = new TRint("FittingApp", NULL, NULL);
   // Read in Batch Configuration file to find the Initial Configuration File
   ConfigFile configFile(configFileName);
   ///////////////////////////////////////////////////////////////////////////////////////
   // Read in Initial Configuration from file.
   InitialConfig initialConfig(configFile);   
   GenerateBeam(initialConfig);
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Output up benchmark
   benchmark.Stop("UCNSIM");
   cout << "-------------------------------------------" << endl;
   benchmark.Print("UCNSIM");
   cout << "-------------------------------------------" << endl;
   // Enter ROOT interactive session
   theApp->Run();
   return EXIT_SUCCESS;
}

//__________________________________________________________________________
void GenerateBeam(const InitialConfig& initialConfig)
{
   // -- Make a 'virtual' beam volume within which we will generate our initial particles
   const std::string beamShapeName = initialConfig.BeamShape();
   const Double_t beamRMin = 0.0;
   const Double_t beamRMax = initialConfig.BeamRadius();
   const Double_t beamHalfLength = initialConfig.BeamLength()/2.0;
   const Double_t beamPhi = initialConfig.BeamPhi();
   const Double_t beamTheta = initialConfig.BeamTheta();
   const Double_t beamPsi = initialConfig.BeamPsi();
   const Double_t beamXPos = initialConfig.BeamDisplacement().X();
   const Double_t beamYPos = initialConfig.BeamDisplacement().Y();
   const Double_t beamZPos = initialConfig.BeamDisplacement().Z();
   
   // -- Make a Geomanager
   TGeoManager* geoManager = new TGeoManager("GeoManager","Geometry Manager");
   Materials::BuildMaterials(geoManager);
   TGeoMedium* liquidHelium = geoManager->GetMedium("HeliumII");
   // -- Create the Beam volume within which we shall generate the particles
   TGeoShape* beamShape = NULL;
   if (beamShapeName == "Tube") {
      beamShape = new Tube("BeamShape", beamRMin, beamRMax, beamHalfLength);
   } else {
      beamShape = new Box("BeamShape", beamRMax, beamRMax, beamHalfLength);
   }
   TrackingVolume* beam = new TrackingVolume("Beam", beamShape, liquidHelium);
   TGeoRotation beamRot("BeamRot",beamPhi,beamTheta,beamPsi);
   TGeoTranslation beamTra("BeamTra",beamXPos,beamYPos,beamZPos);
   TGeoCombiTrans beamCom(beamTra,beamRot);
   TGeoHMatrix beamMat = beamCom;
   TGeoMatrix* beamMatrix = new TGeoHMatrix(beamMat);
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Generate the particles
   GenerateParticles(initialConfig, beam, beamMatrix);
   
   return;
}

//__________________________________________________________________________
Bool_t GenerateParticles(const InitialConfig& initialConfig, const TGeoVolume* beamVolume, const TGeoMatrix* beamMatrix)
{
   // Generates a uniform distribution of particles with random directions all with 
   // the same total energy (kinetic plus potential) defined at z = 0.   
   const Int_t particles = TMath::Abs(initialConfig.InitialParticles());
   const Double_t vmax = TMath::Abs(initialConfig.InitialMaxVelocity())*Units::m/Units::s;
   const Double_t maxEnergy = 0.5*Neutron::mass_eV_c2*TMath::Power(vmax,2.0);
   const Double_t fillTime = TMath::Abs(initialConfig.FillingTime())*Units::s;
   
   const Double_t percentPolarised = initialConfig.PercentPolarised();
   const TVector3 spinAxis = initialConfig.SpinAxis();
   const Bool_t spinUp = initialConfig.SpinUp();
   
   const Double_t minTheta = initialConfig.DirectionMinTheta();
   const Double_t maxTheta = initialConfig.DirectionMaxTheta();
   const Double_t minPhi = initialConfig.DirectionMinPhi();
   const Double_t maxPhi = initialConfig.DirectionMaxPhi();
   
   // Calculate the boundaries of the beam volume using the beam matrix
   Box* boundary = dynamic_cast<Box*>(beamVolume->GetShape());
   Double_t posBounds[3] = {0.,0.,0.};
   Double_t negBounds[3] = {0.,0.,0.};
   Double_t posLocalBounds[3] = {boundary->GetDX(), boundary->GetDY(), boundary->GetDZ()};
   Double_t negLocalBounds[3] = {-boundary->GetDX(), -boundary->GetDY(), -boundary->GetDZ()};
   beamMatrix->LocalToMaster(posLocalBounds, posBounds);
   beamMatrix->LocalToMaster(negLocalBounds, negBounds);
   beamMatrix->Print();
   
   // Create Histograms to view the initial particle distributions
   const Int_t nbins = 100;
   TH1F* initialXHist = new TH1F("initial:X","X (m)", nbins, negBounds[0], posBounds[0]);
   initialXHist->SetLineColor(kBlue);
   initialXHist->SetFillStyle(3001);
   initialXHist->SetFillColor(kBlue);
   TH1F* initialYHist = new TH1F("initial:Y","Y (m)", nbins, negBounds[1], posBounds[1]); 
   initialYHist->SetLineColor(kBlue);
   initialYHist->SetFillStyle(3001);
   initialYHist->SetFillColor(kBlue);
   TH1F* initialZHist = new TH1F("initial:Z","Z (m)", nbins, negBounds[2], posBounds[2]);
   initialZHist->SetLineColor(kBlue);
   initialZHist->SetFillStyle(3001);
   initialZHist->SetFillColor(kBlue);
   TH1F* initialVXHist = new TH1F("initial:Vx","Vx (m/s)", nbins, -vmax, vmax);
   initialVXHist->SetLineColor(kRed);
   initialVXHist->SetFillStyle(3001);
   initialVXHist->SetFillColor(kRed);
   TH1F* initialVYHist = new TH1F("initial:Vy","Vy (m/s)", nbins, -vmax, vmax);
   initialVYHist->SetLineColor(kRed);
   initialVYHist->SetFillStyle(3001);
   initialVYHist->SetFillColor(kRed);
   TH1F* initialVZHist = new TH1F("initial:Vz","Vz (m/s)", nbins, -vmax, vmax);
   initialVZHist->SetLineColor(kRed);
   initialVZHist->SetFillStyle(3001);
   initialVZHist->SetFillColor(kRed);
   TH1F* initialVHist = new TH1F("initial:Velocity","Velocity (m/s)", nbins, 0.0, vmax);
   initialVHist->SetLineColor(kRed);
   initialVHist->SetFillStyle(3001);
   initialVHist->SetFillColor(kRed);
   TH1F* initialTHist = new TH1F("initial:Time","Time (s)", nbins, 0.0, fillTime);
   
   TPolyMarker3D* positions = new TPolyMarker3D(particles, 1); // 1 is marker style
   positions->SetMarkerColor(2);
   positions->SetMarkerStyle(6);
   
   cout << "-------------------------------------------" << endl;
   cout << "Generating " << particles << " Particles." << endl;
   cout << "Boundary X (m): " << boundary->GetDX() << "\t";
   cout << "Y (m): " << boundary->GetDX() << "\t";
   cout << "Z (m): " << boundary->GetDX() << endl;
   cout << "Max Energy (neV): " << maxEnergy/Units::neV << endl;
   cout << "-------------------------------------------" << endl;
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Create storage for the particles
   const string outputFileName = initialConfig.OutputFileName();
   TFile* file = Analysis::DataFile::OpenRootFile(outputFileName,"RECREATE");
   TTree tree("Particles","Tree of Particle Data");
   Particle* particle = new Particle();
   TBranch* initialBranch = tree.Branch(States::initial.c_str(), particle->ClassName(), &particle);
   ParticleManifest manifest;
   // -- Loop over the total number of particles to be created. 
   for (Int_t i = 1; i <= particles; i++) {
      // -- Create particle at a random position inside beam volume
      particle->SetId(i);
      CreateRandomParticle(particle, fillTime, beamVolume, beamMatrix);
      // -- Initialise particle's momentum
      DetermineParticleVelocity(particle, vmax, minTheta, maxTheta, minPhi, maxPhi);
      // -- Setup polarisation
      DefinePolarisation(particle, percentPolarised, spinAxis, spinUp); 
      // -- Fill histograms
      initialXHist->Fill(particle->X());
      initialYHist->Fill(particle->Y());
      initialZHist->Fill(particle->Z());
      initialVXHist->Fill(particle->Vx());
      initialVYHist->Fill(particle->Vy());
      initialVZHist->Fill(particle->Vz());
      initialVHist->Fill(particle->V());
      initialTHist->Fill(particle->T());
      positions->SetPoint(i-1, particle->X(), particle->Y(), particle->Z());
      // -- Add particle to data file
      initialBranch->Fill();
      int branchIndex = initialBranch->GetEntries() - 1;
      manifest.AddEntry(States::initial, particle->Id(), branchIndex);
      // -- Update progress bar
      Algorithms::ProgressBar::PrintProgress(i,particles,1);
   }
   // -- Write the tree and manifest to file
   manifest.Write();
   tree.Write();
   // -- Navigate to histogram folder
   Analysis::DataFile::NavigateToHistDir(*file);
   // -- Save initial state plots to histogram folder
   TCanvas *canvas1 = new TCanvas("InitialPhaseSpace","Initial Phase Space",60,0,1000,800);
   canvas1->Divide(4,2);
   canvas1->cd(1);
   initialXHist->Draw();
   initialXHist->Write(initialXHist->GetName(),TObject::kOverwrite);
   canvas1->cd(2);
   initialYHist->Draw();
   initialYHist->Write(initialYHist->GetName(),TObject::kOverwrite);
   canvas1->cd(3);
   initialZHist->Draw();
   initialZHist->Write(initialZHist->GetName(),TObject::kOverwrite);
   canvas1->cd(4);
   initialTHist->Draw();
   initialTHist->Write(initialTHist->GetName(),TObject::kOverwrite);
   canvas1->cd(5);
   initialVXHist->Draw();
   initialVXHist->Write(initialVXHist->GetName(),TObject::kOverwrite);
   canvas1->cd(6);
   initialVYHist->Draw();
   initialVYHist->Write(initialVYHist->GetName(),TObject::kOverwrite);
   canvas1->cd(7);
   initialVZHist->Draw();
   initialVZHist->Write(initialVZHist->GetName(),TObject::kOverwrite);
   canvas1->cd(8);
   initialVHist->Draw();
   initialVHist->Write(initialVHist->GetName(),TObject::kOverwrite);
   // -- Close file
   file->Close();
   
   
   TCanvas* canvas2 = new TCanvas("InitialPositions","Positions",60,0,100,100);
   canvas2->cd();
   string geomFileName = initialConfig.GeomVisFileName();
   if (geomFileName.empty()) geomFileName = initialConfig.GeomFileName();
   assert(!geomFileName.empty());
   TGeoManager* geoManager = TGeoManager::Import(geomFileName.c_str());
   if (geoManager == NULL) return EXIT_FAILURE;
   geoManager->GetTopVolume()->Draw("ogl");
   geoManager->SetVisLevel(4);
   geoManager->SetVisOption(0);
   positions->Draw();
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
   // -- Set Camera centre to Centre of the Beam
   const TVector3 beamCentre = initialConfig.BeamDisplacement();
   Double_t cameraCentre[3] = {beamCentre.X(),beamCentre.Y(),beamCentre.Z()};
   glViewer->SetPerspectiveCamera(camera,4,100,&cameraCentre[0],0,0);
   // -- Draw Reference Point, Axes
   Double_t refPoint[3] = {0.,0.,0.};
   // Int_t axesType = 0(Off), 1(EDGE), 2(ORIGIN), Bool_t axesDepthTest, Bool_t referenceOn, const Double_t referencePos[3]
   glViewer->SetGuideState(0, kFALSE, kFALSE, refPoint);
   glViewer->UpdateScene();
   glViewer = 0;
   
   cout << "Successfully generated " << particles << " particles." << endl;
   cout << "-------------------------------------------" << endl;	
   return kTRUE;
}

//__________________________________________________________________________
Bool_t CreateRandomParticle(Particle* particle, const Double_t fillTime, const TGeoVolume* beamVolume, const TGeoMatrix* beamMatrix) 
{
   // -- Find a random point inside the Volume
   Double_t point[3] = {0.,0.,0.}, localPoint[3] = {0.,0.,0.};
   
   // -- Determine the dimensions of the source volume's bounding box.
   TGeoShape* beamShape = beamVolume->GetShape();
   TGeoBBox* boundingBox = static_cast<TGeoBBox*>(beamShape);
   const Double_t boxWall[3] = {boundingBox->GetDX(), boundingBox->GetDY(), boundingBox->GetDZ()};
   const Double_t boxOrigin[3] = {boundingBox->GetOrigin()[0], boundingBox->GetOrigin()[1], boundingBox->GetOrigin()[2]};
   
   // Find a random point inside the volume provided
   while(kTRUE) {
      // First generate random point inside bounding box, in the local coordinate frame of the box
      for (Int_t i=0; i<3; i++) localPoint[i] = boxOrigin[i] + boxWall[i]*gRandom->Uniform(-1.0, 1.0);
      // Then test to see if this point is inside the volume
      if (beamVolume->Contains(&localPoint[0]) == kFALSE) {
         continue;
      } else {
         break;
      }
   }
   // Next transform point to the global coordinate frame
   beamMatrix->LocalToMaster(localPoint, point);
   // -- Set Starting time
   // Pick a time uniformly between start, and the filling time of the experiment
   // Thus a particle can be generated at any time in the filling time process when the beam is on
   Double_t startTime = gRandom->Uniform(0.0,fillTime);
   // -- Create Particle
   particle->SetPosition(point[0], point[1], point[2], startTime);
   
   return kTRUE;
}

//__________________________________________________________________________
Bool_t DetermineParticleVelocity(Particle* particle, const Double_t maxVelocity, const Double_t minTheta, const Double_t maxTheta, const Double_t minPhi, const Double_t maxPhi)
{
   // -- Determine a random direction vector on the unit sphere dOmega = sin(theta).dTheta.dPhi
   // Convert limits to radians
   Double_t minThetaRad = minTheta*TMath::Pi()/180.0;
   Double_t maxThetaRad = maxTheta*TMath::Pi()/180.0;
   Double_t minPhiRad = minPhi*TMath::Pi()/180.0;
   Double_t maxPhiRad = maxPhi*TMath::Pi()/180.0;
   Double_t minU = -TMath::Cos(minThetaRad);
   Double_t maxU = -TMath::Cos(maxThetaRad);
   // Get Random angle between limits
   Double_t phi = gRandom->Uniform(minPhiRad,maxPhiRad);
   Double_t u   = gRandom->Uniform(minU,maxU);
   Double_t theta = TMath::ACos(-u);
   // Convert spherical polars into cartesian nx,ny,nz
   Double_t dir[3];
   dir[0] = TMath::Cos(phi)*TMath::Sin(theta);
   dir[1] = TMath::Sin(phi)*TMath::Sin(theta);
   dir[2] = TMath::Cos(theta);
   // Check that it is a normalised direction vector
   assert(TMath::Abs(TMath::Sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]) - 1.0) < 1.E-10);

   // -- Determine Particle Velocity
   // Pick random velocity in range, distributed along curve alpha.v^2, alpha is normalisation
   Double_t normalisation = (3.)/(TMath::Power(maxVelocity,3.0));
   Double_t prob = gRandom->Uniform(0.0,1.0);
   Double_t velocity = TMath::Power(((3.0*prob)/normalisation),(1.0/3.0));   
   Double_t vel[3];
   vel[0] = velocity*dir[0];
   vel[1] = velocity*dir[1];
   vel[2] = velocity*dir[2];
   particle->SetVelocity(vel[0], vel[1], vel[2]);
   return kTRUE;
}

//__________________________________________________________________________
void DefinePolarisation(Particle* particle, const Double_t percentPolarised, const TVector3& spinAxis, const Bool_t spinUp)
{
   Double_t percentage;
   // Check if percent polarised falls outside allowed bounds
   if (percentPolarised < 0.0 || percentPolarised > 100.0) {
      percentage = 100.0;
   } else {
      percentage = percentPolarised;
   }
   // For the polarised ones define along provided axis
   if (gRandom->Uniform(0.0, 100.0) <= percentage) {
      particle->Polarise(spinAxis, spinUp);
   } else {
      // For the unpolarised ones, define a random vector on the 3D sphere, and polarise
      // at random either spin up or down along that.
      Double_t phi = gRandom->Uniform(0.0, 1.0)*2.0*TMath::Pi();
      Double_t u   = gRandom->Uniform(-1.0, 1.0);
      Double_t theta = TMath::ACos(-u);
      // Convert spherical polars into cartesian nx,ny,nz
      Double_t dir[3];
      dir[0] = TMath::Cos(phi)*TMath::Sin(theta);
      dir[1] = TMath::Sin(phi)*TMath::Sin(theta);
      dir[2] = TMath::Cos(theta);   
      TVector3 randomvec(dir[0],dir[1],dir[2]);
      TVector3 unitvec = randomvec.Unit();
      // Polarise particle along random unit vector
      if (gRandom->Uniform(0.0,2.0) <= 1.0) {
         particle->Polarise(unitvec, kTRUE);
      } else {
         particle->Polarise(unitvec, kFALSE);         
      }
   }
}
