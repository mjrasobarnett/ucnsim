// @(#):$Id: TGeoBoolNode.h 25878 2008-10-18 19:12:08Z rdm $
// Author: Andrei Gheata   30/05/02

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TUCNGeoBoolNode
#define ROOT_TUCNGeoBoolNode

#ifndef ROOT_TObject
#include "TObject.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// TUCNGeoBoolNode - Base class for boolean nodes. A boolean node has pointers //
//  to two shapes having two transformations with respect to the mother     //
//  composite shape they belong to. It represents the boolean operation     //
//  between the two component shapes.                                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// forward declarations
class TGeoShape;
class TGeoMatrix;
class TGeoHMatrix;

class TUCNGeoBoolNode : public TObject
{

   ClassDef(TUCNGeoBoolNode, 1)              // a boolean node
};

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// TUCNGeoUnion - Boolean node representing a union between two components.    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

class TUCNGeoUnion : public TUCNGeoBoolNode
{
public:

   ClassDef(TUCNGeoUnion, 1)              // union node
};

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// TUCNGeoIntersection - Boolean node representing an intersection between two //
// components.                                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

class TUCNGeoIntersection : public TUCNGeoBoolNode
{
public:

   ClassDef(TUCNGeoIntersection, 1)              // intersection node
};

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// TUCNGeoSubtraction - Boolean node representing a subtraction.               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

class TUCNGeoSubtraction : public TUCNGeoBoolNode
{
public:


   ClassDef(TUCNGeoSubtraction, 1)              // subtraction node
};
#endif

