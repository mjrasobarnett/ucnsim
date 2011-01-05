#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "NodeStack.h"

//#define PRINT_CONSTRUCTORS 
//#define VERBOSE

using namespace std;

//______________________________________________________________________________
NodeStack::NodeStack(const int size)
          :fSize(size)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "NodeStack Construct" << endl;
   #endif
}

//______________________________________________________________________________
NodeStack::NodeStack(const NodeStack& other)
          :list<StackElement>(other)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "NodeStack Copy Construct" << endl;
   #endif
} 

//______________________________________________________________________________
NodeStack::~NodeStack()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "NodeStack Destruct" << endl;
   #endif
}

//______________________________________________________________________________
bool NodeStack::operator==(const NodeStack& other) const
{
   // -- Comparison operator.
   list<StackElement>::const_iterator thisIter = this->begin(), otherIter = other.begin();
   while (thisIter != this->end() && otherIter != other.end()) {
      if (thisIter->first->GetPoint() != otherIter->first->GetPoint() ||
          thisIter->second != otherIter->second) {
             return false;
      }
      thisIter++;
      otherIter++;
   }
   return true;
}

//______________________________________________________________________________
bool NodeStack::AddNode(const KDTreeNode* node, const double distance)
{
   // -- Take pointer to node and its distance from the search-point, and insert
   // -- into node stack if its distance is less than any other node in the stack.
   // -- Maintain ordering of the stack from closest to furthest away. Return whether
   // -- node was added or not. All nodes in the stack must be unique - no duplicates
   
   // If stack is empty, just add the node
   if (this->empty() == true) {
      this->push_back(StackElement(node, distance));
      #ifdef VERBOSE
         cout << "Adding Node to Stack" << endl;
         cout << "Stack size: " << this->size() << endl;
      #endif
      return true;
   }
   // Otherwise, search node-stack for whether to add node and at what position
   list<StackElement>::iterator listIter;
   for (listIter=this->begin(); listIter!=this->end(); listIter++) {
      // If node already exists in Stack, do nothing
      if (node->GetPoint() == listIter->first->GetPoint()) {return false;}
      // Determine whether node's distance is smaller than others in the stack 
      if (distance < listIter->second) {
         // If node is closer, then insert into stack in front of current position
         this->insert(listIter, StackElement(node, distance));
         // If list has now grown greater than the specific size, throw out the last
         // (most distant) node in the stack
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
   // add the node at the back
   if (this->size() < fSize) {
      this->push_back(StackElement(node, distance));
      #ifdef VERBOSE
         cout << "Adding Node to Stack" << endl;
         cout << "Stack size: " << this->size() << endl;
      #endif
      return true;
   }
   return false;
}
