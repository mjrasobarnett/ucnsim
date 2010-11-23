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
                  fParticleData()
{
   // Constructor
   Info("TUCNSpinObserver","Default Constructor");
}

//_____________________________________________________________________________
TUCNSpinObserver::TUCNSpinObserver(const TUCNRunConfig& runConfig)
                 :TUCNObserver()
{
   // Constructor
   Info("TUCNSpinObserver","Constructor");
   fMeasAxis = runConfig.PolarisationAxis();
}

//_____________________________________________________________________________
TUCNSpinObserver::TUCNSpinObserver(const TUCNSpinObserver& other)
                 :TUCNObserver(other),
                  fParticleData(other.fParticleData),
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
      PurgeContainer();
      fParticleData = other.fParticleData;
      fMeasAxis = other.fMeasAxis;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNSpinObserver::~TUCNSpinObserver()
{
   // Destructor
   Info("TUCNSpinObserver","Destructor");
   cout << "No. of Particles Stored: " << fParticleData.size() << endl;
   PurgeContainer();
   cout << "After Purge: " << fParticleData.size() << endl;
   
}

//_____________________________________________________________________________
void TUCNSpinObserver::RegisterInterest(TUCNParticle& particle)
{
   // Create storage for this particle's data
   fParticleData.insert(pair<Int_t,TUCNSpinObservables*>(particle.Id(), new TUCNSpinObservables()));
   // Register as an observer with the particle
   particle.Attach(this);
}

//_____________________________________________________________________________
void TUCNSpinObserver::RecordEvent(const TUCNParticle& particle)
{
   // -- Record the current polarisation
   map<Int_t, TUCNSpinObservables*>::iterator entry = fParticleData.find(particle.Id());
   TUCNSpinObservables* spinobservable = (entry->second);
   spinobservable->insert(pair<Double_t, Bool_t>(particle.T(), particle.IsSpinUp(fMeasAxis)));
}

//_____________________________________________________________________________
void TUCNSpinObserver::PurgeContainer()
{
   // -- Clean up all Observables in container
   if (fParticleData.empty() == kFALSE) {
      map<Int_t,TUCNSpinObservables*>::iterator it;
      for(it = fParticleData.begin(); it != fParticleData.end(); ++it) {
         if (it->second != NULL) delete (it->second);
         it->second = NULL;
         fParticleData.erase(it);
      }
   }
}