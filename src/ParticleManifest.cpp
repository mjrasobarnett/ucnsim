#include <iostream>
#include <iomanip>
#include <cassert>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "ParticleManifest.hpp"

using namespace std;

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

ClassImp(Listing)

//______________________________________________________________________________
Listing::Listing()
        :TNamed(), fTreeIndexes(), fParticleIDs()
{
   #ifdef PRINT_CONSTRUCTORS
      Info("ParticleManifest","Default Constructor");
   #endif
}

//______________________________________________________________________________
Listing::Listing(std::string name)
        :TNamed(name.c_str(),""), fTreeIndexes(), fParticleIDs()
{
   #ifdef PRINT_CONSTRUCTORS
      Info("ParticleManifest","Constructor");
   #endif
}

//______________________________________________________________________________
Listing::Listing(const Listing& other)
        :TNamed(other), fTreeIndexes(other.fTreeIndexes), fParticleIDs(other.fParticleIDs)
{
   #ifdef PRINT_CONSTRUCTORS
      Info("ParticleManifest","Copy Constructor");
   #endif
}

//______________________________________________________________________________
Listing::~Listing()
{
   #ifdef PRINT_CONSTRUCTORS
      Info("ParticleManifest","Destructor");
   #endif
}

//______________________________________________________________________________
void Listing::AddEntry(const int id, const int index)
{
   fParticleIDs.push_back(id);
   fTreeIndexes.push_back(index);
}

//______________________________________________________________________________
const std::vector<int>& Listing::GetTreeIndexes() const
{
   return fTreeIndexes;
}

//______________________________________________________________________________
const std::vector<int>& Listing::GetParticleIDs() const
{
   return fParticleIDs;
}

//______________________________________________________________________________
void Listing::Extend(const Listing& other)
{
   string newName = this->GetName();
   newName.append("+");
   newName.append(other.GetName());
   this->SetName(newName.c_str());
   fParticleIDs.insert(fParticleIDs.end(), other.GetParticleIDs().begin(), other.GetParticleIDs().end());
   fTreeIndexes.insert(fTreeIndexes.end(), other.GetTreeIndexes().begin(), other.GetTreeIndexes().end());
   sort(fParticleIDs.begin(), fParticleIDs.end());
   sort(fTreeIndexes.begin(), fTreeIndexes.end());
}


ClassImp(ParticleManifest)

//______________________________________________________________________________
ParticleManifest::ParticleManifest()
                 :TNamed("ParticleManifest","Dictionary of Particle States"),
                  fDictionary()
{
   // -- Default constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("ParticleManifest","Default Constructor");
   #endif
}

//_____________________________________________________________________________
ParticleManifest::ParticleManifest(const ParticleManifest& other)
                 :TNamed(other),
                  fDictionary(other.fDictionary)
{
   // -- Copy Constructor
   #ifdef PRINT_CONSTRUCTORS
      Info("ParticleManifest","Copy Constructor");
   #endif
}

//______________________________________________________________________________
ParticleManifest::~ParticleManifest()
{
   // -- Destructor
   #ifdef PRINT_CONSTRUCTORS
      Info("ParticleManifest","Destructor");
   #endif
}

//______________________________________________________________________________
void ParticleManifest::AddEntry(const string state, const int id, const int index)
{
   string l_state = boost::to_lower_copy(state);
   map<string, Listing>::iterator iter = fDictionary.find(l_state);
   if (iter == fDictionary.end()) {
      Listing newListing(state);
      newListing.AddEntry(id, index);
      fDictionary[l_state] = newListing;
   } else {
      iter->second.AddEntry(id, index);
   }
}

//______________________________________________________________________________
Listing ParticleManifest::GetListing(const std::string state) const
{
   string l_state = boost::to_lower_copy(state);
   map<string, Listing>::const_iterator iter = fDictionary.find(l_state);
   if (iter == fDictionary.end()) {
      return Listing(state);
   }
   return iter->second;
}

//______________________________________________________________________________
Listing ParticleManifest::GetListing(const vector<string> states) const
{
   Listing newListing;
   vector<string>::const_iterator stateIter;
   for (stateIter = states.begin(); stateIter != states.end(); stateIter++) {
      string l_state = boost::to_lower_copy(*stateIter);
      map<string, Listing>::const_iterator iter = fDictionary.find(l_state);
      if (iter != fDictionary.end()) {
         const Listing& foundListing = iter->second;
         newListing.Extend(foundListing);
      }
   }
   return newListing;
}

//______________________________________________________________________________
void ParticleManifest::Print() const
{
   map<string, Listing >::const_iterator iter;
   for (iter = fDictionary.begin(); iter != fDictionary.end(); iter++) {
      cout << left << setw(20) << iter->first << ": \t" << iter->second.Entries() << endl;
   }
}
