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
   const Double_t sourceSegRMin = 0.; 
   const Double_t sourceSegRMax = 31.5*Units::mm;
   const Double_t sourceSegHalfLength = 125.*Units::mm;
   const Double_t sourceSegAngle = 90.0;
   const Double_t sourceSegYDisplacement = 125.*Units::mm;
   
   // -- Neutron Beam Area
   const Double_t neutronBeamAreaRMin = 0.;
   const Double_t neutronBeamAreaRMax = 15.*Units::mm;
   const Double_t neutronBeamAreaHalfLength = (13.*sourceSegHalfLength)/2.0;
   const Double_t neutronBeamAreaAngle = 90.0;
   const Double_t neutronBeamAreaYDisplacement = neutronBeamAreaHalfLength;
   
   // -- Valve Volume Entrance
   const Double_t valveVolEntranceRMin = 0.;
   const Double_t valveVolEntranceRMax = 31.5*Units::mm;
   const Double_t valveVolEntranceHalfLength = 46.*Units::mm;
   const Double_t valveVolEntranceAngle = 90.0; 
   const Double_t valveVolEntranceYDisplacement = 13.0*(2.0*sourceSegHalfLength) + valveVolEntranceHalfLength;
   
   // -- Valve Volume Front
   const Double_t valveVolFrontRMin = 0.;
   const Double_t valveVolFrontRMax = 31.5*Units::mm;
   const Double_t valveVolFrontHalfLength = 12.75*Units::mm;
   const Double_t valveVolFrontAngle = 90.0; 
   const Double_t valveVolFrontYDisplacement = valveVolEntranceYDisplacement + valveVolEntranceHalfLength + valveVolFrontHalfLength;
   
   // -- Valve Volume
   // Valve Volume Back
   const Double_t valveVolBackRMin = 0.;
   const Double_t valveVolBackRMax = 36.0*Units::mm;
   const Double_t valveVolBackHalfLength = 17.75*Units::mm;
   const Double_t valveVolBackAngle = 90.0;
   // Bend Entrance
   const Double_t bendEntranceHalfX = 30.0*Units::mm;
   const Double_t bendEntranceHalfY = 15.0*Units::mm;
   const Double_t bendEntranceHalfZ = 55.0*Units::mm;
   const Double_t bendEntranceAngle = 0.0;
   const Double_t bendEntranceZDisplacement = bendEntranceHalfZ;
   // Composite
   const Double_t valveVolAngle = 0.0;
   const Double_t valveVolYDisplacement = valveVolFrontYDisplacement + valveVolFrontHalfLength + valveVolBackHalfLength;
   
   // -- Bend
   const Double_t bendRMin = 160.0*Units::mm;
   const Double_t bendRMax = 220.0*Units::mm;
   const Double_t bendHalfLength = 15.0*Units::mm;
   const Double_t bendVolAngle = 90.0;
   const Double_t bendVolYDisplacement = valveVolYDisplacement;
   const Double_t bendVolXDisplacement = (bendRMin + bendRMax)/2.0;
   const Double_t bendVolZDisplacement = 2.0*bendEntranceHalfZ;							
   
   // -- Detector Valve Volume
   const Double_t detectorValveVolHalfX = 55.0*Units::mm;
   const Double_t detectorValveVolHalfY = 30.0*Units::mm;
   const Double_t detectorValveVolHalfZ = 30.0*Units::mm;
   const Double_t detectorValveVolAngle = 0.0;
   const Double_t detectorValveVolXDisplacement = bendVolXDisplacement + detectorValveVolHalfX;
   const Double_t detectorValveVolYDisplacement = valveVolYDisplacement;
   const Double_t detectorValveVolZDisplacement = bendVolZDisplacement + bendVolXDisplacement;
   
   // -- Detector Tube Top
   const Double_t detectorTubeTopRMin = 0.;
   const Double_t detectorTubeTopRMax = 25.0*Units::mm;
   const Double_t detectorTubeTopHalfLength = 15.75*Units::mm;
   const Double_t detectorTubeTopAngle = 0.0;
   const Double_t detectorTubeTopXDisplacement = detectorValveVolXDisplacement;
   const Double_t detectorTubeTopYDisplacement = valveVolYDisplacement;
   const Double_t detectorTubeTopZDisplacement = detectorValveVolZDisplacement + detectorValveVolHalfZ + detectorTubeTopHalfLength;
   
   // -- Detector Tube
   const Double_t detectorTubeRMin = 0.;
   const Double_t detectorTubeRMax = 27.85*Units::mm;
   const Double_t detectorTubeHalfLength = 250.75*Units::mm;
   const Double_t detectorTubeAngle = 0.0;
   const Double_t detectorTubeXDisplacement = detectorValveVolXDisplacement;
   const Double_t detectorTubeYDisplacement = valveVolYDisplacement;
   const Double_t detectorTubeZDisplacement = detectorTubeTopZDisplacement + detectorTubeTopHalfLength + detectorTubeHalfLength;
   
   // -- Detector
   const Double_t detectorRMin = 0.;
   const Double_t detectorRMax = 27.85*Units::mm;
   const Double_t detectorHalfLength = 5.*Units::mm;
   const Double_t detectorAngle = 0.0;
   const Double_t detectorXDisplacement = detectorValveVolXDisplacement;
   const Double_t detectorYDisplacement = valveVolYDisplacement;
   const Double_t detectorZDisplacement = detectorTubeZDisplacement + detectorTubeHalfLength + detectorHalfLength;
   
   // -- Guide Sections Segment
   // Note that I am unsure of the exact number and lengths of each individual section. The total
   // length of the whole assembly is definitely 2890.6mm and is either made of 4 or 5 sections.
   // For now we will use 5 equal sections.
   const Double_t guideSegHalfX = 25.0*Units::mm;
   const Double_t guideSegHalfY = 25.0*Units::mm;
   const Double_t guideSegHalfZ = 289.0*Units::mm;
   const Double_t guidePhi = -90;
   const Double_t guideTheta = 90;
   const Double_t guideXDisplacement = -detectorValveVolXDisplacement - detectorValveVolHalfX - guideSegHalfZ;
   const Double_t guideYDisplacement = detectorValveVolYDisplacement;
   const Double_t guideZDisplacement = -detectorValveVolZDisplacement;

   // -- Ramsey Cell Pre-Volume
   const Double_t preVolumeBoxHalfX = 72.0*Units::mm;
   const Double_t preVolumeBoxHalfY = 72.0*Units::mm;
   const Double_t preVolumeBoxHalfZ = 17.5*Units::mm;
   const Double_t preVolumeBoxPhi = -45;
   const Double_t preVolumeBoxTheta = 0;
   const Double_t preVolumeBoxPsi = 0;
   const Double_t preVolumeBoxXDisplacement = 0.;
   const Double_t preVolumeBoxYDisplacement = 0.;
   const Double_t preVolumeBoxZDisplacement = 0.;
   
   const Double_t preVolumeBoxCornerHalfX = 13.0*Units::mm;
   const Double_t preVolumeBoxCornerHalfY = 13.0*Units::mm;
   const Double_t preVolumeBoxCornerHalfZ = preVolumeBoxHalfZ+4.0*Units::mm;
   const Double_t preVolumeBoxCornerPhi = preVolumeBoxPhi;
   const Double_t preVolumeBoxCornerTheta = 0;
   const Double_t preVolumeBoxCornerPsi = 0;
   const Double_t preVolumeBoxCorner1XDisplacement = 84.0*Units::mm;
   const Double_t preVolumeBoxCorner1YDisplacement = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner1ZDisplacement = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner2XDisplacement = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner2YDisplacement = 84.0*Units::mm;
   const Double_t preVolumeBoxCorner2ZDisplacement = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner3XDisplacement = -84.0*Units::mm;
   const Double_t preVolumeBoxCorner3YDisplacement = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner3ZDisplacement = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner4XDisplacement = 0.0*Units::mm;
   const Double_t preVolumeBoxCorner4YDisplacement = -84.0*Units::mm;
   const Double_t preVolumeBoxCorner4ZDisplacement = 0.0*Units::mm;
 
   const Double_t preVolumeCornerRMin = 0.0*Units::mm;
   const Double_t preVolumeCornerRMax = 26.0*Units::mm;
   const Double_t preVolumeCornerHalfZ = preVolumeBoxHalfZ;
   const Double_t preVolumeCornerPhi = 0;
   const Double_t preVolumeCornerTheta = 0;
   const Double_t preVolumeCornerPsi = 0;
   const Double_t preVolumeCorner1XDisplacement = 65.0*Units::mm;
   const Double_t preVolumeCorner1YDisplacement = 0.0*Units::mm;
   const Double_t preVolumeCorner1ZDisplacement = 0.0*Units::mm;
   const Double_t preVolumeCorner2XDisplacement = 0.0*Units::mm;
   const Double_t preVolumeCorner2YDisplacement = 65.0*Units::mm;
   const Double_t preVolumeCorner2ZDisplacement = 0.0*Units::mm;
   const Double_t preVolumeCorner3XDisplacement = -65.0*Units::mm;
   const Double_t preVolumeCorner3YDisplacement = 0.0*Units::mm;
   const Double_t preVolumeCorner3ZDisplacement = 0.0*Units::mm;
   const Double_t preVolumeCorner4XDisplacement = 0.0*Units::mm;
   const Double_t preVolumeCorner4YDisplacement = -65.0*Units::mm;
   const Double_t preVolumeCorner4ZDisplacement = 0.0*Units::mm;
   
   const Double_t preVolumePhi = 90;
   const Double_t preVolumeTheta = 90;
   const Double_t preVolumePsi = 0;
   const Double_t preVolumeXDisplacement = guideXDisplacement - 9*guideSegHalfZ - preVolumeBoxHalfZ + 1*Units::mm;
   const Double_t preVolumeYDisplacement = guideYDisplacement;
   const Double_t preVolumeZDisplacement = guideZDisplacement;
   
   // -- Ramsey Cell
   // Neutral Electrode
   const Double_t neutralElectrodeRMin = 0.0*Units::mm;
   const Double_t neutralElectrodeRMax = 140.0*Units::mm;
   const Double_t neutralElectrodeHalfZ = 7.5*Units::mm;
   const Double_t neutralElectrodePhi = 90;
   const Double_t neutralElectrodeTheta = 90;
   const Double_t neutralElectrodePsi = 0;
   const Double_t neutralElectrodeXDisplacement = preVolumeXDisplacement - preVolumeBoxHalfZ - neutralElectrodeHalfZ + 1*Units::mm;
   const Double_t neutralElectrodeYDisplacement = preVolumeYDisplacement;
   const Double_t neutralElectrodeZDisplacement = preVolumeZDisplacement;
   
   // Neutral Electrode Valve Holes
   const Double_t neutralElectrodeHoleRMin = 0.0*Units::mm;
   const Double_t neutralElectrodeHoleRMax = 15.0*Units::mm;
   const Double_t neutralElectrodeHoleHalfZ = neutralElectrodeHalfZ;
