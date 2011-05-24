#include <iostream>
#include <iomanip>
#include <cassert>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "ParticleManifest.h"

using namespace std;

//#define VERBOSE_MODE
//#define PRINT_CONSTRUCTORS

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
void ParticleManifest::AddEntry(const std::string state, const int id)
{
   string l_state = boost::to_lower_copy(state);
   map<string, vector<int> >::iterator iter = fDictionary.find(l_state);
   if (iter == fDictionary.end()) {
      vector<int> id_list(1,id);
      fDictionary[l_state] = id_list;
   } else {
      iter->second.push_back(id);
   }
}

//______________________________________________________________________________
std::vector<int> ParticleManifest::GetList(const std::string state) const
{
   string l_state = boost::to_lower_copy(state);
   map<string, vector<int> >::const_iterator iter = fDictionary.find(l_state);
   if (iter == fDictionary.end()) {
      return vector<int>();
   }
   return iter->second;
}

//______________________________________________________________________________
void ParticleManifest::Print() const
{
   map<string, vector<int> >::const_iterator iter;
   for (iter = fDictionary.begin(); iter != fDictionary.end(); iter++) {
      cout << left << setw(20) << iter->first << ": \t" << iter->second.size() << endl;
   }
}