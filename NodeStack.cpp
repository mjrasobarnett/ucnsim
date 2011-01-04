#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "NodeStack.hpp"

#define PRINT_CONSTRUCTORS 
#define VERBOSE

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
          :fNodeStack(other.fNodeStack)
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
bool NodeStack::AddNode(const KDTreeNode& node, const double distance)
{
   // -- Take pointer to node and its distance from the search-point, and insert
   // -- into node stack if its distance is less than any other node in the stack.
   // -- Maintain ordering of the stack from closest to furthest away. Return whether
   // -- node was added or not.
   
   // If stack is empty, just add the node
   if (fNodeStack.empty() == true) {
      fNodeStack.push_back(StackElement(&node, distance));
      #ifdef VERBOSE
         cout << "Adding Node to Stack" << endl;
         cout << "Stack size: " << fNodeStack.size() << endl;
      #endif
      return true;
   }
   // Otherwise, search node-stack for whether to add node and at what position
   list<StackElement>::iterator listIter;
   for (listIter=fNodeStack.begin(); listIter!=fNodeStack.end(); listIter++) {
      // Determine whether node's distance is smaller than others in the stack 
      if (distance < listIter->second) {
         // If node is closer, then insert into stack in front of current position
         fNodeStack.insert(listIter, StackElement(&node, distance));
         // If list has now grown greater than the specific size, throw out the last
         // (most distant) node in the stack
         if (fNodeStack.size() > fSize) {
            #ifdef VERBOSE
               cout << "Removing Node from End" << endl;
               cout << "Stack size: " << fNodeStack.size() << endl;
            #endif
            fNodeStack.pop_back();
         }
         #ifdef VERBOSE
            cout << "Adding Node to Stack" << endl;
            cout << "Stack size: " << fNodeStack.size() << endl;
         #endif
         return true;
      }
   }
   // Node is not closer than any others in the list. Finally if stack is not yet full, just
   // add the node at the back
   if (fNodeStack.size() < fSize) {
      fNodeStack.push_back(StackElement(&node, distance));
      #ifdef VERBOSE
         cout << "Adding Node to Stack" << endl;
         cout << "Stack size: " << fNodeStack.size() << endl;
      #endif
      return true;
   }
   return false;
}