/*   const Double_t neutralElectrodeHolePhi = 0.0;
   const Double_t neutralElectrodeHoleTheta = 0.0;
   const Double_t neutralElectrodeHolePsi = 0.0;
   const Double_t neutralElectrodeHoleZDisplacement = 0;
   const Double_t neutralElectrodeHole1XDisplacement = 0;
   const Double_t neutralElectrodeHole1YDisplacement = 65.0*Units::mm;
   const Double_t neutralElectrodeHole1ZDisplacement = neutralElectrodeHoleZDisplacement;
   const Double_t neutralElectrodeHole2XDisplacement = 65.0*Units::mm;
   const Double_t neutralElectrodeHole2YDisplacement = 0;
   const Double_t neutralElectrodeHole2ZDisplacement = neutralElectrodeHoleZDisplacement;
   const Double_t neutralElectrodeHole3XDisplacement = 0;
   const Double_t neutralElectrodeHole3YDisplacement = -65.0*Units::mm;
   const Double_t neutralElectrodeHole3ZDisplacement = neutralElectrodeHoleZDisplacement;
   const Double_t neutralElectrodeHole4XDisplacement = -65.0*Units::mm;
   const Double_t neutralElectrodeHole4YDisplacement = 0;
   const Double_t neutralElectrodeHole4ZDisplacement = neutralElectrodeHoleZDisplacement;
*/   
   const Double_t neutralElectrodeHolePhi = neutralElectrodePhi;
   const Double_t neutralElectrodeHoleTheta = neutralElectrodeTheta;
   const Double_t neutralElectrodeHolePsi = neutralElectrodePsi;
   const Double_t neutralElectrodeHole1XDisplacement = neutralElectrodeXDisplacement;
   const Double_t neutralElectrodeHole1YDisplacement = neutralElectrodeYDisplacement;
   const Double_t neutralElectrodeHole1ZDisplacement = neutralElectrodeZDisplacement+65.0*Units::mm;
   const Double_t neutralElectrodeHole2XDisplacement = neutralElectrodeXDisplacement;
   const Double_t neutralElectrodeHole2YDisplacement = neutralElectrodeYDisplacement+65.0*Units::mm;
   const Double_t neutralElectrodeHole2ZDisplacement = neutralElectrodeZDisplacement;
   const Double_t neutralElectrodeHole3XDisplacement = neutralElectrodeXDisplacement;
   const Double_t neutralElectrodeHole3YDisplacement = neutralElectrodeYDisplacement;
   const Double_t neutralElectrodeHole3ZDisplacement = neutralElectrodeZDisplacement-65.0*Units::mm;
   const Double_t neutralElectrodeHole4XDisplacement = neutralElectrodeXDisplacement;
   const Double_t neutralElectrodeHole4YDisplacement = neutralElectrodeYDisplacement-65.0*Units::mm;
   const Double_t neutralElectrodeHole4ZDisplacement = neutralElectrodeZDisplacement;
   
   // Neutral Cell
   const Double_t neutralCellRMin = 0.0*Units::mm;
   const Double_t neutralCellRMax = 125.0*Units::mm;
   const Double_t neutralCellHalfZ = 27.5*Units::mm;
   const Double_t neutralCellPhi = 90;
   const Double_t neutralCellTheta = 90;
   const Double_t neutralCellPsi = 0;
   const Double_t neutralCellXDisplacement = neutralElectrodeXDisplacement - neutralElectrodeHalfZ - neutralCellHalfZ;
   const Double_t neutralCellYDisplacement = neutralElectrodeYDisplacement;
   const Double_t neutralCellZDisplacement = neutralElectrodeZDisplacement;
   
   // Cell connector tube
   const Double_t cellConnectorRMin = 15.0*Units::mm;
   const Double_t cellConnectorRMax = 17.5*Units::mm;
   const Double_t cellConnectorHalfZ = 27.5*Units::mm;
   const Double_t cellConnectorPhi = 90;
   const Double_t cellConnectorTheta = 90;
   const Double_t cellConnectorPsi = 0;
   const Double_t cellConnectorXDisplacement = neutralCellXDisplacement;
   const Double_t cellConnectorYDisplacement = neutralCellYDisplacement;
   const Double_t cellConnectorZDisplacement = neutralCellZDisplacement + 65.0*Units::mm;
