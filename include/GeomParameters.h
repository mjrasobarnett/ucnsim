#ifndef MODEL_PARAMETERS_H
#define MODEL_PARAMETERS_H

#include "../include/Units.h"
#include "../include/Constants.h"

namespace GeomParameters {
   // -- Top
   const Double_t topX = 100.;
   const Double_t topY = 100.;
   const Double_t topZ = 100.;
   
   // -- Chamber
   const Double_t chamberX = 10.;
   const Double_t chamberY = 10.;
   const Double_t chamberZ = 10.;   
   
   // -- SourceTube Segment
   const Int_t sourceSegments = 13;
   const Double_t sourceSegRMin = 0.; 
   const Double_t sourceSegRMax = 31.5*Units::mm;
   const Double_t sourceSegHalfLength = 125.*Units::mm;
   const Double_t sourceSegPhi = 0.0;
   const Double_t sourceSegTheta = 90.0;
   const Double_t sourceSegPsi = 0.0;
   const Double_t sourceSegXPos = 0.;
   const Double_t sourceSegYPos = sourceSegHalfLength;
   const Double_t sourceSegZPos = 0.;
   
   // -- SourceTube Gap
   const Double_t sourceSegGapRMin = 31.5*Units::mm; 
   const Double_t sourceSegGapRMax = 60*Units::mm;
   const Double_t sourceSegGapHalfLength = 1.0*Units::mm;
   const Double_t sourceSegGapAngle = 90.0;
   const Double_t sourceSegGapYPos = sourceSegGapHalfLength;
   
   // -- Valve Volume Entrance
   const Double_t valveVolEntranceRMin = 0.;
   const Double_t valveVolEntranceRMax = 31.5*Units::mm;
   const Double_t valveVolEntranceHalfLength = 46.*Units::mm;
   const Double_t valveVolEntrancePhi = 0.0; 
   const Double_t valveVolEntranceTheta = 90.0; 
   const Double_t valveVolEntrancePsi = 0.0; 
   const Double_t valveVolEntranceXPos = 0.;
   const Double_t valveVolEntranceYPos = sourceSegments*(2.0*sourceSegHalfLength) + valveVolEntranceHalfLength; 
   const Double_t valveVolEntranceZPos = 0.;
   
   // -- Valve Volume Front
   const Double_t valveVolFrontRMin = 0.;
   const Double_t valveVolFrontRMax = 31.5*Units::mm;
   const Double_t valveVolFrontHalfLength = 12.75*Units::mm;
   const Double_t valveVolFrontPhi = 0.0; 
   const Double_t valveVolFrontTheta = 90.0; 
   const Double_t valveVolFrontPsi = 0.0; 
   const Double_t valveVolFrontXPos = 0.;
   const Double_t valveVolFrontYPos = valveVolEntranceYPos + valveVolEntranceHalfLength + valveVolFrontHalfLength;
   const Double_t valveVolFrontZPos = 0.;
   
   // -- Valve Volume
   // Valve Volume Back
   const Double_t valveVolBackRMin = 0.;
   const Double_t valveVolBackRMax = 36.0*Units::mm;
   const Double_t valveVolBackHalfLength = 17.75*Units::mm;
   const Double_t valveVolBackPhi = 0.0;
   const Double_t valveVolBackTheta = 90.0;
   const Double_t valveVolBackPsi = 0.0;
   const Double_t valveVolBackXPos = 0.0;
   const Double_t valveVolBackYPos = 0.0;
   const Double_t valveVolBackZPos = 0.0;
   // Bend Entrance
   const Double_t bendEntranceHalfX = 30.0*Units::mm;
   const Double_t bendEntranceHalfY = 15.0*Units::mm;
   const Double_t bendEntranceHalfZ = 55.0*Units::mm;
   const Double_t bendEntrancePhi = 0.0;
   const Double_t bendEntranceTheta = 0.0;
   const Double_t bendEntrancePsi = 0.0;
   const Double_t bendEntranceXPos = 0.0;
   const Double_t bendEntranceYPos = 0.0;
   const Double_t bendEntranceZPos = -bendEntranceHalfZ;
   // Composite
   const Double_t valveVolPhi = 0.0;
   const Double_t valveVolTheta = 0.0;
   const Double_t valveVolPsi = 0.0;
   const Double_t valveVolXPos = 0.0;
   const Double_t valveVolYPos = valveVolFrontYPos + valveVolFrontHalfLength + valveVolBackHalfLength;
   const Double_t valveVolZPos = 0.0;
   // -- Bend
   const Double_t bendRMin = 160.0*Units::mm;
   const Double_t bendRMax = 220.0*Units::mm;
   const Double_t bendHalfLength = 15.0*Units::mm;
   const Double_t bendBoxPhi = 0.0;
   const Double_t bendBoxTheta = 90.0;
   const Double_t bendBoxPsi = 90.0;
   const Double_t bendBoxXPos = bendRMax;
   const Double_t bendBoxYPos = -bendRMax;
   const Double_t bendBoxZPos = 0.0;
   const Double_t bendVolPhi = 0.0;
   const Double_t bendVolTheta = 90.0;
   const Double_t bendVolPsi = 0.0;
   const Double_t bendVolXPos = -0.5*(bendRMin + bendRMax);
   const Double_t bendVolYPos = valveVolYPos;
   const Double_t bendVolZPos = -2.0*bendEntranceHalfZ;
   
