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
   Double_t fSpinAxisX;
   Double_t fSpinAxisY;
   Double_t fSpinAxisZ;
   Bool_t fSpinUp;
   
public:
   // Constructors
   TUCNInitialConfig();
   TUCNInitialConfig(const std::string& initialConfigFileName);
   virtual ~TUCNInitialConfig();
   
   // Methods
   virtual void Print(Option_t* option = "") const;
   
   ClassDef(TUCNInitialConfig, 1)
};
#endif /*TUCNINITIALCONFIG_H*/