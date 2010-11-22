// UCN Observer class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <cassert>

#include "TUCNObserver.h"
#include "TUCNParticle.h"
#include "TUCNRunConfig.h"
#include "TCanvas.h"

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNObserver                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

using namespace std;

ClassImp(TUCNObserver)

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNSpinObserver                                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNSpinObserver)

//_____________________________________________________________________________
TUCNSpinObserver::TUCNSpinObserver()
                 :TUCNObserver(),
                  fSpinUpHist(NULL),
                  fSpinDownHist(NULL)
{
   // Constructor
   Info("TUCNSpinObserver","Default Constructor");
}

//_____________________________________________________________________________
TUCNSpinObserver::TUCNSpinObserver(const TUCNRunConfig& runConfig)
                 :fSpinUpHist(NULL),
                  fSpinDownHist(NULL)
{
   // Constructor
   Info("TUCNSpinObserver","Constructor");
   Double_t runTime = runConfig.RunTime();
   Double_t binWidth = runConfig.MaxStepTime(); 
   Int_t nbins = (10*runTime)/binWidth;
   fSpinUpHist = new TH1F("SpinUpHist","SpinUpHist",nbins,0.0,runTime);
   fSpinDownHist = new TH1F("SpinDownHist","SpinDownHist",nbins,0.0,runTime);
   fMeasAxis = runConfig.PolarisationAxis();
}

//_____________________________________________________________________________
TUCNSpinObserver::TUCNSpinObserver(const TUCNSpinObserver& other)
                 :TUCNObserver(other),
                  fSpinUpHist(other.fSpinUpHist),
                  fSpinDownHist(other.fSpinDownHist),
                  fMeasAxis(other.fMeasAxis)
{
   // Copy Constructor
   Info("TUCNSpinObserver","Copy Constructor");
}

//_____________________________________________________________________________
TUCNSpinObserver& TUCNSpinObserver::operator=(const TUCNSpinObserver& other)
{
   // Assignment
   Info("TUCNSpinObserver","Assignment");
   if(this!=&other) {
      TUCNObserver::operator=(other);
      if (fSpinUpHist != NULL) delete fSpinUpHist; fSpinUpHist = NULL;
      fSpinUpHist = other.fSpinUpHist;
      if (fSpinDownHist != NULL) delete fSpinDownHist; fSpinDownHist = NULL;
      fSpinDownHist = other.fSpinDownHist;
      fMeasAxis = other.fMeasAxis;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNSpinObserver::~TUCNSpinObserver()
{
   // Destructor
   Info("TUCNSpinObserver","Destructor");
   if (fSpinUpHist != NULL) delete fSpinUpHist; fSpinUpHist = NULL;
   if (fSpinDownHist != NULL) delete fSpinDownHist; fSpinDownHist = NULL;
}

//_____________________________________________________________________________
void TUCNSpinObserver::RecordEvent(const TUCNParticle& particle)
{
   // -- Record the current polarisation
   if (particle.IsSpinUp(fMeasAxis) == kTRUE) {
      fSpinUpHist->Fill(particle.T());
   } else {
      fSpinDownHist->Fill(particle.T());
   }
}

//_____________________________________________________________________________
void TUCNSpinObserver::Plot()
{
   fSpinUpHist->Draw();
   new TCanvas();
   fSpinDownHist->Draw();  
}