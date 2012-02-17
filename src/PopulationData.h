#ifndef PopulationData_H
#define PopulationData_H

#include <map>
#include <string>

#include "TObject.h"

class PopulationData : public std::map<double, std::map<std::string, int> >,  public TObject
{
   public:
      PopulationData() {Info("PopulationData","Constructor");}
      virtual ~PopulationData() {Info("PopulationData","Destructor");}
      
      void Fill(double t, std::string statename);
      
   ClassDef(PopulationData, 1)
};

#endif


