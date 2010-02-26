// TUCNGeoManager
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <fstream>
#include <cassert>

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"

#include "TUCNGeoBuilder.h"
#include "TUCNGeoNavigator.h"

#include "Units.h"

#include "TUCNGeoManager.h"

using std::cout;
using std::cerr;
using std::endl;

//#define VERBOSE_MODE


ClassImp(TUCNGeoManager)

//_____________________________________________________________________________
TUCNGeoManager::TUCNGeoManager()
 					:TNamed("UCNGeoManager","Default UCNGeoManager")
{
// Default Constructor
	Info("TUCNGeoManager", "Dummy Constructor");
	fSourceVolumeIndex = -1;
	fSourceMatrixIndex = -1;
}

//_____________________________________________________________________________
TUCNGeoManager::TUCNGeoManager(const char *name, const char *title)
 					:TNamed(name,title)
{
// Constructor
	Info("TUCNGeoManager","Default Constructor");
	fSourceVolumeIndex = -1;
	fSourceMatrixIndex = -1;
}

//_____________________________________________________________________________
TUCNGeoManager& TUCNGeoManager::operator=(const TUCNGeoManager& other)
{
// --assignment operator
	if(this!=&other) {
		TNamed::operator=(other);
		fSourceVolumeIndex = other.fSourceVolumeIndex;
		fSourceMatrixIndex = other.fSourceMatrixIndex;
	}
   return *this;
}

//_____________________________________________________________________________
TUCNGeoManager::TUCNGeoManager(const TUCNGeoManager& other) 
					:TNamed(other),
					 fSourceVolumeIndex(other.fSourceVolumeIndex),
					 fSourceMatrixIndex(other.fSourceMatrixIndex)
{
// Copy Constructor
}

//_____________________________________________________________________________
TUCNGeoManager::~TUCNGeoManager()
{
// Destructor
	Info("TUCNGeoManager", "Destructor");
//	for (Int_t i = 0; i < this->GetNtracks(); i++) {
//		this->GetTrack(i)->GetParticle()->Delete();
//	}	
//	delete TUCNParabola::Instance();
}

//_____________________________________________________________________________
TGeoVolume* TUCNGeoManager::MakeUCNBox(const char *name, TGeoMedium *medium, Double_t dx, Double_t dy, Double_t dz)
{
// Make in one step a volume pointing to a box shape with given medium.
	return TUCNGeoBuilder::UCNInstance(gGeoManager)->MakeUCNBox(name, medium, dx, dy, dz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeUCNTube(const char *name, TGeoMedium *medium,
	Double_t rmin, Double_t rmax, Double_t dz)
{
// Make in one step a volume pointing to a tube shape with given medium.
	return TUCNGeoBuilder::UCNInstance(gGeoManager)->MakeUCNTube(name, medium, rmin, rmax, dz);
}

//_____________________________________________________________________________
void	TUCNGeoManager::SetSourceVolume(TGeoVolume* sourceVolume)
{
	// Find object in the list and store the array index
	Int_t index = gGeoManager->GetListOfVolumes()->IndexOf(sourceVolume);
	if (index < 0) {
		cerr << "Source volume not found in array. Volume must be registered with GeoManager first." << endl;
	} else {
		fSourceVolumeIndex = index;
	}
}

//_____________________________________________________________________________
void	TUCNGeoManager::SetSourceMatrix(TGeoMatrix* sourceMatrix)
{
	// Find object in the list and store the array index
	TObjArray* matricesList = gGeoManager->GetListOfMatrices();
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
TGeoVolume*	TUCNGeoManager::GetSourceVolume() const
{
	// Find object in the list and return
	if (fSourceVolumeIndex > 0) {
		TGeoVolume* sourceVolume = static_cast<TGeoVolume*>(gGeoManager->GetListOfVolumes()->At(fSourceVolumeIndex));
		assert(sourceVolume != 0);
		return sourceVolume;
	} else {
		cerr << "No Source volume has been set" << endl;
		return 0;
	}
}

//_____________________________________________________________________________
TGeoMatrix*	TUCNGeoManager::GetSourceMatrix() const
{
	// Find object in the list and return
	if (fSourceMatrixIndex > 0) {
		TGeoMatrix* sourceMatrix = static_cast<TGeoMatrix*>(gGeoManager->GetListOfMatrices()->At(fSourceMatrixIndex));
		assert(sourceMatrix != 0);
		return sourceMatrix;
	} else {
		cerr << "No Source matrix has been set" << endl;
		return 0;
	}
}