/*   
   const Double_t cellConnectorPhi = 0;
   const Double_t cellConnectorTheta = 0;
   const Double_t cellConnectorPsi = 0;
   const Double_t cellConnectorXDisplacement = 0;
   const Double_t cellConnectorYDisplacement = 65.0*Units::mm;
   const Double_t cellConnectorZDisplacement = 0;
*/ 

   // Central Electrode
   const Double_t centralElectrodeRMin = 0.0*Units::mm;
   const Double_t centralElectrodeRMax = 140.0*Units::mm;
   const Double_t centralElectrodeHalfZ = 7.0*Units::mm;
   const Double_t centralElectrodePhi = 90;
   const Double_t centralElectrodeTheta = 90;
   const Double_t centralElectrodePsi = 0;
   const Double_t centralElectrodeXDisplacement = neutralCellXDisplacement - neutralCellHalfZ - neutralElectrodeHalfZ + 1.0*Units::mm;
   const Double_t centralElectrodeYDisplacement = neutralCellYDisplacement;
   const Double_t centralElectrodeZDisplacement = neutralCellZDisplacement;
   
   // Central Electrode Hole
   const Double_t centralElectrodeHoleRMin = 0.0*Units::mm;
   const Double_t centralElectrodeHoleRMax = 15.0*Units::mm;
   const Double_t centralElectrodeHoleHalfZ = centralElectrodeHalfZ;
