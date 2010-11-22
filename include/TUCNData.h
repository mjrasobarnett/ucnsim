#ifndef TUCNDATA_H
#define TUCNDATA_H

#include "TNamed.h"
#include "TTree.h"
#include <vector>
#include <string>
#include "TUCNParticle.h"

class TUCNParticle;

class TUCNData : public TNamed {
protected:
   TTree          *fInitialParticles;
   TTree          *fPropagatingParticles;
   TTree          *fDetectedParticles;
   TTree          *fDecayedParticles;
   TTree          *fAbsorbedParticles;
   TTree          *fLostParticles;
   TTree          *fBadParticles;
   
public:
   TUCNData();
   TUCNData(const char *name, const char *title);
   TUCNData(const TUCNData& d);
   TUCNData& operator=(const TUCNData& d); 
   virtual ~TUCNData(void);
   
   Bool_t         ChecksOut();
   
   // Add a Particle
   Bool_t         AddParticleState(TString treeName, TUCNParticle* particle);
   Bool_t         AddParticleState(TTree* tree, TUCNParticle* particle);
   Bool_t         AddInitialParticleState(TUCNParticle* particle);
   Bool_t         AddPropagatingParticleState(TUCNParticle* particle);
   Bool_t         AddDetectedParticleState(TUCNParticle* particle);
   Bool_t         AddDecayedParticleState(TUCNParticle* particle);
   Bool_t         AddAbsorbedParticleState(TUCNParticle* particle);
   Bool_t         AddLostParticleState(TUCNParticle* particle);
   Bool_t         AddBadParticleState(TUCNParticle* particle);
   
   // Get a Particle
   TUCNParticle*  GetParticleState(TString treeName, Int_t index);
   TUCNParticle*  GetParticleState(TTree* tree, Int_t index);
   TUCNParticle*  GetInitialParticleState(Int_t index);
   TUCNParticle*  GetPropagatingParticleState(Int_t index);
   TUCNParticle*  GetDetectedParticleState(Int_t index);
   TUCNParticle*  GetDecayedParticleState(Int_t index);
   TUCNParticle*  GetAbsorbedParticleState(Int_t index);
   TUCNParticle*  GetLostParticleState(Int_t index);
   TUCNParticle*  GetBadParticleState(Int_t index);
   
   Int_t          InitialParticles() const   {return fInitialParticles->GetEntriesFast();}
   Int_t          PropagatingParticles() const  {return fPropagatingParticles->GetEntriesFast();}
   Int_t          DetectedParticles() const  {return fDetectedParticles->GetEntriesFast();}
   Int_t          DecayedParticles() const   {return fDecayedParticles->GetEntriesFast();}
   Int_t          AbsorbedParticles() const  {return fAbsorbedParticles->GetEntriesFast();}
   Int_t          LostParticles() const      {return fLostParticles->GetEntriesFast();}
   Int_t          BadParticles() const       {return fBadParticles->GetEntriesFast();}
   Int_t          FinalParticles() const     {return this->PropagatingParticles() +
                                              this->DetectedParticles() + this->DecayedParticles() +
                                              this->AbsorbedParticles() + this->LostParticles() +
                                              this->BadParticles();}
   // Get a Particular Tree
   TTree*         FetchTree(TString treeName);
   
   ClassDef(TUCNData, 1) // UCN Data Object
};

class TUCNDataBranch : public std::vector<TUCNParticle*>
{
   
public:
   TUCNDataBranch();
   TUCNDataBranch(const std::string& name);
   TUCNDataBranch(const TUCNDataBranch& );
   TUCNDataBranch& operator=(const TUCNDataBranch& ); 
   virtual ~TUCNDataBranch(void);
   
   
   ClassDef(TUCNDataBranch, 1)
};

class TUCNDataTest : public TObject
{

public:
   TUCNDataBranch fBranch;
   
   TUCNDataTest();
   virtual ~TUCNDataTest(void);
   
   ClassDef(TUCNDataTest, 1)
};



#endif
