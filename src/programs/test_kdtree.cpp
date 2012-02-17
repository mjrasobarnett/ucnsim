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
#include "KDTreeNode.hpp"
#include "FieldVertex.hpp"
#include "VertexStack.hpp"

#include "TRandom.h"

using namespace std;

//#define VERBOSE

void BenchMark(const int numPoints, const int repetitions, const int numNeighbours, ostream& out); 
VertexStack* BruteForceNearestNeighbours(const vector<const FieldVertex*>& pointList, const FieldVertex& point, const int nearestNeighbours);
void InternetExample1(vector<const FieldVertex*>& points);

//______________________________________________________________________________
int main(int /*argc*/, char ** /*argv*/) {
   ofstream out("temp/benchmark_data.txt");
   out << "Num Points" << "\t" << "Brute Force Time" << "\t" << "Tree Search Time" << "\t";
   out << "Std Dev Brute Force" << "\t" << "Std Dev Tree Search" << "\t";
   out << "Average Nodes Visited" << "\t" << "Std Dev" << "\t";
   out << "Average Percent Visited" << "\t" << "Std Dev" << endl;
   const int repetitions = 100;
   const int numNeighbours = 6;
   BenchMark(10, repetitions, numNeighbours, out);
   BenchMark(100, repetitions, numNeighbours, out);
   BenchMark(1000, repetitions, numNeighbours, out);
   BenchMark(10000, repetitions, numNeighbours, out);
   BenchMark(100000, repetitions, numNeighbours, out);
   BenchMark(1000000, repetitions, numNeighbours, out);
//   BenchMark(10000000, repetitions, numNeighbours, out);
   return 0;
}

//______________________________________________________________________________
void BenchMark(const int numPoints, const int repetitions, const int numNeighbours, ostream& out) {
   // -- Benchmark builds 'numPoints' random points, constructs a kd-tree of these points
   // -- then tests how long it takes to calculate the 'n' nearest neighbours of some random point
   // -- against a brute-force search approach 
   #ifdef VERBOSE
      cout << "Benchmarking kd-tree nearest neighbour search, against brute-force search" << endl;
   #endif
   //-----------------------------------------------------------
   // -- Generate random points
   vector<const FieldVertex*> points;
   for (int i=0; i<numPoints; i++) {
      points.push_back(new FieldVertex(gRandom->Rndm(),gRandom->Rndm(),gRandom->Rndm(),0,0,0,0));
   }
   // Print points
   #ifdef VERBOSE
      cout << "--------------------" << endl;
      cout << "Input Points" << endl;
      vector<FieldVertex*>::iterator it;
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
   // Repeat search for number of times defined by 'repetitions'
   // Store all times in vector for averaging and std dev
   double totBruteForceTime = 0.;
   double totTreeSearchTime = 0.;
   double totVisited = 0.;
   vector<double> bruteForceTimes;
   vector<double> treeSearchTimes;
   vector<double> visited;
   for (int iter = 0; iter < repetitions; iter++) {
      FieldVertex searchPoint(gRandom->Rndm(),gRandom->Rndm(),gRandom->Rndm(),0,0,0,0);
      #ifdef VERBOSE
         cout << "--------------------" << endl;
         cout << "Search Point: " << searchPoint.ToString() << endl;
      #endif
      //-----------------------------------------------------------
      // -- Perform Brute force search for point
      // Set up timing
      start = clock();
      #ifdef VERBOSE
         cout << "--------------------" << endl;
         cout << "Performing Brute Force Search..." << endl;
      #endif
      const VertexStack* bruteList = BruteForceNearestNeighbours(points, searchPoint, numNeighbours);
      // Output time to build tree
      end = clock();
      const double bruteForceTime = (double)(end-start)/CLOCKS_PER_SEC;
      //-----------------------------------------------------------
      // -- Perform Tree-based search for point
      // Set up timing
      start = clock();
      #ifdef VERBOSE
         cout << "--------------------" << endl;
         cout << "Performing Tree-based Search..." << endl;
      #endif
      const VertexStack* treeList = tree.NearestNeighbours(searchPoint, numNeighbours);
      // calculate time to build tree
      end = clock();
      const double treeSearchTime = (double)(end-start)/CLOCKS_PER_SEC;
      //-----------------------------------------------------------
      #ifdef VERBOSE
         cout << "--------------------" << endl;
         cout << "Time required for Brute force search: ";
         cout << bruteForceTime;
         cout << " seconds." << endl;
         cout << "--------------------" << endl;
         cout << "Time required for Tree search: ";
         cout << treeSearchTime;
         cout << " seconds." << endl;
         cout << "--------------------" << endl;
         cout << "Nearest Neighbours found: " << endl;
         cout << "Brute" << "\t";
         cout << bruteList->size() << endl;
         list<StackElement>::const_iterator stackIter;
         for (stackIter = bruteList->begin(); stackIter != bruteList->end(); stackIter++) {
            cout << setfill(' ') << setw(10) << stackIter->first->GetPoint().ToString() << endl;
         }
         cout << endl;
         cout << "Tree" << "\t";
         cout << treeList->size() << endl;
         for (stackIter = treeList->begin(); stackIter != treeList->end(); stackIter++) {
            cout << setfill(' ') << setw(10) << stackIter->first->GetPoint().ToString() << endl;
         }
         cout << "--------------------" << endl;
      #endif
      double numvisited = tree.GetVisited();
      //cout << "Visited: " << tree.GetVisited() << "\t Num Points: " << numPoints;
      //cout << "\t Percent: " << numvisited << endl;
      
      assert(*treeList == *bruteList);
      bruteForceTimes.push_back(bruteForceTime);
      treeSearchTimes.push_back(treeSearchTime);
      visited.push_back(numvisited);
      totBruteForceTime += bruteForceTime;
      totTreeSearchTime += treeSearchTime;
      totVisited += numvisited;
      delete treeList;
      delete bruteList;
   }
   // Calculate mean search times
   double avgBruteForceTime = totBruteForceTime/((double)repetitions);
   double avgTreeSearchTime = totTreeSearchTime/((double)repetitions);
   double avgVisited = totVisited/((double)repetitions);
   double totPercentVisited = 100.*(((double)totVisited)/((double)numPoints));
   double avgPercentVisited = totPercentVisited/((double)repetitions);
   // Calculate Std Dev
   double sumBrute = 0.;
   double sumTree = 0.;
   double sumVisited = 0.;
   double sumPercVisited = 0.;
   for (int iter = 0; iter < repetitions; iter++) {
      sumBrute += pow((bruteForceTimes[iter] - avgBruteForceTime), 2.0);
      sumTree += pow((treeSearchTimes[iter] - avgTreeSearchTime), 2.0);
      sumVisited += pow((visited[iter] - avgPercentVisited), 2.0);
      sumPercVisited += pow((100.*(((double)visited[iter])/((double)numPoints)) - avgPercentVisited), 2.0);
   }
   double stdDevBrute = sqrt(sumBrute/((double)(repetitions-1)));
   double stdDevTree = sqrt(sumTree/((double)(repetitions-1)));
   double stdDevVisited = sqrt(sumVisited/((double)(repetitions-1)));
   double stdDevPercVisited = sqrt(sumPercVisited/((double)(repetitions-1)));
   #ifdef VERBOSE
      cout << "--------------------" << endl;
      cout << "Average Time required for Brute force search: ";
      cout << avgBruteForceTime;
      cout << " seconds." << endl;
      cout << "--------------------" << endl;
      cout << "Average Time required for Tree search: ";
      cout << avgTreeSearchTime;
      cout << " seconds." << endl;
      cout << "--------------------" << endl;
   #endif
   out << numPoints << "\t" << avgBruteForceTime << "\t" << avgTreeSearchTime << "\t";
   out << stdDevBrute << "\t" << stdDevTree << "\t";
   out << avgVisited << "\t" << stdDevVisited << "\t";
   out << avgPercentVisited << "\t" << stdDevPercVisited << endl;
   return;
}