/*   const Double_t centralElectrodeHolePhi = 0.0;
   const Double_t centralElectrodeHoleTheta = 0.0;
   const Double_t centralElectrodeHolePsi = 0.0;
   const Double_t centralElectrodeHoleXDisplacement = 0;
   const Double_t centralElectrodeHoleYDisplacement = 65.0*Units::mm;
   const Double_t centralElectrodeHoleZDisplacement = centralElectrodeHoleZDisplacement;
*/   
   const Double_t centralElectrodeHolePhi = centralElectrodePhi;
   const Double_t centralElectrodeHoleTheta = centralElectrodeTheta;
   const Double_t centralElectrodeHolePsi = centralElectrodePsi;
   const Double_t centralElectrodeHoleXDisplacement = centralElectrodeXDisplacement;
   const Double_t centralElectrodeHoleYDisplacement = centralElectrodeYDisplacement;
   const Double_t centralElectrodeHoleZDisplacement = centralElectrodeZDisplacement+65.0*Units::mm;

   // HV Cell
   const Double_t hvCellRMin = 0.0*Units::mm;
   const Double_t hvCellRMax = 125.0*Units::mm;
   const Double_t hvCellHalfZ = 27.5*Units::mm;
   const Double_t hvCellPhi = 90;
   const Double_t hvCellTheta = 90;
   const Double_t hvCellPsi = 0;
   const Double_t hvCellXDisplacement = centralElectrodeXDisplacement - centralElectrodeHalfZ - hvCellHalfZ;
   const Double_t hvCellYDisplacement = centralElectrodeYDisplacement;
   const Double_t hvCellZDisplacement = centralElectrodeZDisplacement;
   
   // HV Electrode
   const Double_t hvElectrodeRMin = 0.0*Units::mm;
   const Double_t hvElectrodeRMax = 125.0*Units::mm;
   const Double_t hvElectrodeHalfZ = 2.*Units::mm;
   const Double_t hvElectrodePhi = 90;
   const Double_t hvElectrodeTheta = 90;
   const Double_t hvElectrodePsi = 0;
   const Double_t hvElectrodeXDisplacement = hvCellXDisplacement - hvCellHalfZ - hvElectrodeHalfZ;
   const Double_t hvElectrodeYDisplacement = hvCellYDisplacement;
   const Double_t hvElectrodeZDisplacement = hvCellZDisplacement;
   
}

namespace FieldParameters {
   // -- Uniform Solenoid
   // X, Y, Z are in the local coord system of the volume
   const Double_t solenoidBx = 0.;
   const Double_t solenoidBy = 0.;
   const Double_t solenoidBz = 0.01*Units::uT;
   
}

#endif /* MODEL_PARAMETERS_H */

