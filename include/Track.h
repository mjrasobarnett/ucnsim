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

class Track : public std::vector<Vertex>, public TObject
{
private:
   
public:
   // -- constructors
   Track();
   Track(const Track&); 
   Track& operator=(const Track&);
   // -- destructor
   virtual ~Track();
   
   // -- methods
   
   
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
   Double_t X() {return fX;}
   Double_t Y() {return fY;}
   Double_t Z() {return fZ;}
   Double_t T() {return fT;}
   
   ClassDef(Vertex, 1)
};


#endif
