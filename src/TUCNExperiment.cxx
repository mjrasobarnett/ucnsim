// TUCNExperiment
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <stdio.h>

#include "TUCNExperiment.h"

#include "TUCNFieldManager.h"
#include "TUCNGeoManager.h"
#include "TUCNGeoBuilder.h"
#include "TUCNGeoNavigator.h"
#include "TUCNGeoMaterial.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"
#include "TUCNUniformMagField.h"
#include "TUCNParabolicMagField.h"
#include "TUCNConfigFile.h"

#include "TFile.h"
#include "TString.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TVirtualGeoTrack.h"
#include "TGeoManager.h"

#include "Units.h"
#include "Constants.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;

//#define VERBOSE_MODE

ClassImp(TUCNExperiment)

//_____________________________________________________________________________
TUCNExperiment::TUCNExperiment()
					:TNamed("Experiment", "The Experiment")
{
// -- Default constructor
   Info("TUCNExperiment", "Dummy Constructor");
	// Create the GeoManager
	new TGeoManager("GeoManager", "The Geometry Manager");
	// Create the UCNNavigator and initialise in the UCNManager
	// This must be done so that the manager defaults to our navigator. Otherwise it will create a new defualt navigator
	// that is not of type UCNNavigator.
	TUCNGeoNavigator* navigator = new TUCNGeoNavigator(this->GeoManager());
	Int_t navigatorIndex = this->GeoManager()->AddNavigator(navigator);
	this->GeoManager()->SetCurrentNavigator(navigatorIndex);
	// Initialise remaining data members
	fConfigFile = 0;
	fFieldManager = 0;
	fNumberOfRuns = 0;
	fSourceVolumeIndex = 0;
	fSourceMatrixIndex = 0;
} 

//_____________________________________________________________________________
TUCNExperiment::TUCNExperiment(std::string configFileName)
					:TNamed("Experiment", "The Experiment")
{
// -- Default constructor
   Info("TUCNExperiment", "Constructor");
	// Create the GeoManager
	new TGeoManager("GeoManager", "The Geometry Manager");
	// Create the UCNNavigator and initialise in the UCNManager
	// This must be done so that the manager defaults to our navigator. Otherwise it will create a new defualt navigator
	// that is not of type UCNNavigator.
	TUCNGeoNavigator* navigator = new TUCNGeoNavigator(this->GeoManager());
	Int_t navigatorIndex = this->GeoManager()->AddNavigator(navigator);
	this->GeoManager()->SetCurrentNavigator(navigatorIndex);
	// Initialise remaining data members
	fConfigFile = new TUCNConfigFile(configFileName);
	fFieldManager = new TUCNFieldManager();
	fNumberOfRuns = 0;
	fSourceVolumeIndex = 0;
	fSourceMatrixIndex = 0;
}

