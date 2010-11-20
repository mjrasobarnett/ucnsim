// Initial configuration class
// Author: Matthew Raso-Barnett  18/11/2010
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Constants.h"
#include "Units.h"

#include "TUCNInitialConfig.h"
#include "TUCNConfigFile.h"

using namespace std;

ClassImp(TUCNInitialConfig)

//__________________________________________________________________________
TUCNInitialConfig::TUCNInitialConfig()
{
   Info("TUCNInitialConfig","Default Constructor");
}

//__________________________________________________________________________
TUCNInitialConfig::TUCNInitialConfig(const string& initialConfigFileName)
{
   // -- Take Batch config file, open the Initial config file and read in all parameters
   Info("TUCNInitialConfig","Constructor");
   TUCNConfigFile initialConfigFile(initialConfigFileName);
   fRunName = initialConfigFile.GetString("RunName","Name");
   fGeomFile = initialConfigFile.GetString("GeomFile","Files");
   fGeomVisFile = initialConfigFile.GetString("GeomVisFile","Files");
   fOutputDataFile = initialConfigFile.GetString("OutputDataFile","Files");
   
   fInitialParticles = initialConfigFile.GetInt("InitialParticles", "Neutrons");
   fInitialMaxVelocity = initialConfigFile.GetFloat("InitialMaxVelocity", "Neutrons");
   fFillingTime = initialConfigFile.GetFloat("FillingTime", "Neutrons");
   
   fPercentagePolarised = initialConfigFile.GetFloat("PercentagePolarised","Spin");
   // -- Read in Axis against which we will polarise our neutrons
   Double_t spinAxisX = initialConfigFile.GetFloat("SpinAxisX","Spin");
   Double_t spinAxisY = initialConfigFile.GetFloat("SpinAxisY","Spin");
   Double_t spinAxisZ = initialConfigFile.GetFloat("SpinAxisZ","Spin");
   fSpinAxis.SetXYZ(spinAxisX, spinAxisY, spinAxisZ);
   
   // -- Read in and check Spin State
   const string spinState = initialConfigFile.GetString("SpinState","Spin");
   if (spinState == "UP" || spinState ==  "Up" || spinState ==  "up") {
      fSpinUp = kTRUE;
   } else if (spinState == "DOWN" || spinState == "Down" || spinState == "down") {
      fSpinUp = kFALSE;
   } else {
      cout << "WARNING - Unable to read in spin state from config file. ";
      cout << "Setting 'Up' by default" << endl;
      fSpinUp = kTRUE;
   }
   this->Print();
}

//__________________________________________________________________________
TUCNInitialConfig::TUCNInitialConfig(const TUCNInitialConfig& other)
                  :TObject(other),
                   fRunName(other.fRunName),
                   fGeomFile(other.fGeomFile),
                   fGeomVisFile(other.fGeomVisFile),
                   fOutputDataFile(other.fOutputDataFile),
                   fInitialParticles(other.fInitialParticles),
                   fInitialMaxVelocity(other.fInitialMaxVelocity),
                   fFillingTime(other.fFillingTime),
                   fPercentagePolarised(other.fPercentagePolarised),
                   fSpinAxis(other.fSpinAxis),
                   fSpinUp(other.fSpinUp)
{
   Info("TUCNInitialConfig","Copy Constructor");
}

//__________________________________________________________________________
TUCNInitialConfig& TUCNInitialConfig::operator=(const TUCNInitialConfig& other)
{
   if(this!=&other) {
      TObject::operator=(other);
      fRunName = other.fRunName;
      fGeomFile = other.fGeomFile;
      fGeomVisFile = other.fGeomVisFile;
      fOutputDataFile = other.fOutputDataFile;
      fInitialParticles = other.fInitialParticles;
      fInitialMaxVelocity = other.fInitialMaxVelocity;
      fFillingTime = other.fFillingTime;
      fPercentagePolarised = other.fPercentagePolarised;
      fSpinAxis = other.fSpinAxis;
      fSpinUp = other.fSpinUp;
   }
   return *this;
}

//__________________________________________________________________________
TUCNInitialConfig::~TUCNInitialConfig()
{
   cout << "TUCNInitialConfig::Destructor" << endl;
}

//__________________________________________________________________________
void TUCNInitialConfig::Print(Option_t* /*option*/) const
{
   cout << "-------------------------------------------" << endl;
   cout << "Initial Configuration Settings" << endl;
   cout << "Name: " << fRunName << endl;
   cout << "GeomFile: " << fGeomFile << endl;
   cout << "GeomVisFile: " << fGeomVisFile << endl;
   cout << "OutputDataFile: " << fOutputDataFile << endl;
   cout << "InitialParticle: " << fInitialParticles << endl;
   cout << "InitialMaxVelocity: " << fInitialMaxVelocity << " m/s" << endl;
   cout << "FillingTime: " << fFillingTime << " s" << endl;
   cout << "PercentagePolarised: " << fPercentagePolarised << "%" << endl;
   cout << "Spin Up: " << fSpinUp << endl;
   fSpinAxis.Print();
   cout << "-------------------------------------------" << endl;
}