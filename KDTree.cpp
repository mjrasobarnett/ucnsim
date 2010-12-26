#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "KDTree.hpp"

//#define PRINT_CONSTRUCTORS 
#define VERBOSE

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
const KDTreeNode& KDTreeNode::FindContainingNode(const Point& point) const
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
   }
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
      return fLeft->FindContainingNode(point);
   } else {
      return fRight->FindContainingNode(point);
   }
}

//______________________________________________________________________________
const KDTreeNode& KDTreeNode::CheckParentBranches(const Point& point, const KDTreeNode& previousBest) const
{
   // Recursively check parent node for whether its 'splitting hyperplane' (i.e: the axis
   // along which we split the tree at that node, either x, y, or z) intersects a sphere 
   // of radius 'radius' centred on the 'point'. 
   // --
   // If the axis DOES intersect this sphere, then it is possible that another child node
   // of this parent is actually CLOSER to the point than the current best estimate.
   // --
   // Return the final best estimate of the nearest node.
   
   // Get the parent node
   const KDTreeNode* parent = this->GetParent();
   if (parent == NULL) {return previousBest;}
   
   const KDTreeNode* currentBest = &previousBest;
   const double radius = currentBest->GetPoint().DistanceTo(point);
   const int parentDepth = parent->GetDepth();
   const int parentSplitAxis = parentDepth % 3;
   bool planeIntersects = false;
   // Determine splitting plane. Check for whether plane intersects sphere of radius 'radius'
   // centred around 'point'
   switch (parentSplitAxis) {
      case 0 : 
      {  // x-axis splitting
         // Get X-coord of parent node. This coordinate defines the splitting plane (x=value)
         const double plane = parent->GetPoint().X();
         // Check whether splitting plane intersects sphere centred on point 
         if (plane >= (point.X() - radius) && plane <= (point.X() + radius)) {
            planeIntersects = true;
         }
         break;
      }
      case 1 : 
      {  // y-axis splitting
         // Get Y-coord of parent node. This coordinate defines the splitting plane (y=value)
         const double plane = parent->GetPoint().Y();
         // Check whether splitting plane intersects sphere centred on point 
         if (plane >= (point.Y() - radius) && plane <= (point.Y() + radius)) {
            planeIntersects = true;
         }
         break;
      }
      case 2 : 
      {  // z-axis splitting
         // Get Z-coord of parent node. This coordinate defines the splitting plane (z=value)
         const double plane = parent->GetPoint().Z();
         // Check whether splitting plane intersects sphere centred on point 
         if (plane >= (point.Z() - radius) && plane <= (point.Z() + radius)) {
            planeIntersects = true;
         }
         break;
      }
      default : 
         cout << "KDTreeNode - Error: axis: " << parentSplitAxis << endl;
   }
   
   cout << endl << "--------------------" << endl;
   cout << "Checking Parent of current Node: " << this->GetPoint().ToString();
   cout << ", to see if it intersects current best hypershpere around search point" << endl;
   cout << "Parent Node: " << parent->GetPoint().ToString() << endl;
   cout << "Search Point: " << point.ToString() << endl;
   cout << "Sphere Radius: " << radius << endl;
   cout << "Parent Depth: " << parentDepth << endl;
   cout << "Axis: " << parentSplitAxis << " Intersects? " << planeIntersects << endl;
   cout << endl;
   
   // Check if splitting plane intersected sphere
   if (planeIntersects == true) {
      // If so, then we need to check the parent node and
      // and recursively search all (other) children of parent node
      // for any closer nearest neighbours
      const double parentDist = parent->GetPoint().DistanceTo(point);
      const double currentBestDist = currentBest->GetPoint().DistanceTo(point);
      if (parentDist < currentBestDist) {currentBest = parent;}
      cout << "Recursively search children for any closer nodes" << endl;
      currentBest = &(parent->SearchChildren(point, *currentBest, *this));
   }
   
   // Ask Parent to check its Parent Branch to recursively search up the tree 
   currentBest = &(parent->CheckParentBranches(point, *currentBest));
   
   return *currentBest;
}

//______________________________________________________________________________
const KDTreeNode& KDTreeNode::SearchChildren(const Point& point, const KDTreeNode& previousBest, const KDTreeNode& previousChild) const
{
   // Recursively search all children of current node to see whether any are closer to the point
   // than the provided 'current best' 
   if (fLeft == NULL && fRight == NULL) {return previousBest;}
   cout << "Searching Children of Node: " << this->GetPoint().ToString() << endl;
   const KDTreeNode* currentBest = &previousBest;
   cout << "Current Best: " << previousBest.GetPoint().ToString() << endl;
   // If left child is the previous child (i.e: the branch we have come from)
   // then we don't have to check this again 
   if (fLeft != &previousChild && fLeft != NULL) {
      // Else, check if distance of left child to point is less than current best estimate 
      const double leftDist = fLeft->GetPoint().DistanceTo(point);
      double currentBestDist = currentBest->GetPoint().DistanceTo(point);
      cout << "Check Left Child: " << fLeft->GetPoint().ToString() << endl;
      cout << "Left Dist: " << leftDist << "\t" << "Current Best: " << currentBestDist << endl;
      if (leftDist < currentBestDist) {
         // If it is closer to point than current best estimate then update estimate
         currentBestDist = leftDist;
         currentBest = fLeft;
         cout << "Left is closer" << endl;
      }
      // Search children of Left node for any improvement on current best estimate
      currentBest = &(fLeft->SearchChildren(point, *currentBest, previousChild));
   }
   
   // If right child is the previous child (i.e: the branch we have come from)
   // then we don't have to check this again
   if (fRight != &previousChild  && fRight != NULL) {
      // Else, check if distance of right child to point is less than current best estimate 
      const double rightDist = fRight->GetPoint().DistanceTo(point);
      double currentBestDist = currentBest->GetPoint().DistanceTo(point);
      cout << "Check Right Child: " << fRight->GetPoint().ToString() << endl;
      cout << "Right Dist: " << rightDist << "\t" << "Current Best: " << currentBestDist << endl;
      if (rightDist < currentBestDist) {
         // If it is closer to point than current best estimate then update estimate
         currentBestDist = rightDist;
         currentBest = fRight;
         cout << "Right is closer" << endl;
      }
      // Search children of Left node for any improvement on current best estimate
      currentBest = &(fRight->SearchChildren(point, *currentBest, previousChild));
   }
   return *currentBest;
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
   
   // Find leaf node that contains the point. Store as current best nearest 
   const KDTreeNode& currentBest = fRoot->FindContainingNode(point);
   // Calculate distance from point to current best nearest neighbour
   double dist = currentBest.GetPoint().SquaredDistanceTo(point);
   #ifdef VERBOSE
      cout << endl << "--------------------" << endl;
      cout << "Current Best : ";
      cout << currentBest.GetPoint().ToString() << endl;
      cout << "Distance to Point: " << dist << endl;
   #endif
   // Now traverse back up tree looking for if any other nodes are closer
   const KDTreeNode& nearestNode = currentBest.CheckParentBranches(point, dist);
   return currentBest.GetPoint();
}

//______________________________________________________________________________
void KDTree::OutputGraphViz(ostream& out) const
{
   // Output Nodes in form of 'dot' language
   out << "digraph G {" << endl;
   fRoot->OutputGraphViz(out);
   out << "}" << endl;
}