//______________________________________________________________________________
VertexStack* BruteForceNearestNeighbours(const vector<const FieldVertex*>& pointList, const FieldVertex& point, const int nearestNeighbours)
{
   // Take list of points and do a brute force search to find the nearest neighbour
   // Return nearest neighbour
   VertexStack* neighbours = new VertexStack(nearestNeighbours);
   vector<const FieldVertex*>::const_iterator it;
   for (it = pointList.begin(); it != pointList.end(); it++) {
      double dist = (*it)->DistanceTo(point);
      #ifdef VERBOSE
         cout << setfill(' ') << setw(20);
         cout << (*it)->ToString();
         cout << "\t" << dist << endl;
      #endif
      neighbours->ExamineVertex(**it,dist);
   }
   return neighbours;
}

//______________________________________________________________________________
void InternetExample1(vector<const FieldVertex*>& points)
{
 //http://syntaxandsemantic.blogspot.com/2010/03/knn-algorithm-and-kd-trees.html
   points.push_back(new FieldVertex(5,9,10,0,0,0,0));
   points.push_back(new FieldVertex(2,6,8,0,0,0,0));
   points.push_back(new FieldVertex(14,3,7,0,0,0,0));
   points.push_back(new FieldVertex(3,4,9,0,0,0,0));
   points.push_back(new FieldVertex(4,13,5,0,0,0,0));
   points.push_back(new FieldVertex(8,2,1,0,0,0,0));
   points.push_back(new FieldVertex(7,9,6,0,0,0,0));
   points.push_back(new FieldVertex(4,1,6,0,0,0,0));
   points.push_back(new FieldVertex(2,2,10,0,0,0,0));
}
