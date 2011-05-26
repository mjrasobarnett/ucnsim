#ifndef PARTICLEMANIFEST_H
#define PARTICLEMANIFEST_H

#include <map>
#include <string>
#include <vector>

#include "TNamed.h"

class Listing : public TNamed {
private:
   std::vector<int> fTreeIndexes;
   std::vector<int> fParticleIDs;

public:
   // -- Constructors
   Listing();
   Listing(std::string name);
   Listing(const Listing& other);

   // -- Destructor
   virtual ~Listing();
   
   void AddEntry(const int id, const int index);
   const std::vector<int>& GetTreeIndexes() const;
   const std::vector<int>& GetParticleIDs() const;
   size_t Entries() const {return fTreeIndexes.size();}
   void Extend(const Listing& other);
   
   ClassDef(Listing,1)
};

class ParticleManifest : public TNamed
{
private:
   std::map<std::string, std::vector<int> > fDictionary;
   
public:
   // -- Constructors
   ParticleManifest();
   ParticleManifest(const ParticleManifest& other);

   // -- Destructor
   virtual ~ParticleManifest();
   
   void AddEntry(const std::string state, const int id);
   std::vector<int> GetList(const std::string state) const;
   void Print() const;
   

   
   ClassDef(ParticleManifest,1)   // Ultra-Cold Neutron
};

#endif  /*PARTICLEMANIFEST_H*/