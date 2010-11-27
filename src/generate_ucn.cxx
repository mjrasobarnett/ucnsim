#include <iostream>
#include <string>
#include <cassert>

#include "TUCNConfigFile.h"
#include "TUCNInitialConfig.h"
#include "TUCNData.h"
#include "TUCNParticle.h"
#include "TUCNGeoBBox.h"
#include "TUCNGeoTube.h"
#include "TUCNVolume.h"

#include "TMath.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TRint.h"

#include "Materials.h"
#include "Constants.h"
#include "Units.h"
#include "DataFileHierarchy.h"
#include "GeomParameters.h"

using std::cin;
using std::cout;
using std::endl;
using std::cerr;
using std::string;

using namespace GeomParameters;

// Function Declarations
Bool_t FillSourceTube(const TUCNInitialConfig& initialConfig);
Bool_t FillRamseyCell(const TUCNInitialConfig& initialConfig);
Bool_t GenerateParticles(const TUCNInitialConfig& initialConfig, const TGeoVolume* beamVolume, const TGeoMatrix* beamMatrix);
Bool_t CreateRandomParticle(TUCNParticle* particle, const Double_t fillTime, const TGeoVolume* beamVolume, const TGeoMatrix* beamMatrix); 
Bool_t DetermineParticleMomentum(TUCNParticle* particle, const Double_t maxEnergy);
void DefinePolarisation(TUCNParticle* particle, const Double_t percentPolarised, const TVector3& spinAxis, const Bool_t spinUp);

//__________________________________________________________________________
Int_t main(Int_t argc,Char_t **argv)
{
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
   TRint *theApp = new TRint("FittingApp", &argc, argv);
   
   // Read in Batch Configuration file to find the Initial Configuration File
   TUCNConfigFile configFile(configFileName);
   const string initialConfigFileName = configFile.GetString("Config","Initialisation");
   if (initialConfigFileName.empty() == kTRUE) {
      cout << "Unable to read in Initialisation Configuration file name" << endl;
      return -1;
   }
   
   // Read in Initial Configuration from file.
   TUCNInitialConfig initialConfig(initialConfigFileName);   
   // Ask User to choose which Volume to fill
   cout << "Select which volume to fill with UCN: " << endl;
   cout.width(15);
   cout << "   Source Tube" << " -- Enter: 1" << endl;
   cout.width(15);
   cout << "   HV Cell" << " -- Enter: 2" << endl;
   cout.width(15);
   cout << "   Exit" << " -- Enter: 0" << endl;
   Int_t userInput = -1;   
   while (userInput != 0) {
      cin >> userInput;
      if (userInput == 1) {
         FillSourceTube(initialConfig);
         break;
      } else if (userInput == 2) {
         FillRamseyCell(initialConfig);
         break;
      } else if (userInput == 0) {
         break;
      } else {
         cout << "Sorry I didn't understand that. Please try again." << endl;
      }
   }
   
   // Enter ROOT interactive session
   theApp->Run();
   return EXIT_SUCCESS;
}


//__________________________________________________________________________
Bool_t FillRamseyCell(const TUCNInitialConfig& initialConfig)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Find the initial volume
   TString geomFileName = initialConfig.GeomFileName();
   TGeoManager* geoManager = TGeoManager::Import(geomFileName);
   
   TGeoVolume* volume = 0;
   TGeoMatrix* matrix = 0;
   volume = geoManager->GetVolume("HVCell");
   matrix = (TGeoMatrix*)geoManager->GetListOfMatrices()->FindObject("HVCellMat");
   if (volume == 0 || matrix == 0) {
      cout << volume << "\t" << matrix << endl;
      cout << "Unable to find requested volume or matrix" << endl;
      return false;
   }
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Generate the particles
   GenerateParticles(initialConfig, volume, matrix);
   
   return 0;
}

