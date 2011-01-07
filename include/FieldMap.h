// FieldMap
// Author: Matthew Raso-Barnett  13/12/2010

#ifndef ROOT_FieldMap
#define ROOT_FieldMap

#include <string>

#include "TNamed.h"
#include "TVector3.h"
#include "MagField.h"
#include "KDTree.h"

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
   virtual ~FieldMap();
   
   virtual Bool_t BuildMap(const std::string& filename) = 0;
   
   ClassDef(FieldMap, 1)   // Field Map class
};


////////////////////////////////////////////////////////////////////////////
//                                                                        //
// MagFieldMap -                                                          //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Particle;

class MagFieldMap : public FieldMap, public MagField {
private:
   KDTree* fTree;
   
public:
   MagFieldMap();
   MagFieldMap(const std::string& name, const TGeoShape* fieldShape, const TGeoMatrix* fieldPosition);
   MagFieldMap(const MagFieldMap&);
   virtual ~MagFieldMap();
   
   virtual Bool_t Interact(Particle& particle, const Double_t stepTime) const;
   virtual Bool_t BuildMap(const std::string& filename);
   
   ClassDef(MagFieldMap, 1)   // Mag Field Map class
};

#endif