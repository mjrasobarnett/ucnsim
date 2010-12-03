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
   
public:
   // -- constructors
   Vertex();
   Vertex(const Vertex&); 
   Vertex& operator=(const Vertex&);
   // -- destructor
   virtual ~Vertex();
   
   // -- methods
   
   
   ClassDef(Vertex, 1)
};


#endif
