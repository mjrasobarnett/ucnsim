#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "VertexStack.h"
#include "FieldVertex.h"

//#define PRINT_CONSTRUCTORS 
//#define VERBOSE

using namespace std;

//______________________________________________________________________________
VertexStack::VertexStack(const int size)
          :fSize(size)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "VertexStack Construct" << endl;
   #endif
}

//______________________________________________________________________________
VertexStack::VertexStack(const VertexStack& other)
          :list<StackElement>(other)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "VertexStack Copy Construct" << endl;
   #endif
} 

//______________________________________________________________________________
VertexStack::~VertexStack()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "VertexStack Destruct" << endl;
   #endif
}

//______________________________________________________________________________
bool VertexStack::operator==(const VertexStack& other) const
{
   // -- Comparison operator.
   // Check they have same number of elements
   if (this->size() != other.size()) {return false;}
   // Check that all elements are the same and in same order
   list<StackElement>::const_iterator thisIter = this->begin(), otherIter = other.begin();
   while (thisIter != this->end() && otherIter != other.end()) {
      if (thisIter->first != otherIter->first ||
          thisIter->second != otherIter->second) {
             return false;
      }
      thisIter++;
      otherIter++;
   }
   return true;
}

//______________________________________________________________________________
bool VertexStack::ExamineVertex(const FieldVertex& vertex, const double distance)
{
   // -- Take pointer to vertex and its distance from the search-point, and insert
   // -- into vertex stack if its distance is less than any other vertex in the stack.
   // -- Maintain ordering of the stack from closest to furthest away. Return whether
   // -- vertex was added or not. All vertexs in the stack must be unique - no duplicates
   
   // If stack is empty, just add the vertex
   if (this->empty() == true) {
      this->push_back(StackElement(&vertex, distance));
      #ifdef VERBOSE
         cout << "Adding Node to Stack" << endl;
         cout << "Stack size: " << this->size() << endl;
      #endif
      return true;
   }
   // Otherwise, search vertex-stack for whether to add vertex and at what position
   list<StackElement>::iterator listIter;
   for (listIter=this->begin(); listIter!=this->end(); listIter++) {
      // If vertex already exists in Stack, do nothing
      if (vertex == *(listIter->first) ) {return false;}
      // Determine whether vertex's distance is smaller than others in the stack 
      if (distance < listIter->second) {
         // If vertex is closer, then insert into stack in front of current position
         this->insert(listIter, StackElement(&vertex, distance));
         // If list has now grown greater than the specific size, throw out the last
         // (most distant) vertex in the stack
         if (this->size() > fSize) {
            #ifdef VERBOSE
               cout << "Removing Node from End" << endl;
               cout << "Stack size: " << this->size() << endl;
            #endif
            this->pop_back();
         }
         #ifdef VERBOSE
            cout << "Adding Node to Stack" << endl;
            cout << "Stack size: " << this->size() << endl;
         #endif
         return true;
      }
   }
   // Node is not closer than any others in the list. Finally if stack is not yet full, just
   // add the vertex at the back
   if (this->size() < fSize) {
      this->push_back(StackElement(&vertex, distance));
      #ifdef VERBOSE
         cout << "Adding Node to Stack" << endl;
         cout << "Stack size: " << this->size() << endl;
      #endif
      return true;
   }
   return false;
}
