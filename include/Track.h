// Track
// Author: Matthew Raso-Barnett  03/12/2010

#ifndef ROOT_Track
#define ROOT_Track

#include "TObject.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//       Track                                                            //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class Track : public TObject
{
protected:
   
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

#endif
