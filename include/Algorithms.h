// Algorithms.h -- Stores some project-wide algorithms that are too general to go into classes


#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include "DataFileHierarchy.h"

namespace Algorithms {
   //_____________________________________________________________________________
   // Namespace holding functions relevant to floating point precision
   //_____________________________________________________________________________
   namespace Precision 
   {
      //_____________________________________________________________________________
      bool IsEqual(double left, double right, double precision = 1.E-10) {
         return abs(left - right) < precision ? true : false;
      }
      //_____________________________________________________________________________
      bool IsNotEqual(double left, double right, double precision = 1.E-10) {
         return abs(left - right) < precision ? false : true;
      }
      //_____________________________________________________________________________
      bool IsGreaterOrEqual(double left, double right, double precision = 1.E-10) {
         if (abs(left - right) < precision) {
            // Check equality
            return true;
         } else if (left > right) {
            // Check if greater
            return true;
         } else {
            return false;
         }
      }
      //_____________________________________________________________________________
      bool IsLessOrEqual(double left, double right, double precision = 1.E-10) {
         if (abs(left - right) < precision) {
            // Check equality
            return true;
         } else if (left < right) {
            // Check if less than
            return true;
         } else {
            return false;
         }
      }
   }
   //_____________________________________________________________________________
   // Namespace holding functions relevant to datafile structure
   //_____________________________________________________________________________
   namespace DataFile
   {
      //_____________________________________________________________________________
      bool ValidateRootFile(const std::string filename)
      {
         // -- Check that the filename supplied has a .root extension
         size_t found = filename.find_last_of(".");
         if (found == std::string::npos) return false;
         if (filename.substr(found) == ".root") return true;
         return false;
      }
      //_____________________________________________________________________________
      bool ValidateStateNames(const std::vector<std::string>& statenames)
      {
         // -- Check that each statename in list is a valid state as defined
         // -- in DataFileHierarchy lvl 3 and is unique
         std::vector<std::string>::const_iterator iter;
         for (iter = statenames.begin(); iter != statenames.end(); iter++) {
            // Check state-name
            if (*iter != Folders::initial &&
                *iter != Folders::propagating &&
                *iter != Folders::absorbed &&
                *iter != Folders::lost &&
                *iter != Folders::decayed &&
                *iter != Folders::detected &&
                *iter != Folders::anomalous) {
               std::cerr << "Argument, " << *iter << " is not a valid statename" << std::endl;
               return false;
            }
            // Check for duplicates
            std::vector<std::string>::const_iterator second_iter;
            for (second_iter = iter+1; second_iter != statenames.end(); second_iter++) {
               if (*second_iter == *iter) {
                  std::cerr << "Duplicate statenames given: " << *iter << std::endl;
                  return false;
               }
            }
         }
         return true;
      }
      //_____________________________________________________________________________
      void CountParticles(TDirectory * const particleDir)
      {
         // -- Given the particle state directory, count the number of particles
         // -- in each state subfolder
         if (particleDir->cd(Folders::initial.c_str()) == false) return;
         Int_t initial = gDirectory->GetNkeys();
         if (particleDir->cd(Folders::propagating.c_str()) == false) return;
         Int_t propagating = gDirectory->GetNkeys();
         if (particleDir->cd(Folders::absorbed.c_str()) == false) return;
         Int_t absorbed = gDirectory->GetNkeys();
         if (particleDir->cd(Folders::detected.c_str()) == false) return;
         Int_t detected = gDirectory->GetNkeys();
         if (particleDir->cd(Folders::decayed.c_str()) == false) return;
         Int_t decayed = gDirectory->GetNkeys();
         if (particleDir->cd(Folders::lost.c_str()) == false) return;
         Int_t lost = gDirectory->GetNkeys();
         if (particleDir->cd(Folders::anomalous.c_str()) == false) return;
         Int_t anomalous = gDirectory->GetNkeys();
         std::cout << "Initial Particles: " << initial << std::endl;
         std::cout << "Final Total: ";
         std::cout << propagating+detected+absorbed+decayed+lost+anomalous << std::endl;
         std::cout << "Propagating: " << propagating << std::endl;
         std::cout << "Detected: " << detected << std::endl;
         std::cout << "Absorbed: " << absorbed << std::endl;
         std::cout << "Decayed: " << decayed << std::endl;
         std::cout << "Lost: " << lost << std::endl;
         std::cout << "Anomalous: " << anomalous << std::endl;
      }
   }
}
#endif
