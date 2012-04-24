#ifndef PopulationData_H
#define PopulationData_H

#include <map>
#include <string>

#include "TObject.h"

class PopulationData : public std::map<double, std::map<std::string, int> >,  public TObject
{
   public:
      PopulationData() {;}
      virtual ~PopulationData() {;}
      
      void Fill(double t, std::string statename);
      
   ClassDef(PopulationData, 1)
};

#endif