//__________________________________________________________________________
Bool_t FillSourceTube(const TUCNInitialConfig& initialConfig)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Make the particle beam volume
   TGeoManager* geoManager = new TGeoManager("GeoManager","Geometry Manager");
   Materials::BuildMaterials(geoManager);
   TGeoMedium* liquidHelium = geoManager->GetMedium("HeliumII");
   
   TUCNGeoTube* tube = new TUCNGeoTube("NeutronBeamArea", neutronBeamAreaRMin, neutronBeamAreaRMax, neutronBeamAreaHalfLength);
   TUCNTrackingVolume* neutronBeamArea = new TUCNTrackingVolume("NeutronBeamArea", tube, liquidHelium);
   
   TGeoRotation neutronBeamAreaRot("NeutronBeamAreaRot",0,neutronBeamAreaAngle,0); // phi,theta,psi
   TGeoTranslation neutronBeamAreaTra("NeutronBeamAreaTra",0.,neutronBeamAreaYDisplacement,0.);
   TGeoCombiTrans neutronBeamAreaCom(neutronBeamAreaTra,neutronBeamAreaRot);
   TGeoHMatrix neutronBeamAreaMat = neutronBeamAreaCom;
   TGeoMatrix* neutronBeamAreaMatrix = new TGeoHMatrix(neutronBeamAreaMat);
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Generate the particles
   GenerateParticles(initialConfig, neutronBeamArea, neutronBeamAreaMatrix);
   
   return 0;
}

//__________________________________________________________________________
Bool_t GenerateParticles(const TUCNInitialConfig& initialConfig, const TGeoVolume* beamVolume, const TGeoMatrix* beamMatrix)
{
   // Generates a uniform distribution of particles with random directions all with 
   // the same total energy (kinetic plus potential) defined at z = 0.   
   Int_t particles = TMath::Abs(initialConfig.InitialParticles());
   Double_t vmax = TMath::Abs(initialConfig.InitialMaxVelocity())*Units::m/Units::s;
   Double_t fillTime = TMath::Abs(initialConfig.FillingTime())*Units::s;
   
   Double_t percentPolarised = initialConfig.PercentPolarised();
   TVector3 spinAxis = initialConfig.SpinAxis();
   Bool_t spinUp = initialConfig.SpinUp();
   
   cout << "-------------------------------------------" << endl;
   cout << "Generating " << particles << " Particles." << endl;
   // Create Histograms to view the initial particle distributions
   Int_t nbins = 100;
   Double_t boundary[3];
   boundary[0] = neutronBeamAreaRMax;
   boundary[1] = 2.0*neutronBeamAreaHalfLength;
   boundary[2] = neutronBeamAreaRMax;
   cout << "Boundary X (m): " << boundary[0] << "\t" << "Y (m): " << boundary[1] << "\t";
   cout << "Z (m): " << boundary[2] << endl;
   
   TH1F* initialXHist = new TH1F("InitialXHist","Initial X Position, Units of (m)", nbins, -boundary[0], boundary[0]);
   TH1F* initialYHist = new TH1F("InitialYHist","Initial Y Position, Units of (m)", nbins, 0., boundary[1]); 
   TH1F* initialZHist = new TH1F("InitialZHist","Initial Z Position, Units of (m)", nbins, -boundary[2], boundary[2]);
   
   TH1F* initialVXHist = new TH1F("InitialVXHist","Initial VX velocity, Units of (m/s)", nbins, -vmax, vmax);
   TH1F* initialVYHist = new TH1F("InitialVYHist","Initial VY velocity, Units of (m/s)", nbins, -vmax, vmax);
   TH1F* initialVZHist = new TH1F("InitialVZHist","Initial VZ velocity, Units of (m/s)", nbins, -vmax, vmax);
   TH1F* initialVHist = new TH1F("InitialVHist","Initial V velocity, Units of (m/s)", nbins, 0.0, vmax);
   TH1F* initialTHist = new TH1F("InitialTHist","Initial T time, Units of s", nbins, 0.0, fillTime);
   
   Double_t maxEnergy = 0.5*Neutron::mass_eV_c2*TMath::Power(vmax,2.0);
   cout << "Max Energy (neV): " << maxEnergy/Units::neV << endl;
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Create storage for the particles
   TUCNData * data = new TUCNData();
   data->Initialise(initialConfig);
   
   // -- Loop over the total number of particles to be created. 
   for (Int_t i = 1; i <= particles; i++) {
      // -- Create particle at a random position inside beam volume
      TUCNParticle* particle = new TUCNParticle();
      particle->SetId(i);
      CreateRandomParticle(particle, fillTime, beamVolume, beamMatrix);
      // -- Initialise particle's momentum
      DetermineParticleMomentum(particle, maxEnergy);
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
      // -- Add particle to data file
      data->SaveParticle(particle, Folders::initialstates);
   }
   // -- Close the data
   delete data;
   data = NULL;
   
   TCanvas *canvas1 = new TCanvas("InitialPhaseSpace","Initial Phase Space",60,0,1000,800);
   canvas1->Divide(4,2);
   canvas1->cd(1);
   initialXHist->Draw();
   canvas1->cd(2);
   initialYHist->Draw();
   canvas1->cd(3);
   initialZHist->Draw();
   canvas1->cd(4);
   initialTHist->Draw();
   canvas1->cd(5);
   initialVXHist->Draw();
   canvas1->cd(6);
   initialVYHist->Draw();
   canvas1->cd(7);
   initialVZHist->Draw();
   canvas1->cd(8);
   initialVHist->Draw();
   cout << "Successfully generated " << particles << " particles." << endl;
   cout << "-------------------------------------------" << endl;	
   return kTRUE;
}

