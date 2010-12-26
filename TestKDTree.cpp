#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <time.h>
#include <string>

#include "KDTree.hpp"
#include "MWC.hpp"

using namespace std;

void BenchMark(const int numPoints); 
const Point& BruteForceNearestNeighbour(const vector<Point*>& pointList, const Point& point);

int main(int argc, char **argv) {
   
   BenchMark(10);
	
	return 0;
}

//______________________________________________________________________________
void BenchMark(const int numPoints) {
   // Generate random points
   MWC rng;
   vector<Point*> points;
   for (int i=0; i<numPoints; i++) {
      points.push_back(new Point(rng.rnd(),rng.rnd(),rng.rnd()));
   }
   // Print points
   cout << "--------------------" << endl;
   cout << "Input Points" << endl;
   vector<Point*>::iterator it;
   for (it = points.begin(); it != points.end(); it++) {
      cout << it - points.begin() << "\t" << (*it)->ToString() << endl;
   }
   cout << "--------------------" << endl;

   // Set up timing
   clock_t start, end;
   start = clock();
   // Build Tree
   KDTree tree(points);
   // Output time to build tree
   end = clock();
   const double treeBuildTime = (double)(end-start)/CLOCKS_PER_SEC;
   cout << "--------------------" << endl;
   cout << "Time required to build Tree: ";
   cout << treeBuildTime;
   cout << " seconds." << endl;
   cout << "--------------------" << endl;
   
   
   Point searchPoint(0.,0.,0.);
   cout << "--------------------" << endl;
   cout << "Test methods of finding nearest neighbour of Point: " << searchPoint.ToString() << endl;
   cout << "--------------------" << endl;
   
   // Set up timing
   start = clock();
   // Perform Brute force search for point
   cout << "--------------------" << endl;
   cout << "Performing Brute Force Search..." << endl;
   const Point& brutePoint = BruteForceNearestNeighbour(points, searchPoint);
   // Output time to build tree
   end = clock();
   const double bruteForceTime = (double)(end-start)/CLOCKS_PER_SEC;

   // Set up timing
   start = clock();
   // Perform Brute force search for point
   cout << "--------------------" << endl;
   cout << "Performing Tree-based Search..." << endl;
   const Point& treePoint = tree.NearestNeighbour(searchPoint);
   // Output time to build tree
   end = clock();
   const double treeSearchTime = (double)(end-start)/CLOCKS_PER_SEC;
   
   
   cout << "--------------------" << endl;
   cout << "Brute force solution: " << brutePoint.ToString() << endl;
   cout << "Time required for Brute force search: ";
   cout << bruteForceTime;
   cout << " seconds." << endl;
   cout << "--------------------" << endl;
   cout << "Tree solution: " << treePoint.ToString() << endl;
   cout << "Time required for Tree search: ";
   cout << treeSearchTime;
   cout << " seconds." << endl;
   cout << "--------------------" << endl;
}

//______________________________________________________________________________
const Point& BruteForceNearestNeighbour(const vector<Point*>& pointList, const Point& point)
{
   // Take list of points and do a brute force search to find the nearest neighbour
   // Return nearest neighbour
   double currentBestDist = 0.0;
   int currentBestPoint = 0;
   vector<Point*>::const_iterator it;
   for (it = pointList.begin(); it != pointList.end(); it++) {
      double dist = (*it)->DistanceTo(point);
      cout << (*it)->ToString() << "\t" << dist << endl;
      if (currentBestDist == 0.0 || dist < currentBestDist) {
         currentBestDist = dist;
         currentBestPoint = it - pointList.begin();
      }
   }
   return *(pointList[currentBestPoint]);
}
