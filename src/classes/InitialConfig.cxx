// Initial configuration class
// Author: Matthew Raso-Barnett  18/11/2010
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Constants.h"
#include "Units.h"

#include "InitialConfig.h"
#include "ConfigFile.h"

using namespace std;

ClassImp(InitialConfig)

//__________________________________________________________________________
InitialConfig::InitialConfig()
              :fRunName(""), fGeomFile(""), fGeomVisFile(""), fOutputDataFile(""),
               fBeamShape(""), fBeamRadius(0.), fBeamLength(0.), fBeamPhi(0.), fBeamTheta(0.),
               fBeamPsi(0.), fBeamDisplacement(), fInitialParticles(0), fInitialMaxVelocity(0.),
               fFillingTime(0.), fDirMinTheta(0.), fDirMaxTheta(0.), fDirMinPhi(0.), fDirMaxPhi(0.),
               fPercentagePolarised(0.), fSpinAxis(), fSpinUp(kTRUE)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "InitialConfig::Default Constructor" << endl;
   #endif
}

//__________________________________________________________________________
InitialConfig::InitialConfig(const ConfigFile& masterConfig)
{
   // -- Take Batch config file, open the Initial config file and read in all parameters
   #ifdef PRINT_CONSTRUCTORS
      cout << "InitialConfig::Constructor" << endl;
   #endif
   // Extract full path to config files 
   const string compressedfolderpath = masterConfig.GetString("Path","Folder");
   const string folderpath = masterConfig.ExpandFilePath(compressedfolderpath);
   // Use full folder path to fetch the initial config file
   const string initialConfigName = folderpath + masterConfig.GetString("Config","Initialisation");
   if (initialConfigName.empty() == kTRUE) {
      cout << "Unable to read in Initialisation Configuration file name" << endl;
      throw runtime_error("Error fetching name of Initial Config file");
   }
   
   ConfigFile initialConfigFile(initialConfigName);
   fRunName = initialConfigFile.GetString("RunName","Name");
   
   fGeomFile = folderpath + initialConfigFile.GetString("GeomFile","Files");
   string geomVisFileName = initialConfigFile.GetString("GeomVisFile","Files");
   if (geomVisFileName.empty() == false) {fGeomVisFile = folderpath + geomVisFileName;}
   
   fOutputDataFile = folderpath + initialConfigFile.GetString("OutputDataFile","Files");
   
   fBeamShape = initialConfigFile.GetString("Shape","Beam");
   fBeamRadius = initialConfigFile.GetFloat("Radius","Beam");
   fBeamLength = initialConfigFile.GetFloat("Length","Beam");
   fBeamPhi = initialConfigFile.GetFloat("Phi","Beam");
   fBeamTheta = initialConfigFile.GetFloat("Theta","Beam");
   fBeamPsi = initialConfigFile.GetFloat("Psi","Beam");
   Double_t xPosition = initialConfigFile.GetFloat("XPos","Beam");
   Double_t yPosition = initialConfigFile.GetFloat("YPos","Beam");
   Double_t zPosition = initialConfigFile.GetFloat("ZPos","Beam");
   fBeamDisplacement.SetXYZ(xPosition,yPosition,zPosition);
   
   fInitialParticles = initialConfigFile.GetInt("InitialParticles", "Neutrons");
   fInitialMaxVelocity = initialConfigFile.GetFloat("InitialMaxVelocity", "Neutrons");
   fFillingTime = initialConfigFile.GetFloat("FillingTime", "Neutrons");
   
   fDirMinTheta = initialConfigFile.GetFloat("MinTheta","Direction");
   fDirMaxTheta = initialConfigFile.GetFloat("MaxTheta","Direction");
   fDirMinPhi = initialConfigFile.GetFloat("MinPhi","Direction");
   fDirMaxPhi = initialConfigFile.GetFloat("MaxPhi","Direction");
   
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
InitialConfig::InitialConfig(const InitialConfig& other)
              :fRunName(other.fRunName),
               fGeomFile(other.fGeomFile),
               fGeomVisFile(other.fGeomVisFile),
               fOutputDataFile(other.fOutputDataFile),
               fBeamShape(other.fBeamShape),
               fBeamRadius(other.fBeamRadius),
               fBeamLength(other.fBeamLength),
               fBeamPhi(other.fBeamPhi),
               fBeamTheta(other.fBeamTheta),
               fBeamPsi(other.fBeamPsi),
               fBeamDisplacement(other.fBeamDisplacement),
               fInitialParticles(other.fInitialParticles),
               fInitialMaxVelocity(other.fInitialMaxVelocity),
               fFillingTime(other.fFillingTime),
               fDirMinTheta(other.fDirMinTheta),
               fDirMaxTheta(other.fDirMaxTheta),
               fDirMinPhi(other.fDirMinPhi),
               fDirMaxPhi(other.fDirMaxPhi),
               fPercentagePolarised(other.fPercentagePolarised),
               fSpinAxis(other.fSpinAxis),
               fSpinUp(other.fSpinUp)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "InitialConfig::Copy Constructor" << endl;
   #endif
}

