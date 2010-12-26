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
//______________________________________________________________________________

//______________________________________________________________________________
KDTreeNode::KDTreeNode()
           :fPoint(NULL), 
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
           :fPoint(other.fPoint), 
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
const KDTreeNode& KDTreeNode::FindContainingNode(const Point& point, int depth) const
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
   int axis = depth % 3;
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
      return fLeft->FindContainingNode(point, depth+1);
   } else {
      return fRight->FindContainingNode(point, depth+1);
   }
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
   const KDTreeNode& currentBest = fRoot->FindContainingNode(point, 0);
   // Calculate distance from point to current best nearest neighbour
   double dist = currentBest.GetPoint().SquaredDistanceTo(point);
   #ifdef VERBOSE
      cout << endl << "--------------------" << endl;
      cout << "Current Best : ";
      cout << currentBest.GetPoint().ToString() << endl;
      cout << "Distance to Point: " << dist << endl;
   #endif
   // Now traverse back up tree looking for if any other nodes are closer
   const KDTreeNode& nearestNode = currentBest->
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
