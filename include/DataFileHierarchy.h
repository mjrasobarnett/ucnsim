#ifndef DATAFILEHIERARCHY_H
#define DATAFILEHIERARCHY_H

#include <string>

namespace Folders
{
   // Level 1
   const std::string particles = "particles";
   // Level 2
   const std::string initialstates = "initialstates";
   const std::string finalstates = "finalstates";
   // Level 3
   const std::string propagating = "propagating";
   const std::string absorbed = "absorbed";
   const std::string detected = "detected";
   const std::string decayed = "decayed";
   const std::string lost = "lost";
   const std::string bad = "bad";
   
}

#endif