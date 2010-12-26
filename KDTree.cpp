#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "KDTree.hpp"

//#define PRINT_CONSTRUCTORS 
//#define VERBOSE

using namespace std;

//______________________________________________________________________________
Point::Point()
      :fX(0.), fY(0.), fZ(0.)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point Default Construct" << endl;
   #endif
}

//______________________________________________________________________________
Point::Point(double x, double y, double z)
      :fX(x), fY(y), fZ(z)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point Construct" << endl;
   #endif
}

//______________________________________________________________________________
Point::Point(const Point& other)
      :fX(other.fX),
       fY(other.fY),
       fZ(other.fZ)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point Copy Construct" << endl;
   #endif
}

//______________________________________________________________________________
Point::~Point()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "Point Destruct" << endl;
   #endif
}

//______________________________________________________________________________
bool Point::operator==(const Point& other) const
{
   if (other.X() == fX && other.Y() == fY && other.Z() == fZ){
      return true;
   } else {
      return false;
   }
}

//______________________________________________________________________________
string Point::ToString() const 
{
   ostringstream oss;
   oss.precision(2);
   oss << "(" << fX << ", " << fY << ", " << fZ << ")";
   return oss.str();
}

//______________________________________________________________________________
double Point::SquaredDistanceTo(const Point& point) const
{
   double xdist = pow((point.X() - this->X()), 2.0);
   double ydist = pow((point.Y() - this->Y()), 2.0);
   double zdist = pow((point.Z() - this->Z()), 2.0);
   return xdist + ydist + zdist;
}

//______________________________________________________________________________
double Point::DistanceTo(const Point& point) const
{
   return sqrt(SquaredDistanceTo(point));
}


//______________________________________________________________________________
//______________________________________________________________________________

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
const KDTreeNode& KDTreeNode::CheckParentForCloserNodes(const Point& point, const KDTreeNode& previousBest) const
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
   const KDTreeNode* currentBest = &previousBest;
   // Get the parent node
   const KDTreeNode* parent = this->GetParent();
   // If there is no parent we have reached the root node and can stop our search
   if (parent == NULL) {return previousBest;}
   // Check if parent node is closer than current best
   if (parent->IsCloserToPoint(point, *currentBest) == true) {currentBest = parent;}
   // Define radius of search sphere as distance from current best to point
   const double radius = currentBest->GetPoint().DistanceTo(point);
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
         if (otherChild->IsCloserToPoint(point,*currentBest)) {currentBest = otherChild;}
         // Check all of child's children for closer nodes
         currentBest = &(otherChild->SearchChildren(point, *currentBest));
      }
   }
   
   // Ask Parent to check its Parent Branch to recursively search up the tree 
   currentBest = &(parent->CheckParentForCloserNodes(point, *currentBest));
   return *currentBest;
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
const KDTreeNode& KDTreeNode::SearchChildren(const Point& point, const KDTreeNode& previousBest) const
{
   // -- Recursively search all children of current node to see whether any are closer to the point
   // -- than the provided 'current best' 
   if (fLeft == NULL && fRight == NULL) {return previousBest;}
   #ifdef VERBOSE
      cout << "Searching Children of Node: " << this->GetPoint().ToString() << endl;
      cout << "Current Best: " << previousBest.GetPoint().ToString() << endl;
   #endif
   const KDTreeNode* currentBest = &previousBest;
   ////////////////////////////////////////////////////////////////////////////////////
   // If left child is the previous child (i.e: the branch we have come from)
   // then we don't have to check this again 
   if (fLeft != NULL) {
      // Else, check if distance of left child to point is less than current best estimate 
      if (fLeft->IsCloserToPoint(point, *currentBest) == true) {
         // If it is closer to point than current best estimate then update estimate
         currentBest = fLeft;
         #ifdef VERBOSE
            cout << "Left is closer" << endl;
         #endif
      }
      // Recursively search children of Left node for any improvement on current best estimate
      currentBest = &(fLeft->SearchChildren(point, *currentBest));
   }
   ////////////////////////////////////////////////////////////////////////////////////
   // If right child is the previous child (i.e: the branch we have come from)
   // then we don't have to check this again
   if (fRight != NULL) {
      // Else, check if distance of right child to point is less than current best estimate 
      if (fRight->IsCloserToPoint(point, *currentBest) == true) {
         // If it is closer to point than current best estimate then update estimate
         currentBest = fRight;
         #ifdef VERBOSE
            cout << "Right is closer" << endl;
         #endif
      }
      // Recursively search children of Left node for any improvement on current best estimate
      currentBest = &(fRight->SearchChildren(point, *currentBest));
   }
   // Return the current best estimate of the nearest neighbour
   return *currentBest;
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

//______________________________________________________________________________
//______________________________________________________________________________


//______________________________________________________________________________
KDTree::KDTree(vector<Point*>& pointList)
       :fRoot(NULL)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "KDTree Construct" << endl;
   #endif
   fRoot = this->BuildNode(pointList,0);
   #ifdef VERBOSE
      cout << endl << "Root Point: " << fRoot->GetPoint().ToString() << endl;
   #endif
}

