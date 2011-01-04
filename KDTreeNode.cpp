#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "KDTreeNode.hpp"
#include "NodeStack.hpp"

//#define PRINT_CONSTRUCTORS 
//#define VERBOSE

using namespace std;

//______________________________________________________________________________
KDTreeNode::KDTreeNode()
           :fDepth(0),
            fPoint(NULL), 
            fParent(NULL), 
            fLeft(NULL),
            fRight(NULL)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "KDTreeNode Construct" << endl;
   #endif
}

//______________________________________________________________________________
KDTreeNode::KDTreeNode(const KDTreeNode& other)
           :fDepth(other.fDepth),
            fPoint(other.fPoint), 
            fParent(other.fParent), 
            fLeft(other.fLeft),
            fRight(other.fRight)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "KDTreeNode Copy Construct" << endl;
   #endif
} 

//______________________________________________________________________________
KDTreeNode::~KDTreeNode()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "KDTreeNode Destruct" << endl;
   #endif
   if(fPoint) delete fPoint; fPoint = NULL;
   if(fLeft) delete fLeft; fLeft = NULL;
   if(fRight) delete fRight; fRight = NULL;
   // Don't delete parent
   fParent = NULL;
}

//______________________________________________________________________________
const KDTreeNode& KDTreeNode::FindNodeContaining(const Point& point) const
{
   // -- Check whether this node or its daughters are the containing
   // -- node of point
   if (fLeft == NULL && fRight == NULL) {
      // If node has no daughters (is leaf node) then it is the containing node
      #ifdef VERBOSE
         cout << "No Children. This is Containing Node: ";
         cout << this->GetPoint().ToString() << endl;
      #endif
      return *this;
   } else if (fLeft == NULL) {
      // If node has only right node then this is containing node
      #ifdef VERBOSE
         cout << "One Child. This is Containing Node: ";
         cout << fRight->GetPoint().ToString() << endl;
      #endif
      return *fRight;
   } else if (fRight == NULL) {
      // If node has only left node then this is containing node
      #ifdef VERBOSE
         cout << "One Child. This is Containing Node: ";
         cout << fLeft->GetPoint().ToString() << endl;
      #endif
      return *fLeft;
   }
   // Otherwise there are two children. Find out which child contains point
   // Choosing splitting axis
   int axis = this->GetDepth() % 3;
   bool insideLeft = false;
   switch (axis) {
      case 0 : 
         // x-axis splitting
         if (point.X() < this->GetPoint().X()) { insideLeft = true; }
         break;
      case 1 : 
         // y-axis splitting
         if (point.Y() < this->GetPoint().Y()) { insideLeft = true; }
         break;
      case 2 : 
         // z-axis splitting
         if (point.Z() < this->GetPoint().Z()) { insideLeft = true; }
         break;
     default : 
         cout << "KDTreeNode - Error: axis: " << axis << endl;
   }
   #ifdef VERBOSE
      cout << "Node Point: " << this->GetPoint().ToString() << endl;
      cout << "Splitting on axis: " << axis << endl;
      cout << "Inside Left child? " << insideLeft << endl;
   #endif
   // Search children
   if (insideLeft == true) {
      return fLeft->FindNodeContaining(point);
   } else {
      return fRight->FindNodeContaining(point);
   }
}

//______________________________________________________________________________
bool KDTreeNode::CheckParentForCloserNodes(const Point& point, NodeStack& neighbours) const
{
   // -- Check to see if parent node is closer than current nearest neighbour. Determine
   // -- whether other child nodes of parent could possibly be closer, and if so, search all of them
   // -- for closer nearest neighbours.
   // --
   // -- This is done by checking for whether its 'splitting hyperplane' (i.e: the axis
   // -- along which we split the tree at that node, either x, y, or z) intersects a sphere 
   // -- of radius 'radius' centred on the 'point'.
   // -- If the axis DOES intersect this sphere, then it is possible that another child node
   // -- of this parent is actually CLOSER to the point than the current best estimate.
   // --
   // -- Recursively search all parents up the tree until we reach the Root node.
   // -- Return the final best estimate of the nearest node.
   
   // Get the parent node
   const KDTreeNode* parent = this->GetParent();
   // If there is no parent we have reached the root node and can stop our search
   if (parent == NULL) {return true;}
   // Check if parent node is closer than current best
   neighbours.AddNode(parent, parent->DistanceTo(point));
   // Define radius of search sphere as distance from current nearest neighbour to point
   // Actually use the **furthest** of the n-th nearest neighbours so that we look for possible
   // improvements on any of the n-nearest neighbours in the list
   const double radius = neighbours.back().second;
   // Check whether we need to search other children of parent for closer neighbouring points
   if (parent->CheckOtherSideofSplittingPlane(point, radius) == true) {
      // If so, then we need to recursively search all branches of the other child of
      // the parent node for any closer nearest neighbours
      const KDTreeNode* otherChild = NULL;
      #ifdef VERBOSE
         cout << "Recursively search children for any closer nodes" << endl;
      #endif
      // Determine whether the current node is the left or right child of parent. We don't need
      // to search the current node since we have just come from there(!) and have been moving
      // up the tree from this direction
      if (this == parent->GetLeftChild()) {
         // Came from left child. Need to search the right child
         otherChild = parent->GetRightChild();
      } else if (this == parent->GetRightChild()) {
         // Came from right child. Need to search the left child
         otherChild = parent->GetLeftChild();
      }
      // Check if parent actually has another child
      if (otherChild != NULL) {
         // Check whether child is closer than current best 
         neighbours.AddNode(otherChild, otherChild->DistanceTo(point));
         // Check all of child's children for closer nodes
         otherChild->SearchChildren(point, neighbours);
      }
   }
   // Ask Parent to check its Parent Branch to recursively search up the tree 
   parent->CheckParentForCloserNodes(point, neighbours);
   return true;
}