//_____________________________________________________________________________
TUCNExperiment::TUCNExperiment(const TUCNExperiment& other)
					:TNamed(other),
					 fConfigFile(other.fConfigFile),
					 fFieldManager(other.fFieldManager),
					 fNumberOfRuns(other.fNumberOfRuns),
					 fSourceVolumeIndex(other.fSourceVolumeIndex),
					 fSourceMatrixIndex(other.fSourceMatrixIndex)
{
// -- Copy Constructor
	Info("TUCNExperiment", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNExperiment& TUCNExperiment::operator=(const TUCNExperiment& other)
{
// --assignment operator
	if(this!=&other) {
      TNamed::operator=(other);
		fConfigFile = other.fConfigFile;
		fFieldManager = other.fFieldManager;
		fNumberOfRuns = other.fNumberOfRuns;
		fSourceVolumeIndex = other.fSourceVolumeIndex;
		fSourceMatrixIndex = other.fSourceMatrixIndex;
	}
   return *this;
}

//______________________________________________________________________________
TUCNExperiment::~TUCNExperiment()
{ 
// -- Destructor
	Info("TUCNExperiment", "Destructor");
	if (fConfigFile) delete fConfigFile;
	if (gGeoManager) delete gGeoManager;
	if (fFieldManager) delete fFieldManager;
}

// -- METHODS --
//_____________________________________________________________________________
TGeoVolume* TUCNExperiment::MakeUCNBox(const char *name, TGeoMedium *medium, Double_t dx, Double_t dy, Double_t dz)
{
// Make in one step a volume pointing to a box shape with given medium.
	return TUCNGeoBuilder::UCNInstance(this->GeoManager())->MakeUCNBox(name, medium, dx, dy, dz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNExperiment::MakeUCNTube(const char *name, TGeoMedium *medium,
	Double_t rmin, Double_t rmax, Double_t dz)
{
// Make in one step a volume pointing to a tube shape with given medium.
	return TUCNGeoBuilder::UCNInstance(this->GeoManager())->MakeUCNTube(name, medium, rmin, rmax, dz);
}

//_____________________________________________________________________________
void	TUCNExperiment::SetSourceVolume(TGeoVolume* sourceVolume)
{
	// Find object in the list and store the array index
	Int_t index = this->GeoManager()->GetListOfVolumes()->IndexOf(sourceVolume);
	if (index < 0) {
		cerr << "Source volume not found in array. Volume must be registered with GeoManager first." << endl;
	} else {
		fSourceVolumeIndex = index;
	}
}

//_____________________________________________________________________________
void	TUCNExperiment::SetSourceMatrix(TGeoMatrix* sourceMatrix)
{
	// Find object in the list and store the array index
	TObjArray* matricesList = this->GeoManager()->GetListOfMatrices();
	if (!sourceMatrix->IsRegistered()) {
		cerr << "SourceMatrix not previously registered. Registering now..." << endl;
		sourceMatrix->RegisterYourself();
	}
	
	Int_t matrixIndex = matricesList->IndexOf(sourceMatrix);
	Int_t errorValue = matricesList->LowerBound() - 1;
	if (matrixIndex == errorValue) {
		cerr << "Error: Cannot find matrix in the GeoManager. No source matrix will be set" << endl;
	} else {
		fSourceMatrixIndex = matrixIndex;
	}
}

//_____________________________________________________________________________
TGeoVolume*	TUCNExperiment::GetSourceVolume() const
{
	// Find object in the list and return
	if (fSourceVolumeIndex > 0) {
		TGeoVolume* sourceVolume = static_cast<TGeoVolume*>(this->GeoManager()->GetListOfVolumes()->At(fSourceVolumeIndex));
		assert(sourceVolume != 0);
		return sourceVolume;
	} else {
		cerr << "No Source volume has been set" << endl;
		return 0;
	}
}

//_____________________________________________________________________________
TGeoMatrix*	TUCNExperiment::GetSourceMatrix() const
{
	// Find object in the list and return
	if (fSourceMatrixIndex > 0) {
		TGeoMatrix* sourceMatrix = static_cast<TGeoMatrix*>(this->GeoManager()->GetListOfMatrices()->At(fSourceMatrixIndex));
		assert(sourceMatrix != 0);
		return sourceMatrix;
	} else {
		cerr << "No Source matrix has been set" << endl;
		return 0;
	}
}

//______________________________________________________________________________
Bool_t TUCNExperiment::BuildGeometry(TGeoManager* geoManager, TUCNConfigFile* configFile)
{
	// -------------------------------------
	// BUILDING GEOMETRY
	
	// Read in value of FermiPotential
	Double_t V = 0.0;
	Double_t fermiPotentialneV  = configFile->GetFloat("FermiPotential(neV)", "Geometry")*Units::neV;
	Double_t fermiPotentialm    = configFile->GetFloat("FermiPotential(m)", "Geometry")*Constants::height_equivalent_conversion;
	// Determine which of these two values is zero (if any) and returning the remaining value.
	if (fermiPotentialneV != 0.0) {
		V = fermiPotentialneV;
	} else if (fermiPotentialm != 0.0) {
		V = fermiPotentialm; 
	} else {
		Warning("BuildGeometry","No value of the fermiPotential has been set! Aborting.");
		return kFALSE;
	}
	// Read in value of f
	Double_t f = configFile->GetFloat("f", "Geometry");
	// Check values were set
	if (f == 0.0 || V == 0.0) {
		cout << "Boundary Material properties, f or V, have not been set! Check ConfigFile and try again." << endl;
		return kFALSE;
	}
	// Determine W
	Double_t W = f*V;
	
	// Materials
	TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", V, W);
	cout << "Boundary Material has been defined with V (neV): " << V/Units::neV << "  W (neV): " << W/Units::neV << endl;
	
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackHole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
	
	// -- Making Top Volume
	TGeoVolume* chamber = this->MakeUCNBox("TOP",blackHole,20,20,20);
	geoManager->SetTopVolume(chamber);
	
	// -- Make a GeoTube object via the UCNGeoManager
	Double_t rMin = 0.0, rMax = 0.236, length = 0.121; 
	TGeoVolume* tube = this->MakeUCNTube("tube",boundary, rMin, rMax, length/2.);
	TGeoVolume* innerTube = this->MakeUCNTube("innerTube",vacuum, rMin, rMax-0.001, (length-0.001)/2.);
	
	
	// -- Define the transformation of the volume
	TGeoRotation r1,r2; 
	r1.SetAngles(0,0,0);          //rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,length/2.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	
	TGeoVolume* volume = tube;
	TGeoVolume* innerVolume = innerTube;
	
	// -- Create the nodes	
	volume->AddNode(innerVolume, 1);
	chamber->AddNode(volume, 1, matrix);
	
	// -- Define the Source in our geometry where we will create the particles
	this->SetSourceVolume(innerVolume);
	this->SetSourceMatrix(matrix);
	
	// -- Arrange and close geometry
	geoManager->CloseGeometry();
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNExperiment::ClearTracks()
{
	// Wipe the GeoManager of all tracks stored, and also delete the particles held by these Tracks
	// Why aren't ROOT TParticles owned by the track anyway!!??
	cout << "-------------------------------------------" << endl;
	cout << "Cleaning up all Tracks and Particles from the current Run." << endl;
	// -- First Reset the Navigator to wipe current Node etc...
	this->GeoManager()->GetCurrentNavigator()->ResetAll();	
	// -- Delete all the TParticles stored by the tracks
	for (Int_t trackID = 0; trackID < this->GeoManager()->GetNtracks(); trackID++) {
		TObject* particle = this->GeoManager()->GetTrack(trackID)->GetParticle();
		if (particle != NULL) {delete particle; particle = 0;}
	}
	// -- Finally Clear out all the tracks from the Manager
	this->GeoManager()->ClearTracks();
	cout << "Tracks and Particles of current Run have been successfully deleted." << endl;
	cout << "-------------------------------------------" << endl;
	return kTRUE;
}

//______________________________________________________________________________
Bool_t TUCNExperiment::GenerateParticles(TUCNRun* run)
{
	// Generates a uniform distribution of particles with random directions all with the same total energy
	// (kinetic plus potential) defined at z = 0.	
	cout << "-------------------------------------------" << endl;
	cout << "Generating Particles for Run: " << run->GetName() << endl;
	Int_t neutrons = run->Neutrons();
	Double_t totalEnergy = run->TotalEnergy();
	
	// -- 0. Get the source volume and matrix that places it in the geometry
	if (this->GetSourceVolume() == NULL || this->GetSourceMatrix() == NULL) {
		Error("GenerateParticle","Source Volume/Matrix has not been set.");
		return kFALSE;
	}
	
	// -- 1. Check for presence of field
	// If there is a gravitational field we will need to later determine the kinetic energy from the TotalEnergy provided. 
	Bool_t fieldPresent = kFALSE;
	Double_t gravAcceleration = 0.0;
	Double_t fieldDir[3] = {0.,0.,0.};
	
	if (this->FieldManager()->GravField() != NULL) {
		fieldPresent = kTRUE;
		gravAcceleration = (this->FieldManager()->GravField()->GravAcceleration());
		fieldDir[0] = this->FieldManager()->GravField()->Nx(); 
		fieldDir[1] = this->FieldManager()->GravField()->Ny();
		fieldDir[2] = this->FieldManager()->GravField()->Nz();
	}

	// -- 2. Get the Source volume and matrix transformation of volume	
	// Determine the dimensions of the source volume's bounding box.
	TGeoShape* shape = this->GetSourceVolume()->GetShape();
	TGeoBBox* boundingBox = static_cast<TGeoBBox*>(shape);
	const Double_t boxWallX = boundingBox->GetDX();
	const Double_t boxWallY = boundingBox->GetDY();
	const Double_t boxWallZ = boundingBox->GetDZ();
	const Double_t boxOriginX = (boundingBox->GetOrigin())[0];
	const Double_t boxOriginY = (boundingBox->GetOrigin())[1];
	const Double_t boxOriginZ = (boundingBox->GetOrigin())[2];

	#ifdef VERBOSE_MODE
		cout << "Generating random particle inside source volume: " << this->GetSourceVolume()->GetName() << endl;
		cout << "Walls of Bounding Box - X: " << boxWallX << "\t" << "Y: " << boxWallY << "\t" << "Z: " << boxWallZ << endl; 
		cout << "Origin of Bounding Box - X: " << boxOriginX << "\t" << "Y: " << boxOriginY << "\t" << "Z: " << boxOriginZ << endl;
		cout << "Finding random point inside source volume..." << endl;
	#endif
	
	// -- Loop over the total number of particles to be created. 
	for (Int_t i = 0; i < neutrons; i++) {
	
		// -- 3. Find a random point inside the Volume
		TGeoNode* currentNode = NULL;
		Double_t point[3] = {0.,0.,0.}, lpoint[3] = {0.,0.,0.}, dir[3] = {0.,0.,0.}, mom[3] = {0.,0.,0.};
		Double_t kineticEnergy = 0.0, momentum = 0.0, height = 0.0, startTime = 0.0;
	
		// Create a particle
		TUCNParticle* particle = new TUCNParticle();
	
		// Find a random point inside the volume provided
		while (currentNode == NULL) {
			// First generate random point inside bounding box, in the local coordinate frame of the box
			lpoint[0] = boxOriginX + boxWallX*gRandom->Uniform(-1.0, 1.0);
			lpoint[1] = boxOriginY + boxWallY*gRandom->Uniform(-1.0, 1.0);
			lpoint[2] = boxOriginZ + boxWallZ*gRandom->Uniform(-1.0, 1.0);
			// Next transform point to the global coordinate frame
			this->GetSourceMatrix()->LocalToMaster(lpoint, point);
			// Then test to see if this point is inside the volume
			this->GeoManager()->SetCurrentPoint(point);
			currentNode = this->GeoManager()->FindNode();
			
			if (currentNode == NULL) {
				cerr << "Point - X: " << point[0] << "\t" << "Y: " << point[1] << "\t" << "Z:" << point[2] << "\t" << "not found in geometry!" << endl;
				return kFALSE;
			} else if (currentNode->GetVolume() != this->GetSourceVolume()) {
				currentNode = NULL;
				continue;
			}
			
			// -- 5. Calculate the particle's kinetic energy

			// If there is a grav field we need to calculate the height of our particle in the gravitational potential 
			// in order to get the particle's kinetic energy from the defined total energy. 
			if (fieldPresent == kTRUE) {
				// Determine the height of our particle in the global coordinate system of TOP.
				// Take the dot product of the point vector with the field direction unit vector to get the height
				// of this point in the gravitational field. This assumes a convention that 'height' is measured along
				// an axis that INCREASES in the opposite direction to the field direction vector (which is usually 'downwards')
				height = -1.0*(point[0]*fieldDir[0] + point[1]*fieldDir[1] + point[2]*fieldDir[2]);
				#ifdef VERBOSE_MODE
					cout << "Height from origin of TOP volume: " <<  height << endl; 
				#endif
			}

			// Kinetic Energy
			kineticEnergy = totalEnergy - particle->Mass_GeV_c2()*gravAcceleration*height;
			if (kineticEnergy <= 0.0) {
				// Energy is negative. Find another point.
				currentNode = NULL;
				kineticEnergy = 0.0;
				height = 0.0;
				continue;
			}
			#ifdef VERBOSE_MODE
				cout << "Total Energy (neV): " << totalEnergy/Units::neV << "\t" <<  "KE (neV): " << kineticEnergy/Units::neV << endl;; 
			#endif
			
			
		}
		#ifdef VERBOSE_MODE
			cout << "Final Point Chosen - X: " << point[0] << "\t" << "Y: " << point[1] << "\t" << "Z:" << point[2] << endl;
		#endif

		// -- 4. Determine a random direction vector on the unit sphere weighted by an additional cos(theta).
		// Phi ranges from 0 to 2*Pi, Theta from 0 to Pi.
		Double_t phi = gRandom->Uniform(0.0, 1.0)*2.0*TMath::Pi();
		Double_t u   = gRandom->Uniform(0.0, 0.5);
		Double_t theta = 0.0; 
		if (gRandom->Uniform(0.0,1.0) > 0.5) {
			theta = TMath::ACos(TMath::Sqrt(1.0 - 2*u));
		} else {
			theta = TMath::ACos(-TMath::Sqrt(1.0 - 2*u));
		}
		dir[0] = TMath::Cos(phi)*TMath::Sin(theta);
		dir[1] = TMath::Sin(phi)*TMath::Sin(theta);
		dir[2] = TMath::Cos(theta);
		// Check that it is a normalised direction vector
		assert(TMath::Abs(TMath::Sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]) - 1.0) < 1.E-10);
	
		// -- 6. Initialise Particle
		// Momentum (eV)
		momentum = TMath::Sqrt(2.0*particle->Mass_GeV()*kineticEnergy);
		mom[0] = momentum*dir[0];
		mom[1] = momentum*dir[1];
		mom[2] = momentum*dir[2];
		
		#ifdef VERBOSE_MODE
			cout << "Particle Momentum (eV) - P: " << momentum/Units::eV << "\t" << "Px:" << mom[0]/Units::eV << "\t" << "Py: " << mom[1]/Units::eV << "\t" << "Pz: " << mom[2]/Units::eV << endl; 
		#endif
		
		// Initialise particle 
		particle->SetProductionVertex(point[0], point[1], point[2], startTime);
		particle->SetMomentum(mom[0], mom[1], mom[2], kineticEnergy);
//		particle->Print();
		
		// -- 7. Create the track in the geoManager
		// Make track and add to list
		#ifdef VERBOSE_MODE
			cout << "Creating particle track..." << endl; 
		#endif
		TVirtualGeoTrack* track = this->GeoManager()->MakeTrack(i, particle->GetPdgCode(), particle);
		this->GeoManager()->AddTrack(track);
		// Add initial point to track
		track->AddPoint(particle->Vx(), particle->Vy(), particle->Vz(), particle->T());
	}
	cout << "Successfully generated " << this->GeoManager()->GetNtracks() << " particles." << endl;
	cout << "-------------------------------------------" << endl;	
	return kTRUE;
}

//______________________________________________________________________________
Bool_t TUCNExperiment::Initialise()
{
// -- Initialise the runManager with from the Configuration File and create the runs
	cout << "-------------------------------------------" << endl;
	cout << "Initialising the Experiment" << endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Geometry Creation
	///////////////////////////////////////////////////////////////////////////////////////
	cout << "-------------------------------------------" << endl;
	cout << "Building Geometry..." << endl;
	cout << "-------------------------------------------" << endl;	
	// Build Geometry
	if (!(this->BuildGeometry(this->GeoManager(), this->ConfigFile()))) { 
		Warning("Initialise","Failed building geometry.");
		return kFALSE;
	}
	cout << "-------------------------------------------" << endl;
	cout << "Geometry has been created succesfully" << endl;
	cout << "Source Volume: " << this->GetSourceVolume()->GetName() << endl;
	cout << "Source Matrix: " << this->GetSourceMatrix() << endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Field Initialisation
	///////////////////////////////////////////////////////////////////////////////////////
	if(!(this->FieldManager()->Initialise(this->ConfigFile()))) {
		Warning("Initialise","Failed in field initialisation.");
		return kFALSE;
	}
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Storing the Number of Runs
	///////////////////////////////////////////////////////////////////////////////////////
	Int_t numberOfRuns = this->ConfigFile()->GetInt("NumberOfRuns","Runs");
	if (numberOfRuns < 1) {
		Error("Initialise", "Number of Runs is less than 1!");
		return kFALSE;
	} else {
		fNumberOfRuns = numberOfRuns;
	}
	return kTRUE;
}

//______________________________________________________________________________
Bool_t TUCNExperiment::Run()
{
// -- For every Run, initialise itself, create the tracks/particles, propagate them, write the run to file, tidy up
	
	// -- Export Geometry to File
	TString outputFile = this->ConfigFile()->GetString("OutputFile","I/O");
	cout << "-------------------------------------------" << endl;
	cout << "Exporting Geometry to DataFile: " << outputFile << endl;
	cout << "-------------------------------------------" << endl;
	if (!(this->GeoManager()->Export(outputFile))) {
		Error("Initialise", "Could not open export GeoManager to file!");
		return kFALSE;
	}
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Create the Runs
	///////////////////////////////////////////////////////////////////////////////////////
	for (Int_t runNumber = 1; runNumber <= this->NumberOfRuns(); runNumber++) {
		// Create the Run
		Char_t name[10], title[20];
		sprintf(name,"Run%d",runNumber); 
		sprintf(title,"Run no:%d",runNumber);
		cout << endl << endl;
		cout << "-------------------------------------------" << endl;
		cout << "Creating New Run, Name: " << name << " Title: " << title << endl;
		cout << "-------------------------------------------" << endl;
		TUCNRun theRun(name, title);
		
		// Initialise the Run
		if (!(theRun.Initialise(this->ConfigFile()))) {
			Error("Run","Run: %s failed to initialise properly.", theRun.GetName());
			return kFALSE;
		}
		
		// Create Particles/Tracks
		if (!(this->GenerateParticles(&theRun))) {
			Error("Run","Could not create particles for Run: %s.", theRun.GetName());
			return kFALSE;
		}
		
		// Propagate the Run
		if (!(theRun.Propagate(this->GeoManager(), this->FieldManager()))) {
			Fatal("Run","Run: &s failed to Propagate Correctly", theRun.GetName());
			return kFALSE;
		}
		
		// Write the Run data to outputFile
		if (!(theRun.Export(outputFile))) {
			Error("Run","Could not write Run, %s, out to File.", theRun.GetName());
			return kFALSE;
		}
		
		// Tidy up
		if (!(this->ClearTracks())) {
			Error("Run","Could not clean up current tracks/particles completely.");
			return kFALSE;
		}
	}
	return kTRUE;
}

//______________________________________________________________________________
Bool_t TUCNExperiment::Export()
{
// -- Export Experiment to File
	TString outputFile = this->ConfigFile()->GetString("OutputFile","I/O");
	cout << "-------------------------------------------" << endl;
	cout << "Writing " << this->GetName() << " out to File: " << outputFile << endl;
	if (!(outputFile.Contains(".root"))) {
		Error("Export", "OutputFile specified is not a ROOT file");
		return kFALSE;
	} else {
		// Save geometry as a root file
		TFile *f = TFile::Open(outputFile,"update");
		if (!f || f->IsZombie()) {
		   Error("Export","Cannot open file");
		   return kFALSE;
		}
		char keyname[256];
		strcpy(keyname,this->GetName());
		this->Write(keyname);
		cout << keyname << " was successfully written to file" << endl;
		cout << "-------------------------------------------" << endl;
		f->ls(); // List the contents of the file
		cout << "-------------------------------------------" << endl;
		delete f;
	}
	return kTRUE;
}
