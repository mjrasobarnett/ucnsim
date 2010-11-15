#ifndef MODEL_PARAMETERS_H
#define MODEL_PARAMETERS_H

#include "../include/Units.h"
#include "../include/Constants.h"

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
   const Double_t valveVolFrontHalfLength = 12.75.*Units::mm;
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
   const Double_t ramseyCellRMin = 0.; 
   const Double_t ramseyCellRMax = 235.0*Units::mm;
   const Double_t ramseyCellHalfLength = 120.*Units::mm;
   const Double_t ramseyCellAngle = 90.0;

   const Double_t bFieldX = 0.;
   const Double_t bFieldY = 0.;
   const Double_t bFieldZ = 5.0*Units::uT;
}
#endif /* MODEL_PARAMETERS_H */

