#ifndef __VERTEX_STACK_H
#define __VERTEX_STACK_H

#include <iostream>
#include <list>

class FieldVertex;

//--------------------------------------------------------------------------
//--
//--  Vertex Stack
//--  This class functions as a list of 'Stack Elements' (themselves just pairs
//--  of FieldVertex objects with a number, in this case distance from some other
//--  point). The list is always kept sorted on each Element's distance, with the
//--  the smallest distance at the front. The list has a finite size defined on
//--  construction and thus when inserting new vertices into the list, those at
//--  the back will be popped off to ensure that the list never grows above its
//--  maximum size.
//-- 
//--------------------------------------------------------------------------

typedef std::pair<const FieldVertex*, double> StackElement;

class VertexStack : public std::list<StackElement> {
   public:
      VertexStack(const int size);
      VertexStack(const VertexStack& other);
      virtual ~VertexStack();
      
      bool operator==(const VertexStack& other) const;
      
      bool ExamineVertex(const FieldVertex& vertex, const double distance);
      
   private:
      unsigned int fMaxSize;
};

#endif
