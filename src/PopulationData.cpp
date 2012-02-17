#include <iostream>

#include "PopulationData.h"

using namespace std;

ClassImp(PopulationData);

//______________________________________________________________________________
void PopulationData::Fill(double currentTime, std::string statename)
{
   typedef map<double, map<string, int> > popdata;
   popdata::iterator timeBinIter = this->find(currentTime);
   if (timeBinIter == this->end()) {
      // No entry for time 't' currently exists. Insert an empty map for this time
      timeBinIter = this->insert(this->begin(), pair<double, map<string, int> >( currentTime, map<string, int>() ) );
   }
   // For current time, insert
   map<string, int>& populationTable = timeBinIter->second;
   map<string, int>::iterator tableIter = populationTable.find(statename);
   if (tableIter == populationTable.end()) {
      // No entry for this state currently exists. Insert an entry for this.
      tableIter = populationTable.insert(populationTable.begin(), pair<string, int>( statename, 0 ) );
   }
   // Increment counter in table for this state
   tableIter->second += 1;
}