//______________________________________________________________________________
KDTree::KDTree(const KDTree& other)
       :fRoot(other.fRoot)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "KDTree Copy Construct" << endl;
   #endif
} 

//______________________________________________________________________________
KDTree::~KDTree()
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "KDTree Destruct" << endl;
   #endif
   if(fRoot) delete fRoot; fRoot = NULL;
}

//______________________________________________________________________________
KDTreeNode* KDTree::BuildNode(vector<Point*>& points, KDTreeNode* parent, int depth)
{
   if (points.empty()) {
      return NULL;
   } else {
      // Select axis based on depth so that axis cycles through all valid values
      int axis = depth % 3;
      // Sort point list and choose median as pivot element
      switch (axis) {
         case 0 : 
            sort(points.begin(), points.end(), sortX());
            break;
         case 1 : 
            sort(points.begin(), points.end(), sortY());
            break;
         case 2 : 
            sort(points.begin(), points.end(), sortZ());
            break;
        default : 
            cout << "KDTree::Build - Error: axis: " << axis << endl;
            return NULL;
      }
      // Find median -- ceil used to round up in case of odd number of points.
      // Minus 1 turns it from the point number to a 'c-style' index 
      int median = ceil(points.size()/2.0) - 1; 
      // Create node and construct subtrees
      KDTreeNode* node = new KDTreeNode();
      node->SetDepth(depth);
      node->SetParent(parent);
      node->SetPoint(points[median]);
      // Create list of only those points before and after median
      vector<Point*> beforeMedian, afterMedian;
      // Note: copy works elements in range [first, last)
      copy(points.begin(), points.begin()+median, back_inserter(beforeMedian));
      copy(points.begin()+median+1, points.end(), back_inserter(afterMedian));
      #ifdef VERBOSE
         cout << "--------------------" << endl;
         cout << "Ordered by Axis: " << axis << endl;
         vector<Point*>::iterator it;
         for (it=points.begin(); it<points.end(); it++) {
            cout << it - points.begin() << "\t" << (*it)->ToString() << endl;
         }
         cout << "Median Index: " << median << endl;
         cout << "Median: " << points[median]->ToString() << endl;
         cout << "Before Median: " << "\t";
         for (it=beforeMedian.begin(); it<beforeMedian.end(); it++) {
            cout << it  -beforeMedian.begin() << "\t";
            cout << (*it)->ToString() << endl;
         }
         cout << "After Median: " << "\t";
         for (it=afterMedian.begin(); it<afterMedian.end(); it++) {
            cout << it - afterMedian.begin() << "\t";
            cout << (*it)->ToString() << endl;
         }
         cout << endl;
      #endif
      // Build subtrees
      node->SetLeftChild(BuildNode(beforeMedian, node, depth+1));
      node->SetRightChild(BuildNode(afterMedian, node, depth+1));
      return node;
   }  
}

//______________________________________________________________________________
const Point& KDTree::NearestNeighbour(const Point& point) const
{
   // Find the nearest neighbouring point to the supplied point
   #ifdef VERBOSE
      cout << endl << "--------------------" << endl;
      cout << "Search for nearest neightbour of Point : ";
      cout << point.ToString() << endl;
   #endif
   // Find leaf node that contains the point. Store as first guess 
   const KDTreeNode& firstGuess = fRoot->FindNodeContaining(point);
   // Calculate distance from point to first guess nearest neighbour
   double dist = firstGuess.GetPoint().DistanceTo(point);
   #ifdef VERBOSE
      cout << endl << "--------------------" << endl;
      cout << "First Guess : ";
      cout << firstGuess.GetPoint().ToString() << endl;
      cout << "Distance to Point: " << dist << endl;
   #endif
   // Now traverse back up tree looking for if any other nodes are closer
   const KDTreeNode& nearestNode = firstGuess.CheckParentForCloserNodes(point, firstGuess);
   #ifdef VERBOSE
      cout << "--------------------" << endl;
      cout << "Nearest Node : ";
      cout << nearestNode.GetPoint().ToString() << endl;
      cout << "Distance to Point: " << nearestNode.GetPoint().DistanceTo(point) << endl;
   #endif
   return nearestNode.GetPoint();
}

//______________________________________________________________________________
void KDTree::OutputGraphViz(ostream& out) const
{
   // Output Nodes in form of 'dot' language
   out << "digraph G {" << endl;
   fRoot->OutputGraphViz(out);
   out << "}" << endl;
}
