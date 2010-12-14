// FieldMap
// Author: Matthew Raso-Barnett  13/12/2010

#ifndef ROOT_FieldMap
#define ROOT_FieldMap

#include "TNamed.h"
#include "TVector3.h"
#include "Particle.h"
#include "MagField.h"
#include <string>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// FieldMap -                                                             //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class FieldMap {
private:
   
public:
   FieldMap();
   FieldMap(const FieldMap&);
   FieldMap& operator=(const FieldMap&);
   virtual ~FieldMap();
   
   Bool_t ReadFile(const std::string& filename);
   
   ClassDef(FieldMap, 1)              // Field Map class
};


////////////////////////////////////////////////////////////////////////////
//                                                                        //
// MagFieldMap -                                                          //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class MagFieldMap : public FieldMap, public MagField {
private:
   
public:
   MagFieldMap();
   MagFieldMap(const MagFieldMap&);
   MagFieldMap& operator=(const MagFieldMap&);
   virtual ~MagFieldMap();
   
   virtual Bool_t Contains(const TVector3& point) const = 0;
   virtual Bool_t Interact(Particle& particle, const Double_t stepTime) const = 0;
   
   ClassDef(MagFieldMap, 1)              // Mag Field Map class
};

#endif