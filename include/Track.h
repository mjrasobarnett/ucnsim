// Track
// Author: Matthew Raso-Barnett  03/12/2010

#ifndef ROOT_Track
#define ROOT_Track

#include <vector>
#include "TObject.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//       Track                                                            //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Vertex;

class Track : public TObject
{
private:
   std::vector<Vertex*> fVertices;
   
   void PurgeContainer();
   
public:
   // -- constructors
   Track();
   Track(const Track&); 
   Track& operator=(const Track&);
   // -- destructor
   virtual ~Track();
   
   // -- methods
   void           AddVertex(const Double_t x, const Double_t y, const Double_t z, const Double_t t);
   const Vertex&  GetVertex(unsigned int i) const;
   unsigned int   TotalVertices() const {return fVertices.size();}
   
   std::vector<Double_t> OutputPointsArray();
   
   ClassDef(Track, 1)
};


////////////////////////////////////////////////////////////////////////////
//                                                                        //
//       Vertex                                                           //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class Vertex : public TObject
{
private:
   Double_t fX, fY, fZ, fT;
   
public:
   // -- constructors
   Vertex();
   Vertex(const Double_t x, const Double_t y, const Double_t z, const Double_t t);
   Vertex(const Vertex&); 
   Vertex& operator=(const Vertex&);
   // -- destructor
   virtual ~Vertex();
   
   // -- methods
   Double_t X() const {return fX;}
   Double_t Y() const {return fY;}
   Double_t Z() const {return fZ;}
   Double_t T() const {return fT;}
   
   ClassDef(Vertex, 1)
};


#endif