//__________________________________________________________________________
InitialConfig& InitialConfig::operator=(const InitialConfig& other)
{
   if(this!=&other) {
      fRunName = other.fRunName;
      fGeomFile = other.fGeomFile;
      fGeomVisFile = other.fGeomVisFile;
      fOutputDataFile = other.fOutputDataFile;
      fBeamShape = other.fBeamShape;
      fBeamRadius = other.fBeamRadius;
      fBeamLength = other.fBeamLength;
      fBeamPhi = other.fBeamPhi;
      fBeamTheta = other.fBeamTheta;
      fBeamPsi = other.fBeamPsi;
      fBeamDisplacement = other.fBeamDisplacement;
      fInitialParticles = other.fInitialParticles;
      fInitialMaxVelocity = other.fInitialMaxVelocity;
      fFillingTime = other.fFillingTime;
      fDirMinTheta = other.fDirMinTheta;
      fDirMaxTheta = other.fDirMaxTheta;
      fDirMinPhi = other.fDirMinPhi;
      fDirMaxPhi =other.fDirMaxPhi;
      fPercentagePolarised = other.fPercentagePolarised;
      fSpinAxis = other.fSpinAxis;
      fSpinUp = other.fSpinUp;
   }
   return *this;
}

//__________________________________________________________________________
InitialConfig::~InitialConfig()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "InitialConfig::Destructor" << endl;
   #endif
}

//__________________________________________________________________________
void InitialConfig::Print(Option_t* /*option*/) const
{
   cout << "-------------------------------------------" << endl;
   cout << "Initial Configuration Settings" << endl;
   cout << "Name: " << fRunName << endl;
   cout << "GeomFile: " << fGeomFile << endl;
   cout << "GeomVisFile: " << fGeomVisFile << endl;
   cout << "OutputDataFile: " << fOutputDataFile << endl;
   cout << "Beam Shape: " << fBeamShape << endl;
   cout << "Beam Radius: " << fBeamRadius << endl;
   cout << "Beam Length: " << fBeamLength << endl;
   cout << "Beam Phi: " << fBeamPhi << "\t";
   cout << "Theta: " << fBeamTheta << "\t";
   cout << "Psi: " << fBeamPsi << endl;
   cout << "Beam Displacement: " << endl;
   fBeamDisplacement.Print();
   cout << "InitialParticles: " << fInitialParticles << endl;
   cout << "InitialMaxVelocity: " << fInitialMaxVelocity << " m/s" << endl;
   cout << "FillingTime: " << fFillingTime << " s" << endl;
   cout << "Beam Direction Min Theta: " << fDirMinTheta << "\t";
   cout << "Max Theta: " << fDirMaxTheta << endl;
   cout << "Beam Direction Min Theta: " << fDirMinPhi << "\t";
   cout << "Max Theta: " << fDirMaxPhi << endl;
   cout << "PercentagePolarised: " << fPercentagePolarised << "%" << endl;
   cout << "Spin Up: " << fSpinUp << endl;
   fSpinAxis.Print();
   cout << "-------------------------------------------" << endl;
}
