// UCN Observer class
// Author: Matthew Raso-Barnett  19/11/2010
#include <iostream>
#include <sstream>
#include <cassert>

#include "TUCNObserver.h"

#include "TUCNParticle.h"
#include "TUCNRunConfig.h"
#include "TUCNData.h"

#include "TKey.h"

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
                  fSpinObservables(NULL)
{
   // Constructor
   Info("TUCNSpinObserver","Default Constructor");
}

//_____________________________________________________________________________
TUCNSpinObserver::TUCNSpinObserver(const TUCNRunConfig& runConfig)
                 :TUCNObserver(),
                  fSpinObservables(NULL)
{
   // Constructor
   Info("TUCNSpinObserver","Constructor");
   fMeasAxis = runConfig.PolarisationAxis();
}

//_____________________________________________________________________________
TUCNSpinObserver::TUCNSpinObserver(const TUCNSpinObserver& other)
                 :TUCNObserver(other),
                  fSpinObservables(other.fSpinObservables),
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
      fSpinObservables = other.fSpinObservables;
      fMeasAxis = other.fMeasAxis;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNSpinObserver::~TUCNSpinObserver()
{
   // Destructor
   Info("TUCNSpinObserver","Destructor");
}

//_____________________________________________________________________________
void TUCNSpinObserver::RegisterInterest(TUCNParticle& particle)
{
   // -- Register as an observer with the particle
   if (fSpinObservables != NULL) delete fSpinObservables; fSpinObservables = NULL;
   fSpinObservables = new TUCNSpinObservables();
   particle.Attach(this);
}

//_____________________________________________________________________________
void TUCNSpinObserver::RecordEvent(const TUCNParticle& particle, const string& context)
{
   // -- Record the current polarisation
   if (context == Context::Spin) {
      fSpinObservables->insert(pair<Double_t, Bool_t>(particle.T(), particle.IsSpinUp(fMeasAxis)));
   }
}

//_____________________________________________________________________________
void TUCNSpinObserver::LoadExistingObservables(TDirectory* const particleDir)
{
   // -- Look for a TUCNSpinObservables object and if so load into memory
   particleDir->cd();
   // -- Loop on all entries of this directory
   TKey *key;
   TIter nextkey(particleDir->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TUCNSpinObservables")) {
         if (fSpinObservables != NULL) delete fSpinObservables; fSpinObservables = NULL;
         fSpinObservables = dynamic_cast<TUCNSpinObservables*>(key->ReadObj());
         break;
      }
   }
}

//_____________________________________________________________________________
void TUCNSpinObserver::WriteToFile(TDirectory* const particleDir)
{
   // -- Write out the current observable to the provided directory
   particleDir->cd();
   fSpinObservables->Write("TUCNSpinObservables",TObject::kOverwrite);
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    TUCNBounceObserver                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

ClassImp(TUCNBounceObserver)

//_____________________________________________________________________________
TUCNBounceObserver::TUCNBounceObserver()
                   :TUCNObserver(),
                    fBounceObservables(NULL)
{
   // Constructor
   Info("TUCNBounceObserver","Default Constructor");
}

//_____________________________________________________________________________
TUCNBounceObserver::TUCNBounceObserver(const TUCNRunConfig& /*runConfig*/)
                 :TUCNObserver(),
                  fBounceObservables(NULL)
{
   // Constructor
   Info("TUCNBounceObserver","Constructor");
}

//_____________________________________________________________________________
TUCNBounceObserver::TUCNBounceObserver(const TUCNBounceObserver& other)
                 :TUCNObserver(other),
                  fBounceObservables(other.fBounceObservables)
{
   // Copy Constructor
   Info("TUCNBounceObserver","Copy Constructor");
}

//_____________________________________________________________________________
TUCNBounceObserver& TUCNBounceObserver::operator=(const TUCNBounceObserver& other)
{
   // Assignment
   Info("TUCNBounceObserver","Assignment");
   if(this!=&other) {
      TUCNObserver::operator=(other);
      fBounceObservables = other.fBounceObservables;
   }
   return *this;
}

//_____________________________________________________________________________
TUCNBounceObserver::~TUCNBounceObserver()
{
   // Destructor
   Info("TUCNBounceObserver","Destructor");
}

//_____________________________________________________________________________
void TUCNBounceObserver::RegisterInterest(TUCNParticle& particle)
{
   // -- Register as an observer with the particle
   if (fBounceObservables != NULL) delete fBounceObservables; fBounceObservables = NULL;
   fBounceObservables = new TUCNBounceObservables();
   particle.Attach(this);
}

//_____________________________________________________________________________
void TUCNBounceObserver::RecordEvent(const TUCNParticle& particle, const string& context)
{
   // -- Record the current polarisation
   if (context == Context::SpecBounce) {
      fBounceObservables->RecordSpecular();
   } else if (context == Context::DiffBounce) {
      fBounceObservables->RecordDiffuse();
   }
}

//_____________________________________________________________________________
void TUCNBounceObserver::LoadExistingObservables(TDirectory* const particleDir)
{
   // -- Look for a TUCNSpinObservables object and if so load into memory
   particleDir->cd();
   // -- Loop on all entries of this directory
   TKey *key;
   TIter nextkey(particleDir->GetListOfKeys());
   while ((key = static_cast<TKey*>(nextkey.Next()))) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname);
      if (!cl) continue;
      if (cl->InheritsFrom("TUCNBounceObservables")) {
         if (fBounceObservables != NULL) delete fBounceObservables; fBounceObservables = NULL;
         fBounceObservables = dynamic_cast<TUCNBounceObservables*>(key->ReadObj());
         break;
      }
   }
}

//_____________________________________________________________________________
void TUCNBounceObserver::WriteToFile(TDirectory* const particleDir)
{
   // -- Write out the current observable to the provided directory
   particleDir->cd();
   fBounceObservables->Write("TUCNBounceObservables",TObject::kOverwrite);
}
