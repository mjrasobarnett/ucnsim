#ifndef __VERTEX_STACK_H
#define __VERTEX_STACK_H

#include <iostream>
#include <list>

class FieldVertex;

typedef std::pair<const FieldVertex*, double> StackElement;

class VertexStack : public std::list<StackElement> {
   public:
      VertexStack(const int size);
      VertexStack(const VertexStack& other);
      virtual ~VertexStack();
      
      bool operator==(const VertexStack& other) const;
      
      bool ExamineVertex(const FieldVertex& vertex, const double distance);
      
   private:
      unsigned int fSize;
};

#endif
