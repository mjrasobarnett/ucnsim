// Initial Configuration class
// Author: Matthew Raso-Barnett  
// 18/11/2010

#ifndef TUCNINITIALCONFIG_H
#define TUCNINITIALCONFIG_H

#include "TObject.h"
#include "TVector3.h"
#include <string>

class TUCNInitialConfig : public TObject {
private:
   std::string fRunName;
   
   std::string fGeomFile;
   std::string fGeomVisFile;
   std::string fOutputDataFile;
   
   Int_t fInitialParticles;
   Double_t fInitialMaxVelocity; // m/s
   Double_t fFillingTime;  // s
   
   Double_t fPercentagePolarised;
   TVector3 fSpinAxis;
   Bool_t fSpinUp;
   
public:
   // Constructors
   TUCNInitialConfig();
   TUCNInitialConfig(const std::string& initialConfigFileName);
   TUCNInitialConfig(const TUCNInitialConfig&); 
   TUCNInitialConfig& operator=(const TUCNInitialConfig&);
   virtual ~TUCNInitialConfig();
   
   // Methods
   std::string RunName() const {return fRunName;}
   std::string GeomFileName() const {return fGeomFile;}
   std::string GeomVisFileName() const {return fGeomVisFile;}
   std::string OutputFileName() const {return fOutputDataFile;}
   
   Int_t InitialParticles() const {return fInitialParticles;}
   Double_t InitialMaxVelocity() const {return fInitialMaxVelocity;}
   Double_t FillingTime() const {return fFillingTime;}
   
   TVector3 SpinAxis() const {return fSpinAxis;}
   Bool_t SpinUp() const {return fSpinUp;}
   Double_t PercentPolarised() const {return fPercentagePolarised;}
   
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(TUCNInitialConfig, 1)
};
#endif /*TUCNINITIALCONFIG_H*/