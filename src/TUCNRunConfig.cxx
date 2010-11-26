// Run configuration class
// Author: Matthew Raso-Barnett  18/11/2010
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Constants.h"
#include "Units.h"

#include "TUCNRunConfig.h"
#include "TUCNConfigFile.h"

using namespace std;

ClassImp(TUCNRunConfig)

//__________________________________________________________________________
TUCNRunConfig::TUCNRunConfig()
              :TObject()
{
   Info("TUCNRunConfig","Default Constructor");
}

//__________________________________________________________________________
TUCNRunConfig::TUCNRunConfig(const string& runConfigFileName)
              :TObject()
{
   Info("TUCNRunConfig","Constructor");
   TUCNConfigFile runConfigFile(runConfigFileName);
   fRunName = runConfigFile.GetString("RunName","Name");
   fGeomFile = runConfigFile.GetString("GeomFile","Files");
   fGeomVisFile = runConfigFile.GetString("GeomVisFile","Files");
   fInputDataFile = runConfigFile.GetString("InputDataFile","Files");
   fOutputDataFile = runConfigFile.GetString("OutputDataFile","Files");
   fFieldsFile = runConfigFile.GetString("FieldsFile","Files");
   
   fInputRunName = runConfigFile.GetString("InputRunName","Particles");
   fParticlesToLoad = runConfigFile.GetString("WhichParticles","Particles");
   fLoadAllParticles = runConfigFile.GetBool("AllParticles","Particles");
   fRestartParticles = runConfigFile.GetBool("RunFromBeginning","Particles");
   
   fGravFieldOn = runConfigFile.GetBool("GravField","Properties");
   fMagFieldOn = runConfigFile.GetBool("MagFields","Properties");
   fWallLossesOn = runConfigFile.GetBool("WallLosses","Properties");
   
   fRunTime = runConfigFile.GetFloat("RunTime(s)","Properties");
   fMaxStepTime = runConfigFile.GetFloat("MaxStepTime(s)","Properties");
   
   fObsPolarisation = runConfigFile.GetBool("Polarisation","Observables");
   Double_t obsMeasAxisX = runConfigFile.GetFloat("MeasureAxisX","Observables");
   Double_t obsMeasAxisY = runConfigFile.GetFloat("MeasureAxisY","Observables");
   Double_t obsMeasAxisZ = runConfigFile.GetFloat("MeasureAxisZ","Observables");
   fObsMeasAxis.SetXYZ(obsMeasAxisX, obsMeasAxisY, obsMeasAxisZ);
   
   this->Print();
}

//__________________________________________________________________________
TUCNRunConfig::TUCNRunConfig(const TUCNRunConfig& other)
              :TObject(other),
               fRunName(other.fRunName),
               fGeomFile(other.fGeomFile),
               fGeomVisFile(other.fGeomVisFile),
               fInputDataFile(other.fInputDataFile),
               fOutputDataFile(other.fOutputDataFile),
               fFieldsFile(other.fFieldsFile),
               fInputRunName(other.fInputRunName),
               fParticlesToLoad(other.fParticlesToLoad),
               fLoadAllParticles(other.fLoadAllParticles),
               fRestartParticles(other.fRestartParticles),
               fGravFieldOn(other.fGravFieldOn),
               fMagFieldOn(other.fMagFieldOn),
               fWallLossesOn(other.fWallLossesOn),
               fRunTime(other.fRunTime),
               fMaxStepTime(other.fMaxStepTime),
               fObsPolarisation(other.fObsPolarisation),
               fObsMeasAxis(other.fObsMeasAxis)
{
   Info("TUCNRunConfig","Copy Constructor");
}

//__________________________________________________________________________
TUCNRunConfig& TUCNRunConfig::operator=(const TUCNRunConfig& other)
{
   if(this!=&other) {
      TObject::operator=(other);
      fRunName = other.fRunName;
      fGeomFile=other.fGeomFile;
      fGeomVisFile=other.fGeomVisFile;
      fInputDataFile=other.fInputDataFile;
      fOutputDataFile=other.fOutputDataFile;
      fFieldsFile=other.fFieldsFile;
      fInputRunName=other.fInputRunName;
      fParticlesToLoad=other.fParticlesToLoad;
      fLoadAllParticles=other.fLoadAllParticles;
      fRestartParticles=other.fRestartParticles;
      fGravFieldOn=other.fGravFieldOn;
      fMagFieldOn=other.fMagFieldOn;
      fWallLossesOn=other.fWallLossesOn;
      fRunTime=other.fRunTime;
      fMaxStepTime=other.fMaxStepTime;
      fObsPolarisation=other.fObsPolarisation;
      fObsMeasAxis=other.fObsMeasAxis;
   }
   return *this;
}

//__________________________________________________________________________
TUCNRunConfig::~TUCNRunConfig()
{
   Info("TUCNRunConfig","Destructor");
}

//__________________________________________________________________________
void TUCNRunConfig::Print(Option_t* /*option*/) const
{
   cout << "-------------------------------------------" << endl;
   cout << "Run Configuration Settings" << endl;
   cout << "Name: " << fRunName << endl;
   cout << "GeomFile: " << fGeomFile << endl;
   cout << "GeomVisFile: " << fGeomVisFile << endl;
   cout << "InputDataFile: " << fInputDataFile << endl;
   cout << "OutputDataFile: " << fOutputDataFile << endl;
   cout << "FieldsFile: " << fFieldsFile << endl;
   cout << "InputRunName: " << fInputRunName << endl;
   cout << "InitialParticleBranch: " << fParticlesToLoad << endl;
   cout << "LoadAllParticles: " << fLoadAllParticles << endl;
   cout << "RestartParticles: " << fRestartParticles << endl;
   cout << "GravFieldOn: " << fGravFieldOn << endl;
   cout << "MagFieldOn: " << fMagFieldOn << endl;
   cout << "WallLossesOn: " << fWallLossesOn << endl;
   cout << "RunTime: " << fRunTime << " s"<< endl;
   cout << "MaxStepTime: " << fMaxStepTime << " s"<< endl;
   cout << "Observe Polarisation: " << fObsPolarisation << endl;
   cout << "Polrisation Axis: " << fObsMeasAxis.X() << "\t" << fObsMeasAxis.Y();
   cout << "\t" << fObsMeasAxis.Z() << endl;
   cout << "-------------------------------------------" << endl;
}