   // -- Detector Valve Volume
   const Double_t detectorValveVolHalfX = 55.0*Units::mm;
   const Double_t detectorValveVolHalfY = 30.0*Units::mm;
   const Double_t detectorValveVolHalfZ = 30.0*Units::mm;
   const Double_t detectorValveVolPhi = 0.0;
   const Double_t detectorValveVolTheta = 0.0;
   const Double_t detectorValveVolPsi = 0.0;
   const Double_t detectorValveVolXPos = bendVolXPos - detectorValveVolHalfX;
   const Double_t detectorValveVolYPos = valveVolYPos;
   const Double_t detectorValveVolZPos = bendVolZPos + bendVolXPos;
   
   // -- Detector Valve
   const Double_t detectorValveHalfX = 2.0*Units::mm;
   const Double_t detectorValveHalfY = 29.0*Units::mm;
   const Double_t detectorValveHalfZ = 40.0*Units::mm;
   const Double_t detectorValvePhi = 90;
   const Double_t detectorValveTheta = -60.0;
   const Double_t detectorValvePsi = 90;
   const Double_t detectorValveXPos = 10.0*Units::mm;
   const Double_t detectorValveYPos = 0.;
   const Double_t detectorValveZPos = -12.0*Units::mm;
   
   // -- Detector Tube Top
   const Double_t detectorTubeTopRMin = 0.;
   const Double_t detectorTubeTopRMax = 25.0*Units::mm;
   const Double_t detectorTubeTopHalfLength = 15.75*Units::mm;
   const Double_t detectorTubeTopPhi = 0.0;
   const Double_t detectorTubeTopTheta = 0.0;
   const Double_t detectorTubeTopPsi = 0.0;
   const Double_t detectorTubeTopXPos = detectorValveVolXPos;
   const Double_t detectorTubeTopYPos = valveVolYPos;
   const Double_t detectorTubeTopZPos = detectorValveVolZPos - detectorValveVolHalfZ - detectorTubeTopHalfLength;
   
   // -- Detector Tube
   const Double_t detectorTubeRMin = 0.;
   const Double_t detectorTubeRMax = 27.85*Units::mm;
   const Double_t detectorTubeHalfLength = 250.75*Units::mm;
   const Double_t detectorTubePhi = 0.0;
   const Double_t detectorTubeTheta = 0.0;
   const Double_t detectorTubePsi = 0.0;
   const Double_t detectorTubeXPos = detectorValveVolXPos;
   const Double_t detectorTubeYPos = valveVolYPos;
   const Double_t detectorTubeZPos = detectorTubeTopZPos - detectorTubeTopHalfLength - detectorTubeHalfLength;
   
   // -- Detector Tube Bottom
   const Double_t detectorTubeBottomRMin = 15.0*Units::mm;
   const Double_t detectorTubeBottomRMax = 27.85*Units::mm;
   const Double_t detectorTubeBottomHalfLength = 5.*Units::mm;
   const Double_t detectorTubeBottomPhi = 0.0;
   const Double_t detectorTubeBottomTheta = 0.0;
   const Double_t detectorTubeBottomPsi = 0.0;
   const Double_t detectorTubeBottomXPos = detectorValveVolXPos;
   const Double_t detectorTubeBottomYPos = valveVolYPos;
   const Double_t detectorTubeBottomZPos = detectorTubeZPos - detectorTubeHalfLength - detectorTubeBottomHalfLength;
   
