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

int main(int argc, char **argv) {
   // Set up timing
   clock_t start, end;
   start = clock();

   // Generate random points
   MWC rng;
   vector<Point*> points;
   const int N=10;
   for (int i=0; i<N; i++) {
      points.push_back(new Point(rng.rnd(),rng.rnd(),rng.rnd()));
   }
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
   // Create dot file
   string filename = "mytree.dot";
   ofstream out(filename.c_str());
   tree.OutputGraphViz(out);
   cout << "Tree written to: " << filename << endl;
   out.close();
   
   // Output timing
   end = clock();
   cout << "--------------------" << endl;
   cout << "Time required for execution: ";
   cout << (double)(end-start)/CLOCKS_PER_SEC;
   cout << " seconds." << endl;
   cout << "--------------------" << endl;
   
	return 0;
}

