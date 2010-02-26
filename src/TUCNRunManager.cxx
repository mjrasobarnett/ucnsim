// TUCNRunManager
// Author: Matthew Raso-Barnett  01/08/2009

#include <iostream>
#include <cassert>
#include <vector>
#include <stdio.h>

#include "TUCNRunManager.h"

#include "TUCNGeoManager.h"
#include "TUCNGeoNavigator.h"
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

#include "Units.h"
#include "Constants.h"

using std::cout;
using std::endl;
using std::cerr;

//#define VERBOSE_MODE

ClassImp(TUCNRunManager)

//_____________________________________________________________________________
TUCNRunManager::TUCNRunManager()
					:TNamed("RunManager", "Default Run Manager")
{
// -- Default constructor
   Info("TUCNRunManager", "Constructor");
	fRuns = new TObjArray();
} 

//_____________________________________________________________________________
TUCNRunManager::TUCNRunManager(const TUCNRunManager& runm)
					:TNamed(runm),
					 fRuns(runm.fRuns)
{
// -- Copy Constructor
	Info("TUCNRunManager", "Copy Constructor");
}

//_____________________________________________________________________________
TUCNRunManager& TUCNRunManager::operator=(const TUCNRunManager& runm)
{
// --assignment operator
	if(this!=&runm) {
      TNamed::operator=(runm);
		fRuns = runm.fRuns;
	}
   return *this;
}

//______________________________________________________________________________
TUCNRunManager::~TUCNRunManager()
{ 
// -- Destructor
	Info("TUCNRunManager", "Destructor");
	if (fRuns) fRuns->Delete();
}

// -- METHODS --

//______________________________________________________________________________
Bool_t TUCNRunManager::Initialise(TUCNConfigFile& configFile)
{
// -- Initialise the runManager with from the Configuration File and create the runs
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
TUCNRun* TUCNRunManager::GetRun(Int_t index) const
{
	return (index<fRuns->GetEntries()) ? static_cast<TUCNRun*>(fRuns->At(index)) : 0;
}

//______________________________________________________________________________
void TUCNRunManager::CreateRuns(Int_t numberOfRuns, TUCNConfigFile& configFile)
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
void	TUCNRunManager::WriteToFile(TFile* file)
{
	assert(file->IsOpen());
	cout << "Writing data to file: " << file->GetName() << endl;
	gGeoManager->Write();
	this->Write();
	//fRuns->Write();
	file->Close();
	cout << "WriteOutData completed" << endl;		
}