   // -- Detector
   const Double_t detectorRMin = 0.;
   const Double_t detectorRMax = 15.0*Units::mm;
   const Double_t detectorHalfLength = 5.0*Units::mm;
   const Double_t detectorPhi = 0.0;
   const Double_t detectorTheta = 0.0;
   const Double_t detectorPsi = 0.0;
   const Double_t detectorXPos = detectorValveVolXPos;
   const Double_t detectorYPos = valveVolYPos;
   const Double_t detectorZPos = detectorTubeZPos - detectorTubeHalfLength - detectorHalfLength;
   
   // -- Guide Sections Segment
   // Note that I am unsure of the exact number and lengths of each individual section. The total
   // length of the whole assembly is definitely 2890.6mm and is either made of 4 or 5 sections.
   // For now we will use 5 equal sections.
   const Double_t guideSegHalfX = 25.0*Units::mm;
   const Double_t guideSegHalfY = 25.0*Units::mm;
   const Double_t guideSegHalfZ = 289.0*Units::mm;
   const Double_t guidePhi = -90;
   const Double_t guideTheta = 90;
   const Double_t guidePsi = 0;
   const Double_t guideXPos = detectorValveVolXPos - detectorValveVolHalfX - guideSegHalfZ;
   const Double_t guideYPos = detectorValveVolYPos;
   const Double_t guideZPos = detectorValveVolZPos;

   // -- Ramsey Cell Pre-Volume
   const Double_t preVolumeBoxHalfX = 72.0*Units::mm;
   const Double_t preVolumeBoxHalfY = 72.0*Units::mm;
   const Double_t preVolumeBoxHalfZ = 17.5*Units::mm;
   const Double_t preVolumeBoxPhi = -45;
   const Double_t preVolumeBoxTheta = 0;
   const Double_t preVolumeBoxPsi = 0;
   const Double_t preVolumeBoxXPos = 0.;
   const Double_t preVolumeBoxYPos = 0.;
   const Double_t preVolumeBoxZPos = 0.;
   
   const Double_t preVolumeBoxCornerHalfX = 13.0*Units::mm;
   const Double_t preVolumeBoxCornerHalfY = 13.0*Units::mm;
   const Double_t preVolumeBoxCornerHalfZ = preVolumeBoxHalfZ+4.0*Units::mm;
   const Double_t preVolumeBoxCornerPhi = preVolumeBoxPhi;
   const Double_t preVolumeBoxCornerTheta = 0;
   const Double_t preVolumeBoxCornerPsi = 0;
   const Double_t preVolumeBoxCorner1XPos = 84.0*Units::mm;
   const Double_t preVolumeBoxCorner1YPos = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner1ZPos = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner2XPos = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner2YPos = 84.0*Units::mm;
   const Double_t preVolumeBoxCorner2ZPos = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner3XPos = -84.0*Units::mm;
   const Double_t preVolumeBoxCorner3YPos = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner3ZPos = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner4XPos = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner4YPos = -84.0*Units::mm;
   const Double_t preVolumeBoxCorner4ZPos = 0.0*Units::mm;
 
   const Double_t preVolumeCornerRMin = 0.0*Units::mm;
   const Double_t preVolumeCornerRMax = 26.0*Units::mm;
   const Double_t preVolumeCornerHalfZ = preVolumeBoxHalfZ;
   const Double_t preVolumeCornerPhi = 0;
   const Double_t preVolumeCornerTheta = 0;
   const Double_t preVolumeCornerPsi = 0;
   const Double_t preVolumeCorner1XPos = 65.0*Units::mm;
   const Double_t preVolumeCorner1YPos = 0.0*Units::mm;
   const Double_t preVolumeCorner1ZPos = 0.0*Units::mm;
   const Double_t preVolumeCorner2XPos = 0.0*Units::mm;
   const Double_t preVolumeCorner2YPos = 65.0*Units::mm;
   const Double_t preVolumeCorner2ZPos = 0.0*Units::mm;
   const Double_t preVolumeCorner3XPos = -65.0*Units::mm;
   const Double_t preVolumeCorner3YPos = 0.0*Units::mm;
   const Double_t preVolumeCorner3ZPos = 0.0*Units::mm;
   const Double_t preVolumeCorner4XPos = 0.0*Units::mm;
   const Double_t preVolumeCorner4YPos = -65.0*Units::mm;
   const Double_t preVolumeCorner4ZPos = 0.0*Units::mm;
   
   const Double_t preVolumePhi = 90;
   const Double_t preVolumeTheta = 90;
   const Double_t preVolumePsi = 45;
   const Double_t preVolumeXPos = guideXPos - 9*guideSegHalfZ - preVolumeBoxHalfZ;
   const Double_t preVolumeYPos = guideYPos;
   const Double_t preVolumeZPos = guideZPos;
   