//__________________________________________________________________________
Bool_t CreateRandomParticle(TUCNParticle* particle, const Double_t fillTime, const TGeoVolume* beamVolume, const TGeoMatrix* beamMatrix) 
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
   particle->SetVertex(point[0], point[1], point[2], startTime);
   
   return kTRUE;
}

//__________________________________________________________________________
Bool_t DetermineParticleMomentum(TUCNParticle* particle, const Double_t maxEnergy)
{
   // -- Determine a random direction vector on the unit sphere dOmega = sin(theta).dTheta.dPhi
   // Phi ranges from 0 to 2*Pi, Theta from 0 to Pi.
   Double_t phi = gRandom->Uniform(0.0, 1.0)*2.0*TMath::Pi();
   Double_t u   = gRandom->Uniform(-1.0, 1.0);
   Double_t theta = TMath::ACos(-u);
   // Convert spherical polars into cartesian nx,ny,nz
   Double_t dir[3];
   dir[0] = TMath::Cos(phi)*TMath::Sin(theta);
   dir[1] = TMath::Sin(phi)*TMath::Sin(theta);
   dir[2] = TMath::Cos(theta);
   // Check that it is a normalised direction vector
   assert(TMath::Abs(TMath::Sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]) - 1.0) < 1.E-10);

   // -- Determine Particle Kinetic Energy
   // Pick random velocity in range, distributed along curve alpha.v^2, alpha is normalisation
   Double_t maxVelocity = TMath::Sqrt(2.*maxEnergy/Neutron::mass_eV_c2);
   Double_t normalisation = (3.)/(TMath::Power(maxVelocity,3.0));
   Double_t prob = gRandom->Uniform(0.0,1.0);
   Double_t velocity = TMath::Power(((3.0*prob)/normalisation),(1.0/3.0));
   
   Double_t kineticEnergy = 0.5*Neutron::mass_eV_c2*TMath::Power(velocity,2.0);
   // -- Determine Particle Momentum (eV)
   Double_t mom[3];
   Double_t momentum = TMath::Sqrt(2.0*Neutron::mass_eV*kineticEnergy);
   mom[0] = momentum*dir[0];
   mom[1] = momentum*dir[1];
   mom[2] = momentum*dir[2];  
   
   particle->SetMomentum(mom[0], mom[1], mom[2], kineticEnergy);
   
   return kTRUE;
}

//__________________________________________________________________________
void DefinePolarisation(TUCNParticle* particle, const Double_t percentPolarised, const TVector3& spinAxis, const Bool_t spinUp)
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