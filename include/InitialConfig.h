// Initial Configuration class
// Author: Matthew Raso-Barnett  
// 18/11/2010

#ifndef INITIALCONFIG_H
#define INITIALCONFIG_H

#include "TVector3.h"
#include <string>

class InitialConfig {
private:
   std::string fRunName;
   
   std::string fGeomFile;
   std::string fGeomVisFile;
   std::string fOutputDataFile;
   
   std::string fBeamShape;
   double fBeamRadius;
   double fBeamLength;
   double fBeamPhi;
   double fBeamTheta;
   double fBeamPsi;
   TVector3 fBeamDisplacement;
   
   int fInitialParticles;
   double fInitialMaxVelocity; // m/s
   double fFillingTime;  // s
   
   double fDirMinTheta;
   double fDirMaxTheta;
   double fDirMinPhi;
   double fDirMaxPhi;
   
   double fPercentagePolarised;
   TVector3 fSpinAxis;
   bool fSpinUp;
   
public:
   // Constructors
   InitialConfig();
   InitialConfig(const std::string& initialConfigFileName);
   InitialConfig(const InitialConfig&); 
   InitialConfig& operator=(const InitialConfig&);
   virtual ~InitialConfig();
   
   // Methods
   std::string RunName() const {return fRunName;}
   std::string GeomFileName() const {return fGeomFile;}
   std::string GeomVisFileName() const {return fGeomVisFile;}
   std::string OutputFileName() const {return fOutputDataFile;}
   
   std::string BeamShape() const {return fBeamShape;}
   double BeamRadius() const {return fBeamRadius;}
   double BeamLength() const {return fBeamLength;}
   double BeamPhi() const {return fBeamPhi;}
   double BeamTheta() const {return fBeamTheta;}
   double BeamPsi() const {return fBeamPsi;}
   const TVector3& BeamDisplacement() const {return fBeamDisplacement;}

   int InitialParticles() const {return fInitialParticles;}
   double InitialMaxVelocity() const {return fInitialMaxVelocity;}
   double FillingTime() const {return fFillingTime;}
   
   double DirectionMinTheta() const {return fDirMinTheta;}
   double DirectionMaxTheta() const {return fDirMaxTheta;}
   double DirectionMinPhi() const {return fDirMinPhi;}
   double DirectionMaxPhi() const {return fDirMaxPhi;}
   
   const TVector3& SpinAxis() const {return fSpinAxis;}
   bool SpinUp() const {return fSpinUp;}
   double PercentPolarised() const {return fPercentagePolarised;}
   
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(InitialConfig, 0)
};
#endif /*INITIALCONFIG_H*/