   // -- Ramsey Cell
   // Neutral Electrode
   const Double_t neutralElectrodeRMin = 0.0*Units::mm;
   const Double_t neutralElectrodeRMax = 140.0*Units::mm;
   const Double_t neutralElectrodeHalfZ = 7.5*Units::mm;
   const Double_t neutralElectrodePhi = 90;
   const Double_t neutralElectrodeTheta = 90;
   const Double_t neutralElectrodePsi = 0;
   const Double_t neutralElectrodeXPos = preVolumeXPos - preVolumeBoxHalfZ - neutralElectrodeHalfZ;
   const Double_t neutralElectrodeYPos = preVolumeYPos;
   const Double_t neutralElectrodeZPos = preVolumeZPos;
   
   // Neutral Electrode Valve Holes
   const Double_t neutralElectrodeHoleRMin = 0.0*Units::mm;
   const Double_t neutralElectrodeHoleRMax = 15.0*Units::mm;
   const Double_t neutralElectrodeHoleHalfZ = neutralElectrodeHalfZ;
/*   const Double_t neutralElectrodeHolePhi = 0.0;
   const Double_t neutralElectrodeHoleTheta = 0.0;
   const Double_t neutralElectrodeHolePsi = 0.0;
   const Double_t neutralElectrodeHoleZPos = 0;
   const Double_t neutralElectrodeHole1XPos = 0;
   const Double_t neutralElectrodeHole1YPos = 65.0*Units::mm;
   const Double_t neutralElectrodeHole1ZPos = neutralElectrodeHoleZPos;
   const Double_t neutralElectrodeHole2XPos = 65.0*Units::mm;
   const Double_t neutralElectrodeHole2YPos = 0;
   const Double_t neutralElectrodeHole2ZPos = neutralElectrodeHoleZPos;
   const Double_t neutralElectrodeHole3XPos = 0;
   const Double_t neutralElectrodeHole3YPos = -65.0*Units::mm;
   const Double_t neutralElectrodeHole3ZPos = neutralElectrodeHoleZPos;
   const Double_t neutralElectrodeHole4XPos = -65.0*Units::mm;
   const Double_t neutralElectrodeHole4YPos = 0;
   const Double_t neutralElectrodeHole4ZPos = neutralElectrodeHoleZPos;
*/   
   const Double_t neutralElectrodeHolePhi = neutralElectrodePhi;
   const Double_t neutralElectrodeHoleTheta = neutralElectrodeTheta;
   const Double_t neutralElectrodeHolePsi = neutralElectrodePsi;
   const Double_t neutralElectrodeHole1XPos = neutralElectrodeXPos;
   const Double_t neutralElectrodeHole1YPos = neutralElectrodeYPos;
   const Double_t neutralElectrodeHole1ZPos = neutralElectrodeZPos+65.0*Units::mm;
   const Double_t neutralElectrodeHole2XPos = neutralElectrodeXPos;
   const Double_t neutralElectrodeHole2YPos = neutralElectrodeYPos+65.0*Units::mm;
   const Double_t neutralElectrodeHole2ZPos = neutralElectrodeZPos;
   const Double_t neutralElectrodeHole3XPos = neutralElectrodeXPos;
   const Double_t neutralElectrodeHole3YPos = neutralElectrodeYPos;
   const Double_t neutralElectrodeHole3ZPos = neutralElectrodeZPos-65.0*Units::mm;
   const Double_t neutralElectrodeHole4XPos = neutralElectrodeXPos;
   const Double_t neutralElectrodeHole4YPos = neutralElectrodeYPos-65.0*Units::mm;
   const Double_t neutralElectrodeHole4ZPos = neutralElectrodeZPos;
   
   // Neutral Cell
   const Double_t neutralCellRMin = 0.0*Units::mm;
   const Double_t neutralCellRMax = 125.0*Units::mm;
   const Double_t neutralCellHalfZ = 22.5*Units::mm;
   const Double_t neutralCellPhi = 90;
   const Double_t neutralCellTheta = 90;
   const Double_t neutralCellPsi = 0;
   const Double_t neutralCellXPos = neutralElectrodeXPos - neutralElectrodeHalfZ - neutralCellHalfZ;
   const Double_t neutralCellYPos = neutralElectrodeYPos;
   const Double_t neutralCellZPos = neutralElectrodeZPos;
   
