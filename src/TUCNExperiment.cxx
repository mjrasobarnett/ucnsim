// TUCNExperiment
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>
#include <vector>
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

//#define VERBOSE_MODE

ClassImp(TUCNExperiment)

//_____________________________________________________________________________
TUCNExperiment::TUCNExperiment()
					:TNamed("Experiment", "The Experiment")
{
// -- Default constructor
   Info("TUCNExperiment", "Constructor");
	fGeoManager = new TGeoManager("GeoManager", "The Geometry Manager");
	fRuns = new TObjArray();
	fSourceVolumeIndex = 0;
	fSourceMatrixIndex = 0;
} 

//_____________________________________________________________________________
TUCNExperiment::TUCNExperiment(const TUCNExperiment& other)
					:TNamed(other),
					 fGeoManager(other.fGeoManager),
					 fRuns(other.fRuns),
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
		fGeoManager = other.fGeoManager;
		fRuns = other.fRuns;
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
	if (fGeoManager) delete fGeoManager;
	if (fRuns) {fRuns->Delete(); delete fRuns;}
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
Bool_t TUCNExperiment::Initialise(TUCNConfigFile& configFile)
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
	if(!(this->BuildGeometry(*(this->GeoManager()), configFile))) { 
		Warning("Initialise","Failed building geometry. Program aborting.");
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
	TUCNFieldManager* fieldManager = new TUCNFieldManager();
	if(!(fieldManager->Initialise(configFile))) {
		Warning("Initialise","Failed in field initialisation. Program aborting.");
		return kFALSE;
	}
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Runs Initialisation
	///////////////////////////////////////////////////////////////////////////////////////
	cout << "-------------------------------------------" << endl;
	cout << "Initialising the Runs" << endl;
	cout << "-------------------------------------------" << endl;
	Int_t numberOfRuns = configFile.GetInt("NumberOfRuns","Runs");
	if (!(numberOfRuns < 1)) {
		this->CreateRuns(numberOfRuns, configFile);
	} else {
		return kFALSE;
	}
	cout << "-------------------------------------------" << endl;
	cout << "Runs successfully initialised" << endl;
	cout << "-------------------------------------------" << endl;
	return kTRUE;
}

//______________________________________________________________________________
Bool_t TUCNExperiment::BuildGeometry(TGeoManager& geoManager, TUCNConfigFile& configFile)
{
	// -------------------------------------
	// BUILDING GEOMETRY
	
	// Create the UCNNavigator and initialise in the UCNManager
	// This must be done so that the manager defaults to our navigator. Otherwise it will create a new defualt navigator
	// that is not of type UCNNavigator.
	TUCNGeoNavigator* navigator = new TUCNGeoNavigator(&geoManager);
	Int_t navigatorIndex = geoManager.AddNavigator(navigator);
	geoManager.SetCurrentNavigator(navigatorIndex);
	
	// Read in value of FermiPotential
	Double_t V = 0.0;
	Double_t fermiPotentialneV  = configFile.GetFloat("FermiPotential(neV)", "Geometry")*Units::neV;
	Double_t fermiPotentialm    = configFile.GetFloat("FermiPotential(m)", "Geometry")*Constants::height_equivalent_conversion;
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
	Double_t f = configFile.GetFloat("f", "Geometry");
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
	geoManager.SetTopVolume(chamber);
	
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
	geoManager.CloseGeometry();
	return kTRUE;
}

//______________________________________________________________________________
TUCNRun* TUCNExperiment::GetRun(Int_t index) const
{
	return (index<fRuns->GetEntries()) ? static_cast<TUCNRun*>(fRuns->At(index)) : 0;
}

//______________________________________________________________________________
void TUCNExperiment::CreateRuns(Int_t numberOfRuns, TUCNConfigFile& configFile)
{
	for(Int_t i = 1; i <= numberOfRuns; i++) {
		Char_t name[10], title[20];
		sprintf(name,"Run%d",i); 
		sprintf(title,"Run no:%d",i);
		Info("CreateRuns", "Run Name: %s,  Title: %s", name, title);
		TUCNRun* newRun = new TUCNRun(name, title);
		newRun->Initialise(configFile);
		fRuns->Add(newRun);
	}
}

//_____________________________________________________________________________
void	TUCNExperiment::WriteToFile(TFile* file)
{
	assert(file->IsOpen());
	cout << "Writing data to file: " << file->GetName() << endl;
	this->GeoManager()->Write();
	this->Write();
	//fRuns->Write();
	file->Close();
	cout << "WriteOutData completed" << endl;		
}
