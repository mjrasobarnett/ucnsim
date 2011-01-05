#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <stdexcept>

#include "KDTree.h"

//#define PRINT_CONSTRUCTORS 
//#define VERBOSE

using namespace std;

//______________________________________________________________________________
KDTree::KDTree(vector<FieldVertex*>& pointList)
       :fRoot(NULL)
{
   #ifdef PRINT_CONSTRUCTORS
      cout << "KDTree Construct" << endl;
   #endif
   fRoot = this->BuildNode(pointList,0);
   #ifdef VERBOSE
      cout << endl << "Root FieldVertex: " << fRoot->GetPoint().ToString() << endl;
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
KDTreeNode* KDTree::BuildNode(vector<FieldVertex*>& points, KDTreeNode* parent, int depth)
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
      vector<FieldVertex*> beforeMedian, afterMedian;
      // Note: copy works elements in range [first, last)
      copy(points.begin(), points.begin()+median, back_inserter(beforeMedian));
      copy(points.begin()+median+1, points.end(), back_inserter(afterMedian));
      #ifdef VERBOSE
         cout << "--------------------" << endl;
         cout << "Ordered by Axis: " << axis << endl;
         vector<FieldVertex*>::iterator it;
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
const NodeStack* KDTree::NearestNeighbours(const FieldVertex& point, const int numberNeighbours) const
{
   // -- For a given point, find the 'n' closest neighbouring nodes, defined by numberNeighbours.
   if (numberNeighbours < 1) {
      throw runtime_error("Invalid number of nearest numbers requested");
   }
   // Define the storage for the 'n'-nearest-neighbours
   NodeStack* neighbours = new NodeStack(numberNeighbours);
   #ifdef VERBOSE
      cout << endl << "--------------------" << endl;
      cout << "Search for " << numberNeighbours << " nearest neighbours of Point : ";
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
   // Add First guess to list of nearest neighbours
   neighbours->AddNode(&firstGuess, dist);
   // Now traverse back up tree looking for if any other nodes are closer
   firstGuess.CheckParentForCloserNodes(point, *neighbours);
   #ifdef VERBOSE
      cout << "--------------------" << endl;
      cout << "Nearest Node : ";
      cout << neighbours->front()->GetPoint().ToString() << endl;
      cout << "Distance to Point: " << neighbours->front()->second << endl;
   #endif
   return neighbours;
}
//______________________________________________________________________________
void KDTree::OutputGraphViz(ostream& out) const
{
   // Output Nodes in form of 'dot' language
   out << "digraph G {" << endl;
   fRoot->OutputGraphViz(out);
   out << "}" << endl;
}