//______________________________________________________________________________
bool KDTreeNode::CheckOtherSideofSplittingPlane(const Point& point, const double radius) const
{
   // -- Check node for whether its 'splitting hyperplane' (i.e: the axis
   // -- along which we split the tree at that node, either x, y, or z) intersects a sphere 
   // -- of radius 'radius' centred on the 'point'.   
   // -- Return whether this plane intersects or not
   const int depth = this->GetDepth();
   const int splitAxis = depth % 3;
   bool planeIntersects = false;
   // Determine splitting plane. Check for whether plane intersects sphere of radius 'radius'
   // centred around 'point'
   switch (splitAxis) {
      case 0 : 
      {  // x-axis splitting
         // Get X-coord of node. This coordinate defines the splitting plane (x=value)
         const double plane = this->GetPoint().X();
         // Check whether splitting plane intersects sphere centred on point 
         if (plane >= (point.X() - radius) && plane <= (point.X() + radius)) {
            planeIntersects = true;
         }
         break;
      }
      case 1 : 
      {  // y-axis splitting
         // Get Y-coord of node. This coordinate defines the splitting plane (y=value)
         const double plane = this->GetPoint().Y();
         // Check whether splitting plane intersects sphere centred on point 
         if (plane >= (point.Y() - radius) && plane <= (point.Y() + radius)) {
            planeIntersects = true;
         }
         break;
      }
      case 2 : 
      {  // z-axis splitting
         // Get Z-coord of node. This coordinate defines the splitting plane (z=value)
         const double plane = this->GetPoint().Z();
         // Check whether splitting plane intersects sphere centred on point 
         if (plane >= (point.Z() - radius) && plane <= (point.Z() + radius)) {
            planeIntersects = true;
         }
         break;
      }
      default : 
         cout << "KDTreeNode - Error: axis: " << splitAxis << endl;
   }
   #ifdef VERBOSE
      cout << endl << "--------------------" << endl;
      cout << "Checking Node: " << this->GetPoint().ToString();
      cout << ", to see if it intersects current best hypershpere around search point" << endl;
      cout << "Search Point: " << point.ToString() << endl;
      cout << "Sphere Radius: " << radius << endl;
      cout << "Parent Depth: " << depth << endl;
      cout << "Axis: " << splitAxis << " Intersects? " << planeIntersects << endl;
      cout << endl;
   #endif
   return planeIntersects;
}

//______________________________________________________________________________
bool KDTreeNode::SearchChildren(const Point& point, NodeStack& neighbours) const
{
   // -- Recursively search all children of current node to see whether any are closer to the point
   // -- than the provided 'current best' 
   if (fLeft == NULL && fRight == NULL) {return true;}
   #ifdef VERBOSE
      cout << "Searching Children of Node: " << this->GetPoint().ToString() << endl;
      cout << "Current Best: " << neighbours.Front().first->GetPoint().ToString() << endl;
   #endif
   ////////////////////////////////////////////////////////////////////////////////////
   // If left child is the previous child (i.e: the branch we have come from)
   // then we don't have to check this again 
   if (fLeft != NULL) {
      // Else, check if distance of left child to point is less than current best estimate 
      // If it is closer to point than current best estimate then update estimate
      neighbours.AddNode(fLeft, fLeft->DistanceTo(point));
      // Recursively search children of Left node for any improvement on current best estimate
      fLeft->SearchChildren(point, neighbours);
   }
   ////////////////////////////////////////////////////////////////////////////////////
   // If right child is the previous child (i.e: the branch we have come from)
   // then we don't have to check this again
   if (fRight != NULL) {
      // Else, check if distance of right child to point is less than current best estimate 
      // If it is closer to point than current best estimate then update estimate
      neighbours.AddNode(fRight, fRight->DistanceTo(point));
      // Recursively search children of Left node for any improvement on current best estimate
      fRight->SearchChildren(point, neighbours);
   }
   // Return the current best estimate of the nearest neighbour
   return true;
}

//______________________________________________________________________________
bool KDTreeNode::IsCloserToPoint(const Point& point, const KDTreeNode& currentBest) const
{
   // -- Calculate whether current node is closer to point than the 'current best'
   const double dist = this->GetPoint().DistanceTo(point);
   double currentBestDist = currentBest.GetPoint().DistanceTo(point);
   #ifdef VERBOSE
      cout << "Node: " << this->GetPoint().ToString() << endl;
      cout << "Node Dist: " << dist << "\t" << "Current Best: " << currentBestDist << endl;
   #endif
   if (dist < currentBestDist) {
      // If it is closer to point than current best estimate then update estimate
      #ifdef VERBOSE
         cout << "Node is closer" << endl;
      #endif
      return true;
   }
   return false;
}

//______________________________________________________________________________
void KDTreeNode::OutputGraphViz(ostream& out) const
{
   // Output Nodes in form of 'dot' language
   if (fParent != NULL) {
      out << "\t";
      out << "\"" << fParent->GetPoint().ToString() << "\"";
      out << " -> ";
      out << "\"" << this->GetPoint().ToString() << "\"";
   }
   if (fLeft) fLeft->OutputGraphViz(out);
   if (fRight) fRight->OutputGraphViz(out);
}
