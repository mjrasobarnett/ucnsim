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
   std::map<std::string, Listing> fDictionary;
   
public:
   // -- Constructors
   ParticleManifest();
   ParticleManifest(const ParticleManifest& other);

   // -- Destructor
   virtual ~ParticleManifest();
   
   void AddEntry(const std::string state, const int id, const int index);
   Listing GetListing(const std::string state) const;
   Listing GetListing(const std::vector<std::string> states) const;
   void Print() const;
   

   
   ClassDef(ParticleManifest,1)
};

#endif  /*PARTICLEMANIFEST_H*/
