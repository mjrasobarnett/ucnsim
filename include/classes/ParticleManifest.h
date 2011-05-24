#ifndef PARTICLEMANIFEST_H
#define PARTICLEMANIFEST_H

#include <map>
#include <string>
#include <vector>

#include "TNamed.h"

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