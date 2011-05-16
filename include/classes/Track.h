// Track
// Author: Matthew Raso-Barnett  03/12/2010

#ifndef ROOT_Track
#define ROOT_Track

#include <vector>
#include "TObject.h"
#include "Point.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//       Track                                                            //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class Point;

class Track : public TObject
{
private:
   std::vector<Point*> fPoints;
   
   void PurgeContainer();
   
public:
   // -- constructors
   Track();
   Track(const Track&); 
   Track& operator=(const Track&);
   // -- destructor
   virtual ~Track();
   
   // -- methods
   void           AddPoint(const Point& point);
   const Point&   GetPoint(unsigned int i) const;
   unsigned int   TotalPoints() const {return fPoints.size();}
   
   std::vector<Double_t> OutputPointsArray();
   
   ClassDef(Track, 1)
};

#endif
