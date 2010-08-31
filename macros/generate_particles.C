#include <iostream>

#include "include/Materials.h"
#include "include/Constants.h"
#include "include/Units.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;

namespace ModelParameters {
   // -- SourceTube Segment
   const Double_t sourceSegRMin = 0.; 
   const Double_t sourceSegRMax = 31.5*Units::mm;
   const Double_t sourceSegHalfLength = 125.*Units::mm;
   const Double_t sourceSegAngle = 90.0;
   const Double_t sourceSegYDisplacement = 125.*Units::mm;
   
   // -- Neutron Beam Area
   const Double_t neutronBeamAreaRMin = 0.;
   const Double_t neutronBeamAreaRMax = 15.*Units::mm;
   const Double_t neutronBeamAreaHalfLength = (13.0*sourceSegHalfLength);
   const Double_t neutronBeamAreaAngle = 90.0;
   const Double_t neutronBeamAreaYDisplacement = neutronBeamAreaHalfLength;
}

using namespace ModelParameters;


Bool_t FillSourceTube(const char* configFileName);
Bool_t FillRamseyCell(const char* configFileName);

Bool_t GenerateParticles(const Int_t neutrons, const Double_t fillTime, const Double_t vmax, const TGeoVolume* beamVolume, const TGeoMatrix* beamMatrix, TUCNData* dataTree);
Bool_t CreateRandomParticle(TUCNParticle* particle, const Double_t fillTime, const TGeoVolume* beamVolume, const TGeoMatrix* beamMatrix); 
Bool_t DetermineParticleMomentum(TUCNParticle* particle, const Double_t maxEnergy);

//__________________________________________________________________________
Int_t generate_particles(const char* configFileName)
{
//   FillSourceTube(configFileName);
   FillRamseyCell(configFileName);
   return 0;
}

//__________________________________________________________________________
Bool_t FillRamseyCell(const char* configFileName)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Build the ConfigFile
   TUCNConfigFile configFile(configFileName);
   
   //////////////////////////////////////////////////////////////////////////////////////
   // -- Generate the particles
   
   // Check number of runs
   const Int_t numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
   if (numberOfRuns < 1) {
      cerr << "Cannot read valid number of runs from ConfigFile" << endl;
      return EXIT_FAILURE;
   }
   cout << "Number of Runs: " << numberOfRuns << endl << endl;
   if (numberOfRuns > 1) {
      cout << "More than one Run specified. Plot_rundata needs updating to handle this." << endl;
      return EXIT_FAILURE;
   }
   
   // Create the run to store particles
   Char_t runName[20], runTitle[20];
   sprintf(runName,"Run%d",numberOfRuns);
   sprintf(runTitle,"Run no:%d",1);
   TUCNRun* run = new TUCNRun(runName,runTitle);
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Find the initial volume
   TString geomFileName = configFile.GetString("GeomFile",runName);
   TGeoManager* geoManager = TGeoManager::Import(geomFileName);
   
   TGeoVolume* volume = 0;
   TGeoMatrix* matrix = 0;
   volume = geoManager->GetVolume("RamseyCell");
   matrix = (TGeoMatrix*)geoManager->GetListOfMatrices()->FindObject("RamseyCellMat");
   
   if (volume == 0 || matrix == 0) {
      cout << volume << "\t" << matrix << endl;
      cout << "Unable to find requested volume or matrix" << endl;
      return false;
   }
   
   Int_t particles = TMath::Abs(configFile.GetInt("InitialParticles", runName));
   Double_t vmax = TMath::Abs(configFile.GetFloat("InitialMaxVelocity", runName))*Units::m/Units::s;
   Double_t fillTime = TMath::Abs(configFile.GetFloat("FillingTime", runName))*Units::s;
   
   GenerateParticles(particles, fillTime, vmax, volume, matrix, run->GetData());
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Write out the particle tree
   TString dataFileName = configFile.GetString("InputDataFile",runName);
   if (dataFileName == "") { 
      cout << "No File holding the particle data has been specified" << endl;
      return kFALSE;
   }
   // -- Open and Write to File
   TFile *file = TFile::Open(dataFileName, "recreate");
   if (!file || file->IsZombie()) {
      cerr << "Cannot open file: " << dataFileName << endl;
      return 0;
   }
   run->Write("Run1");
   cout << "Initial Particle Data for: " << run->GetName();
   cout << " was successfully written to file" << endl;
   cout << "-------------------------------------------" << endl;
   file->ls(); // List the contents of the file
   cout << "-------------------------------------------" << endl;
   delete file;
   
   // -- Enter Root interactive session
   return 0;
}

