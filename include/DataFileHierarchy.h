#ifndef DATAFILEHIERARCHY_H
#define DATAFILEHIERARCHY_H

#include <string>

namespace Folders
{
   // Level 1
   const std::string particles = "particles";
   const std::string config = "config";
   const std::string histograms = "histograms";
   const std::string geometry = "geometry";
   // Level 2 -- Sub dirs of particles
   const std::string initial = "initial";
   const std::string propagating = "propagating";
   const std::string absorbed = "absorbed";
   const std::string detected = "detected";
   const std::string decayed = "decayed";
   const std::string lost = "lost";
   const std::string anomalous = "anomalous";
   
}

#endif
