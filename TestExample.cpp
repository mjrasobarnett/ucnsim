#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <time.h>
#include <string>

#include "KDTree.hpp"
#include "KDTreeNode.hpp"
#include "Point.hpp"
#include "MWC.hpp"

using namespace std;

void InternetExample1(vector<Point*>& points);

int main(int argc, char **argv) {
   // Set up timing
   clock_t start, end;
   start = clock();

   // Generate points
   vector<Point*> points;
   InternetExample1(points);

   // Print points
   cout << "--------------------" << endl;
   cout << "Input Points" << endl;
   vector<Point*>::iterator it;
   for (it=points.begin(); it<points.end(); it++) {
      cout << it - points.begin() << "\t" << (*it)->ToString() << endl;
   }
   cout << "--------------------" << endl;

   // Build Tree
   KDTree tree(points);
   string filename = "mytree.dot";
   ofstream out(filename.c_str());
   tree.OutputGraphViz(out);
   cout << "Tree written to: " << filename << endl;
   out.close();
   
   // Find nearest neighbour
   Point testPoint(3,6,8);
   const Point& nearestPoint = tree.NearestNeighbour(testPoint);
   cout << "Containing Node: " << nearestPoint.ToString() << endl;
   
   // Output timing
   end = clock();
   cout << "--------------------" << endl;
   cout << "Time required for execution: ";
   cout << (double)(end-start)/CLOCKS_PER_SEC;
   cout << " seconds." << endl;
   cout << "--------------------" << endl;
   
	return 0;
}


//______________________________________________________________________________
void InternetExample1(vector<Point*>& points)
{
 //http://syntaxandsemantic.blogspot.com/2010/03/knn-algorithm-and-kd-trees.html
   points.push_back(new Point(5,9,10));
   points.push_back(new Point(2,6,8));
   points.push_back(new Point(14,3,7));
   points.push_back(new Point(3,4,9));
   points.push_back(new Point(4,13,5));
   points.push_back(new Point(8,2,1));
   points.push_back(new Point(7,9,6));
   points.push_back(new Point(4,1,6));
   points.push_back(new Point(2,2,10));
}