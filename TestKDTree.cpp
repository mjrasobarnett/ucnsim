#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <time.h>
#include <string>
#include <cassert>

#include "KDTree.hpp"
#include "MWC.hpp"

using namespace std;

//#define VERBOSE

void BenchMark(const int numPoints); 
const Point& BruteForceNearestNeighbour(const vector<Point*>& pointList, const Point& point);

//______________________________________________________________________________
int main(int argc, char **argv) {
   
   BenchMark(100);
	
	return 0;
}

//______________________________________________________________________________
void BenchMark(const int numPoints) {
   // -- Benchmark builds 'numPoints' random points, constructs a kd-tree of these points
   // -- then tests how long it takes to calculate the nearest neighbour of some random search point
   // -- against an brute-force search 
   cout << "Benchmarking kd-tree nearest neighbour search, against brute-force search" << endl;
   //-----------------------------------------------------------
   // -- Generate random points
   MWC rng;
   vector<Point*> points;
   for (int i=0; i<numPoints; i++) {
      points.push_back(new Point(rng.rnd(),rng.rnd(),rng.rnd()));
   }
   // Print points
   #ifdef VERBOSE
      cout << "--------------------" << endl;
      cout << "Input Points" << endl;
      vector<Point*>::iterator it;
      for (it = points.begin(); it != points.end(); it++) {
         cout << it - points.begin() << "\t" << (*it)->ToString() << endl;
      }
      cout << "--------------------" << endl;
   #endif
   //-----------------------------------------------------------
   // -- Build Tree
   // Set up timing
   clock_t start, end;
   start = clock();
   KDTree tree(points);
   // Output time to build tree
   end = clock();
   const double treeBuildTime = (double)(end-start)/CLOCKS_PER_SEC;
   cout << "--------------------" << endl;
   cout << "Number of points in Tree: " << numPoints << endl;
   cout << "Time required to build Tree: ";
   cout << treeBuildTime;
   cout << " seconds." << endl;
   //-----------------------------------------------------------
   Point searchPoint(rng.rnd(),rng.rnd(),rng.rnd());
   #ifdef VERBOSE
      cout << "--------------------" << endl;
      cout << "Search Point: " << searchPoint.ToString() << endl;
   #endif
   //-----------------------------------------------------------
   // -- Perform Brute force search for point
   // Set up timing
   start = clock();
   cout << "--------------------" << endl;
   cout << "Performing Brute Force Search..." << endl;
   const Point& brutePoint = BruteForceNearestNeighbour(points, searchPoint);
   // Output time to build tree
   end = clock();
   const double bruteForceTime = (double)(end-start)/CLOCKS_PER_SEC;
   //-----------------------------------------------------------
   // -- Perform Tree-based search for point
   // Set up timing
   start = clock();
   cout << "--------------------" << endl;
   cout << "Performing Tree-based Search..." << endl;
   const Point& treePoint = tree.NearestNeighbour(searchPoint);
   // calculate time to build tree
   end = clock();
   const double treeSearchTime = (double)(end-start)/CLOCKS_PER_SEC;
   
   //-----------------------------------------------------------
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
   assert(treePoint == brutePoint);
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
      #ifdef VERBOSE
         cout << setfill(' ') << setw(20);
         cout << (*it)->ToString();
         cout << "\t" << dist << endl;
      #endif
      if (currentBestDist == 0.0 || dist < currentBestDist) {
         currentBestDist = dist;
         currentBestPoint = it - pointList.begin();
      }
   }
   return *(pointList[currentBestPoint]);
}
