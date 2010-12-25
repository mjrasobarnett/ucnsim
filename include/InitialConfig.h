// Initial Configuration class
// Author: Matthew Raso-Barnett  
// 18/11/2010

#ifndef INITIALCONFIG_H
#define INITIALCONFIG_H

#include "TObject.h"
#include "TVector3.h"
#include <string>

class InitialConfig : public TObject {
private:
   std::string fRunName;
   
   std::string fGeomFile;
   std::string fGeomVisFile;
   std::string fOutputDataFile;
   
   std::string fBeamShape;
   Double_t fBeamRadius;
   Double_t fBeamLength;
   Double_t fBeamPhi;
   Double_t fBeamTheta;
   Double_t fBeamPsi;
   TVector3 fBeamDisplacement;
   
   Int_t fInitialParticles;
   Double_t fInitialMaxVelocity; // m/s
   Double_t fFillingTime;  // s
   
   Double_t fDirMinTheta;
   Double_t fDirMaxTheta;
   Double_t fDirMinPhi;
   Double_t fDirMaxPhi;
   
   Double_t fPercentagePolarised;
   TVector3 fSpinAxis;
   Bool_t fSpinUp;
   
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
   Double_t BeamRadius() const {return fBeamRadius;}
   Double_t BeamLength() const {return fBeamLength;}
   Double_t BeamPhi() const {return fBeamPhi;}
   Double_t BeamTheta() const {return fBeamTheta;}
   Double_t BeamPsi() const {return fBeamPsi;}
   const TVector3& BeamDisplacement() const {return fBeamDisplacement;}

   Int_t InitialParticles() const {return fInitialParticles;}
   Double_t InitialMaxVelocity() const {return fInitialMaxVelocity;}
   Double_t FillingTime() const {return fFillingTime;}
   
   Double_t DirectionMinTheta() const {return fDirMinTheta;}
   Double_t DirectionMaxTheta() const {return fDirMaxTheta;}
   Double_t DirectionMinPhi() const {return fDirMinPhi;}
   Double_t DirectionMaxPhi() const {return fDirMaxPhi;}
   
   const TVector3& SpinAxis() const {return fSpinAxis;}
   Bool_t SpinUp() const {return fSpinUp;}
   Double_t PercentPolarised() const {return fPercentagePolarised;}
   
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(InitialConfig, 1)
};
#endif /*INITIALCONFIG_H*/