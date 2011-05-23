#ifndef VALIDSTATES_H
#define VALIDSTATES_H

#include <string>

namespace States
{
   const std::string initial = "initial";
   const std::string propagating = "propagating";
   const std::string absorbed = "absorbed";
   const std::string detected = "detected";
   const std::string decayed = "decayed";
   const std::string lost = "lost";
   const std::string anomalous = "anomalous";   
}

#endif