//__________________________________________________________________________
Int_t FillSourceTube(const char* configFileName)
{
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Build the ConfigFile
   TUCNConfigFile configFile(configFileName);
   
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
   
   // Check number of runs
   const Int_t numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
   if (numberOfRuns < 1) {
      cerr << "Cannot read valid number of runs from ConfigFile" << endl;
      return EXIT_FAILURE;
   }
   cout << "Number of Runs: " << numberOfRuns << endl << endl;
   if (numberOfRuns > 1) {
      cout << "More than one Run specified. Plot_rundata needs updating to handle this." << endl;
      return EXIT_FAILURE;
   }
   
   // Create the run to store particles
   Char_t runName[20], runTitle[20];
   sprintf(runName,"Run%d",numberOfRuns);
   sprintf(runTitle,"Run no:%d",1);
   TUCNRun* run = new TUCNRun(runName,runTitle);
   
   Int_t particles = TMath::Abs(configFile.GetInt("InitialParticles", runName));
   Double_t vmax = TMath::Abs(configFile.GetFloat("InitialMaxVelocity", runName))*Units::m/Units::s;
   Double_t fillTime = TMath::Abs(configFile.GetFloat("FillingTime", runName))*Units::s;
   
   GenerateParticles(particles, fillTime, vmax, neutronBeamArea, neutronBeamAreaMatrix, run->GetData());
   
   ///////////////////////////////////////////////////////////////////////////////////////
   // -- Write out the particle tree
   TString dataFileName = configFile.GetString("InputDataFile",runName);
   if (dataFileName == "") { 
      cout << "No File holding the particle data has been specified" << endl;
      return kFALSE;
   }
   // -- Open and Write to File
   TFile *file = TFile::Open(dataFileName, "recreate");
   if (!file || file->IsZombie()) {
      cerr << "Cannot open file: " << dataFileName << endl;
      return 0;
   }
   run->Write("Run1");
   cout << "Initial Particle Data for: " << run->GetName();
   cout << " was successfully written to file" << endl;
   cout << "-------------------------------------------" << endl;
   file->ls(); // List the contents of the file
   cout << "-------------------------------------------" << endl;
   delete file;
   
   // -- Enter Root interactive session
   return 0;
}

//__________________________________________________________________________
Bool_t GenerateParticles(const Int_t neutrons, const Double_t fillTime, const Double_t vmax, const TGeoVolume* beamVolume, const TGeoMatrix* beamMatrix, TUCNData* dataTree)
{
   // Generates a uniform distribution of particles with random directions all with 
   // the same total energy (kinetic plus potential) defined at z = 0.
   cout << "-------------------------------------------" << endl;
   cout << "Generating " << neutrons << " Particles." << endl;
   
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
   
   Double_t maxEnergy = 0.5*Constants::neutron_mass*TMath::Power(vmax,2.0);
   cout << "Max Energy (neV): " << maxEnergy/Units::neV << endl;
   
   // -- Loop over the total number of particles to be created. 
   for (Int_t i = 0; i < neutrons; i++) {
      // -- Create particle at a random position inside beam volume
      TUCNParticle* particle = new TUCNParticle();
      particle->SetId(i);
      CreateRandomParticle(particle, fillTime, beamVolume, beamMatrix);
      // -- Initialise particle's momentum
      DetermineParticleMomentum(particle, maxEnergy);
      // -- Fill histograms
      initialXHist->Fill(particle->X());
      initialYHist->Fill(particle->Y());
      initialZHist->Fill(particle->Z());
      initialVXHist->Fill(particle->Vx());
      initialVYHist->Fill(particle->Vy());
      initialVZHist->Fill(particle->Vz());
      initialVHist->Fill(particle->V());
      initialTHist->Fill(particle->T());
      // -- Add particle to data tree
      dataTree->AddInitialParticleState(particle);
   }
   
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
   cout << "Successfully generated " << neutrons << " particles." << endl;
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
   Double_t maxVelocity = TMath::Sqrt(2.*maxEnergy/Constants::neutron_mass);
   Double_t normalisation = (3.)/(TMath::Power(maxVelocity,3.0));
   Double_t prob = gRandom->Uniform(0.0,1.0);
   Double_t velocity = TMath::Power(((3.0*prob)/normalisation),(1.0/3.0));
   
   Double_t kineticEnergy = 0.5*Constants::neutron_mass*TMath::Power(velocity,2.0);
   // -- Determine Particle Momentum (eV)
   Double_t mom[3];
   Double_t momentum = TMath::Sqrt(2.0*particle->Mass_eV()*kineticEnergy);
   mom[0] = momentum*dir[0];
   mom[1] = momentum*dir[1];
   mom[2] = momentum*dir[2];  
   
   particle->SetMomentum(mom[0], mom[1], mom[2], kineticEnergy);
   
   return kTRUE;
}