   // Cell connector tube
   const Double_t cellConnectorRMin = 15.0*Units::mm;
   const Double_t cellConnectorRMax = 17.5*Units::mm;
   const Double_t cellConnectorHalfZ = 27.5*Units::mm;
   const Double_t cellConnectorPhi = 90;
   const Double_t cellConnectorTheta = 90;
   const Double_t cellConnectorPsi = 0;
   const Double_t cellConnectorXPos = neutralCellXPos;
   const Double_t cellConnectorYPos = neutralCellYPos;
   const Double_t cellConnectorZPos = neutralCellZPos + 65.0*Units::mm;
/*   
   const Double_t cellConnectorPhi = 0;
   const Double_t cellConnectorTheta = 0;
   const Double_t cellConnectorPsi = 0;
   const Double_t cellConnectorXPos = 0;
   const Double_t cellConnectorYPos = 65.0*Units::mm;
   const Double_t cellConnectorZPos = 0;
*/ 

   // Central Electrode
   const Double_t centralElectrodeRMin = 0.0*Units::mm;
   const Double_t centralElectrodeRMax = 140.0*Units::mm;
   const Double_t centralElectrodeHalfZ = 7.0*Units::mm;
   const Double_t centralElectrodePhi = 90;
   const Double_t centralElectrodeTheta = 90;
   const Double_t centralElectrodePsi = 0;
   const Double_t centralElectrodeXPos = neutralCellXPos - neutralCellHalfZ - neutralElectrodeHalfZ + 1.0*Units::mm;
   const Double_t centralElectrodeYPos = neutralCellYPos;
   const Double_t centralElectrodeZPos = neutralCellZPos;
   
   // Central Electrode Hole
   const Double_t centralElectrodeHoleRMin = 0.0*Units::mm;
   const Double_t centralElectrodeHoleRMax = 15.0*Units::mm;
   const Double_t centralElectrodeHoleHalfZ = centralElectrodeHalfZ;
/*   const Double_t centralElectrodeHolePhi = 0.0;
   const Double_t centralElectrodeHoleTheta = 0.0;
   const Double_t centralElectrodeHolePsi = 0.0;
   const Double_t centralElectrodeHoleXPos = 0;
   const Double_t centralElectrodeHoleYPos = 65.0*Units::mm;
   const Double_t centralElectrodeHoleZPos = centralElectrodeHoleZPos;
*/   
   const Double_t centralElectrodeHolePhi = centralElectrodePhi;
   const Double_t centralElectrodeHoleTheta = centralElectrodeTheta;
   const Double_t centralElectrodeHolePsi = centralElectrodePsi;
   const Double_t centralElectrodeHoleXPos = centralElectrodeXPos;
   const Double_t centralElectrodeHoleYPos = centralElectrodeYPos;
   const Double_t centralElectrodeHoleZPos = centralElectrodeZPos+65.0*Units::mm;

   // HV Cell
   const Double_t hvCellRMin = 0.0*Units::mm;
   const Double_t hvCellRMax = 125.0*Units::mm;
   const Double_t hvCellHalfZ = 22.5*Units::mm;
   const Double_t hvCellPhi = 90;
   const Double_t hvCellTheta = 90;
   const Double_t hvCellPsi = 0;
   const Double_t hvCellXPos = centralElectrodeXPos - centralElectrodeHalfZ - hvCellHalfZ;
   const Double_t hvCellYPos = centralElectrodeYPos;
   const Double_t hvCellZPos = centralElectrodeZPos;
   
   // HV Electrode
   const Double_t hvElectrodeRMin = 0.0*Units::mm;
   const Double_t hvElectrodeRMax = 125.0*Units::mm;
   const Double_t hvElectrodeHalfZ = 2.*Units::mm;
   const Double_t hvElectrodePhi = 90;
   const Double_t hvElectrodeTheta = 90;
   const Double_t hvElectrodePsi = 0;
   const Double_t hvElectrodeXPos = hvCellXPos - hvCellHalfZ - hvElectrodeHalfZ;
   const Double_t hvElectrodeYPos = hvCellYPos;
   const Double_t hvElectrodeZPos = hvCellZPos;
   
}

namespace FieldParameters {
   // -- Uniform Solenoid
   // X, Y, Z are in the local coord system of its volume
   const Double_t solenoidBx = 0.0*Units::uT;
   const Double_t solenoidBy = 0.0*Units::uT;
   const Double_t solenoidBz = 5.0*Units::uT;
   
   // -- Electric Field
   // X, Y, Z are in the local coord system of its volume
   const Double_t hvCellEx = 0.0*Units::KV;
   const Double_t hvCellEy = 0.0*Units::KV;
   const Double_t hvCellEz = 40.0*Units::KV;
   
}

#endif /* MODEL_PARAMETERS_H */

