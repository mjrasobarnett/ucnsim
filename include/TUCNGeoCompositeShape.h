// @(#)root/geom:$Id: TUCNGeoCompositeShape.h 24879 2008-07-18 08:04:40Z brun $
// Author: Andrei Gheata   31/01/02

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TUCNGeoCompositeShape
#define ROOT_TUCNGeoCompositeShape

#ifndef ROOT_TUCNGeoBBox
#include "TUCNGeoBBox.h"
#endif
    
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// TUCNGeoCompositeShape - composite shape class. A composite shape contains //
//   a list of primitive shapes, the list of coresponding transformations    //
//   and a boolean finder handling boolean operations among components.      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//class TUCNGeoBoolNode;

class TUCNGeoCompositeShape : public TUCNGeoBBox
{

   ClassDef(TUCNGeoCompositeShape, 1)         // boolean composite shape
};



#endif

